#include "ApeDecoder.h"

ApeDecoder::ApeDecoder(){
	pAPEDec = NULL;
}
ApeDecoder::~ApeDecoder(){
	Close();
}
int ApeDecoder::CheckSupport(const char* fname){
	int ret;
	wchar_t tmp[260];
	wsprintfW(tmp,L"%hs",fname);
	IAPEDecompress* _pAPEDec = CreateIAPEDecompress(tmp, &ret);
	if (_pAPEDec == NULL)
		return ERR_NOT_SUPPORT;
	delete _pAPEDec;
	return DECODER_OK;
}
int ApeDecoder::Open(const char* fname){
	int ret;
	wchar_t tmp[260];
	wsprintfW(tmp,L"%hs",fname);
	pAPEDec = CreateIAPEDecompress(tmp, &ret);
	if (pAPEDec == NULL)
		return ERR_NOT_SUPPORT;
	channel = pAPEDec->GetInfo(APE_INFO_CHANNELS);
	bitrate = pAPEDec->GetInfo(APE_INFO_AVERAGE_BITRATE );
	rate = pAPEDec->GetInfo(APE_INFO_SAMPLE_RATE);
	duration = double(pAPEDec->GetInfo(APE_INFO_LENGTH_MS)) / 1000.0;
	sprintf_s(CurrentCodec,"APE V%.2f", float(pAPEDec->GetInfo(APE_INFO_FILE_VERSION)) / float(1000));
	return DECODER_OK;
}
int ApeDecoder::Seek(double sec){
	int total_blocks = pAPEDec->GetInfo(APE_INFO_TOTAL_BLOCKS);
	pAPEDec->Seek(int (sec / duration * total_blocks));
	return DECODER_OK;
}
void ApeDecoder::GetFileExt(char* Description, char* FileExt){
	char des[] = "APE(*.ape)";
	char ext[] = "*.ape";
	strcpy_s(Description, sizeof(des), des);
	strcpy_s(FileExt, sizeof(ext), ext);
}
int ApeDecoder::GetFormat(long* rate,int* channel){
	*rate = this->rate;
	*channel = this->channel;
	return DECODER_OK;
}
int ApeDecoder::GetFileInfo(Info* info){
	info->bitrate = bitrate;
	info->channel = channel;
	info->duration = duration;
	info->rate = rate;
	strcpy_s(info->codec, CurrentCodec);
	return DECODER_OK;
}
int ApeDecoder::GetFileInfo(const char* filename, Info* info){
	int ret;
	wchar_t tmp[260];
	wsprintfW(tmp,L"%hs",filename);
	IAPEDecompress* _pAPEDec = CreateIAPEDecompress(tmp, &ret);
	if (_pAPEDec == NULL)
		return ERR_NOT_SUPPORT;
	info->bitrate = _pAPEDec->GetInfo(APE_INFO_AVERAGE_BITRATE );
	info->channel = _pAPEDec->GetInfo(APE_INFO_CHANNELS);
	info->duration = double(_pAPEDec->GetInfo(APE_INFO_LENGTH_MS)) / 1000.0;
	info->rate = _pAPEDec->GetInfo(APE_INFO_SAMPLE_RATE);
	sprintf_s(info->codec,"APE V%.2f", float(_pAPEDec->GetInfo(APE_INFO_FILE_VERSION)) / float(1000));
	delete _pAPEDec;
	return DECODER_OK;
}
int ApeDecoder::ReadAndDecode(unsigned char* buf,int size,unsigned int* read){
	int r;
	if (pAPEDec->GetData((char*)buf, size / pAPEDec->GetInfo(APE_INFO_BLOCK_ALIGN), &r) != ERROR_SUCCESS){
		*read = 0;
		return ERR_GENERIC;
	}
	*read = r * pAPEDec->GetInfo(APE_INFO_BLOCK_ALIGN);
	if (*read == 0){
		return DECODER_DONE;
}
	return DECODER_OK;
}
double ApeDecoder::GetPosition(int buffer_size){
	int total_block = pAPEDec->GetInfo(APE_INFO_TOTAL_BLOCKS);
	double now = double(pAPEDec->GetInfo(APE_DECOMPRESS_CURRENT_MS)) / 1000.0;
	now -= double(buffer_size) / pAPEDec->GetInfo(APE_INFO_BYTES_PER_SAMPLE) / channel / total_block * duration;
	return now;
}
double ApeDecoder::GetLength(){
	return duration;
}
int ApeDecoder::Close(){
	if (pAPEDec)
		delete[] pAPEDec;
	pAPEDec = NULL;
	return DECODER_OK;
}