#ifndef _PARTIALSUMS_CPP_
#define _PARTIALSUMS_CPP_
#include "partialSums.h"

// auxiliary functions
int msb(uint v) {
	int count = 0;
	while (v >>= 1 & 1) {
		count++;
	}
	return count;
}

uint *sort(uint *a, uint n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (a[i] < a[j]) {
				uint aux = a[i];
				a[i] = a[j];
				a[j] = aux;
			}
		}
	}
	return a;
}
void PStest() {

	//rand() % 10 + 1;
	int n = 100;
	int k = 1;
	int m = 1000;
	uint *A = new uint[n];
	uint *B = new uint[n];
	srand(time(NULL));

	for (uint i = 0; i < n; i++) {
		A[i] = rand() % m;
	}

	A = sort(A, n);
	cout << "entre" << endl;
	for (int k = 5; k < 100; k += 5) {
		cout << "testing k = " << k << endl;
		CompressedPsums * ps = new CompressedPsums(A, n, k, encodeGamma,
				decodeGamma);

		ps->encode();
		cout << "size = " << ps->getSize() << endl;

		for (int i = 0; i < n; i++) {
			B[i] = ps->decode(i);
			//cout << B[i] << endl;
		}

		for (int i = 0; i < n; i++) {
			//	cout << "A[" << i << "]" << "=?" << "B[" << i << "]" << "(" << A[i] << " != ? " << B[i] << ")" <<  endl;
			if (A[i] != B[i]) {
				cout << "Error: i = " << i << " A != B ( " << A[i] << " != "
						<< B[i] << " ) " << endl;
				exit(0);
			}
		}
	}
}
#endif
