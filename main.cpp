#pragma warning(disable:4996)

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <set>
#include <time.h>
#include <cctype>
#include "porter2_stemmer.h"
#include "Frequency.h"
#include "Document_INFO.h"

using namespace std;
long doc_count = 1;

namespace CHECK
{
	enum
	{
		END, START, LOWERCASE
	};
}

set<string> stop_Word;		//Global var

void Set_StopWord()
{
	string str;
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

class File
{
private:
	ifstream readFile;
	ofstream writeFile;
	string str;

public:
	File(string readFile_Str, string writeFile_Str)
	{
		readFile.open(readFile_Str);
		//writeFile.open(writeFile_Str);

		if (readFile.fail() || writeFile.fail())
			return;
	}

	inline void Process(TF *termFrequency, DF *document_Frequency, CF *collection_Frequency, Document_Info *document_Info, int What)
	{
		int indexingLine = CHECK::END;
		int doc_Length = 0;
		int to_Get_Doc_Name = CHECK::END;
		string document_Name;

		while (getline(readFile, str))
		{
			if (str.compare("[TEXT] : ") == 0)
			{
				indexingLine = CHECK::START;
				to_Get_Doc_Name = CHECK::END;
			}

			if (indexingLine = CHECK::START)
			{
				char *buf = strdup(str.c_str());
				char *token = strtok(buf, " :");

				while (token != NULL)
				{
					string convert_token = token;

					if (convert_token.find("[HEADLINE]") != string::npos || convert_token.find("[TEXT]") != string::npos)
						to_Get_Doc_Name = CHECK::END;

					if (to_Get_Doc_Name == CHECK::START) { document_Name += convert_token; }

					/*Starting Indexing  --->   TF, DF, CF  */		/*[Text] Part*/
					if (indexingLine == CHECK::START && to_Get_Doc_Name == CHECK::END)
					{
						document_Frequency->Set_DocumentFrequency(convert_token);		/*DF*/
						termFrequency->Set_TermFrequency(convert_token, doc_count);		/*TF*/
						collection_Frequency->Set_CollectionFrequency(convert_token);		/*CF*/
						doc_Length++;   /*Word Number after working stop word*/
					}

					if (convert_token.find("[DOCNO]") != string::npos)
						to_Get_Doc_Name = CHECK::START;

					token = strtok(NULL, " :");
				}
			}

			if (str.find("[HEADLINE] :") != string::npos)
				to_Get_Doc_Name = CHECK::END;

			/* doc_count == id(document_Name) */
			if (str.find("[DOCNO] :") != string::npos)
			{
				indexingLine = CHECK::END;
				to_Get_Doc_Name = CHECK::START;

				document_Info->Set_Document_Info(document_Name, doc_Length, doc_count);	/*Document*/
				document_Frequency->Merge_DocumentFrequency();

				/*doc_Count = document ID*/
				if(What == 0)
					termFrequency->Get_TermFrequency_APW();		/*Total TF*/
				if(What == 1)
					termFrequency->Get_TermFrequency_NYT();
				
				++doc_count;

				//cout << "Document\t " + document_Name + "\t\t processing" << endl;
				document_Name.clear();
				doc_Length = 0;		/*Doc_File*/   /*After Eliminate StopWord*/
			}
		}
	}

	inline void Parsing(string &cur_Document)
	{
		clock_t begin, end;
		begin = clock();

		/* Line & Lowercase & StemmingCase*/
		int lineCheck = CHECK::END;
		int lowercase = CHECK::END;
		int stemmingCase = CHECK::END;
		int indexingLine = CHECK::END;

		string document_Name;

		/* Line by Line */
		while (getline(readFile, str))
		{
			/* Start Parsing */
			if (str.find("<DOCNO>") != string::npos || str.find("<HEADLINE>") != string::npos || str.find("<TEXT>") != string::npos)
				lineCheck = CHECK::START;

			/*Stemming = <HEADLINE>, <TEXT>*/
			if (str.find("<HEDLINE>") != string::npos || str.find("<TEXT>") != string::npos)
				stemmingCase = CHECK::START;

			/* Start converting string to lowerCase */
			if (str.find("</TEXT>") != string::npos || str.find("</HEADLINE>") != string::npos)
				lowercase = CHECK::END;

			/* Unnecessary Line */
			if (str.compare("<P>") == 0 || str.compare("</P>") == 0 || !lineCheck)
				continue;

			/* Necessary Line */
			else
			{
				set<string>::iterator iter;
				char *buf = strdup(str.c_str());
				char *token = strtok(buf, " ,.`-\t?;&'()");

				/* Start Tokenizing */
				while (token != NULL)
				{
					string convert_token = token;

					/* Converting Tolowercase */
					if (lowercase == CHECK::LOWERCASE)
					{
						strlwr(token);
						convert_token = token;
					}

					iter = stop_Word.find(convert_token);

					/* If token is not stopword */
					if (iter == stop_Word.end())
					{
						/*Converting contents from < > to [ ] : */
						if (convert_token.compare("<DOCNO>") == 0 || convert_token.find("<HEADLINE>") != string::npos || convert_token.find("<TEXT>") != string::npos || convert_token.find("<text>") != string::npos)
						{
							convert_token[0] = '[';
							convert_token[strlen(token) - 1] = ']';
							convert_token += " : ";
						}

						/*Porter2 Stemming*/
						if (stemmingCase == CHECK::START) { Porter2Stemmer::stem(convert_token); }

						/*Writing string to Final File after stopwording & porter2 stemming */
						writeFile << convert_token;
						writeFile << " ";

						if (convert_token.compare("[TEXT] : ") == 0)
							indexingLine = CHECK::START;
					}
					token = strtok(NULL, " ,.`-\t?;&'()");
				}
				writeFile << "\n";

				/*End converting string to lowerCase*/
				if (str.find("<TEXT>") != string::npos || str.find("<HEADLINE>") != string::npos)
					lowercase = CHECK::LOWERCASE;
			}

			/*End Parsing*/
			if (str.find("</DOCNO>") != string::npos || str.find("</HEADLINE>") != string::npos || str.find("</TEXT>") != string::npos)
				lineCheck = CHECK::END;

			/*End Stemming*/
			if (str.find("</TEXT>") != string::npos)
			{
				stemmingCase = CHECK::END;
				indexingLine = CHECK::END;
			}
		}

		/*Close output!!!  Necessary!!*/
		writeFile.close();

		end = clock();
		cout << cur_Document << "\tParsing Time:  " << ((end - begin) / (CLOCKS_PER_SEC)) << "sec" << endl;
	}
};

int main(void)
{
	int year;
	clock_t begin, end;
	begin = clock();

	Set_StopWord();
	TF *termFrequency = new TF();
	DF *document_Frequency = new DF();
	CF *collection_Frequency = new CF();

	Document_Info *document_Info = new Document_Info();
	Word_Data *wordData_Info = new Word_Data();

	File *file;
	int count = 1;
	termFrequency->Set_Document();

	for (year = 6; year <= 12; year++)
	{
		for (int month = 1; month <= 30; month++)
		{
			string file_Name = "1998";
			string revised = "[STEMMED]__";

			if (year < 10)
				file_Name += "0" + to_string(year);
			else
				file_Name += to_string(year);

			if (month< 10)
				file_Name += "0" + to_string(month);
			else
				file_Name += to_string(month);

			file_Name += "_APW_ENG";

			cout << file_Name + " 파일에 대해 색인어를 추출하고 있습니다." << endl;


			file = new File(revised + file_Name, revised + file_Name);
			//file->Parsing(file_Name);
			file->Process(termFrequency , document_Frequency, collection_Frequency, document_Info, 0);
			delete file;
			count++;
		}
	}

	for (year = 1; year <= 12; year++)
	{
		for (int month = 1; month <= 30; month++)
		{
			string file_Name = "1999";
			string revised = "[STEMMED]__";

			if (year < 10)
				file_Name += "0" + to_string(year);
			else
				file_Name += to_string(year);

			if (month< 10)
				file_Name += "0" + to_string(month);
			else
				file_Name += to_string(month);

			file_Name += "_APW_ENG";

			cout << file_Name + " 파일에 대해 색인어를 추출하고 있습니다." << endl;

			file = new File(revised + file_Name, revised + file_Name);
			//file->Parsing(file_Name);
			file->Process(termFrequency, document_Frequency, collection_Frequency, document_Info, 0);

			delete file;
			count++;
		}
	}

	for (year = 1; year <= 9; year++)
	{
		for (int month = 1; month <= 30; month++)
		{
			string file_Name = "2000";
			string revised = "[STEMMED]__";

			if (year < 10)
				file_Name += "0" + to_string(year);
			else
				file_Name += to_string(year);

			if (month< 10)
				file_Name += "0" + to_string(month);
			else
				file_Name += to_string(month);

			file_Name += "_APW_ENG";

			cout << file_Name + " 파일에 대해 색인어를 추출하고 있습니다." << endl;

			file = new File(revised + file_Name, revised + file_Name);
			//file->Parsing(file_Name);
			file->Process(termFrequency, document_Frequency, collection_Frequency, document_Info, 0);
			delete file;
			count++;
		}
	}

	for (year = 6; year <= 12; year++)
	{
		for (int month = 1; month <= 30; month++)
		{
			string file_Name = "1998";
			string revised = "[STEMMED]__";

			if (year < 10)
				file_Name += "0" + to_string(year);
			else
				file_Name += to_string(year);

			if (month< 10)
				file_Name += "0" + to_string(month);
			else
				file_Name += to_string(month);

			file_Name += "_NYT";

			cout << file_Name + " 파일에 대해 색인어를 추출하고 있습니다." << endl;

			file = new File(revised + file_Name, revised + file_Name);
			//file->Parsing(file_Name);
			file->Process(termFrequency, document_Frequency, collection_Frequency, document_Info, 1);

			delete file;
			count++;
		}
	}

	for (year = 1; year <= 12; year++)
	{
		for (int month = 1; month <= 30; month++)
		{
			string file_Name = "1999";
			string revised = "[STEMMED]__";

			if (year < 10)
				file_Name += "0" + to_string(year);
			else
				file_Name += to_string(year);

			if (month< 10)
				file_Name += "0" + to_string(month);
			else
				file_Name += to_string(month);

			file_Name += "_NYT";

			cout << file_Name + " 파일에 대해 색인어를 추출하고 있습니다." << endl;

			file = new File(revised + file_Name, revised + file_Name);
			//file->Parsing(file_Name);
			file->Process(termFrequency, document_Frequency, collection_Frequency, document_Info, 1);

			delete file;
			count++;
		}
	}

	for (year = 1; year <= 9; year++)
	{
		for (int month = 1; month <= 30; month++)
		{
			string file_Name = "2000";
			string revised = "[STEMMED]__";

			if (year < 10)
				file_Name += "0" + to_string(year);
			else
				file_Name += to_string(year);

			if (month< 10)
				file_Name += "0" + to_string(month);
			else
				file_Name += to_string(month);

			file_Name += "_NYT";

			cout << file_Name + " 파일에 대해 색인어를 추출하고 있습니다." << endl;

			file = new File(revised + file_Name, revised + file_Name);
			//file->Parsing(file_Name);
			file->Process(termFrequency, document_Frequency, collection_Frequency, document_Info, 1);

			delete file;
			count++;
		}
	}

	termFrequency->Merge_APW_NYT();

	document_Frequency->Get_DocumentFrequency();		/*Making DF.txt File*/
	collection_Frequency->Get_Collection();					/*Making CF.txt File*/

	wordData_Info->Set_WordData(*document_Frequency, *collection_Frequency);
	cout << "색인 중입니다….단어 정보 파일 word.dat 를 생성했습니다." << endl;

	document_Info->Get_Document_Info();
	cout << "문서 파일 doc.dat를 생성했습니다." << endl;

	termFrequency->CheckTF(*wordData_Info);
	cout << "역색인 파일 index.dat를 생성했습니다." << endl;


	end = clock();
	cout << count - 1 << "개 파일 전체 Time:  " << ((end - begin) / (CLOCKS_PER_SEC)) << "sec" << endl;

	getchar();
	return 0;
}