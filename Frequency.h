#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class DF
{
private:
	string word;
	string document;
	ofstream df_File;

public:
	map<string, int> temp;
	map<string, int> df;
	map<string, int>::iterator iter;
	void Set_DocumentFrequency(string &word);
	void Get_DocumentFrequency();
	void Merge_DocumentFrequency();
};

class CF
{
private:
	string word;
	string document;
	ofstream cf_File;

public:
	map<string, int> cf;
	map<string, int>::iterator iter;
	void Set_CollectionFrequency(string &word);
	void Get_Collection();

};

/*Word.dat : <Index ID, Index, DF, CF, Start Position>*/
class Word_Data
{
private:
	int query_ID;

public:
	map<string, vector<int>> dotDat;
	map<string, vector<int>>::iterator iter;

	void Set_WordData(DF&, CF&);
};

class TF
{
private:
	string word;
	string document;
	ofstream tf_File;

	map<string, vector<int>> tf;
	map<string, vector<int>>::iterator iter;
	vector<int>:: iterator vec_Iter;

public:
	map<string, vector<int>> IndexDat;
	map<string, vector<int>>::iterator index_Iter;
	map<string, vector<int> > NYT_Dat;
	map<string, vector<int> >::iterator NYT_iter;

	void Set_TermFrequency(string &word, int document_Name);
	void Set_Document();
	void Get_TermFrequency_APW();
	void Get_TermFrequency_NYT();
	void Merge_APW_NYT();
	void CheckTF(Word_Data &);
};
