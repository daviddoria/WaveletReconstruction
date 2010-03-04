/** \file
\brief Parsing functions
\author Josiah Manson

Modified: June 26, 2006
I'm so confused about what version is the newest. some consolidation taken place

Modified: June 20, 2005
Added the functions readIntInline

Modified: July 28, 2005
Some stuff. Use it.
**/


#pragma once
//#pragma warning (disable : 4786)

#ifndef _parse_h
#define _parse_h

#ifndef WIN32
#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long
#endif

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

// general parsing stuff
string implode(vector<string>& wordlist, string sep);

void parseString(vector<string> *commList, const string& input, string sep);
void parseQuotedString(vector<string> *commList, const string& input, string sep);
void parseQuotedStringInclusive(vector<string> *commList, const string& input, string sep);

void trimWhite(string *str);
void trimWhite(vector<string> *commList);
void removeEmptyStrings(vector<string> *commList);

string makeLowercase(string input);
string makeUppercase(string input);
string makeCapitalized(string input);

string escapeString(const string& input);

/////// toStr
template<class type> inline string toStr(const type &value)
{
    ostringstream streamOut;
	streamOut.flags(ios::fixed);
    streamOut << value;
    return streamOut.str();
}

///// fromStr
void fromStr(string& str, float& v);
void fromStr(string& str, int& v);
void fromStr(string& str, bool& v);

///// path parsing
string getFileBase(const string& filename);
string getFileExtension(const string& filename);

string getFilePath(const string& filename);
string getFileName(const string& filename);

///// file stuff
int loadCommentedFileToString(const string& filename, string *str);
int loadFileToString(const string& filename, string *str);

void writeInline(ostream& f, const string& s);
void readInline(istream& f, string& s);

template <class T> void writeInline(ostream& f, const T& v)
{
	f.write((char*)&v, sizeof(T));
}

template <class T> void writeInline(ostream& f, const T *v, __int32 num)
{
	f.write((char*)v, sizeof(T)*num);
}

template <class T> void writeInline(ostream& f, const vector<T>& v)
{
	writeInline(f, v.size());
	writeInline(f, &v[0], v.size());
}

template <class T> void readInline(istream& f, T& v)
{
	f.read((char*)&v, sizeof(T));
}

template <class T> void readInline(istream& f, T *v, volatile __int32 num)
{
	f.read((char*)v, sizeof(T)*num);
}

template <class T> void readInline(istream& f, vector<T>& v)
{
	volatile __int32 s;
	readInline(f, s);
	v.resize(s);
	readInline(f, &v[0], s);
}


#endif
