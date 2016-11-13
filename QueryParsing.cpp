#pragma warning(disable:4996)

#include <iostream>
#include <fstream>
#include <set>
#include "porter2_stemmer.h"

using namespace std;

set<string> stop_Word;
set<string>::iterator iter;
string str, convert_Token;

/*Setting Stop Word Library*/
void Set_StopWord()
{
	ifstream stopWord_File;

	stopWord_File.open("StopWord.txt");

	/*Parsing StopWord and insert in Set Library*/
	while (getline(stopWord_File, str))
	{
		stop_Word.insert(str);
		stop_Word.insert(str + " ");
	}
	stopWord_File.close();
}

void QueryParsing()
{
	ifstream queryDOC;
	ofstream parsedTopic;
	int descPart = 0, narrPart=0, count = 0;

	queryDOC.open("topics25.txt");
	parsedTopic.open("parsed_Topic25.txt");

	while (getline(queryDOC, str))
	{
		if (str.find("<top>") != string::npos)
		{
			if (count)
			{
				parsedTopic << "===========END===========\n\n\n";
				parsedTopic << "===========Start===========";
			}
			else
			{
				parsedTopic << "===========Start===========";
				count = 1;
			}
			descPart = 0; narrPart = 0;
			continue;
		}

		else if (str.find("<num>") != string::npos)
		{
			char *buf = strdup(str.c_str());
			char *token = strtok(buf, " ");

			while (token != NULL)
			{
				convert_Token = token;
				if (convert_Token.compare("<num>") == 0 || convert_Token.compare("Number:") == 0)
				{
					token = strtok(NULL, " ");
					continue;
				}
				else
				{
					parsedTopic << convert_Token;
					parsedTopic << " ";
				}
				token = strtok(NULL, " ");
			}
		}

		else if (str.find("<title>") != string::npos)
		{
			char *buf = strdup(str.c_str());
			char *token = strtok(buf, " ?-");

			while (token != NULL)
			{
				strlwr(token);
				convert_Token = token;

				iter = stop_Word.find(convert_Token);

				if (iter != stop_Word.end() || convert_Token.compare("<title>") == 0)
				{
					token = strtok(NULL, " ?-");
					continue;
				}
				
					Porter2Stemmer::stem(convert_Token);
					parsedTopic << convert_Token;
					parsedTopic << " ";
				
				token = strtok(NULL, " ?-");
			}
			continue;
		}

		else if (str.find("<desc>") != string::npos)
			descPart = 1;

		else if (str.find("<narr>") != string::npos)
		{
			descPart = 0;
			narrPart = 1;
		}

		else if (descPart || narrPart)
		{
			char *buf = strdup(str.c_str());
			char *token = strtok(buf, " ,.?!()-");

			while (token != NULL)
			{
				strlwr(token);
				convert_Token = token;

				iter = stop_Word.find(convert_Token);

				if (iter != stop_Word.end() || !isalpha(convert_Token[0]))
				{
					token = strtok(NULL, " ,.?!()-");
					continue;
				}

				Porter2Stemmer::stem(convert_Token);
				parsedTopic << convert_Token;
				parsedTopic << " ";
				token = strtok(NULL, " ,.?!()-");
			}
		}

		else if (str.find("</top>") != string::npos)
			parsedTopic << "End";

		parsedTopic << "\n";
	}
	parsedTopic << "===========END===========";
}

int main(void)
{
	Set_StopWord();
	QueryParsing();
}