/*
 * utils.h
 *
 *  Created on: 2015年4月25日
 *      Author: John
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;

template<class CL = microsec_clock>
class MyTimer {
private:
	ptime m_startTime;
	ptime m_endTime;
public:
	MyTimer() {
		Restart();
	}
	void Restart() {
		m_startTime = CL::local_time();
	}

	ptime Elapsed() {
		m_endTime = CL::local_time();
		cout << m_endTime - m_startTime << endl;
	}
};
// auxiliary functions
inline int msb(uint v) {
	int count = 0;
	while (v >>= 1 & 1) {
		count++;
	}
	return count;
}
vector<string> getFiles(string dir);

void Tokenize(const string& str, vector<string>& tokens,
		const string& delimiters = " ");

#endif /* SRC_UTILS_H_ */
