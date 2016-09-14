#include <iostream>
#include <string>
#include <set>
#include <cstring>
#include <fstream>
#include <sstream>

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

    set<string> origin_Word;
    set<string> stop_Word;
    set<string> parsed_Word;

public:
    File(string readFile, string writeFile)
    {
        this->readFile.open(readFile);
        this->writeFile.open(writeFile);

        if (this->readFile.fail() || this->writeFile.fail())
        {
            cout << "Error!" << endl;
            getchar();
        }
    }

    void set_StopWord()
    {
        stopWord_File.open("StopWord.txt");
        string str;

        while (getline(stopWord_File, str))
            stop_Word.insert(str);

        stopWord_File.close();
    }

    void Parsing()
    {
        string str;
        int lineCheck = CHECK::END;
		int lowercase = CHECK::END;

		/*Line by Line*/
		while (getline(readFile, str))
        {
            /*Start Parsing*/
            if (str.find("<DOCNO>") != string::npos || str.find("<HEADLINE>") != string::npos || str.find("<TEXT>") != string::npos)
                lineCheck = CHECK::START;
			
			/*Start converting string to lowerCase*/
			if(str.find("</TEXT>") != string::npos || str.find("</HEADLINE>") != string::npos)
				lowercase = CHECK::END;

			/*Unnecessary Line*/
            if (!lineCheck)
                continue;

			/*Necessary Line*/
			else
            {
				/*Converting Tolowercase*/
				if(lowercase == CHECK::LOWERCASE)
					for(unsigned int i = 0; i < str.length(); ++i) 
						str[i] = tolower(str[i]);

                set<string>::iterator iter;
				char* buf = strdup(str.c_str());
                char *token = strtok(buf, " ,.-\t");

				/*Start Tokenizing*/
				while(token != NULL)
				{
					string convert_token = token;
					iter = stop_Word.find(convert_token);

					/*If token is not stopword*/
					if (iter == stop_Word.end())
					{
						cout << convert_token << " ";
						writeFile << convert_token;
						writeFile << " ";
					}
					token = strtok(NULL, " ,.-\t");
				}

				cout << endl;
				writeFile << "\n";
							
				/*End converting string to lowerCase*/
				if(str.find("<TEXT>") != string::npos || str.find("<HEADLINE>") != string::npos)
					lowercase = CHECK::LOWERCASE;
            }

            /*End Parsing*/
            if (str.find("</DOCNO>") != string::npos || str.find("</HEADLINE>") != string::npos || str.find("</TEXT>") != string::npos)
                lineCheck = CHECK::END;
			
        }

		/*Close output!!!  Necessary!!*/
        writeFile.close();
    }
};

int main(void)
{
    File file("a.txt", "sibal1.txt");
    file.set_StopWord();
    file.Parsing();

    getchar();
    return 0;
}