//============================================================================
// Name        : pc_lab_2.cpp
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

double func(double x){
	return 1 / pow(sin(x+1), 2) * tan(x+1);
}

int main(int argc, char *argv[]) {
	int N;

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

	double a = M_PI / 4 - 1, b = M_PI / 3 - 1;
	double step = (b - a) / (N + 1);
	double s = 0.;

	clock_t begin = clock();
#pragma omp parallel for reduction(+:s)
	for(int i = 0; i < N; ++i){
		double x = (i + 0.5) * step;
		s += step * func(x);
	}
	clock_t exec_time_omp = clock() - begin;

	begin = clock();
	double s_naive = 0.;
	for(int i = 0; i < N; ++i){
		double x = (i + 0.5) * step;
		s_naive += step * func(x);
	}
	clock_t exec_time_naive = clock() - begin;

	cout << "Result OMP: " << s << endl << "Result naive:" << s_naive << endl;
	cout << "Exec. time OMP: " << (double)exec_time_omp / CLOCKS_PER_SEC << "s"<< endl \
			<< "Exec. time naive:" << (double)exec_time_naive / CLOCKS_PER_SEC << "s" << endl;

	return 0;
}
