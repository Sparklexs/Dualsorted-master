#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Terms
{
	private:
		int n_terms;
		int n_docs;
		
	public:
		vector <string> terms;
		vector < vector <int> > documents;
		vector <int> frequencies;
		Terms(int nt,int nd);
		void addDataStream(Datastream *ds);		
		void getTerm(int id);
		void getTerm(string t);
		int inVector(string t);
		int getn_docs();
		vector<int> getDocs(string t);
		vector<int> getFreqs();
		void writeFiles(string name);
		vector<int> getDocsFreq(string t);
		
};
