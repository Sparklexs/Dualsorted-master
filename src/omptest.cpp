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
//第一次调用时使用的方法，生成archive文件，之后就可以直接调用load加快速度
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
	ds->DStest();

	return 0;
}
