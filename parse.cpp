/** \file
\brief Parsing functions
\author Josiah Manson

**/

#include "parse.h"

#include <fstream>
#include <algorithm>

using namespace std;

typedef unsigned int uint;

///// fromStr
void fromStr(string& str, float& v)
{
	v = (float)atof(str.c_str());
}

void fromStr(string& str, int& v)
{
	v = atoi(str.c_str());
}

void fromStr(string& str, bool& v)
{
	if (str == "true" || str == "yes")
		v = true;
	else if (str == "false" || str == "no")
		v = false;
	else
		v = atoi(str.c_str()) == 0 ? false : true;
}

///// file parsing stuff
string implode(vector<string>& l, string sep)
{
	string ret;

	for (unsigned int i = 0; i < l.size(); i++)
	{
		ret += l[i];
		if (i  < l.size() - 1)
			ret += sep;
	}

	return ret;
}

void parseString(vector<string> *commList, const string& input, string sep)
{
	string comm;

	for (unsigned int i = 0; i < input.size(); i++)
	{
		const char ch = input[i];

		bool added = false;

		for (unsigned int s = 0; s < sep.size(); s++)
		{
			if (ch == sep[s])
			{
				commList->push_back(comm);
				comm = "";
				added = true;
				break;
			}
		}

		if (!added)
		{
			comm += ch;
		}
	}
	
	if (comm != "")
		commList->push_back(comm);
}

void parseQuotedString(vector<string> *commList, const string& input, string sep)
{
	string comm;
	int quotes = 0;

	for (unsigned int i = 0; i < input.size(); i++)
	{
		const char ch = input[i];

		if (ch == '\"')
			quotes++;

		if (ch != '\"')
		{
			// not in quotes
			if ((quotes & 1) == 0)
			{
				bool added = false;

				for (unsigned int s = 0; s < sep.size(); s++)
				{
					if (ch == sep[s])
					{
						commList->push_back(comm);
						comm = "";
						added = true;
						break;
					}
				}

				if (!added)
				{
					comm += ch;
				}
			}
			// in quotes
			else
			{
				comm += ch;
			}
		}
	}
	
	if (comm != "")
		commList->push_back(comm);
}

void parseQuotedStringInclusive(vector<string> *commList, const string& input, string sep)
{
	string comm;
	int quotes = 0;

	for (unsigned int i = 0; i < input.size(); i++)
	{
		const char ch = input[i];

		if (ch == '\"')
			quotes++;

		// not in quotes
		if ((quotes & 1) == 0)
		{
			bool added = false;

			for (unsigned int s = 0; s < sep.size(); s++)
			{
				if (ch == sep[s])
				{
					commList->push_back(comm);
					comm = "";
					added = true;
					break;
				}
			}

			if (!added)
			{
				comm += ch;
			}
		}
		// in quotes
		else
		{
			comm += ch;
		}
	}
	
	if (comm != "")
		commList->push_back(comm);
}


void trimWhite(string *str)
{
	int first = str->find_first_not_of(" \n\t\v\r\f");
	int last = str->find_last_not_of(" \n\t\v\r\f");
		
	if (first == -1 && last == -1)
		*str = "";
	else
		*str = str->substr(first, (last - first) + 1);
}

void trimWhite(vector<string> *commList)
{
	vector<string>::iterator iter;

	int first, last;
	for (iter = commList->begin(); iter != commList->end(); ++iter){
		first = iter->find_first_not_of(" \n\t\v\r\f");
		last = iter->find_last_not_of(" \n\t\v\r\f");
			
		if (first == -1 && last == -1)
			*iter = "";
		else
			*iter = iter->substr(first, (last - first) + 1);
	}
}

struct isEmpty
{
	bool operator()(const string& s) const {return s.empty();}
};

void removeEmptyStrings(vector<string> *commList)
{
	/*vector<string>::iterator iter;

	for (iter = commList->begin(); iter != commList->end() && !commList->empty(); ++iter){
		if (iter->empty()){
			iter = commList->erase(iter);
			if (!commList->empty())
				--iter;
		}
	}*/

	commList->erase(remove_if(commList->begin(), commList->end(), isEmpty()), commList->end());
}


int loadCommentedFileToString(const string& file, string *input)
{
	ifstream in;
	in.open(file.c_str(), ios::in | ios::binary);

	if (in.fail())
		return 0;

	*input = "";

	char next;
	char ch;
	bool comment = false;

	while (1)
	{
		in.get(ch);
		
		// check for eof
		if (in.eof())
		{
			in.close();
			return 1;
		}

		next = in.peek();

		if (ch == '/' && next == '/')
			comment = true;
		if (ch == '\n')
			comment = false;

		if (ch != '\r' && !comment)
			*input += ch;
	}

	return 1;
}

string makeLowercase(string input)
{
	int max = input.size();
	int caseOff = 'a' - 'A';

	for (int i = 0; i < max; i++){
		if (input[i] >= 'A' && input[i] <= 'Z')
			input[i] += caseOff;
	}

	return input;
}

string makeUppercase(string input)
{
	int max = input.size();
	int caseOff = 'a' - 'A';

	for (int i = 0; i < max; i++){
		if (input[i] >= 'a' && input[i] <= 'z')
			input[i] -= caseOff;
	}

	return input;
}

string makeCapitalized(string input)
{
	int max = input.size();
	int caseOff = 'a' - 'A';

	bool white = true;
	for (int i = 0; i < max; i++){
		if (white && input[i] >= 'a' && input[i] <= 'z')
			input[i] -= caseOff;
		else if (!white && input[i] >= 'A' && input[i] <= 'Z')
			input[i] += caseOff;

		if (input[i] == ' ')
			white = true;
		else
			white = false;
	}

	return input;
}

void writeStringInline(ofstream& f, const string& s)
{
	volatile __int32 num = s.size();
	writeInline(f, num);
	f.write((char*)s.c_str(), num);
}

void readStringInline(ifstream& f, string& s)
{
	volatile __int32 num;
	readInline(f, num);
	
	char *ptr = new char [num + 1];
	ptr[num] = 0;
	
	f.read(ptr, num);
	s = ptr;
	
	delete [] ptr;
}

int readIntInline(ifstream& f)
{
	volatile int v;
	f.read((char*)&v, sizeof(int));
	return v;
}

string getFileBase(const string& filename)
{
	string name;

	int begin = filename.find_last_of("/\\");
	if (begin == string::npos)
		begin = 0;
	else
		begin++;

	
	int end;
	if ( (end = filename.find_last_of(".")) == string::npos )
		name = filename.substr(begin);
	else
		name = filename.substr(begin, end - begin);

	return name;
}

string getFileExtension(const string& filename)
{
	if (filename.size() <= 3)
		return "";

	int x = filename.find_last_of(".");
	if (x == string::npos)
		return "";

	string ext = filename.substr(x + 1);

	// change case
	for (uint i = 0; i < ext.size(); i++)
	{
		if (ext[i] >= 'A' && ext[i] <= 'Z')
			ext[i] += 'a' - 'A';
	}

	return ext;
}


string getFilePath(const string& filename)
{
	int end;
	if ( (end = filename.find_last_of("/\\")) == string::npos )
		return string();

	return filename.substr(0, end);
}

string getFileName(const string& f)
{
	return getFileBase(f) + "." + getFileExtension(f);
}


string escapeString(const string& str)
{
	string esc;
	esc.reserve(str.size());

	for (uint i = 0; i < str.size(); i++)
	{
		switch (str[i])
		{
		case '\\':
			esc += "\\\\";
			break;
		case '\"':
			esc += "\\\"";
			break;
		default:
			esc += str[i];
		}
	}

	return esc;
}

#ifdef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#endif

int loadFileToString(const string& file, string *input)
{
#ifdef WIN32
	// reserve enough space
	struct _stat stats;
	_stat(file.c_str(), &stats);
	int len = stats.st_size;

	// read that shit in
	ifstream in;
	in.open(file.c_str(), ios::in | ios::binary);
	if (in.fail())
		return 0;

	input->resize(len);
	in.read(&((*input)[0]), len);

	in.close();
#else
	// this is the slow and shitty method
	ifstream in;
	in.open(file.c_str(), ios::in | ios::binary);
	if (in.fail())
		return 0;

	char buff;
	while (1)
	{
		in.get(buff);
		
		// check for error (including eof)
		if (in.eof())
		{
			in.close();
			return 1;
		}

		if (buff != '\r')
			*input += buff;
	}

	in.close();
#endif

	return 1;
}

void writeInline(ostream& f, const string& s)
{
	volatile __int32 num = s.size();
	writeInline(f, num);
	f.write((char*)s.c_str(), num);
}

void readInline(istream& f, string& s)
{
	volatile __int32 num;
	readInline(f, num);
	
	char *ptr = new char [num + 1];
	ptr[num] = 0;
	
	f.read(ptr, num);
	s = ptr;
	
	delete [] ptr;
}
