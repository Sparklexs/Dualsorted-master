#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
//#include "utils.cpp"
#include <algorithm>
#include <google/sparse_hash_map>
#include <WaveletTree.h>
#include <Sequence.h>
#include <Mapper.h>
#include <BitSequenceRG.h>
#include <BitString.h>
#include "partialSums.cpp"
//#include "delta.c"

using namespace std;
using namespace cds_static;
using namespace __gnu_cxx;

class Dualsorted {
private:
	// Main Data Structures

	WaveletMatrix *L; //把所有term对应的docid倒排链链接而成
	BitSequence *st; //term的docid链的bitmap连接图
	CompressedPsums ** ps; //指向每个term对应的frequency倒排链
	google::sparse_hash_map<string, uint> terms; //存储term及termID
	//const char ** terms;
	// Temporal use for construction
	vector<int> freqs;
	vector<vector<int>> result;	//存储每个term对应的docid倒排链

	//	vector<string> terms;

	// Lengths
	uint size_terms;
	size_t L_size;
	int k;

public:
	size_t *doclens;
	size_t ndocuments;
	Dualsorted(vector<string> terms, vector<vector<int> > &result,
			vector<int> &freqs, uint size_terms, size_t *doclens,
			size_t ndocuments);
	BitSequence *buildSt();
	Sequence * buildL();
	void buildSums();

	// Requested function implementations

	//TODO 根据输入的term和该term倒排链内的索引，获取给索引代表的docid
	uint getDocid(string term, uint i);

	//获取给定term在倒排链第i个doc中的频率
	int getFreq(const char*, int i);

	//获取给定term的document frequency
	uint getPostingSize(string term);

	//TODO
	vector<pair<uint, size_t> > mrqq(string term, size_t k, size_t kp);

	//获取给定term对应的那部分倒排链中x到y，起点从0开始，所对应的docid
	vector<uint> range(string t, size_t x, size_t y);

	//FIXME 应该是给定qsizes个查询词，得到其倒排链相交的结果
	//但该函数并没有返回值
	void intersect(string *terms, uint qsizes);

	//返回给定term对应的倒排链从起始位置到i，起点从0开始，对应的docid序列
	vector<uint> getRange(string term, uint i);

	// others

	//获取在给定term对应的倒排链中，docid为d在其中的位置
	int getPosTerm(string t, uint d);

	//获取给定term对应的termID
	uint getTermPosition(const char *t);

	//获取当前DualSorted占用的内存大小（in B）
	size_t getSize();

	void DStest();
};
