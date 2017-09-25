#include "TagSystem.h"
#include <string>
#include <sstream>
#include <cstdio>
#include "stdafx.h"
#define TAGLIB_STATIC 
#include "3rdparty\tag\include\fileref.h"
#include "3rdparty\tag\include\tag.h"
#pragma comment(lib, "lib/tag.lib")

bool TagSystem::GetTag(const char* filename, TAG* tag){
	tag->valid = true;
	TagLib::FileRef f(filename);

	if(!f.isNull() && f.tag()) {
		TagLib::Tag *t = f.tag();
		if (t->artist().isAscii()){
			fprintf(stderr, "artist is ascii\n");
			strcpy_s(tag->artist, t->artist().toCString());
		}else if (t->artist().isLatin1()){
			fprintf(stderr, "artist is latin1\n");
			strcpy_s(tag->artist, t->artist().to8Bit().c_str());
		}else{
			wcs2mbs(t->artist().toWString().c_str(), tag->artist, sizeof(tag->artist));
		}

		if (t->title().isAscii()){
			fprintf(stderr, "title is ascii\n");
			strcpy_s(tag->title, t->title().toCString());
		}else if (t->title().isLatin1()){
			fprintf(stderr, "title is latin1\n");
			strcpy_s(tag->title, t->title().to8Bit().c_str());
		}else{
			wcs2mbs(t->title().toWString().c_str(), tag->title, sizeof(tag->title));
		}
	}else{
		tag->valid = false;
		return false;
	}
	return true;
}

bool TagSystem::GetTagW(const char* filename, TAGW* tag){
	tag->valid = true;
	TagLib::FileRef f(filename);
	wchar_t temp[100];
	if(!f.isNull() && f.tag()) {
		TagLib::Tag *t = f.tag();
		if (t->artist().isAscii()){
			//fprintf(stderr, "artist is ascii\n");
			mbs2wcs(t->artist().toCString(), temp, sizeof(temp));
			tag->artist = temp;
			//fwprintf(stderr, L"%s - ", temp);
		}else if (t->artist().isLatin1()){
			//fprintf(stderr, "artist is latin1\n");
			mbs2wcs(t->artist().to8Bit().c_str(), temp, sizeof(temp));
			tag->artist = temp;
		}else{
			tag->artist = t->artist().toWString();
		}

		if (t->title().isAscii()){
			//fprintf(stderr, "title is ascii\n");
			mbs2wcs(t->title().toCString(), temp, sizeof(temp));
			tag->title = temp;
		}else if (t->title().isLatin1()){
			//fprintf(stderr, "title is latin1\n");
			mbs2wcs(t->title().to8Bit().c_str(), temp, sizeof(temp));
			tag->title = temp;
		}else{
			tag->title = t->title().toWString();
		}
	}else{
		tag->valid = false;
		return false;
	}
	return true;
}