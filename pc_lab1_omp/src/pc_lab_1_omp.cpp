//============================================================================
// Name        : pc_lab_1.cpp
// Author      : efti-nile
// Version     :
// Copyright   : WTFPL
// Description : 
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <omp.h>
using namespace std;

int main(int argc, char *argv[]) {
	int i, N;
	double *A, *B, *C;

	if(argc < 2){
		cout << "specify length of vectors as first arg." << endl;
		exit(1);
	}else{
		N = strtol(argv[1], NULL, 10);
		if(N < 1){
			cout << "length of vectors less than one OR non-numerical first arg." << endl;
			exit(1);
		}
	}

	A = new double[N];
	B = new double[N];
	C = new double[N];

	if( A == NULL || B == NULL || C == NULL){
		cout << "Problem allocating memory" << endl;
		if(A != NULL) delete A;
		if(B != NULL) delete B;
		if(C != NULL) delete C;
		exit(1);
	}

	cout << "Allocated in heap: " << sizeof(double) * N * 3 / (1024.0 * 1024.0) << " MiB" << endl;

	// First use OpenMP for loop and check exec. time
	clock_t begin = clock();
#pragma omp parallel for shared(N, A, B, C)
	for(int i = 0; i < N; ++i){
		// Variant number 2
		A[i] = i*sin(i*M_PI/N);
		B[i] = i/pow(N, 2) + pow(N, 2)/(i+1);
		C[i] = A[i] * B[i];
	}
	clock_t exec_time_omp = clock() - begin;

	// Just for curiosity do the same loop in one thread and check exec. time
	begin = clock();
	for(i = 0; i < N; ++i){
		// Variant number 2
		A[i] = i*sin(i*M_PI/N);
		B[i] = i/pow(N, 2) + pow(N, 2)/(i+1);
		C[i] = A[i] * B[i];
	}
	clock_t exec_time_naive = clock() - begin;

	cout << "First 4 elements:" << endl;
	for(i = 0; i < 4 && i < N; i++){
		cout << fixed \
			 << "A[" << i << "] = " << A[i] \
			 <<	"   B[" << i << "] = " << B[i] \
			 << "   C[" << i << "] = " << C[i] << endl;
	}

	cout << "Executed for N = " << N << " in " \
		 << exec_time_omp * 1000 / CLOCKS_PER_SEC << "ms for OMP-for and in "\
		 << exec_time_naive * 1000 / CLOCKS_PER_SEC << "ms for naive for" << endl;

	delete A;
	delete B;
	delete C;

	return 0;
}
