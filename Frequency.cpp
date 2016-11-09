#include "Frequency.h"

/*Doc by Doc*/
void TF::Set_TermFrequency(string &word, int document_Name)
{
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
}

/*Setting TF.txt*/
void TF::Set_Document() 
{ 
	tf_File.open("TF.txt");
	tf_File << "Index\t\t" << "Doc_­ID\t\t" << "TF\n\n";
}

/*Total TF*/
void TF::Get_TermFrequency_APW()
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
			
			/*Error Position*/
			index_Iter->second.push_back(iter->second[0]);		// Doc_ID
			index_Iter->second.push_back(iter->second[1]);		// TF
			index_Iter->second.push_back(-1);							// Delimeter

			//cout << index_Iter->second.size() << "\t\t" << iter->first << "\t" << (iter->second[1]) << endl;
		}

		//tf_Fi le<< Index       <<  Tap     <<  Doc_ID				<< Tap		<<  TF
		tf_File << iter->first << "\t\t" << (iter->second)[0] << "\t\t" << (iter->second)[1];
		tf_File << "\n";
	}
	tf_File << "\n";
	tf.clear();
}

void TF::Get_TermFrequency_NYT()
{
	pair<map<string, vector<int>>::iterator, bool> pr;

	for (iter = tf.begin(); iter != tf.end(); ++iter)
	{
		iter->second.push_back(-1);					// Delimeter
		pr = NYT_Dat.insert(make_pair(iter->first, iter->second));
		//iter->first = Index,   iter->second = vector<int>   [0] Doc_ID  [1] TF  [2] -1

		// Collision
		if (pr.second != true)
		{
			index_Iter = NYT_Dat.find(iter->first);							// Find Duplicate

			index_Iter->second.push_back(iter->second[0]);		// Doc_ID
			index_Iter->second.push_back(iter->second[1]);		// TF
			index_Iter->second.push_back(-1);							// Delimeter
			
			//cout << index_Iter->second.size() << "\t\t" << iter -> first << "\t" << (iter->second[1]) << endl;

																		//cout << index_Iter->second.size() << endl;
		}

		//tf_Fi le<< Index       <<  Tap     <<  Doc_ID				<< Tap		<<  TF
		tf_File << iter->first << "\t\t" << (iter->second)[0] << "\t\t" << (iter->second)[1];
		tf_File << "\n";
	}
	tf_File << "\n";
	tf.clear();

}

void TF::Merge_APW_NYT()
{

}

/*Index.txt*/
void TF::CheckTF(Word_Data &df)
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
		df.iter = df.dotDat.find(index_Iter->first);		/*Index*/
		int count = 0;

		// Doc_ID -> TF -> -1 -> Doc_ID -> TF -> -1 -> Doc_ID -> TF -> -1 ....  Loop
		for (vec_Iter = index_Iter->second.begin(); vec_Iter != index_Iter->second.end(); ++vec_Iter, count ++)
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
				of << (df.iter->second)[0] /*Index_ID*/ << "\t\t" << *vec_Iter /*Doc_ID*/ << "\t\t";
				continue;
			}
			/*TF Weight*/
			if (count % 3 == 1)
			{
				weight = *vec_Iter / (double)(df.iter->second[1]);
				of << *vec_Iter  /*TF*/ << "\t\t" << weight;
			}
		}
	}

	for (index_Iter = NYT_Dat.begin(); index_Iter != NYT_Dat.end(); ++index_Iter)
	{
		/*dotDat ==  Index,   <ID Num, DF, CF> */
		df.iter = df.dotDat.find(index_Iter->first);		/*Index*/
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
				of << (df.iter->second)[0] /*Index_ID*/ << "\t\t" << *vec_Iter /*Doc_ID*/ << "\t\t";
				continue;
			}
			/*TF Weight*/
			if (count % 3 == 1)
			{
				weight = *vec_Iter / (double)(df.iter->second[1]);
				of << *vec_Iter  /*TF*/ << "\t\t" << weight;
			}
		}
	}
	of.close();
}

/*Doc by Doc*/
void DF::Set_DocumentFrequency(string &word)
{
	if (isalpha(word[0]) && isalpha(word[1]) && isalpha(word[2]))
	{
		pair<map<string, int>::iterator, bool> pr;
		pr = temp.insert(pair<string, int>(word, 1));
	}
}

/*DF.txt*/
void DF::Get_DocumentFrequency()
{
	df_File.open("DF.txt");

	df_File << "Index\tDF\n\n";

	for (iter = df.begin(); iter != df.end(); ++iter)
	{
		df_File << iter->first  /*Index*/ << "\t\t" << iter->second /*DF*/;
		df_File << "\n";
	}
	df_File.close();
}

/*Total DF*/
void DF::Merge_DocumentFrequency()
{
	pair<map<string, int>::iterator, bool> pr;
	map<string, int>::iterator temp_Iter;


	for (temp_Iter = temp.begin(); temp_Iter != temp.end(); ++temp_Iter)
	{
		pr = df.insert(pair<string, int>(temp_Iter->first, 1));

		if (pr.second != true)
		{
			iter = df.find(temp_Iter->first);  // Index
			iter->second++;						// DF 
		}
	}
	temp.clear();
}

/*Doc by Doc*/
void CF::Set_CollectionFrequency(string &word)
{
	if (isalpha(word[0]) && isalpha(word[1]) && isalpha(word[2]))
	{
		pair<map<string, int>::iterator, bool> pr;
		pr = cf.insert(pair<string, int>(word, 1));

		if (pr.second != true)
		{
			iter = cf.find(word);		// Index
			iter->second++;			// CF
		}
	}
}

/*CF.txt*/
void CF::Get_Collection()
{
	cf_File.open("CF.txt");

	cf_File << "Index\t\tCF\n\n";

	for (iter = cf.begin(); iter != cf.end(); ++iter)
	{
		cf_File << iter->first << "\t\t" << iter->second;
		cf_File << "\n";
	}
	cf_File.close();
}

/*Word.txt*/
void Word_Data::Set_WordData(DF &document_Frequency, CF &collection_Frequency)
{
	ofstream wordData;
	wordData.open("Word.txt");

	map<string, int>::iterator df_iter;
	map<string, int>::iterator cf_iter;
	map<string, vector<int>>::iterator iter;

	pair<map<string, vector<int>>::iterator, bool> pr;

	int idNum = 1;		/*ID Number  1 to n count*/

	for (df_iter = document_Frequency.df.begin(), cf_iter = collection_Frequency.cf.begin(); df_iter != document_Frequency.df.end(); ++df_iter, ++cf_iter)
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