#include "Terms.h"
using namespace std;


Terms::Terms(int nt,int nd) 
{
	this->n_terms = 0;
	this->n_docs = 0;
	for (int i = 0 ; i < nt ;i++)
	{
		vector <int> d;
		this->documents.push_back(d);
		for (int j = 0 ; j<nd;j++)
		{
			this->documents[i].push_back(-1);
		}	
	}
}

int Terms::getn_docs()
{
	return this->n_docs;
}

vector<int> Terms::getFreqs()
{
	return this->frequencies;
}
vector<int> Terms::getDocs(string t)
{
	int n = this->inVector(t);
	int count = 0;	
	for (int i = 0 ; i < this->documents[n].size();i++)
	{
		count = count+this->documents[n].size();
		if (this->documents[n][i] != -1)
		{
//			cout << "found:" << t << " at document = " << i << " with freq= " << this->documents[n][i] << endl;
		//	count++;
		}
	}
//	cout << "COUNT = " << count << endl;
	vector<int> result;
	int *result_freq = new int[count];
	int aux = 0;
//	cout << "DOCUMENT SIZE" <<  this->documents[n].size() << endl;
	for (int i = 0 ; i < this->documents[n].size() ;i++)
	{
		
		if (this->documents[n][i] != -1)
		{
			result_freq[aux] = this->documents[n][i];
			this->frequencies.push_back(this->documents[n][i]);
			result.push_back(i);
			aux++;
		}
		
	}

	for (int i = 0 ; i < aux; i++) // Improve this
	{
		for (int j = 0 ;j<aux;j++)
		{
			if (result_freq[i] > result_freq[j])
			{
				int a = result_freq[i];
				result_freq[i] = result_freq[j];
				result_freq[j] = a;

				int b = result[i];
				result[i] = result[j];
				result[j] =  b;
			}
		}

	}
	return result;
}


vector<int> Terms::getDocsFreq(string t)
{
	int n = this->inVector(t);
	int count = 0;	
	for (int i = 0 ; i < this->documents[n].size();i++)
	{
		count = count + this->documents[n].size();
//		if (this->documents[n][i] != -1)
//		{
//			cout << "found:" << t << " at document = " << i << " with freq= " << this->documents[n][i] << endl;
//			count++;
//		}
	}
	//cout << "COUNT = " << count << endl;
	vector<int> result;
	int *result_freq = new int[count];
	int aux = 0;
	//cout << "DOCUMENT SIZE" <<  this->documents[n].size() << endl;
	for (int i = 0 ; i < this->documents[n].size() ;i++)
	{
		
		if (this->documents[n][i] != -1)
		{
			result_freq[aux] = this->documents[n][i];
			this->frequencies.push_back(this->documents[n][i]);
			result.push_back(i);
			aux++;
		}
		
	}

	for (int i = 0 ; i < aux; i++) // Improve this
	{
		for (int j = 0 ;j<aux;j++)
		{
			if (result_freq[i] > result_freq[j])
			{
				int a = result_freq[i];
				result_freq[i] = result_freq[j];
				result_freq[j] = a;

				int b = result[i];
				result[i] = result[j];
				result[j] =  b;
			}
		}

	}
	vector <int> result_freq_v;	
	for (int i = 0 ; i < aux ; i++)
	{
		result_freq_v.push_back(result_freq[i]);
	}
	return result_freq_v;
}

void Terms::addDataStream(Datastream *ds)
{
	vector <string> dterm = ds->getdterms();
	int n_terms = this->n_terms;
	int n_docs = this->n_docs;
	int size = dterm.size();
	for (int i = 0 ; i < size;i++)
	{
		if (isStopWord(dterm[i]))
		{
		   // cout << dterm[i] << " is a stopword" << endl;
		    continue;
		}
		int n = this->inVector(dterm[i]);
		if (n == -1)
		{
			this->terms.push_back(dterm[i]);
			//cout << "agregando n_term = " << this->n_terms << " n_docs = " << this->n_docs << " term = " << dterm[i] << " freq = " << ds->freq_terms[i] << endl;
			this->documents[this->n_terms][this->n_docs] = ds->freq_terms[i];
			this->n_terms++;
		}
		else
		{
			//cout << "* agregando n_term = " << n << " n_docs = " << this->n_docs << " term = " << dterm[i] << " freq = " << ds->freq_terms[i] << endl;
			this->documents[n][this->n_docs] = ds->freq_terms[i];
		}
	}
	this->n_docs++;
}

void Terms::writeFiles(string name)
{
	ofstream outfile;
	outfile.open(name + ".words",ios::out | ios::app);
	for (int i = 0 ; i < this->terms.size();i++)
	{
		outfile << this->terms[i] << endl;
	}
	outfile.close();

	outfile.open(name + ".invlist",ios::out | ios::app);
	
	for (int i = 0 ; i < this->terms.size();i++)
	{
		string term = terms[i];
		vector<int> docs = this->getDocs(term);
		int size = docs.size();
		outfile << size << " ";
		for (int j = 0 ;j < docs.size();j++)
		{
			outfile << docs[j] << " ";
		}
		outfile << endl;
	}
	outfile.close();

	outfile.open(name + ".invlistfreq",ios::out | ios::app);
	for (int i = 0 ; i < this->terms.size();i++)
	{
		string term = terms[i];
		vector<int> docsfreq =  this->getDocsFreq(term);
		int size = docsfreq.size();
		outfile << size << " ";
		for (int j = 0 ;j < docsfreq.size();j++)
		{
			outfile << docsfreq[j] << " ";
		}
		outfile << endl;
	}

}
int Terms::inVector(string t)
{
	for (int i = 0 ; i < this->terms.size();i++)
	{
		if (this->terms[i] == t)
		{
			return i;
		}
	}
	return -1;
}

