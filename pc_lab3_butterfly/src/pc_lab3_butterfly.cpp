/*
 ============================================================================
 Name        : pc_lab3_butterfly.cpp
 Author      : efti-nile
 Version     :
 Copyright   : WTFPL
 Description :
 ============================================================================
 */

#include "mpi.h" 
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <math.h>
//#define __DBG

using namespace std;

bool is_pwr2(int n) {
	return (n & (n - 1)) == 0;
}

int log2(int n) {
	int i = 0;
	while (n >>= 1)
		++i;
	return i;
}

double dmin, buf;

int main(int argc, char *argv[]) {
	int N, size, rank;
	MPI_Status st;
	double time_begin;
	double *A;

	if (argc < 2) {
		cout << "specify size of local vector as first argument" << endl;
		exit(1);
	} else {
		N = strtol(argv[1], NULL, 10);
		if (N < 1) {
			cout << "size less than one OR non-numerical first argument"
					<< endl;
			exit(1);
		}
	}

	A = new double[N];
	if (!A) {
		cout << "error allocating memory" << endl;
		exit(1);
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (!is_pwr2(size)) {
		cout << "number of processes must be a power of 2" << endl;
		exit(-1);
	}

	if (rank == 0)
		time_begin = MPI_Wtime();

	int log2_size = log2(size);

	cout << "Hi! I'm procces " << rank << " of " << size - 1 << endl;

	srand(time(NULL) * rank); // Set different seed for every process
	for (int j = 0; j < N; ++j) {
		A[j] = -1.0
				+ static_cast<double>(rand())
						/ static_cast<double>(RAND_MAX / 2); // random double number from -1 to 1
	}

	dmin = A[0];
	for (int j = 1; j < N; ++j) {
		if (A[j] < dmin) {
			dmin = A[j];
		}
	}

	cout << "Process " << rank << " local result: " << dmin << endl;

	for (int d = 1, tag = 1; d < size; d <<= 1, ++tag) {
		int r = rank % (d << 1);
		if (r < d) {
			MPI_Send((unsigned char *) &dmin, sizeof(double), MPI_DOUBLE,
					rank + d, tag, MPI_COMM_WORLD);
			MPI_Recv((unsigned char *) &buf, sizeof(double), MPI_DOUBLE,
					rank + d, tag, MPI_COMM_WORLD, &st);
			dmin = min(buf, dmin);
		} else {
			MPI_Recv((unsigned char *) &buf, sizeof(double), MPI_DOUBLE,
					rank - d, tag, MPI_COMM_WORLD, &st);
			dmin = min(buf, dmin);
			MPI_Send((unsigned char *) &dmin, sizeof(double), MPI_DOUBLE,
					rank - d, tag, MPI_COMM_WORLD);
		}
	};

	cout << "Process " << rank << " result:" << dmin << endl;
	if (rank == 0)
		cout << "Executed in " << MPI_Wtime() - time_begin << " s" << endl;

	MPI_Finalize();

	delete A;
	return 0;
}
