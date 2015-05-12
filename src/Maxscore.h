/*
 * Maxscore.h
 *
 *  Created on: 2015年4月26日
 *      Author: John
 */

#ifndef SRC_MAXSCORE_H_
#define SRC_MAXSCORE_H_
#include <sys/types.h>

namespace Early_Termination {
class Maxscore {
private:
	uint *pages;
	inline void evaluate();
	inline bool find_smallest_did_in_essential_set();
	inline void find_new_essential_lists();
public:
	Maxscore(uint* pgs):pages(pgs){	}
	void operator()();
};
}

#endif /* SRC_MAXSCORE_H_ */
