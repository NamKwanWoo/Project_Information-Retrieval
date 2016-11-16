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
	queryDOC.close();
	parsedTopic.close();
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
	queryFile.close();
	queryVectorFile.close();
}

std::fstream& GotoLine(std::fstream& file, unsigned int num) {
	file.seekg(std::ios::beg);
	for (int i = 0; i < num - 1; ++i) {
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	return file;
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

	string query_Index;
	string index, conv, inconv;
	int startposition, indexNum,DF, count = 1, query_TF;
	long double doc_ID, weight;

	wordDat.open("Word.txt");
	docuDat.open("doc_dat.txt");

	ofstream dummy;
	dummy.open("dummy.txt");

	/* indexdata =  indexNumber, <document_ID, weight, document_ID, weigth...>*/
	/* Worddata =  Index,  [indexNum, StartPosition]*/
	
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
	
	map<long,  vector< long double> > document;								/*Docu_ID,   (weight1, weight2, weight3....)*/
	map<long, vector< long double> >::iterator docu_Iter;
	pair < map<long, vector< long double> >::iterator, bool> expression;
	vector<long double> vector_ID;
	vector<long double>::iterator vec_Iter;
	
	/* Worddata =  Index,  [indexNum, DF,  StartPosition]*/
	/*Query_Number,   map<string, int>  (term, tf, term, tf .....) */
	for (map_Iter = query_Vector.begin(); map_Iter != query_Vector.end(); ++map_Iter)
	{
		document.clear();
		vector_ID.clear();

		int outCount = 1;
		int num_of_Query_term = 0;
		/*Get Similarity Start*/
		cout << "Query #: " << map_Iter->first << endl;
		indexDat.open("Index.txt");

		/* Index in query by index,   key = query term,  value = query term frequency*/
		for (inner_Iter = map_Iter->second.begin(); inner_Iter != map_Iter->second.end(); ++inner_Iter, outCount++)
		{
			query_Index = inner_Iter->first;					/*Query Index*/
			query_TF = inner_Iter->second;					/*Query Term Frequency*/

			if(outCount == 1)			
				continue;
			
			cout << "Query_Index: " << query_Index << "\tQuery_TF: " << query_TF << endl;

			word_Iter = Worddata.find(query_Index);

			indexNum = word_Iter->second[0];
			DF = word_Iter->second[1];
			startposition = word_Iter->second[2];
			
			/*MAP document:    ==>       Term_ID,   (Docu_ID, weight), Docu_ID, weight, Docu_ID, weight....)*/
			/*query_Index ==  current Term*/

			while (getline(indexDat, str))
			{
				char *buf = strdup(str.c_str());
				char *token = strtok(buf, "\t");				/*Index ID*/
				long  docu_ID;
				long double weight;

				conv = token;											/*Index ID*/

				if (atoi(conv.c_str()) > indexNum)				/*End position*/
					break;
				if (atoi(conv.c_str()) == indexNum)			/*<--   Start Position  by index_ID*/
				{
					token = strtok(NULL, "\t");					/*Document ID*/
					inconv = token;							/*Document ID*/
					docu_ID = atoi(inconv.c_str());				/*Document ID*/
						
					token = strtok(NULL, "\t");					/*TF */				
					token = strtok(NULL, "\t");					/*Weight*/
					inconv = token;									/*Weight*/			
					weight = atof(inconv.c_str());				/*Weight*/

					vector_ID.push_back(weight);
					expression = document.insert(pair<long, vector<long double> >(docu_ID, vector_ID));			/*Insert map = Document_ID, <weight1, weigth2>*/

					if (expression.second != true)
					{
						docu_Iter = document.find(docu_ID);
						docu_Iter->second.push_back(weight);
					}

					conv.clear();
					vector_ID.clear();
				}
			}
			num_of_Query_term++;
		}

		indexDat.close();
		cout << "\nNumber of Query:  " << num_of_Query_term << "\n\n";
		
		/*=============================================Attention=============================================*/
		/*		map<int, vector<long double> > document;			--->		Document_ID,   (Term_ID, weight, Term_ID, weight, Term_ID, weight....)*/
		/*     Key = Document_ID (int)  ////////    Value = <vector> -->  <Term_ID, Weight, Term_ID, Weight, Term_ID, Weight...>*/
		/*     	query_Index = inner_Iter->first;   Query Term*/
	}
}

void BM25()
{

}

int main(void)
{
	//Set_StopWord();
	//QueryParsing();
	Get_QueryVector();
	vectorSpaceModel();


	getchar();
 }