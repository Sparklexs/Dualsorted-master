/*
 * omptest.cpp
 *
 *  Created on: 2015年3月26日
 *      Author: John
 */

#include "Dualsorted.cpp"
#include <queue>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
//#include "parse_invlist.cpp"
//#include "utils.cpp"
using namespace std;
void save(vector<string> words, vector<vector<int> > result, vector<int> freqs,
		uint *doclens, size_t ndocuments);
Dualsorted* ReadFiles(char** argv) {
	const char* invlist = argv[1];
	const char* invlistfreq = argv[2];
	const char* vocab = argv[3];
	const char* doclens_file = argv[4];

	string line;

	//读取词典文件
	ifstream wordsfile;
	wordsfile.open(vocab);
	vector<string> words;

	while (wordsfile.good()) {
		getline(wordsfile, line);
		words.push_back(line);
	}
//	uint count = words.size();
//	for (int i = 0; i < count; i++)
//		cout << i << ":" << words[i] << endl;
	//最后一行为空白，需要弹出
	words.pop_back();
	wordsfile.close();

	//读取倒排链
	ifstream docfile;
	docfile.open(invlist);
	vector<vector<int> > result;
	vector<string> strs;
//	int times = 1;
	while (docfile.good()) {
		vector<int> r;
		getline(docfile, line);
		boost::split(strs, line, boost::is_any_of(" "));
		int doc_size = atoi(strs[0].c_str());
		for (int i = 1; i <= doc_size; i++)
			r.push_back(atoi(strs[i].c_str()));
//		cout << times++ << "r.size():" << r.size() << endl;
//		for (int i = 0; i < r.size(); i++)
//			cout << r[i] << endl;

		result.push_back(r);
	}
	//最后一行为空白，弹出最后一行
	result.pop_back();
	docfile.close();

	//读取词频
	ifstream freqfile;
	docfile.open(invlistfreq);
	vector<int> freqs;
	vector<string> strs2;
	while (docfile.good()) {
		getline(docfile, line);
		boost::split(strs2, line, boost::is_any_of(" "));
		int freq_size = atoi(strs2[0].c_str());
		for (int i = 1; i <= freq_size; i++)
			freqs.push_back(atoi(strs2[i].c_str()));
	}

	//这里最后一行虽然是空白，但是i从1开始
	//并没有压入freqs，所以不再弹出
	freqfile.close();

	//读取doclens
	string sfilter = " \n\0";
	size_t ndocuments;
	uint *doclens;
	ifstream doclen_data(doclens_file);
	//	cout << "constructing doc_lens data..." << endl;
	if (doclen_data.is_open()) {
		getline(doclen_data, line);
		//	cout << "line = " << line << endl;
		ndocuments = atoi(line.c_str());
		doclens = new uint[ndocuments];
		for (int i = 0; i < ndocuments; i++) {
			vector<string> doc_data;
			getline(doclen_data, line);
			//cout << "line = " << line << endl;
			Tokenize(line, doc_data, sfilter);
			doclens[atoi(doc_data[0].c_str())] = atoi(doc_data[1].c_str());
		}
	} else {
		cout << "couldn't open file " << doclens_file << endl;
	}
	//这里由于限定了只读取ndocuments行，不需要弹出
	docfile.close();

	//保存读取的数据为二进制文件
	save(words, result, freqs, doclens, ndocuments);
	//返回生成的DS
	return new Dualsorted(words, result, freqs, words.size(), doclens,
			ndocuments);
}

void save(vector<string> words, vector<vector<int> > result, vector<int> freqs,
		uint *doclens, size_t ndocuments) {
	MyTimer<microsec_clock> t;
	vector<uint> vdoclens(doclens, doclens + ndocuments);

//binary使用时间
	cout << "bin序列化时间: ";
	t.Restart();
	std::ofstream file("./serialization/vectors.dat");
	boost::archive::binary_oarchive bina(file);
	bina & BOOST_SERIALIZATION_NVP(words) & BOOST_SERIALIZATION_NVP(result)
	& BOOST_SERIALIZATION_NVP(freqs) & BOOST_SERIALIZATION_NVP(vdoclens);
	t.Elapsed();

	//另外存储给Dualsorted恢复使用
	std::ofstream file1("./serialization/words.dat");
	boost::archive::binary_oarchive bina1(file1);
	bina1 & BOOST_SERIALIZATION_NVP(words);
	std::ofstream file2("./serialization/doclens.dat");
	boost::archive::binary_oarchive bina2(file2);
	bina2 & BOOST_SERIALIZATION_NVP(vdoclens);

}

Dualsorted* load() {
	MyTimer<microsec_clock> t;
	cout << "bin反序列化时间: ";
	std::ifstream file2("./serialization/vectors.dat");
	t.Restart();
	vector<string> words;
	vector<vector<int> > result;
	vector<int> freqs;
	vector<uint> vdoclens;

	boost::archive::binary_iarchive ia(file2);
	ia & BOOST_SERIALIZATION_NVP(words);
	ia & BOOST_SERIALIZATION_NVP(result);
	ia & BOOST_SERIALIZATION_NVP(freqs);
	ia & BOOST_SERIALIZATION_NVP(vdoclens);
	uint * doclens = new uint[vdoclens.size()];
	memcpy(doclens, &vdoclens[0], vdoclens.size());
	t.Elapsed();

	return new Dualsorted(words, result, freqs, words.size(), doclens,
			vdoclens.size());
}

Dualsorted* loadForGOV2() {

	MyTimer<microsec_clock> t;
	vector<string> words;
	vector<vector<int> > result;
	vector<int> freqs;
	vector<uint> vdoclens;

	cout << "bin反序列化时间: ";
	//还原words
	ifstream file("./serialization/words.dat");
	boost::archive::binary_iarchive ia(file);
	ia & BOOST_SERIALIZATION_NVP(words);
	//还原result
	ifstream file1("./serialization/result.dat");
	boost::archive::binary_iarchive ia1(file1);
	ia1 & BOOST_SERIALIZATION_NVP(result);
	//还原freqs
	ifstream file2("./serialization/freqs.txt");
	boost::archive::text_iarchive ia2(file2);
	ia2 & BOOST_SERIALIZATION_NVP(freqs);
	//还原doclens
	ifstream file3("./serialization/doclens.dat");
	boost::archive::binary_iarchive ia3(file3);
	ia3 & BOOST_SERIALIZATION_NVP(vdoclens);
	uint * doclens = new uint[vdoclens.size()];
	memcpy(doclens, &vdoclens[0], vdoclens.size());
	t.Elapsed();

	return new Dualsorted(words, result, freqs, words.size(), doclens,
			vdoclens.size());

}

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

// 所有的实现都没有score，只是简单地获取到文档id而已
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

// 所有的实现都没有score，只是简单地获取到文档id而已
inline void executeOR(Dualsorted* ds, string ** terms, uint *qsizes,
		size_t total_queries) {
	// 这个实现没有什么意义，获取每个倒排链的长度，然后在for
	// 循环中自加，而不是直接累加赋值，甚至没有去重
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

//第一次调用时使用的方法，生成archive文件，之后就可以直接调用load加快速度
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
			if (q.size() != 0) {
				qsize[i] = q.size() - 1;
				qterms[i] = new string[q.size() - 1];
				for (int j = 0; j < q.size() - 1; j++)
					qterms[i][j] = q[j + 1];
			}
		}
		i++;
	}
//	for (int i = 0; i < total_queries; i++) {
//		cout << "query " << i;
//		for (int j = 0; j < qsize[i]; j++)
//			cout << " " << qterms[i][j].c_str();
//		cout << endl;
//	}
	executeAND(ds, qterms, qsize, total_queries);
//	if (query_type == 1)
//		executeOR(ds, qterms, qsize, total_queries);
//	if (query_type == 0)
//		executePersin(ds, qterms, qsize, top_k, total_queries);
//	if (query_type == 2)
//		executeAND(ds, qterms, qsize, total_queries);
//	if (query_type == 3)
//		executeANDPersin(ds, qterms, qsize, top_k, total_queries);
}

int main(int argc, char** argv) {
//		BitString *bs=new BitString(4655778182);
//		bs->setBit(4294967299);
//		bs->setBit(4294967302);
//		BitSequenceRG *bsrg = new BitSequenceRG(*bs, 2);
	//测试partialSums类
//	PStest();

//测试parse_invlist类,并行导致输出不可控，已经obsolete
//	vector<string> filelist = getFiles("/cygdrive/d/WorkSpaceC/var");
//	int length = filelist.size();
//	InvList *inv = new InvList(string("/cygdrive/d/WorkSpaceC/var/file.list"),
//			string("song"));

//测试DualSorted类
	//关于读取Dualsorted，如果已经save过，优先读取save的，否则查看
	//是否把vector已经save过了，否则最后才从原始文件一步步初始化
	Dualsorted* ds;
	ifstream ifwm("./serialization/wm.dat");
	if (ifwm.good())
		ds = Dualsorted::load();
	else {
		ifstream ifvec("./serialization/vectors.dat");
		if (ifvec.good())
			ds = load();
		else
			ds = ReadFiles(argv);
		ds->save();
	}
	// 读取查询词
	executeQueries(ds, argv[5], 0);

	ds->DStest();

	return 0;
}
