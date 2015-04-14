/*
 * omptest.cpp
 *
 *  Created on: 2015年3月26日
 *      Author: John
 */

#include "Dualsorted.cpp"
#include <queue>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include "parse_invlist.cpp"
using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;
template<class T = microsec_clock>
class MyTimer {
private:
	ptime m_startTime;
	ptime m_endTime;
public:
	MyTimer() {
		Restart();
	}
	void Restart() {
		m_startTime = T::local_time();
	}

	ptime Elapsed() {
		m_endTime = T::local_time();
		cout << m_endTime - m_startTime << endl;
	}
};

class SerializedDS {
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive &ar, const unsigned int version) {
	}
public:
	Dualsorted ds;

};

void save(vector<string> words, vector<vector<int> > result, vector<int> freqs,
		size_t *doclens, size_t ndocuments) {

	stringstream txtStream;
	stringstream xmlStream;
	stringstream binStream;
	int max = 1;
	MyTimer<microsec_clock> t;
	vector<size_t> vdoclens(doclens, doclens + ndocuments);

	//txt使用时间
//	cout << "txt序列化时间： ";
//	t.Restart();
//	std::ofstream file1("archive.txt");
//	boost::archive::text_oarchive txta(txtStream);
//	for (int i = 0; i < max; ++i) {
//		txta & BOOST_SERIALIZATION_NVP(words) & BOOST_SERIALIZATION_NVP(result)
//		& BOOST_SERIALIZATION_NVP(freqs) & BOOST_SERIALIZATION_NVP(vdoclens);
//		//		cout << "size:" << txtStream.tellg() << endl;
//	}
//	t.Elapsed();

	cout << "txt反序列化时间： ";
	std::ifstream file1("archive.txt");
	t.Restart();
	for (int i = 0; i < max; ++i) {
		words.clear();
		result.clear();
		freqs.clear();
		vdoclens.clear();

		boost::archive::text_iarchive ia(file1);
		ia & BOOST_SERIALIZATION_NVP(words);
		ia & BOOST_SERIALIZATION_NVP(result);
		ia & BOOST_SERIALIZATION_NVP(freqs);
		ia & BOOST_SERIALIZATION_NVP(vdoclens);
	}
	t.Elapsed();

	//xml使用时间
//	cout << "xml序列化时间： ";
//	t.Restart();
//	std::ofstream file("archive.xml");
//	boost::archive::xml_oarchive oa(xmlStream);
//	for (int i = 0; i < max; ++i) {
//		oa & BOOST_SERIALIZATION_NVP(words) & BOOST_SERIALIZATION_NVP(result)
//		& BOOST_SERIALIZATION_NVP(freqs) & BOOST_SERIALIZATION_NVP(vdoclens);
////		cout << "size:" << xmlStream.tellg() << endl;
//	}
//	t.Elapsed();

//	cout << "xml反序列化时间： ";
//	std::ifstream file("archive.xml");
//	t.Restart();
//	for (int i = 0; i < max; ++i) {
//		words.clear();
//		result.clear();
//		freqs.clear();
//		vdoclens.clear();
//
//		boost::archive::xml_iarchive ia(file);
//		ia & BOOST_SERIALIZATION_NVP(words);
//		ia & BOOST_SERIALIZATION_NVP(result);
//		ia & BOOST_SERIALIZATION_NVP(freqs);
//		ia & BOOST_SERIALIZATION_NVP(vdoclens);
////		cout<<words[i]<<endl;
//	}
//	t.Elapsed();

//binary使用时间
//	cout << "bin序列化时间： ";
//	t.Restart();
//	std::ofstream file2("archive.dat");
//	boost::archive::binary_oarchive bina(binStream);
//	for (int i = 0; i < max; ++i) {
//		bina & BOOST_SERIALIZATION_NVP(words) & BOOST_SERIALIZATION_NVP(result)
//		& BOOST_SERIALIZATION_NVP(freqs) & BOOST_SERIALIZATION_NVP(vdoclens);
////		cout << "size:" << binStream.tellg() << endl;
//	}
//	t.Elapsed();

	cout << "bin反序列化时间： ";
	std::ifstream file2("archive.dat");
	t.Restart();
	for (int i = 0; i < max; ++i) {
		words.clear();
		result.clear();
		freqs.clear();
		vdoclens.clear();

		boost::archive::binary_iarchive ia(file2);
		ia & BOOST_SERIALIZATION_NVP(words);
		ia & BOOST_SERIALIZATION_NVP(result);
		ia & BOOST_SERIALIZATION_NVP(freqs);
		ia & BOOST_SERIALIZATION_NVP(vdoclens);
	}
	t.Elapsed();
}
void load() {

}
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

	save(words, result, freqs, doclens, ndocuments);

	return new Dualsorted(words, result, freqs, words.size(), doclens,
			ndocuments);
	//return *ds;
	//ds->DStest();
}

int main(int argc, char** argv) {
//		BitString *bs=new BitString(4655778182);
//		bs->setBit(4294967299);
//		bs->setBit(4294967302);
//		BitSequenceRG *bsrg = new BitSequenceRG(*bs, 2);
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

	return 0;
}
