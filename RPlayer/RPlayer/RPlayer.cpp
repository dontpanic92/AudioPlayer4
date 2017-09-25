#include "RPlayer.h"

RPlayer* RPlayer::theSingleton = NULL;

RPlayer::RPlayer():buffer(RPlayerBuffer::getInstance()),time(0),volume(SDL_MIX_MAXVOLUME),
				status(STOP),hPlayThread(NULL), 
				pNotify(NULL), NotifyParam(NULL), hwnd(NULL){
	
	filename[0] = '\0';
	memset(silence_buf, 0, sizeof(silence_buf));
	InitializeCriticalSection(&cs);
	hEvent = CreateEvent(NULL, FALSE, TRUE, L"readanddecode");
}

RPlayer::~RPlayer(){

}

RPlayer* RPlayer::getInstance(){
	if(theSingleton == NULL){
		theSingleton = new RPlayer();
	}
	return theSingleton;
}

void RPlayer::cleanInstance(){
	if (theSingleton != NULL){
		delete theSingleton;
	}
}

RDecoderManager* RPlayer::GetDecoderManager(){
	return &decoder;
}

void RPlayer::NotifyUI(int msg, int param1, char* param2){
	if (hwnd != NULL)
		PostMessage(hwnd, RM_CHANGE_SONG, 0, 0);
}
//void RPlayer::SetNotify(NotifyCallBack pFunc, void* param){
void RPlayer::SetNotify(HWND hWnd){
	//pNotify = pFunc;
	//NotifyParam = param;
	hwnd = hWnd;
}

void RPlayer::ClearCache(){
	buffer->Clear();
}
int RPlayer::Open(){
	decoder.Open(filename);
	Info info;
	decoder.GetInfo(&info);
	wanted_spec.channels = info.channel;
	wanted_spec.freq = info.rate;
	wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.silence = 0;
    wanted_spec.samples = 1024;
    wanted_spec.callback = CallBack;
    wanted_spec.userdata = this;
	SDL_OpenAudio(&wanted_spec, &spec);
	SDL_PauseAudio(0);
	ClearCache();
	return 0;
}
void RPlayer::Close(){
	decoder.Close();
	SDL_CloseAudio();
	ClearCache();
}
void RPlayer::Resume(){
	if (status == PAUSE){
		status = PLAY;
		return;
	}
}
int RPlayer::Play(char* filename){
	Stop();
	//_MIDIPlayer.Play(filename);
	//return DECODER_OK;
	strncpy_s(this->filename, filename, 260);
	if (filename != NULL && filename[0] != '\0'){
		Open();
		status = PLAY;
		hPlayThread = SDL_CreateThread(PlayThread, this);
	}
	else{
		Close();
		hPlayThread = NULL;
	}
	return 0;
}
void RPlayer::Pause(){
	status = PAUSE;
}
void RPlayer::Stop(){
	status = STOP;
	loop = false;
	SetEvent(hEvent);
	if (hPlayThread != NULL){
		SDL_WaitThread(hPlayThread, NULL);
		hPlayThread = NULL;
	}
	Close();
	time = 0;
}
int RPlayer::GetStatus(){
	return status;
}
int RPlayer::PlayThread(void* param){
	RPlayer* pThis = (RPlayer*)param;
	pThis->ReadAndDecode();
	return 0;
}

void RPlayer::ReadAndDecode(){
	loop = true;
	while(loop){
		unsigned int size = buffer->GetSpareSize();
		if (status == PLAY && (size = buffer->GetSpareSize()) * 10 > buffer->GetMaxSize()){
			int mc = decoder.ReadAndDecode(NULL, size, NULL);		
			if (mc != DECODER_OK){
				if (mc == DECODER_DONE && buffer->GetActualSize() <= 0){
					status = STOP;
					NotifyUI(NOTIFY_SONG_FINISHED);
					return;
				}else if (mc == ERR_GENERIC)
					fprintf(stderr,"err\n");
			}
		}
			//WaitForSingleObject(hEvent, 100);
		Sleep(5);
	}
}
void RPlayer::CallBack(void *opaque, Uint8 *stream, int len){
	RPlayer* pThis = (RPlayer*) opaque;
	pThis->FillBuffer(stream, len);
	fprintf(stderr, "%u       \r", RPlayerBuffer::getInstance()->GetActualSize());
}

void RPlayer::FillBuffer(Uint8 *stream, int len){
	int len1;
	if (buffer->GetActualSize() == 0 || status != PLAY){
		SDL_MixAudio(stream, (uint8_t *)silence_buf, sizeof(silence_buf), 128);
		return;
	}
	unsigned char buf[4096];// = new unsigned char [len];
    while (len > 0) {
		if (buffer->GetActualSize() == 0)
			break;
		len1 = buffer->GetBuffer(buf, sizeof(buf));
		SDL_MixAudio(stream, (uint8_t *)buf, len1, volume);
        len -= len1;
        stream += len1;
    }
	if (buffer->GetSpareSize() * 10 > buffer->GetMaxSize()){
		SetEvent(hEvent);
	}
}

double RPlayer::GetPosition(){
	return decoder.GetPosition(buffer->GetActualSize());
}

double RPlayer::GetDuration(){
	return decoder.GetDuration();
}

void RPlayer::Seek(double sec){
	//bool paused = false;
	//if (status == PLAY)
		//Pause();
	//else
		//paused = true;
	ClearCache();
	decoder.Seek(sec);
	SetEvent(hEvent);
	//if (!paused)
		//Resume();
	time = sec;
}
void RPlayer::SetVolume(int vol){
	int temp = int((double) vol / 100 * SDL_MIX_MAXVOLUME);
    if (temp > SDL_MIX_MAXVOLUME)
        volume = SDL_MIX_MAXVOLUME;
    else if (temp < 0)
        volume = 0;
    else volume = temp;
}
int RPlayer::GetVolume(){
	return int((double) volume / 128 * 100);
}

int RPlayer::MIDIPlayer::Play(const char* filename){
	MCIDEVICEID d_id;
	MCI_OPEN_PARMS OpenParms; 
	OpenParms.lpstrDeviceType = (LPCTSTR) MCI_DEVTYPE_SEQUENCER;//MIDI类型 
	OpenParms.lpstrElementName = (LPCTSTR) L"c:\\1.mid"; 
	OpenParms.wDeviceID = 0; 
	mciSendCommand (NULL, MCI_OPEN, 
	MCI_WAIT | MCI_OPEN_TYPE | 
	MCI_OPEN_TYPE_ID | MCI_OPEN_ELEMENT, 
	(DWORD)(LPVOID) &OpenParms) ;
	MCI_PLAY_PARMS PlayParms; 
	d_id = OpenParms.wDeviceID;
PlayParms.dwFrom = 0; 
// 指定从什么地方（时间）播放 
mciSendCommand (d_id, MCI_PLAY, 
MCI_FROM, (DWORD)(LPVOID) 
&PlayParms);
	return DECODER_OK;
}