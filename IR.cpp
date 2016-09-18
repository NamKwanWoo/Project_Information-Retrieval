#pragma warning(disable:4996)

#include <iostream>
#include <string>
#include <set>
#include <cstring>
#include <fstream>
#include <sstream>
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

class File
{
private:
    ifstream readFile;
    ifstream stopWord_File;
    ofstream writeFile;

	string str;

    set<string> origin_Word;
    set<string> stop_Word;
    set<string> parsed_Word;

public:
    File(string readFile, string writeFile)
    {
        this->readFile.open(readFile);
        this->writeFile.open(writeFile);
		this->stopWord_File.open("StopWord.txt");

		if (this->readFile.fail() || this->writeFile.fail() || this->stopWord_File.fail())
        {
            cout << "Error!" << endl;
            exit(-1);
        }

        while (getline(this->stopWord_File, str))
            this->stop_Word.insert(str);
        this->stopWord_File.close();
    }

    inline void Parsing()
    {
        int lineCheck = CHECK::END;
        int lowercase = CHECK::END;

        /*Line by Line*/
        while (getline(readFile, str))
        {
            /*Start Parsing*/
            if (str.find("<DOCNO>") != string::npos || str.find("<HEADLINE>") != string::npos ||
                str.find("<TEXT>") != string::npos)
                lineCheck = CHECK::START;

            /*Start converting string to lowerCase*/
            if (str.find("</TEXT>") != string::npos || str.find("</HEADLINE>") != string::npos)
                lowercase = CHECK::END;

            /*Unnecessary Line*/
            if (str.compare("<P>") == 0 || str.compare("</P>") == 0 || !lineCheck)
                continue;

            /*Necessary Line*/
            else
            {
                set<string>::iterator iter;

				char *buf = strdup(str.c_str());
                char *token = strtok(buf, " ,.`-\t");

                /*Start Tokenizing*/
                while (token != NULL)
                {
                    string convert_token = token;
                    iter = stop_Word.find(convert_token);

                    /*If token is not stopword*/
                    if (iter == stop_Word.end())
                    {
                        //cout << convert_token << " ";
						//Porter2Stemmer::stem(convert_token);
						/*Converting Tolowercase*/

						if (lowercase == CHECK::LOWERCASE)
						{
							strlwr(token);
							convert_token = token;
						}
						
                        writeFile << convert_token;
                        writeFile << " ";
                    }
                    token = strtok(NULL, " ,.`-\t");
                }
                writeFile << "\n";

                /*End converting string to lowerCase*/
                if (str.find("<TEXT>") != string::npos || str.find("<HEADLINE>") != string::npos)
                    lowercase = CHECK::LOWERCASE;
            }

            /*End Parsing*/
            if (str.find("</DOCNO>") != string::npos || str.find("</HEADLINE>") != string::npos ||
                str.find("</TEXT>") != string::npos)
                lineCheck = CHECK::END;
        }
        /*Close output!!!  Necessary!!*/
        writeFile.close();
    }
};

int main(void)
{
	clock_t begin, end;
	begin = clock();

	for(auto i=1; i<2; i++)
	{
		string str = "199806";
		if(i<10)
			str += "0" + to_string(i) + "_NYT";
		else
			str += to_string(i) + "_NYT";

		File file(str, "sibal1.txt");
		file.Parsing();
	}

	end = clock();
	cout << "Time:  " << ((end-begin)/(CLOCKS_PER_SEC)) << "sec" << endl;

	getchar();
    return 0;
}