#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include "utils.cpp"
#include <algorithm>

using namespace std;

class Datastream
{
	private:
		ifstream f;	
		string fname;
		vector <string> terms;
		vector <string> dterms;
		string filter;
	
	public:
		vector <uint> freq_terms; 
		Datastream(string name);
		vector<string> getTerms();
		vector<string> getDistinctTerms();
		vector<uint> getFreqVector();
		uint getTermSize();
		vector<string> getdterms();
		
};



		
