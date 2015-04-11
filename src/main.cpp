//#include "Datastream.cpp"
//#include "Terms.cpp"
#include "Dualsorted.cpp"
#include <ctime>
#include <queue>
#include "utils.cpp"
using namespace std;
#include <boost/algorithm/string.hpp>
//#define  int2char (integer) integer+0x30

class Accumulator {
public:
	size_t doc_id;
	long double impact;
};

class AccComparison {
	bool reverse;
public:
	AccComparison(const bool& revparam = false) {
		reverse = revparam;
	}
	bool operator()(Accumulator * lhs, Accumulator *rhs) const {
		if (reverse)
			return (lhs->impact > rhs->impact);
		else
			return (lhs->impact < rhs->impact);
	}
};

inline void executeAND(Dualsorted* ds, string ** terms, uint *qsizes,
		size_t total_queries) {
	//cout << "wazaaaaa!!! " << endl;
	google::sparse_hash_map<uint, uint> documents;
	clock_t start, finish;
	double time;
	double total = 0;
	start = clock();
	size_t total_results = 0;
	for (uint i = 0; i < total_queries; i++) {
		if (i % 100 == 0)
			cout << " query = " << i << endl;
		ds->intersect(terms[i], qsizes[i]);
	}
	finish = clock();
	time = (double(finish) - double(start)) / CLOCKS_PER_SEC;
	total += time;
	cout << total << "," << ds->getMemSize() << endl;
}

inline void executeOR(Dualsorted* ds, string ** terms, uint *qsizes,
		size_t total_queries) {
	google::sparse_hash_map<uint, uint> documents;
	clock_t start, finish;
	double time;
	double total = 0;
	start = clock();
	size_t total_results = 0;
	for (uint i = 0; i < total_queries; i++) {
		for (uint j = 0; j < qsizes[i]; j++) {
			uint posting_size = ds->getPostingSize(terms[i][j].c_str());
			cout << "term = " << terms[i][j] << endl;
			cout << "posting_size = " << posting_size << endl;
			vector<uint> results = ds->rangeTo(terms[i][j], posting_size);
			cout << "result_sisze =" << results.size() << endl;
			for (uint x = 0; x < results.size(); x++) {

				total_results++;
				//documents[test[x]] = 1;
			}
		}
	}
	finish = clock();
	time = (double(finish) - double(start)) / CLOCKS_PER_SEC;
	total += time;
	cout << total << "," << ds->getMemSize() << endl;
}
inline void executeANDPersin(Dualsorted* ds, string ** terms, uint *qsizes,
		uint top_k, size_t total_queries) {
}/*
 google::sparse_hash_map<uint, uint>  documents;
 clock_t start,finish;
 double time;
 double total = 0;
 start = clock();
 size_t total_results = 0;
 for (uint i = 0 ; i < total_queries ; i+=2)
 {
 if (i%100 ==0)
 {
 finish = clock();
 time = (double(finish)-double(start))/CLOCKS_PER_SEC;
 total += time;
 start = clock();
 }
 for (uint j = 1 ; j < qsizes[i]-1 ; j++)
 {
 ds->intersect(terms[i][j],terms[i][j-1]);
 }
 }
 cout << total*0.20 << "," << ds->getMemSize() << endl;
 }/**/

inline void executePersin(Dualsorted* ds, string ** terms, uint *qsizes,
		uint top_k, size_t total_queries) {
	clock_t start, finish;
	double time;
	double total = 0;
	double ex_total = 0;
	size_t documents = ds->ndocuments;
	priority_queue<Accumulator*, vector<Accumulator*>, AccComparison> persin;

	for (uint i = 0; i < total_queries; i++) {
		persin = priority_queue<Accumulator*, vector<Accumulator*>,
				AccComparison>();
		Accumulator **acc = new Accumulator*[ds->ndocuments];
		ex_total = total;
		for (int x = 0; x < top_k + 1; x++) {
			acc[x] = new Accumulator();
			acc[x]->impact = 0;
			acc[x]->doc_id = i;

		}
		start = clock();
		for (uint j = 0; j < qsizes[i] - 1; j++) {
			uint posting_size = ds->getPostingSize(terms[i][j].c_str());
			if (posting_size < 2)
				continue;
			vector<uint> results = ds->rangeTo(terms[i][j], 20);
//		cout << "results.size() = " << results.size() << endl;
			if (posting_size < results.size())
				break;
			if (results.size() == 0)
				continue;

			for (uint k = 0; k < results.size() - 2; k++) {
//			cout << "posting_size = " << posting_size << endl;
//			cout << "k = " << k << " ->  " << results[k] << endl;
//			cout << "term = " << terms[i][j] << endl;		
				if (k < posting_size - 1) {
					uint freq = ds->getFreqOfPosting(terms[i][j].c_str(), k);
					double idf = msb(
							(ds->doclens[results[k]] - posting_size + 0.5)
									/ (posting_size + 0.5));
					double score = (idf * (freq) * (2.2))
							/ (freq + 1.2 * (1 - 0.75));
					acc[((results[k]) % top_k)]->impact += score;
					acc[((results[k] % top_k))]->doc_id = results[k];
				}

			}

			results.clear();

		}
		finish = clock();
		time = (double(finish) - double(start)) / CLOCKS_PER_SEC;
		total += time;
		for (size_t j = 0; j < top_k; j++) {
			if (acc[j] != NULL) {
				if (acc[j]->impact != 0.0) {
					start = clock();
					persin.push(acc[j]);
					finish = clock();
					time = (double(finish) - double(start)) / CLOCKS_PER_SEC;
					total += time;
				}
			}
		}

		start = clock();
		for (int x = 0; x < top_k; x++) {

			if (persin.size() == 0)
				break;
			if (i < persin.size() - 1) {
				persin.pop();
			} else {
				break;
			}
		}
		finish = clock();
		time = (double(finish) - double(start)) / CLOCKS_PER_SEC;
		total += time;

		delete[] acc;
	}
	cout << total << "," << ds->getMemSize() << endl;
}

void executeQueries(Dualsorted* ds, const char* queries, int query_type) {

	int top_k = 10;
	ifstream qfile;
	qfile.open(queries);
	string filter = " (),:.;\t\"\'!?-_\0\n[]=#{}";
	string str;
	getline(qfile, str);
	uint total_queries = atoi(str.c_str());
	cout << "total_queries = " << total_queries << endl;
	string ** qterms = new string*[total_queries];
	uint *qsize = new uint[total_queries];
	uint i = 0;
	while (qfile.good()) {
		vector<string> q;
		getline(qfile, str);
		if (str.length() >= 0) {
			transform(str.begin(), str.end(), str.begin(), ::tolower);
			Tokenize(str, q, filter);

			qsize[i] = q.size();
			qterms[i] = new string[q.size()];
			for (int j = 0; j < q.size(); j++)
				qterms[i][j] = q[j];
		}
		i++;
	}
	/*for (int i = 0 ; i < total_queries;i++)
	 {
	 cout << "query " << i;
	 for (int j = 0;j<qsize[i];j++)
	 {
	 cout << qterms[i][j] ;
	 cout << " " << qterms[i][j].c_str() << " ";
	 }
	 cout << endl;
	 }*/

	if (query_type == 1)
		executeOR(ds, qterms, qsize, total_queries);
	if (query_type == 0)
		executePersin(ds, qterms, qsize, top_k, total_queries);
	if (query_type == 2)
		executeAND(ds, qterms, qsize, total_queries);
	if (query_type == 3)
		executeANDPersin(ds, qterms, qsize, top_k, total_queries);
}

int main(int argc, char** argv) {
	if (argc < 2) {
		cout << "usage: ./dualsorted <inverted list>"
				" <inverted list w/freqs > <vocabulary> "
				"<doclens> <queries>" << endl;
		return 0;
	}
	const char* invlist = argv[1];
	const char* invlistfreq = argv[2];
	const char* vocab = argv[3];
	const char* doclens_file = argv[4];
	const char* queries = argv[5];
	int mode = atoi(argv[6]);
	//cout << "Invlist = " << invlist << endl;
	//cout << "Invlist w/freq =" << invlistfreq << endl;
	//cout << "vocab = " << vocab << endl;
	//cout << "queries " << queries << endl;
	ifstream wordsfile;
	wordsfile.open(vocab);
	vector<string> words;
	string line;

	while (wordsfile.good()) {
		getline(wordsfile, line);
		words.push_back(line);
	}
	uint count = words.size();
	//cout << "count = " << count << endl;
	wordsfile.close();

	ifstream docfile;
	docfile.open(invlist);
	vector<vector<int> > result;
	vector<string> strs;

	while (docfile.good()) {
		vector<int> r;
		getline(docfile, line);
		boost::split(strs, line, boost::is_any_of(" "));
		int doc_size = atoi(strs[0].c_str());
		for (int i = 1; i < strs.size() - 1; i++) {
			r.push_back(atoi(strs[i].c_str()));
		}
		result.push_back(r);
	}
	docfile.close();

	ifstream freqfile;
	docfile.open(invlistfreq);
	vector<int> freqs;
	vector<string> strs2;
	while (docfile.good()) {
		getline(docfile, line);
		boost::split(strs2, line, boost::is_any_of(" "));
		int freq_size = atoi(strs2[0].c_str());
		for (int i = 1; i < strs2.size() - 1; i++) {
			freqs.push_back(atoi(strs2[i].c_str()));
		}
	}

	string sfilter = " \n\0";
	size_t ndocuments;
	size_t *doclens;
	ifstream doclen_data(doclens_file);
	//	cout << "constructing doc_lens data..." << endl;
	if (doclen_data.is_open()) {
		getline(doclen_data, line);
		//	cout << "line = " << line << endl;
		ndocuments = atoi(line.c_str());
		doclens = new size_t[ndocuments];
		for (int i = 0; i < ndocuments; i++) {
			vector<string> doc_data;
			getline(doclen_data, line);
			//			cout << "line = " << line << endl;
			Tokenize(line, doc_data, sfilter);
			doclens[atoi(doc_data[0].c_str())] = atoi(doc_data[1].c_str());
		}
	} else {
		cout << "couldn't open file " << doclens_file << endl;
	}

	docfile.close();
	words.pop_back();
	result.pop_back();

	Dualsorted *ds = new Dualsorted(words, result, freqs, words.size(), doclens,
			ndocuments);
	executeQueries(ds, queries, mode);
//	executeQueries(ds,queries,1);
}
