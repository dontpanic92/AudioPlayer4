#include "MP3Decoder.h"

int MP3Decoder::CheckSupport(const char* fname){
	mpg123_handle *l_mh = NULL;
	l_mh = mpg123_new(NULL, NULL);
	if (l_mh == NULL){
		return ERR_INIT_ERROR;
	}
	if (mpg123_open(l_mh, fname) != MPG123_OK){
		mpg123_close(l_mh);
		mpg123_delete(l_mh);
		return ERR_OPEN_ERROR;
	}
	if(MPG123_DONE == mpg123_decode_frame(l_mh, NULL, NULL, NULL)){
		mpg123_close(l_mh);
		mpg123_delete(l_mh);
		return ERR_NOT_SUPPORT;
	}
	mpg123_close(l_mh);
	mpg123_delete(l_mh);
	return DECODER_OK;
}
int MP3Decoder::Open(const char* fname){
	Close();
	mh = mpg123_new(NULL, NULL);
	if (mh == NULL){
		return ERR_INIT_ERROR;
	}
	if (mpg123_open(mh, fname) != MPG123_OK){
		Close();
		return ERR_OPEN_ERROR;
	}

	int ret;
	if(MPG123_DONE == (ret = mpg123_decode_frame(mh, NULL, NULL, NULL))){
		Close();
		return ERR_NOT_SUPPORT;
	}else if (ret == MPG123_NEW_FORMAT){
		mpg123_getformat(mh, &rate, &channel, NULL);
		mpg123_frameinfo mi;
		mpg123_scan(mh);
		mpg123_info(mh, &mi);
		duration = double(mpg123_length(mh)) / rate;
		bitrate = mi.bitrate;
		char temp[30];
		if (mi.vbr == MPG123_CBR)
			strcpy_s(temp, "CBR");
		else if (mi.vbr == MPG123_VBR)
			strcpy_s(temp, "VBR");
		else
			strcpy_s(temp, "ABR");

		double ver;
		if (mi.version == MPG123_1_0)
			ver = 1;
		else if (mi.version == MPG123_2_0)
			ver = 2;
		else
			ver = 2.5;

		if (ver == 1)
			sprintf_s(CurrentCodec,"MP%d - %s", mi.layer, /*ver,*/ temp);
		else
			sprintf_s(CurrentCodec,"MP%d V%.1f - %s", mi.layer, ver, temp);
		return DECODER_OK;
	}
	return ERR_GENERIC;
}
int MP3Decoder::Seek(double sec){
	if(mh == NULL)
		return ERR_GENERIC;
	if (mpg123_seek_frame(mh,mpg123_timeframe(mh, sec), SEEK_SET) < 0)
		return ERR_GENERIC;
	else
		return DECODER_OK;
}
void MP3Decoder::GetFileExt(char* Description, char* FileExt){
	char des[] = "MPEG Audio(*.mp1,*.mp2,*.mp3)";
	char ext[] = "*.mp1;*.mp2;*.mp3";
	strcpy_s(Description, sizeof(des), des);
	strcpy_s(FileExt, sizeof(ext), ext);
}
int MP3Decoder::GetFormat(long* prate,int* pchannel){
	*prate = rate;
	*pchannel = channel;
	return 0;
}
int MP3Decoder::GetFileInfo(Info* info){
	info->rate = this->rate;
	info->duration =  this->duration;
	info->bitrate =  this->bitrate;
	info->channel =  this->channel;
	strcpy_s(info->codec, CurrentCodec);
	return DECODER_OK;
}
int MP3Decoder::GetFileInfo(const char* fname, Info* info){
	mpg123_handle *l_mh = NULL;
	mpg123_frameinfo mi;

	l_mh = mpg123_new(NULL, NULL);
	if (l_mh == NULL){
		return ERR_INIT_ERROR;
	}
	if (mpg123_open(l_mh, fname) != MPG123_OK){
		mpg123_close(l_mh);
		mpg123_delete(l_mh);
		return ERR_OPEN_ERROR;
	}
	//if(MPG123_NEW_FORMAT == mpg123_decode_frame(mh, NULL, NULL, NULL)){
		mpg123_getformat(l_mh, &info->rate, &info->channel, NULL);
		mpg123_info(l_mh, &mi);
		info->duration = double(mpg123_length(l_mh)) / info->rate;
		info->bitrate = mi.bitrate;
	char temp[30];
	if (mi.vbr == MPG123_CBR)
		strcpy_s(temp, "CBR");
	else if (mi.vbr == MPG123_VBR)
		strcpy_s(temp, "VBR");
	else
		strcpy_s(temp, "ABR");

	double ver;
	if (mi.version == MPG123_1_0)
		ver = 1;
	else if (mi.version == MPG123_2_0)
		ver = 2;
	else
		ver = 2.5;
	if (ver == 1)
		sprintf_s(info->codec,"MP%d - %s", mi.layer, /*ver,*/ temp);
	else
		sprintf_s(info->codec,"MP%d V%.1f - %s", mi.layer, ver, temp);
	//}
	mpg123_close(l_mh);
	mpg123_delete(l_mh);
	return DECODER_OK;
}

int MP3Decoder::ReadAndDecode(unsigned char* buf,int size,unsigned int* read){
	int mc = mpg123_read(mh, buf, size, read);
	if (mc == MPG123_DONE){
		return DECODER_DONE;
	}else if (mc == MPG123_ERR){
		return ERR_GENERIC;
	}
	return DECODER_OK;
}
double MP3Decoder::GetPosition(int buffer_size){
	if (mh == NULL)
		return 0;
	double sec;
	mpg123_position(mh, 0, buffer_size, NULL, NULL, &sec, NULL);
	return sec;
}
double MP3Decoder::GetLength(){
	if (mh == NULL)
		return 0;
	return double(mpg123_length(mh)) / rate;
}
int MP3Decoder::Close(){
	if (mh) {
		mpg123_close(mh);
		mpg123_delete(mh);
		mh = NULL;
	}
	return DECODER_OK;
}
