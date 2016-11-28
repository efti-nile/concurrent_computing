/*
 ============================================================================
 Name        : pc_lab3_bintree.cpp
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
		cerr << "specify size of local vector as first argument" << endl;
		exit(1);
	} else {
		N = strtol(argv[1], NULL, 10);
		if (N < 1) {
			cerr << "size less than one OR non-numerical first argument"
					<< endl;
			exit(1);
		}
	}

	A = new double[N];
	if (!A) {
		cout << "error allocating memory" << endl;
		return 1;
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size % 2 == 1)
		cerr << "number of processes must be an even number" << endl;

	if (rank == 0)
		time_begin = MPI_Wtime();

	int log2_size = log2(size);
	if ((1 << log2_size) < size)
		log2_size++;

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

	for (int i = 1; i <= log2_size; ++i) {
		int r = rank % (1 << i);
		if (r == 1 << (i - 1)) {
#ifdef __DBG
			cout << rank << "->" << rank - (1 << (i - 1)) << endl;
#endif
			MPI_Send((unsigned char *) &dmin, sizeof(double), MPI_DOUBLE,
					rank - (1 << (i - 1)), i,
					MPI_COMM_WORLD);
#ifdef __DBG
			cout << rank << "<-" << rank - (1 << (i - 1)) << endl;
#endif
			MPI_Recv((unsigned char *) &dmin, sizeof(double), MPI_DOUBLE,
					rank - (1 << (i - 1)), i + log2_size,
					MPI_COMM_WORLD, &st);
			break;
		} else if (r == 0) {
			if (rank + (1 << (i - 1)) < size) {
#ifdef __DBG
				cout << rank << "<-" << rank + (1 << (i - 1)) << endl;
#endif
				MPI_Recv((unsigned char *) &buf, sizeof(double), MPI_DOUBLE,
						rank + (1 << (i - 1)), i,
						MPI_COMM_WORLD, &st);
				dmin = min(buf, dmin);
			}
		}

	}

	for (int i = log2_size; i != 0; --i) {
		int r = rank % (1 << i);
		if (r == 0) {
			if (rank + (1 << (i - 1)) < size) {
#ifdef __DBG
				cout << rank << "->" << rank + (1 << (i - 1)) << endl;
#endif
				MPI_Send((unsigned char *) &dmin, sizeof(double), MPI_DOUBLE,
						rank + (1 << (i - 1)), i + log2_size,
						MPI_COMM_WORLD);
			}
		}
	}

	cout << "Process " << rank << " result:" << dmin << endl;
	if (rank == 0)
		cout << "Executed in " << MPI_Wtime() - time_begin << " s" << endl;

	MPI_Finalize();

	delete A;
	return 0;
}
