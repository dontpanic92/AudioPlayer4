#include "OggDecoder.h"
OggDecoder::OggDecoder(){
	ZeroMemory(&ov_f, sizeof(ov_f));
}
OggDecoder::~OggDecoder(){
	Close();
}
int OggDecoder::CheckSupport(const char* fname){
	OggVorbis_File ovf;
	if (ov_fopen(fname, &ovf) < 0){
		return ERR_NOT_SUPPORT;
	}
	ov_clear(&ovf);
	return DECODER_OK;
}
int OggDecoder::Open(const char* fname){
	Close();
	vorbis_info *pInfo;
	if (ov_fopen(fname, &ov_f) < 0)
		return ERR_NOT_SUPPORT;
	if (ov_streams(&ov_f) != 1){
		ov_clear(&ov_f);
		return ERR_GENERIC;
	}
	pInfo = ov_info(&ov_f, -1);
	if (pInfo == NULL){
		ov_clear(&ov_f);
		return ERR_GENERIC;
	}
	channel = pInfo->channels;
	rate = pInfo->rate;
	bitrate = ov_bitrate(&ov_f, -1) / 1000;
	duration = ov_time_total(&ov_f, -1);
	if (pInfo->version != 0)
		sprintf_s(CurrentCodec,"Ogg Vorbis V%d", pInfo->version);
	else
		sprintf_s(CurrentCodec,"Ogg Vorbis");
	return DECODER_OK;
}
int OggDecoder::Seek(double sec){
	ov_pcm_seek(&ov_f, ogg_int64_t((sec / duration) * ov_pcm_total(&ov_f, -1)));
	return DECODER_OK;
}
void OggDecoder::GetFileExt(char* Description, char* FileExt){
	char des[] = "Ogg Vorbis(*.ogg)";
	char ext[] = "*.ogg";
	strcpy_s(Description, sizeof(des), des);
	strcpy_s(FileExt, sizeof(ext), ext);
}
int OggDecoder::GetFormat(long* rate,int* channel){
	*rate = this->rate;
	*channel = this->channel;
	return DECODER_OK;
}
int OggDecoder::GetFileInfo(Info* info){
	info->bitrate = bitrate;
	info->channel = channel;
	info->duration = duration;
	info->rate = rate;
	strcpy_s(info->codec, CurrentCodec);
	return DECODER_OK;
}
int OggDecoder::GetFileInfo(const char* filename, Info* info){
	vorbis_info *pInfo;
	OggVorbis_File ovf;
	if (ov_fopen(filename, &ovf) < 0)
		return ERR_NOT_SUPPORT;
	if (ov_streams(&ovf) != 1){
		ov_clear(&ovf);
		return ERR_GENERIC;
	}
	pInfo = ov_info(&ovf, -1);
	if (pInfo == NULL){
		ov_clear(&ovf);
		return ERR_GENERIC;
	}
	info->channel = pInfo->channels;
	info->rate = pInfo->rate;
	info->bitrate = ov_bitrate(&ovf, -1) / 1000;
	info->duration = ov_time_total(&ovf, -1);
	if (pInfo->version != 0)
		sprintf_s(info->codec,"Ogg Vorbis V%d", pInfo->version);
	else
		sprintf_s(info->codec,"Ogg Vorbis");
	ov_clear(&ovf);
	return DECODER_OK;
}
int OggDecoder::ReadAndDecode(unsigned char* buf,int size,unsigned int* read){
	int current_section;
	*read = ov_read(&ov_f, (char*)buf, size, 0, 2, 1, &current_section);
	if (*read == 0){ 
		return DECODER_DONE;
	}else if (*read < 0) { 
		return ERR_GENERIC;
    }
	return DECODER_OK;
}
double OggDecoder::GetPosition(int buffer_size){
	double time = ov_time_tell(&ov_f) - double(buffer_size) / channel / 2 / ov_pcm_total(&ov_f, -1) * duration;
	return time;
}
double OggDecoder::GetLength(){
	return duration;
}
int OggDecoder::Close(){
	ov_clear(&ov_f);
	return DECODER_OK;
}