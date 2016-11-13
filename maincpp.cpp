#pragma warning(disable:4996)

#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include "porter2_stemmer.h"
using namespace std;
set<string> stop_Word;
set<string>::iterator iter;
map<int, map<string, int> > query_Vector;
map<int, map<string, int> >::iterator map_Iter;

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
		if (str.find("top") != string::npos)
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
					parsedTopic << "\n";
				
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
			char *token = strtok(buf, " ,.?!()-\"/;:");

			while (token != NULL)
			{
				strlwr(token);
				convert_Token = token;

				iter = stop_Word.find(convert_Token);

				if (iter != stop_Word.end() || !isalpha(convert_Token[0]))
				{
					token = strtok(NULL, " ,.?!()-/;:\"");
					continue;
				}

				Porter2Stemmer::stem(convert_Token);
				parsedTopic << convert_Token;
				parsedTopic << "\n";
				token = strtok(NULL, " ,.?!()-/;:\"");
			}
		}

		else if (str.find("</top>") != string::npos)
			parsedTopic << "End";

		parsedTopic << "\n";
	}
	parsedTopic << "===========END===========";
}

void Get_QueryVector()
{
	ifstream queryFile;
	ofstream queryVectorFile;
	queryFile.open("parsed_Topic25.txt");
	queryVectorFile.open("query_Vector.txt");

	int parsing = 0, numeric = 0;
	int queryNum;
	map<string, int> doc_By;
	map<string, int>::iterator string_int_iter;
	pair<map<string, int>::iterator, bool > pr;

	while (getline(queryFile, str))
	{
		if (str.find("==Start==") != string::npos)
		{
			numeric = 1;
			continue;
		}
		else if (str.find("==END==") != string::npos)
		{
			parsing = 0;
			if(queryNum)
				query_Vector.insert(pair<int, map<string, int> >(queryNum, doc_By));
			doc_By.clear();
		}
		else if (numeric)
		{
			queryNum = atoi(str.c_str());
			parsing = 1;  numeric = 0;
			continue;
		}
		else if (parsing)
		{
			pr = doc_By.insert(pair<string, int>(str, 1));
			if(pr.second != true)
			{
				string_int_iter = doc_By.find(str);
				(string_int_iter->second)++;
			}
		}	
	}

	for (map_Iter = query_Vector.begin(); map_Iter != query_Vector.end(); ++map_Iter)
	{
		queryVectorFile << map_Iter->first;
		queryVectorFile << endl;

		int count = 0;
		map_Iter->second.erase("top>");

		for (string_int_iter = map_Iter->second.begin(); string_int_iter != map_Iter->second.end(); ++string_int_iter, count ++)
		{
			if (!count)
				continue;
			queryVectorFile << string_int_iter->first << "\t" << string_int_iter->second << endl;
		}
		queryVectorFile << endl;
	}
}

int main(void)
{
	cout << "\"" << endl;
	Set_StopWord();
	QueryParsing();
	Get_QueryVector();
	getchar();
}