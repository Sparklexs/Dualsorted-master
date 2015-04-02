#include "Datastream.h"
#include <algorithm>

using namespace std;


Datastream::Datastream(string name)
{
	this->filter = " (),:.;\t\"\'!?-_\0\n[]=#{}";
	this->f.open(name.c_str());
	if(!f) {
	    	cout << "Cannot open input file.\n";
		
	}
  
	this->fname = name;
}
vector <string> Datastream::getdterms()
{
	return this->dterms;

}
vector<string> Datastream::getTerms()
{
	while(this->f.good())
	{
   		string str;
	        getline (this->f,str);
		if (str.length() >=3)
		{
			transform(str.begin(), str.end(),str.begin(), ::tolower);
       			Tokenize(str,this->terms,this->filter);
		}
	}
	return this->terms;
}

vector<string> Datastream::getDistinctTerms()
{
	vector<string> v(this->terms.size());
	copy(this->terms.begin(),this->terms.end(),v.begin());
	sort(v.begin(),v.end());
	v.erase(unique(v.begin(),v.end()),v.end());
	this->dterms = v;
	for (int i = 0 ; i < this->dterms.size(); i++)
	{
		this->freq_terms.push_back(0);
		for (int j = 0 ; j < this->terms.size();j++)
		{
			if (this->dterms[i] == this->terms[j])
			{
				this->freq_terms[i]++;
			}
		}
	}
	return v;
}


vector<uint> Datastream::getFreqVector()
{
	return this->freq_terms;
}

uint Datastream::getTermSize()
{
	return this->terms.size();
}
