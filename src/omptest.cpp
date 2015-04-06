/*
 * omptest.cpp
 *
 *  Created on: 2015年3月26日
 *      Author: John
 */

#include <omp.h>

#include "Dualsorted.cpp"
#include <ctime>
#include <queue>
//#include "utils.cpp" //因为parse_invlist中应用了utils.cpp
#include <boost/algorithm/string.hpp>
#include "parse_invlist.cpp"
using namespace std;

Dualsorted* ReadFiles(char** argv) {
	const char* invlist = argv[1];
	const char* invlistfreq = argv[2];
	const char* vocab = argv[3];
	const char* doclens_file = argv[4];

	ifstream wordsfile;
	wordsfile.open(vocab);
	vector<string> words;
	string line;

	//读取词典文件
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
			//cout << "line = " << line << endl;
			Tokenize(line, doc_data, sfilter);
			doclens[atoi(doc_data[0].c_str())] = atoi(doc_data[1].c_str());
		}
	} else {
		cout << "couldn't open file " << doclens_file << endl;
	}
	//这里由于限定了只读取ndocuments行，不需要弹出
	docfile.close();

	return new Dualsorted(words, result, freqs, words.size(), doclens,
			ndocuments);
	//return *ds;
	//ds->DStest();
}

int main(int argc, char** argv) {
	//测试partialSums类
	//PStest();

//测试parse_invlist类,并行导致输出不可控，已经obsolete
//	vector<string> filelist = getFiles("/cygdrive/d/WorkSpaceC/var");
//	int length = filelist.size();
//	InvList *inv = new InvList(string("/cygdrive/d/WorkSpaceC/var/file.list"),
//			string("song"));

//测试DualSorted类
	Dualsorted* ds = ReadFiles(argv);
	ds->DStest();

//	omp_set_num_threads(10);
//#pragma omp parallel
//	cout << "!!!Hello World!!! from thread " << omp_get_thread_num() << endl;

	return 0;
}
