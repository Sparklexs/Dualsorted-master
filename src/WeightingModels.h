/*
 * MB25.h
 *
 *  Created on: 2015年4月25日
 *      Author: John
 */

#ifndef SRC_WEIGHTINGMODELS_H_
#define SRC_WEIGHTINGMODELS_H_
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>
using namespace std;
namespace Early_Termination {

class WeightingModel {
public:
	WeightingModel(char* filePrefix) {
		string statisticPath(filePrefix);
		statisticPath.append(".statistic");
		ifstream insStatistic(statisticPath);
		if (insStatistic.is_open()) {
			string line;
			vector<string> strs;
			char* endptr;
			getline(insStatistic, line);
			boost::split(strs, line, boost::is_any_of(":"));
			averageDocumentLength = strtod(strs[1].c_str(), &endptr);

			getline(insStatistic, line);
			boost::split(strs, line, boost::is_any_of(":"));
			numberOfDocuments = strtod(strs[1].c_str(), &endptr);
			getline(insStatistic, line);
			boost::split(strs, line, boost::is_any_of(":"));
			numberOfPointers = strtod(strs[1].c_str(), &endptr);
			getline(insStatistic, line);
			boost::split(strs, line, boost::is_any_of(":"));
			numberOfTokens = strtod(strs[1].c_str(), &endptr);
			getline(insStatistic, line);
			boost::split(strs, line, boost::is_any_of(":"));
			numberOfUniqueTerms = strtod(strs[1].c_str(), &endptr);
		}
	}
protected:
	double averageDocumentLength;
	double keyFrequency;
	double termFrequency;
	double documentFrequency;
	double numberOfDocuments;
	double numberOfTokens;
	double numberOfUniqueTerms;
	double numberOfPointers;
	double c = 1.0;
private:
};

class BM25: public WeightingModel {
public:
	BM25(char* filePrefix) :
			WeightingModel(filePrefix) {
	}
	double score(double tf, double docLength, double keyfreq,
			double documentFreq) {
		double K = k_1 * ((1 - b) + b * docLength / averageDocumentLength) + tf;
		return (tf * (k_3 + 1) * keyfreq / ((k_3 + keyfreq) * K))
				* log2(numberOfDocuments - documentFreq + 0.5)
				/ (documentFreq + 0.5);
	}
private:
	double k_1 = 1.2;
	double k_3 = 8;
	double b = 0.75;
};
}

#endif /* SRC_WEIGHTINGMODELS_H_ */
