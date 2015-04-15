#include "Dualsorted.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

using namespace cds_static;

//a是有序字符串，二分查找字符s
inline uint search(const char** a, const char* s, uint n) {
	uint l = 0;
	uint r = n - 1;

	while (l <= r) {
		uint m = (l + r) / 2;
		if (m >= 0 && m < n) {
			if (strcmp(s, a[m]) == 0)
				return m;
			else if (strcmp(s, a[m]) > 0)
				l = m + 1;
			else
				r = m - 1;
		} else
			return -1;
	}
	return -1;
}

//这里的freqs文件是所有的frequency倒排文件链接而成的，可能会很大
Dualsorted::Dualsorted(vector<string> words, vector<vector<int> > &result,
		vector<int> &freqs, size_t size_terms, uint *doclens,
		size_t ndocuments) {
	// cout << "Amount of terms: " << size_terms << endl;

	for (uint i = 0; i < words.size(); i++) {
		//cout << vocab[i] << ": " << i << endl;
		this->terms[words[i]] = i;
	}
	//使用完立即释放
	words.clear();
	//vocab.shrink_to_fit();
	//   cout << "searching" << endl;
	//  cout << search(this->terms,"wikipedia",size_terms) << endl;

	this->doclens = doclens;
	this->ndocuments = ndocuments;

	this->result = result;
	this->freqs = freqs;
	this->size_terms = size_terms;

	this->k = 10;

	cout << "Building St...";
	clock_t start, finish;
	start = clock();
	this->buildSt();
	finish = clock();
	cout << "Done!  time elapsed: "
			<< (double) (finish - start) / CLOCKS_PER_SEC << endl;
	cout << "Building PartialSums...";
	start = clock();
	this->buildSums();
	this->freqs.clear();
	finish = clock();
	cout << "Done!  time elapsed: "
			<< (double) (finish - start) / CLOCKS_PER_SEC << endl;
	cout << "Building L...";
	start = clock();
	this->buildL();
	this->result.clear();
	finish = clock();
	cout << "Done!  time elapsed: "
			<< (double) (finish - start) / CLOCKS_PER_SEC << endl;
}

// Fix this
size_t Dualsorted::getMemSize() {
	size_t size = 0;
	long double size_ps = 0;

	double sparse_tt = 0;
	double sparse_rt = 0;
	double len_tt = 0;
	double len_rt = 0;

//	cout << "L size: " << this->L->getSize() << endl;
//	cout << "st size: " << this->st->getSize() << endl;
	size += this->L->getSize();
	size += this->st->getSize();

	for (uint i = 0; i < this->size_terms - 1; i++) {
		size_ps += this->ps[i]->getSize();
	}
//	cout << "partial_sums: " << size_ps/8 << endl;
	size += size_ps / 8;
//	cout << "Total size: " << size << endl;
//	cout << "Total size (MB): " << size/(1024*1024) << endl;
	return size;
}

uint Dualsorted::getDocidOfPosting(string term, uint i) {
	uint f = this->getTermID(term.c_str());
	uint end, start;
	(f != this->size_terms - 1) ?
			end = this->st->select1(f + 2) - 1 : end = this->L_size - 1;
	start = this->st->select1(f + 1);
	return this->L->access(start + i);
//	return this->L->rangeFromTo(start, end, i);
}

int Dualsorted::getPosOfDoc(string t, uint d) {
	uint f = this->getTermID(t.c_str());

	//if (f == 0)
	//return 0; // FIXME 为什么0就要退出，已修正
	size_t start = this->st->select1(f + 1);
	/**
	 * rank(d, start - 1)表示从0到start-1，docid“d”出现的次数
	 * L->select(d, 1 + this->L->rank(d, start - 1))表示整个L里
	 * “d”第1 + this->L->rank(d, start - 1)次出现的位置，也就是针对
	 * term的倒排链第一次出现的位置，故还要减去start
	 * FIXME，已修正，这里并没有考虑d不在该term中的情况，如果select返回值超过了
	 * 该term的倒排链长，那么说明term没有出现该doc中，返回-1
	 */
	size_t end =
			(f != this->size_terms - 1) ?
					this->st->select1(f + 2) - 1 : this->L_size - 1;
	uint pos = this->L->select(d, 1 + this->L->rank(d, start - 1)) - start;
	return pos > end - start ? -1 : pos;
	//return this->L->select(d, 1 + this->L->rank(d, start - 1)) - start;
}
uint Dualsorted::getPostingSize(string term) {
	uint f = this->getTermID(term.c_str());
	if (f == -1)
		return 0;
	size_t end, start;
	(f != this->size_terms - 1) ?
			end = this->st->select1(f + 2) - 1 : end = this->L_size - 1;
	start = this->st->select1(f + 1);
	return end - start + 1;
}

int Dualsorted::getFreqOfPosting(const char* term, int i) {
	uint j = this->getTermID(term);
	if (j >= 0)
		return this->ps[j]->decode(i);
	else
		return -1;
	//FIXME，这里term"0"对应的id恰好是0，已修正
//	if (j == 0) {
//		return 0;
//	}
////	cout << "term = " << term << endl;
////	cout << "i = " << i << endl;
//	if (j != -1 || j != 0) {
//		if (j > 0)
//			return this->ps[j - 1]->decode(i);
//	} else {
//		return 0;
//	}
//	return 0;
}

uint Dualsorted::getTermID(const char *t) {
	//return search(this->terms,t,this->size_terms);
	//cout << "received = " << t << endl;
	string a(t);
	//cout << "returing = " << this->terms[a] << endl;
	return this->terms[a];
}

vector<uint> Dualsorted::rangeFromTo(string term, size_t x, size_t y) {
	//cout << "searching for: " << term << endl;
	uint f = this->getTermID(term.c_str());
	//cout << "Executing select" << endl;
	// cout << "f = " << f << endl;

	//FIXME 仅限于32位，并没有考虑x和y超过term的倒排链范围
//	if (f == -1 || f == 0 || f > 4294967290) {
//		vector<uint> blank;
//		return blank;
//	}
	size_t start = this->st->select1(f + 1);

	return this->L->range_report_aux(start + x, start + y);
}

vector<uint> Dualsorted::rangeTo(string term, uint i) {
	uint f = this->getTermID(term.c_str());
	cout << "f = " << f << endl;
	size_t end, start;
	(f != this->size_terms - 1) ?
			end = this->st->select1(f + 2) - 1 : end = this->L_size - 1;
	//第一个term的起始位置在st中也是置1
	start = this->st->select1(f + 1);
	//if (end-start < 2)
	//	return vector<uint>();

	if (start + i < end + 1) {
		cout << "start = " << start << endl;
		cout << "end = " << start + i << endl;
		cout << "entre 1 !!!" << endl;
		return this->L->range_report_aux(start - 1, start + i - 1);
	} else {
		cout << "start = " << start << endl;
		cout << "end = " << end << endl;
		cout << "entre 2 !!!" << endl;
		return this->L->range_report_aux(start, end);
	}
}

inline vector<uint> Dualsorted::intersect(string *terms, uint qsizes) {
	//cout << "BLABLA" << endl;
	size_t *start = new size_t[qsizes];
	size_t *end = new size_t[qsizes];
	for (uint i = 0; i < qsizes; i++) {
		uint f = this->getTermID(terms[i].c_str());
		//XXX 没必要等于0时退出 ,已修正
//		if (f == 0)
//			return;
		cout << "term = " << terms[i] << endl;
		cout << "pos = " << f << endl;

		start[i] = this->st->select1(f + 1);
		if (f != this->size_terms - 1)
			end[i] = this->st->select1(f + 2) - 1;
		else
			end[i] = this->L_size - 1;

//		cout << "start[" << i << "] = " << start[i] << endl;
//		cout << "end[" << i << "] = " << end[i] << endl;
	}
//	cout << "start-end = " << end[0] - start[0] << endl;
	//size_t x_start,size_t x_end,size_t y_start, size_t y_end)
	//this->L->range_report_aux(start[0],end[0]);
	//this->L->range_intersect_aux(start[0],end[0],start[1],end[1]);
	return this->L->n_range_intersect_aux(start, end, qsizes);
}
/*vector < pair<uint,size_t> > Dualsorted::mrqq(string term, size_t k, size_t kp)
 {
 uint f = this->getTermID(term.c_str());
 uint end,start;
 (f != this->size_terms-1) ? end = this->st->select1(f+2)-1 : end = this->L_size-1;
 start = this->st->select1(f+1);
 return this->L->range_report_aux(start,end);
 return NULL
 }*/

BitSequence *Dualsorted::buildSt() {
	size_t m2a = 0;	//m2a表示所有term的倒排链长度之和
	// FIXME 仅限于32位长度 ，已修正
	//但是大数据集的倒排链很长，uint在这里装得下吗？
	for (int i = 0; i < this->size_terms; i++) {
		m2a = m2a + result[i].size();
	}
	this->L_size = m2a;
	//bs即表示整个m2a的长度，并在每个term位置置1
	//注意，这里给第0个位置也置1了
	BitString *bs = new BitString(m2a);
	m2a = 0;
	for (int i = 0; i < this->size_terms; i++) {
		bs->setBit(m2a);
		m2a = m2a + result[i].size();
	}

	BitSequenceRG *bsrg = new BitSequenceRG(*bs, 2);
	this->st = bsrg;

	return this->st;
}
void Dualsorted::buildSums() {
	this->ps = new CompressedPsums*[this->size_terms];
	size_t now, next;
	for (int i = 0; i < this->size_terms - 1; i++) {
		//	cout << "entering 2" << endl;
		//这里没有考虑当i表示倒数第二个term时，他的前面只有1个1,
		//那么这时加2会返回一个0xffffffff
		now = this->st->select1(i + 1);
		next = this->st->select1(i + 2);
		//cout << "i=" << i <<  " term = " << this->terms[i] << endl;

		// FIXME 仅限于32位长度，已修正
		// 不再担心32位溢出
//		if (next > 4294967290)	//超过了32bit长度
//				{
//			cout << "!!!length > 4294967290!!!" << endl;
//			//break;
//		}
		//	cout << "now = " << now << " next = " << next << endl;
		uint f = 0;		//一个term对应的倒排链不会超过32bit
		uint *A;		//A本身存储的词频不会超过32bit
		A = new uint[next - now];
		for (size_t j = now; j < next; j++) {

			A[f] = this->freqs[j];
			//		cout << "A[" << f << "] = " << A[f] << endl;
			f++;

		}
		//		cout << "entering 3" << endl;
		//cout << "i=" << i << endl;
		//对每个term的词频倒排链A(长度f)进行Gamma压缩，块的长度设为10
		ps[i] = new CompressedPsums(A, f, 10, encodeGamma, decodeGamma);
		ps[i]->encode();
		//cout << i << " : " << ps[i]->getSize() << endl;
	}
	//注意for循环只进行了size_terms-1次，而ps的长度则是size_terms个
	//也就是最后一个term并没有被压缩
	size_t end = this->L_size;
	uint f = 0;
	uint *A;
	A = new uint[end - next];
	for (size_t j = next; j < end; j++) {
		A[f] = this->freqs[j];
		f++;
	}

	ps[size_terms - 1] = new CompressedPsums(A, f, 10, encodeGamma,
			decodeGamma);
	ps[size_terms - 1]->encode();
//	cout << endl << "decodificando:" << ps[869]->decode(0) << endl;
}

Sequence * Dualsorted::buildL() {
	uint *sequence = new uint[this->L_size];
	size_t m2a = 0;
//	uint m2a2 = 0;
	for (int i = 0; i < result.size(); i++) {
//		m2a2 += result[i].size();
		for (int j = 0; j < result[i].size(); j++) {
			sequence[m2a] = result[i][j];
			m2a++;
		}
	}
	Array *A = new Array(sequence, this->L_size);
	MapperNone * map = new MapperNone();

//   	BitSequenceBuilder * bsb = new BitSequenceBuilderRRR(50);
	BitSequenceBuilder * bsb = new BitSequenceBuilderRG(2);
	WaveletMatrix* seq = new WaveletMatrix(*A, bsb, map);

	this->L = seq;
	return this->L;

}
void Dualsorted::save() {
	ofstream ofst("./serialization/st.dat");
	this->st->save(ofst);
	ofstream ofwm("./serialization/wm.dat");
	this->L->save(ofwm);

	vector<CompressedPsums> vps;
	for (int i = 0; i < this->size_terms; i++)
		vps.push_back(*(this->ps[i]));
	std::ofstream file("./serialization/Psums.dat");
	boost::archive::binary_oarchive bina1(file);
	bina1 & BOOST_SERIALIZATION_NVP(vps);

}
Dualsorted* Dualsorted::load() {
	Dualsorted* ds = new Dualsorted();

	ifstream ifst("./serialization/st.dat");
	ds->st = BitSequenceRG::load(ifst);

	ifstream ifwm("./serialization/wm.dat");
	ds->L = WaveletMatrix::load(ifwm);

	ds->L_size = ds->L->getLength();
	ds->size_terms = ds->st->countOnes();
	ds->k = 10;

	vector<uint> vdoclens;
	vector<string> words;
	std::ifstream file("./serialization/doclens.dat");
	std::ifstream file1("./serialization/words.dat");
	boost::archive::binary_iarchive ia(file);
	boost::archive::binary_iarchive ia1(file1);

	ia & BOOST_SERIALIZATION_NVP(vdoclens);
	ds->ndocuments = vdoclens.size();
	ds->doclens = new uint[vdoclens.size()];
	memcpy(ds->doclens, &vdoclens[0], vdoclens.size());
	vdoclens.clear();

	ia1 & BOOST_SERIALIZATION_NVP(words);
	for (uint i = 0; i < words.size(); i++) {
		//cout << vocab[i] << ": " << i << endl;
		ds->terms[words[i]] = i;
	}
	words.clear();

	vector<CompressedPsums> vps;
	std::ifstream file2("./serialization/Psums.dat");
	boost::archive::binary_iarchive ia2(file2);
	ia2 & BOOST_SERIALIZATION_NVP(vps);
	ds->ps = new CompressedPsums*[ds->size_terms];
	for (int i = 0; i < vps.size(); i++) {
		ds->ps[i] = new CompressedPsums();
		memcpy(ds->ps[i], &vps[i], sizeof(CompressedPsums));
	}
	vps.clear();
	return ds;
}

void Dualsorted::DStest() {

//	const char*s = "19";
//	cout << this->st->select1(this->getTermID(s) + 1) << endl;

	cout << "Test Begin!" << endl;
	google::sparse_hash_map<string, uint>::iterator it = terms.begin();

	cout << "Testing getTermID" << endl;
	for (; it != terms.end(); it++) {
		cout << (*it).first << ":" << this->getTermID((*it).first.c_str())
				<< endl;
	}

	cout << "Testing getPostingSize" << endl;
	it = terms.begin();
	for (; it != terms.end(); it++) {
		uint postLength = this->getPostingSize((*it).first);
		cout << (*it).first << ":" << postLength << endl;
		for (uint i = 0; i < postLength; i++) {
			cout << this->getFreqOfPosting((*it).first.c_str(), i) << endl;
		}
	}
	cout << "Testing getPostTerm" << endl;
	cout << "getLength:" << this->L->getLength() << endl;
	for (int i = 0; i < 100; i++) {
		cout << this->L->access(i) << endl;
		cout << this->L->select(1, i) << endl;
	}

	cout << this->L->rank(1, 1) << endl;
	cout << this->L->rank(12, 20) << endl;

	it = terms.begin();
	for (; it != terms.end(); it++) {
		uint postLength = this->getPostingSize((*it).first);
//		cout << (*it).first << ":" << postLength << endl;
		if ((*it).second == 0) {
			//cout << this->getPosOfDoc((*it).first, 78) << endl;
			//cout << this->getPosOfDoc((*it).first, 21) << endl;
			//cout << this->getPosOfDoc((*it).first, 79) << endl;

			//vector<uint> ranges = this->rangeFromTo((*it).first, 0, postLength-1);
			vector<uint> ranges = this->rangeTo((*it).first, 16);
			for (int i = 0; i < ranges.size(); i++)
				cout << ranges[i] << endl;
		}
//		for (uint i = 0; i < postLength; i++) {
//			cout << this->getPosOfDoc((*it).first, 78) << endl;
		//cout << this->getFreqOfPosting((*it).first.c_str(), i) << endl;
//	}
	}

	cout << "Testing getSize" << endl;
	cout << this->getMemSize() << endl;

	cout << "Testing intersect" << endl;
	string* strs = new string[3]; /*= malloc(sizeof(string));*/
	strs[0] = "1";
	strs[1] = "10";
	strs[2] = "006";
//	it = terms.begin();
//	for (int i = 0; i < 2; i++) {
//		*(strs + i) = (*it).first;
//		it++;
//	}
	vector<uint> r = this->intersect(strs, 3);
	for (int i = 0; i < r.size(); i++)
		cout << r[i] << endl;

//	string* queries=""
//cout << this->intersect() << endl;
	/*	cout << "testing st.... " << endl;
	 for (uint i = 0 ; i < this->L_size;i++)
	 cout << this->st->access(i) << endl;

	 // Testing Tt
	 uint now = 0;
	 uint next = 0;
	 cout << "testing Tt.... " << endl;

	 for (uint i = 0 ; i < this->size_terms;i++)
	 {
	 cout << this->terms[i] << endl;
	 for (int j = 0 ;j<this->Tt[i]->getLength();j++)
	 {
	 cout << this->Tt[i]->access(j) << endl;
	 }
	 }
	 cout << "testing Rt.... " << endl;
	 for (uint i = 0 ; i < this->size_terms;i++)
	 {
	 cout << this->terms[i] << endl;

	 for (int j = 0 ;j<this->Rt[i]->getLength();j++)
	 {
	 cout << this->Rt[i]->access(j) << endl;
	 }
	 }
	 cout << "testing vt.... " << endl;
	 for (int i = 0 ; i < this->size_terms;i++)
	 {
	 cout << this->vt->access(i) << endl;
	 }
	 cout << "testing L " << endl;
	 for (int i = 0 ; i < L->getLength();i++)
	 {
	 cerr << this->L->access(i) << " ";
	 }
	 cout << "End testing... " << endl;*/
}
