#pragma once
#include <string>
using namespace std;

struct TAG
{
	bool valid;      
	char title[50];      
	char artist[30];  
	char album[30]; 
	char year[4];  
	char comment[30]; 
	char track[2];
	char genre[15]; 
};

struct TAGW{
	bool valid;      
	wstring title;      
	wstring artist;  
	wstring album; 
	wstring year;  
	wstring comment; 
	wstring track;
	wstring genre; 
};

class TagSystem{
public:
	bool GetTag(const char* filename, TAG* tag);
	bool GetTagW(const char* filename, TAGW* tag);
};