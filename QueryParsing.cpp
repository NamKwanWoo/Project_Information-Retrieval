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
map<string, int>::iterator inner_Iter;
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

void vectorSpaceModel()
{
	ifstream indexDat, wordDat, docuDat;

	pair<map<int, vector<long double> >::iterator, bool> pr;
	vector<long double> index_inner;
	vector<long double>::iterator veciter;
	map<int, vector<long double> > indexdata;
	map<int, vector<long double> >::iterator index_Iter;

	map<string, vector<int> > Worddata;						/*Index,  [indexNum, StartPosition]*/
	map<string, vector<int> >::iterator word_Iter;
	vector<int> inner;

	string index, conv;
	int startposition, indexNum,DF, count = 1;
	long double doc_ID, weight;

	indexDat.open("Index.txt");
	wordDat.open("Word.txt");
	docuDat.open("doc_dat.txt");

	/* indexdata =  indexNumber, <document_ID, weight, document_ID, weigth...>*/
	/* Worddata =  Index,  [indexNum, StartPosition]*/
	/*
	cout << "Start Parsing!" << endl;
	while (getline(indexDat, str))
	{
		if (str.find("TF") != string::npos)
			continue;

		char *buf = strdup(str.c_str());
		char *token = strtok(buf, "\t");
		count = 1;

		while (token != NULL)
		{
			conv = token;

			if (count == 1)
				indexNum = atoi(conv.c_str());
			else if (count == 2)
				doc_ID = atof(conv.c_str());
			else if (count == 4)
				weight = atof(conv.c_str());

			count++;
			token = strtok(NULL, "\t");
		}
		index_inner.push_back(doc_ID);
		index_inner.push_back(weight);

		cout << indexNum << "  Insert....\t" << doc_ID << " " << weight << endl;

		pr = indexdata.insert(pair<int, vector<long double> >(indexNum, index_inner));
		if (pr.second != true)
		{
			index_Iter = indexdata.find(indexNum);
			index_Iter->second.push_back(doc_ID);
			index_Iter->second.push_back(weight);
		}
	}
	cout << "End Parsing!" << endl;
	*/

	while (getline(wordDat, str))
	{
		if (str.find("Index") != string::npos)
			continue;
		char *buf = strdup(str.c_str());
		char *token = strtok(buf, "\t");
		count = 1;

		while (token != NULL)
		{
			conv = token;

			if (count == 1)
				indexNum = atoi(conv.c_str());
			else if (count == 2)
				index = conv;
			else if (count == 3)
				DF = atoi(conv.c_str());
			else if (count == 5)
				startposition = atoi(conv.c_str());

			count++;
			token = strtok(NULL, "\t");
		}
		inner.push_back(indexNum);
		inner.push_back(DF);
		inner.push_back(startposition);
		
		Worddata.insert(pair <string, vector<int>>(index, inner));
		inner.clear();
	}
	
	string query_Index;
	/* Worddata =  Index,  [indexNum, DF,  StartPosition]*/
	/*Query_Number,   map<string, int>  (term, tf, term, tf .....) */
	for (map_Iter = query_Vector.begin(); map_Iter != query_Vector.end(); ++map_Iter)
	{
		map<int, int> document;
		map<int, int>::iterator docu_Iter;
		int outCount = 1;

		/*Get Similarity Start*/
		cout << "Query #: " << map_Iter->first << endl;

		for (inner_Iter = map_Iter->second.begin(); inner_Iter != map_Iter->second.end(); ++inner_Iter, outCount++)
		{
			query_Index = inner_Iter->first;   /*Query Term*/
			if(outCount == 1)
				continue;

			cout << "Query_Index: " << query_Index << endl;

			word_Iter = Worddata.find(query_Index);
			indexNum = word_Iter->second[0];
			DF = word_Iter->second[1];
			startposition = word_Iter->second[2];
			cout << "Word_Index: " << word_Iter->first << " " << indexNum << " " << DF << " " << startposition << endl;

			long long count = 0;
			while (getline(indexDat, str))
			{
				if (str.find("TF") != string::npos)
					continue;

				if (count == startposition + DF)
					break;
				count++;
			}
			fflush(stdin);

			cout << count << endl;
			cout << endl;
		}
	}

}

int main(void)
{
	//Set_StopWord();
	//QueryParsing();
	Get_QueryVector();
	vectorSpaceModel();

	long long dd = 86403464;
	cout << dd << endl;
	getchar();
 }