#include "RDecoderManager.h"

struct Ext2i{
	char ext[10];
	int i;
};

Ext2i ext2i[] = {{"ogg", 1}, {"oga", 1}, {"ape", 2}, {"mac", 2}, {"wav", 3}};

RDecoderManager::RDecoderManager():DecoderNum(4),CurrentDecoder(-1),pBuffer(RPlayerBuffer::getInstance()){
	CreateAllDecoders();
}

RDecoderManager::~RDecoderManager(){
	DeleteAllDecoders();
}

void RDecoderManager::CreateAllDecoders(){
	decoders[0] = static_cast<Decoder*>(new MP3Decoder);
	decoders[1] = static_cast<Decoder*>(new OggDecoder);
	decoders[2] = static_cast<Decoder*>(new ApeDecoder);
	decoders[3] = static_cast<Decoder*>(new WAVDecoder);
}

void RDecoderManager::DeleteAllDecoders(){
	for (int i = 0; i < DecoderNum; i++)
		delete[] decoders[i];
}

void RDecoderManager::FindDecoder(){
	int i;
	CurrentDecoder = -1;

	if (filename[0] == '\0')
		return;

	/////////////根据后缀名优先选择解码器/////////////////
	char* p;
	bool founded = false;
	int priority = -1;
	for (p = filename + strlen(filename); p >= filename; p--){	//提取后缀名
		if (*p == '.'){
			founded = true;
			break;
		}
	}											
	if (founded == true){
		p++;
		for (int i = 0; i < sizeof(ext2i); i++){				//根据后缀名优先选择解码器
			if (!strcmp(ext2i[i].ext, p)){
				priority = ext2i[i].i;
			}
		}
		if (priority != -1){
			if (decoders[priority]->CheckSupport(filename) == DECODER_OK){
				CurrentDecoder = priority;
				return;
			}
		}
	}
	///////////////////////////////////////////
	for (i = 0; i < DecoderNum; i++){
		if (i == priority)
			continue;
		if (decoders[i]->CheckSupport(filename) == DECODER_OK){
			CurrentDecoder = i;
			break;
		}
	}
}

int RDecoderManager::Open(const char* fname){
	memcpy(filename, fname, sizeof(filename));
	FindDecoder();
	if (CurrentDecoder == -1)
		return -1;
	decoders[CurrentDecoder]->Open(filename);
}

void RDecoderManager::Close(){
	if (CurrentDecoder != -1){
		decoders[CurrentDecoder]->Close();
		CurrentDecoder = -1;
	}
}

bool RDecoderManager::GetInfo(Info* info){
	memset(info, 0, sizeof(Info));
	if (CurrentDecoder != -1){
		decoders[CurrentDecoder]->GetFileInfo(info);
		return true;
	}
	return false;
}

bool RDecoderManager::GetFileInfo(char* filename, Info* info){
	memset(info, 0, sizeof(Info));
	int index = -1;
	char* p;
	bool founded = false;
	int priority = -1;

	for (p = filename + strlen(filename); p >= filename; p--){	//提取后缀名
		if (*p == '.'){
			founded = true;
			break;
		}
	}											

	if (founded == true){
		p++;
		for (int i = 0; i < sizeof(ext2i); i++){				//根据后缀名优先选择解码器
			if (!strcmp(ext2i[i].ext, p)){
				priority = ext2i[i].i;
			}
		}
		if (priority != -1){
			if (decoders[priority]->GetFileInfo(filename, info)/*decoders[priority]->CheckSupport(filename)*/ == DECODER_OK){
				//decoders[priority]->GetFileInfo(filename, info);
				return true;
			}else{
				return false;
			}
		}
	}
	for (int i = 0; i < DecoderNum; i++){
		if (i == priority)
			continue;
		if (decoders[i]->CheckSupport(filename) == DECODER_OK){
			index = i;
			break;
		}
	}
	if (index != -1){
		decoders[index]->GetFileInfo(filename, info);
		return true;
	}
	return false;
}

void RDecoderManager::GetSupportFileType(char* buffer){
	char tmpdes[1000], tmpext[1000], tmpall[1000], tmp[1000];
	char* ptmpall = tmpall, *ptmp = tmp;
	memset(tmp, 0, sizeof(tmp));
	sprintf_s(tmpall, "所有支持的格式\0");
	ptmpall += strlen(tmpall) + 1;
	for (int i = 0; i < DecoderNum; i++){
		decoders[i]->GetFileExt(tmpdes, tmpext);
		memcpy(ptmpall, tmpext, strlen(tmpext));
		ptmpall += strlen(tmpext);
		if (i == DecoderNum - 1)
			*(ptmpall++) = 0;
		else
			*(ptmpall++) = ';';
		memcpy(ptmp, tmpdes, strlen(tmpdes));
		ptmp += strlen(tmpdes);
		*(ptmp++) = 0;
		memcpy(ptmp, tmpext, strlen(tmpext));
		ptmp += strlen(tmpext);
		*(ptmp++) = 0;
	}
	memcpy(buffer, tmpall, ptmpall - tmpall);
	buffer += ptmpall - tmpall;
	//*(buffer++) = 0;
	memcpy(buffer, tmp, ptmp - tmp);
	buffer += ptmp - tmp;
	*(buffer++) = 0;
	*(buffer++) = 0;
}

double RDecoderManager::GetPosition(int buffered){
	if (CurrentDecoder == -1)
		return 0;
	return decoders[CurrentDecoder]->GetPosition(buffered);
}

double RDecoderManager::GetDuration(){
	if (CurrentDecoder == -1)
		return 0;
	return decoders[CurrentDecoder]->GetLength();
}

void RDecoderManager::Seek(double sec){
	if (CurrentDecoder == -1)
		return;
	decoders[CurrentDecoder]->Seek(sec);
}

int RDecoderManager::ReadAndDecode(unsigned char* buffer, unsigned int size, unsigned int* read){
	int ret;
	if (buffer == NULL){
		unsigned char* pbuf = new unsigned char[size];
		ret = decoders[CurrentDecoder]->ReadAndDecode(pbuf, size, &size);
		pBuffer->PutBuffer(pbuf, size);
		delete[] pbuf;
	}else{
		ret = decoders[CurrentDecoder]->ReadAndDecode(buffer, size, read);
	}
	return ret;
}