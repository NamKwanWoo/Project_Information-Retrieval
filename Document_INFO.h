#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

class Document_Info
{
private:
	ofstream file;
	map<string, vector<int>> doc_info;
	map<string, vector<int>>::iterator iter;

public:
	void Set_Document_Info(string &str, int count, int);
	void Get_Document_Info();
};