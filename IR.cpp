#pragma warning(disable:4996)
#include <iostream>
#include <string>
#include <set>
#include <fstream>
#include <time.h>
#include "Porter2.h"
using namespace std;

namespace CHECK
{
	enum
	{
		END, START, LOWERCASE
	};
}

size_t time_Complexity = 0;

class File
{
private:
	ifstream readFile;
	ifstream stopWord_File;
	ofstream writeFile;
	string str;
	
	set<string> stop_Word;

public:
	File(string readFile, string writeFile)
	{
		clock_t begin, end;
		begin = clock();

		this->readFile.open(readFile);
		this->writeFile.open(writeFile);
		this->stopWord_File.open("StopWord.txt");

		if (this->readFile.fail() || this->writeFile.fail() || this->stopWord_File.fail())
		{
			cout << "Error!" << endl;
			getchar();
			exit(-1);
		}

		while (getline(this->stopWord_File, str))
		{
			this->stop_Word.insert(str);
			this->stop_Word.insert(str + " ");
		}
		this->stopWord_File.close();

		end = clock();
		time_Complexity += ((end - begin) / (CLOCKS_PER_SEC));

		cout << "Abstracting " << readFile << " Time:  " << time_Complexity << "sec" << endl;
	}

	inline void Parsing()
	{
		clock_t begin, end;
		begin = clock();

		/* Line & Lowercase & StemmingCase*/
		int lineCheck = CHECK::END;
		int lowercase = CHECK::END;
		int stemmingCase = CHECK::END;

		/* Line by Line */
		while (getline(readFile, str))
		{
			/* Start Parsing */
			if (str.find("<DOCNO>") != string::npos || str.find("<HEADLINE>") != string::npos || str.find("<TEXT>") != string::npos)
				lineCheck = CHECK::START;

			/*Stemming = <HEADLINE>, <TEXT>*/
			if (str.find("<HEDLINE>") != string::npos)
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
				char *token = strtok(buf, " ,.`-\t?;&'");

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
						/*Converting contents from < > to [ ]*/
						if (convert_token.compare("<DOCNO>") == 0 || convert_token.find("<HEADLINE>") != string::npos
							|| convert_token.find("<TEXT>") != string::npos)
						{
							convert_token[0] = '[';
							convert_token[strlen(token) - 1] = ']';
							convert_token += " : ";
						}

						/*Porter2 Stemming*/
						if(stemmingCase)
							Porter2Stemmer::stem(convert_token);

						/*Writing string to Final File after stopwording & porter2 stemming */
						writeFile << convert_token;
						writeFile << " ";
					}
					token = strtok(NULL, " ,.`-\t?;&'");
				}

				/*Dummy Line*/
				if (str.find("</HEADLINE>") == string::npos)
					writeFile << "\n";

				/*End converting string to lowerCase*/
				if (str.find("<TEXT>") != string::npos || str.find("<HEADLINE>") != string::npos)
					lowercase = CHECK::LOWERCASE;
			}

			/*End Parsing*/
			if (str.find("</DOCNO>") != string::npos || str.find("</HEADLINE>") != string::npos ||
				str.find("</TEXT>") != string::npos)
				lineCheck = CHECK::END;
			
			/*End Stemming*/
			if (str.find("</TEXT>") != string::npos)
				stemmingCase = CHECK::END;
		}

		/*Close output!!!  Necessary!!*/
		writeFile.close();

		end = clock();
		cout << "Parsing Time:  " << ((end - begin) / (CLOCKS_PER_SEC)) << "sec" << endl;
	}
};

int main(void)
{
	int i;
	clock_t begin, end;
	begin = clock();

	for (i = 1; i<31; i++)
	{
		string str = "199806";
		string revised = "__";
		if (i<10)
			str += "0" + to_string(i) + "_NYT";
		else
			str += to_string(i) + "_NYT";

		File file(str, revised + to_string(i));
		file.Parsing();
	}

	end = clock();
	cout << i - 1 << "개 파일 Time:  " << ((end - begin) / (CLOCKS_PER_SEC)) << "sec" << endl;
	getchar();
	return 0;
}