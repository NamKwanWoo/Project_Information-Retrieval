#pragma warning(disable:4996)

#include <iostream>
#include <fstream>
#include <time.h>
#include <map>
#include <set>
#include "porter2_stemmer.h"

using namespace std;
int doc_count = 0;

ofstream df_File, tf_File, cf_File;
set<string> stop_Word;		//Global var

map<string, int> temp, df, cf;
map<string, int>::iterator iter_string_int, temp_Iter;
map<string, vector<int>> dotDat, doc_info, tf, IndexDat, NYT_Dat;
map<string, vector<int>>::iterator iter;
map<string, vector<int>>::iterator index_Iter;
vector<int>::iterator vec_Iter;

namespace CHECK
{
	enum
	{
		END, START, LOWERCASE
	};
}

/*Setting TF.txt*/
void Set_Document()
{
	tf_File.open("TF.txt");
	tf_File << "Index\t\t" << "Doc_­ID\t\t" << "TF\n\n";
	return void();
}
/*Doc by Doc*/
void Set_TermFrequency(string word, int document_Name)
{
	/*Except for Numeric word*/
	if (isalpha(word[0]) && isalpha(word[1]) && isalpha(word[2]))
	{
		pair<map<string, vector<int>>::iterator, bool > pr;
		vector<int> parameter;

		parameter.push_back(document_Name);
		parameter.push_back(1);

		pr = tf.insert(pair<string, vector<int>>(word, parameter));
		if (pr.second != true)
		{
			iter = tf.find(word);
			(iter->second)[1]++;
		}
	}
	return void();
}

/*Total TF*/
void Get_TermFrequency_APW()
{
	pair<map<string, vector<int>>::iterator, bool> pr;

	for (iter = tf.begin(); iter != tf.end(); ++iter)
	{
		iter->second.push_back(-1);					// Delimeter
		pr = IndexDat.insert(make_pair(iter->first, iter->second));
		//iter->first = Index,   iter->second = vector<int>   [0] Doc_ID  [1] TF  [2] -1

		// Collision
		if (pr.second != true)
		{
			index_Iter = IndexDat.find(iter->first);							// Find Duplicate

			try
			{
				/*Error Position*/
				index_Iter->second.push_back(iter->second[0]);		// Doc_ID
				index_Iter->second.push_back(iter->second[1]);		// TF
				index_Iter->second.push_back(-1);							// Delimeter
			}
			catch (const std::bad_alloc&)
			{
				cout << "bad_alloc" << endl;
				return;
			}
		}
		iter->second.clear();
	}
	tf.clear();
	return;
}

/*Total TF*/
void Get_TermFrequency_NYT()
{
	pair<map<string, vector<int>>::iterator, bool> pr;

	for (iter = tf.begin(); iter != tf.end(); ++iter)
	{
		iter->second.push_back(-1);					// Delimeter
		pr = IndexDat.insert(make_pair(iter->first, iter->second));
		//iter->first = Index,   iter->second = vector<int>   [0] Doc_ID  [1] TF  [2] -1

		// Collision
		if (pr.second != true)
		{
			index_Iter = IndexDat.find(iter->first);							// Find Duplicate

			try
			{
				index_Iter->second.push_back(iter->second[0]);		// Doc_ID
				index_Iter->second.push_back(iter->second[1]);		// TF
				index_Iter->second.push_back(-1);							// Delimeter
			}
			catch (const bad_alloc&)
			{
				cout << "bad_alloc" << endl;
				return;
			}
		}
		iter->second.clear();
	}
	tf.clear();
	return;
}

void Merge_APW_NYT()
{
}

/*Index.txt*/
void CheckTF()
{
	ofstream of;
	of.open("Index.txt");
	long double weight;

	// dotdat = Index, vector< Index_ID, DF, CF> 
	// IndexDat = Index, vector< Doc_ID, TF, -1, Doc_ID, TF, -1 ...>
	of << "<Index_ID, DOC_ID, TF, Weight>\n";

	for (index_Iter = IndexDat.begin(); index_Iter != IndexDat.end(); ++index_Iter)
	{
		/*dotDat ==  Index,   <ID Num, DF, CF> */
		iter = dotDat.find(index_Iter->first);		/*Index*/
		int count = 0;

		// Doc_ID -> TF -> -1 -> Doc_ID -> TF -> -1 -> Doc_ID -> TF -> -1 ....  Loop
		for (vec_Iter = index_Iter->second.begin(); vec_Iter != index_Iter->second.end(); ++vec_Iter, count++)
		{
			/*NewLine*/
			if (*vec_Iter == -1)
			{
				of << "\n";
				continue;
			}
			/*Index_ID Doc_ID*/
			if (count % 3 == 0)
			{
				// Index_ID
				of << (iter->second)[0] /*Index_ID*/ << "\t\t" << *vec_Iter /*Doc_ID*/ << "\t\t";
				continue;
			}
			/*TF Weight*/
			if (count % 3 == 1)
			{
				weight = *vec_Iter / (double)(iter->second[1]);
				of << *vec_Iter  /*TF*/ << "\t\t" << weight;
			}
		}
	}

	//for (index_Iter = NYT_Dat.begin(); index_Iter != NYT_Dat.end(); ++index_Iter)
	//{
	//	/*dotDat ==  Index,   <ID Num, DF, CF> */
	//	iter = dotDat.find(index_Iter->first);		/*Index*/
	//	int count = 0;

	//	// Doc_ID -> TF -> -1 -> Doc_ID -> TF -> -1 -> Doc_ID -> TF -> -1 ....  Loop
	//	for (vec_Iter = index_Iter->second.begin(); vec_Iter != index_Iter->second.end(); ++vec_Iter, count++)
	//	{
	//		/*NewLine*/
	//		if (*vec_Iter == -1)
	//		{
	//			of << "\n";
	//			continue;
	//		}
	//		/*Index_ID Doc_ID*/
	//		if (count % 3 == 0)
	//		{
	//			// Index_ID
	//			of << (iter->second)[0] /*Index_ID*/ << "\t\t" << *vec_Iter /*Doc_ID*/ << "\t\t";
	//			continue;
	//		}
	//		/*TF Weight*/
	//		if (count % 3 == 1)
	//		{
	//			weight = *vec_Iter / (double)(iter->second[1]);
	//			of << *vec_Iter  /*TF*/ << "\t\t" << weight;
	//		}
	//	}
	//}
	of.close();
	return;
}

/*Doc by Doc*/
void Set_DocumentFrequency(string word)
{
	if (isalpha(word[0]) && isalpha(word[1]) && isalpha(word[2]))
	{
		pair<map<string, int>::iterator, bool> pr;
		pr = temp.insert(pair<string, int>(word, 1));
	}
	return;
}

/*DF.txt*/
void Get_DocumentFrequency()
{
	df_File.open("DF.txt");

	df_File << "Index\tDF\n\n";

	for (iter_string_int = df.begin(); iter_string_int != df.end(); ++iter_string_int)
	{
		df_File << iter_string_int->first  /*Index*/ << "\t\t" << iter_string_int->second /*DF*/;
		df_File << "\n";
	}
	df_File.close();
}

/*Total DF*/
void Merge_DocumentFrequency()
{
	pair<map<string, int>::iterator, bool> pr;

	for (temp_Iter = temp.begin(); temp_Iter != temp.end(); ++temp_Iter)
	{
		pr = df.insert(pair<string, int>(temp_Iter->first, 1));

		if (!pr.second)
		{
			iter_string_int = df.find(temp_Iter->first);  // Index
			iter_string_int->second++;						// DF 
		}
	}
	temp.clear();
	return;
}

/*Doc by Doc*/
void Set_CollectionFrequency(string word)
{
	pair<map<string, int>::iterator, bool> pr;

	if (isalpha(word[0]) && isalpha(word[1]) && isalpha(word[2]))
	{
		pr = cf.insert(pair<string, int>(word, 1));

		if (!pr.second)
		{
			iter_string_int = cf.find(word);		// Index
			iter_string_int->second++;			// CF
		}
	}
	return;
}

/*CF.txt*/
void Get_Collection()
{
	cf_File.open("CF.txt");

	cf_File << "Index\t\tCF\n\n";

	for (iter_string_int = cf.begin(); iter_string_int != cf.end(); ++iter_string_int)
	{
		cf_File << iter_string_int->first << "\t\t" << iter_string_int->second;
		cf_File << "\n";
	}
	cf_File.close();
}

/*Word.txt*/
void Set_WordData()
{
	ofstream wordData;
	wordData.open("Word.txt");

	map<string, int>::iterator df_iter;
	map<string, int>::iterator cf_iter;
	map<string, vector<int>>::iterator iter;

	pair<map<string, vector<int>>::iterator, bool> pr;

	int idNum = 1;		/*ID Number  1 to n count*/

	for (df_iter = df.begin(), cf_iter = cf.begin(); df_iter != df.end(); ++df_iter, ++cf_iter)
	{
		vector<int> parameter;
		parameter.push_back(idNum++);				/*ID num*/
		parameter.push_back(df_iter->second);		/*DF*/
		parameter.push_back(cf_iter->second);		/*CF*/

		pr = dotDat.insert(pair<string, vector<int>>(df_iter->first, parameter));
	}
	wordData << "Index_ID / Index / DF / CF / StartPosition\n\n";

	long long startIDX = 0;

	/*dotDat ==  Index,   <ID Num, DF, CF> */
	for (iter = dotDat.begin(); iter != dotDat.end(); ++iter)
	{
		wordData << (iter->second)[0]				/*Index_ID*/
			<< "\t\t" << iter->first							/*index*/
			<< "\t\t" << (iter->second)[1]				/*DF*/
			<< "\t\t" << (iter->second)[2]				/*CF*/
			<< "\t\t" << startIDX								/*Start_Position*/
			<< "\n";

		startIDX += (iter->second)[1];						/* += DF  */
	}
	wordData.close();
}

/*Document_ID  Document_Name  Document_Length*/
void Set_Document_Info(string str, int length, int count)
{
	pair<map<string, vector<int>>::iterator, bool> pr;

	vector<int> parameter;
	parameter.push_back(count);
	parameter.push_back(length);

	pr = doc_info.insert(pair<string, vector<int>>(str, parameter));
}

/*Doc_dat.txt*/
void Get_Document_Info()
{
	ofstream file;
	file.open("doc_dat.txt");

	file << "문서ID\t\t\t문서명\t\t문서길이\n\n";

	for (iter = doc_info.begin(); iter != doc_info.end(); ++iter)
	{
		/*        Document__ID						<< Document				   << Doc_Length	*/
		file << (iter->second)[0] << "\t\t" << iter->first << "\t\t" << (iter->second)[1];
		file << "\n";
	}
	file.close();
}

/*Setting Stop Word Library*/
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

		if (readFile.fail())
		{
			cout << "File does not exit in Collection!\t" << readFile_Str << endl;
			return;
		}
		else
			cout << readFile_Str + " 파일에 대해 색인어를 추출하고 있습니다." << endl;
	}

	inline void Process(string &cur_Document, int What)
	{
		int getDocumentName = CHECK::END, getIndex = CHECK::END, doc_Length = 0;
		string documentName = "", convert_token;
		char *token, *buf;

		while (getline(readFile, str))
		{
			if (str.find("[DOCNO] : ") != string::npos)
			{
				getIndex = CHECK::END;
				buf = strdup(str.c_str());
				token = strtok(buf, " :");

				if (doc_Length > 0)
				{
					Set_Document_Info(documentName, doc_Length, doc_count);	/*Document*/
					Merge_DocumentFrequency();

					/*doc_Count = document ID*/
					if (What == 0)
						Get_TermFrequency_APW();		/*Total TF*/
					if (What == 1)
						Get_TermFrequency_NYT();
					documentName.clear();
					doc_Length = 0;		/*Doc_File*/   /*After Eliminate StopWord*/
				}

				while (token != NULL)
				{
					convert_token = token;
					if (convert_token.find("[DOCNO]") != string::npos)
					{
						token = strtok(NULL, " :_");
						continue;
					}
					documentName += convert_token;
					token = strtok(NULL, " :_");
				}
				++doc_count;

				//cout << "Document\t " + documentName + "\t\t processing" << endl;
			}

			if (str.find("[TEXT] : ") != string::npos)
				getIndex = CHECK::START;
			
			if (getIndex == CHECK::START)
			{
				buf = strdup(str.c_str());
				token = strtok(buf, " :");

				while (token != NULL)
				{
					convert_token = token;
					Set_DocumentFrequency(convert_token);					/*DF*/
					Set_TermFrequency(convert_token, doc_count);			/*TF*/
					Set_CollectionFrequency(convert_token);					/*CF*/
					doc_Length++;														/*Word Number after working stop word*/
					token = strtok(NULL, " :");
				}
			}
		}
	}

	inline void Parsing()
	{
		/* Line & Lowercase & StemmingCase*/
		int lineCheck = CHECK::END, lowercase = CHECK::END, stemmingCase = CHECK::END, indexingLine = CHECK::END;
		set<string>::iterator iter;
		string convert_token;

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
			if (str.find("</HEADLINE>") != string::npos || str.find("</TEXT>") != string::npos)
				lowercase = CHECK::END;

			/* Unnecessary Line */
			if (str.compare("<P>") == 0 || str.compare("</P>") == 0 || !lineCheck)
				continue;

			/* Necessary Line */
			else
			{
				char *buf = strdup(str.c_str());
				char *token = strtok(buf, " ,.`-\t?!;&'()");
				/* Start Tokenizing */
				while (token != NULL)
				{
					 convert_token = token;
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
						/*Converting from < > to [ ] : */
						if (convert_token.compare("<DOCNO>") == 0 || convert_token.find("<HEADLINE>") != string::npos || convert_token.find("<TEXT>") != string::npos || convert_token.find("<text>") != string::npos)
						{
							convert_token[0] = '[';
							convert_token[strlen(token) - 1] = ']';
							convert_token += " : ";
						}
						/*Porter2 Stemming*/
						//cout << convert_token + "--> ";
						if (stemmingCase == CHECK::START) { Porter2Stemmer::stem(convert_token);   }
						

						/*Writing string to Final File after stopwording & porter2 stemming */
						writeFile << convert_token;
						writeFile << " ";

						if (convert_token.compare("[TEXT] : ") == 0)
							indexingLine = CHECK::START;
					}
					token = strtok(NULL, " ,.`-\t?!;&'()");
				}
				writeFile << "\n";
				/*End converting string to lowerCase*/
				if (str.find("<TEXT>") != string::npos || str.find("<HEADLINE>") != string::npos)
					lowercase = CHECK::LOWERCASE;
			}
			/*End Parsing*/
			if (str.find("</DOCNO>") != string::npos || str.find("</HEADLINE>") != string::npos)
				lineCheck = CHECK::END;
			/*End Stemming*/
			if (str.find("</TEXT>") != string::npos)
			{
				lineCheck = CHECK::END;
				stemmingCase = CHECK::END;
				indexingLine = CHECK::END;
			}
		}
		writeFile.close();
		return;
	}
};

int main(void)
{
	int year, count = 1;
	clock_t begin = clock(), end;
	Set_StopWord(); Set_Document();
	File *file;

	for (year = 6; year <= 12; year++)
	{
		for (int month = 1; month <= 31; month++)
		{
			string file_Name = "[STEMMED]__1998";
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

			file = new File(file_Name, revised + file_Name);

			//file->Parsing();
			file->Process(file_Name, 0);
			delete file;
			count++;
		}
	}

	for (year = 1; year <= 12; year++)
	{
		for (int month = 1; month <= 31; month++)
		{
			string file_Name = "[STEMMED]__1999";
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

			file = new File(file_Name, revised + file_Name);
			//file->Parsing();
			file->Process(file_Name, 0);
			delete file;
			count++;
		}
	}

	for (year = 1; year <= 9; year++)
	{
		for (int month = 1; month <= 31; month++)
		{
			string file_Name = "[STEMMED]__2000";
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

			file = new File(file_Name, revised + file_Name);
			//file->Parsing();
			file->Process(file_Name, 0);
			delete file;
			count++;
		}
	}

	for (year = 6; year <= 12; year++)
	{
		for (int month = 1; month <= 31; month++)
		{
			string file_Name = "[STEMMED]__1998";
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

			file = new File(file_Name, revised + file_Name);
			//file->Parsing();
			file->Process(file_Name, 1);

			delete file;
			count++;
		}
	}

	for (year = 1; year <= 12; year++)
	{
		for (int month = 1; month <= 31; month++)
		{
			string file_Name = "[STEMMED]__1999";
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

			file = new File(file_Name, revised + file_Name);
			//file->Parsing();
			file->Process(file_Name, 1);

			delete file;
			count++;
		}
	}

	for (year = 1; year <= 9; year++)
	{
		for (int month = 1; month <= 31; month++)
		{
			string file_Name = "[STEMMED]__2000";
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

			file = new File(file_Name, revised + file_Name);
			//file->Parsing();
			file->Process(file_Name, 1);

			delete file;
			count++;
		}
	}

	Get_DocumentFrequency();		/*Making DF.txt File*/
	cout << "색인 중입니다….DF.dat 를 생성했습니다." << endl;

	Get_Collection();					/*Making CF.txt File*/
	cout << "색인 중입니다….CF.dat 를 생성했습니다." << endl;

	cout << "단어정보파일을 생성 중입니다...." << endl;
	Set_WordData();
	cout << "색인 중입니다….단어 정보 파일 word.dat 를 생성했습니다.\n" << endl;

	cout << "문서정보파일을 생성 중입니다...." << endl;
	Get_Document_Info();
	cout << "문서 정보 파일 doc.dat를 생성했습니다.\n" << endl;

	cout << "색인파일을 생성 중입니다...." << endl;
	CheckTF();
	cout << "역색인 파일 index.dat를 생성했습니다.\n" << endl;

	end = clock();
	cout << count - 1 << "개 파일 전체 Time:  " << ((end - begin) / (CLOCKS_PER_SEC)) << "sec" << endl;

	getchar();
	return 0;
}