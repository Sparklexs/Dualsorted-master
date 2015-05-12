#ifndef _DUALSORTED_H
#define _DUALSORTED_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <google/sparse_hash_map>
#include <WaveletTree.h>
#include <Sequence.h>
#include <Mapper.h>
#include <BitSequenceRG.h>
#include <BitString.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include "utils.h"
#include "partialSums.h"

//#include "delta.c"
/* 有两个需要说明的问题，st里面把第0个term开始的位置（即0）置1了
 * 另外WM是从低位而不是高位开始判断的，因此叶子的顺序并不是自然数顺序
 */

using namespace std;
using namespace cds_static;
using namespace __gnu_cxx;

class Dualsorted {
private:
	// Main Data Structures
	WaveletMatrix *L; //把所有term对应的docid倒排链链接而成
	BitSequence *st; //term的docid链的bitmap连接图
	CompressedPsums ** ps; //指向每个term对应的frequency倒排链
	//const char ** terms;
	// Temporal use for construction
	vector<int> freqs;
	vector<vector<int>> result;	//存储每个term对应的docid倒排链

	//	vector<string> terms;

	// Lengths
	size_t size_terms;
	size_t L_size;
	int k;

public:
	google::sparse_hash_map<string, uint> terms; //存储term及termID
	uint *doclens;
	size_t ndocuments;
	Dualsorted(vector<string> terms, vector<vector<int> > &result,
			vector<int> &freqs, size_t size_terms, uint *doclens,
			size_t ndocuments);
	Dualsorted() :
			k(10) {
	}
	;
	BitSequence *buildSt();
	Sequence * buildL();
	void buildSums();

	// Requested function implementations

	//保存
	void save();
	static Dualsorted* load();
	//根据输入的term和该term倒排链内的索引，获取给索引代表的docid
	// 注意并没有考虑i越界的问题
	uint getDocidOfPosting(string term, uint i);

	//获取给定term在倒排链第i个doc中的频率
	int getFreqOfPosting(string term, int i);

	//获取给定term的document frequency
	uint getPostingSize(string term);

	//TODO
	vector<pair<uint, size_t> > mrqq(string term, size_t k, size_t kp);

	//获取给定term对应的那部分倒排链中x到y，起点从0开始，所对应的docid
	vector<uint> getRangeFromTo(string t, size_t x, size_t y);

	// 应该是给定qsizes个查询词，得到其倒排链相交的结果
	vector<uint> getIntersection(string *terms, uint qsizes);

	//返回给定term对应的倒排链从起始位置到i，起点从0开始，对应的docid序列
	vector<uint> getRangeTo(string term, uint i);

	// others

	//获取在给定term对应的倒排链中，docid为d在其中的位置
	int getPosOfDocid(string t, uint d);

	//获取给定term对应的termID
	uint getTermID(string);

	//获取当前DualSorted占用的内存大小（in B）
	size_t getMemSize();

	void DStest();
};
#endif
