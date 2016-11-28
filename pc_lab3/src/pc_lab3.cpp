/*
 ============================================================================
 Name        : pc_lab3.cpp
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
#include <ctime>

using namespace std;

double *A;

int main(int argc, char *argv[]) {
	int N, size, rank;
	MPI_Status st1, st2;
	double time_begin;

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

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		time_begin = MPI_Wtime();
	}

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	cout << "Hi! I'm procces " << rank << " of " << size - 1 << endl;

	A = new double[N];
	if (!A) {
		cout << "Process " << rank << ": trouble in allocating memory" << endl;
		return 1;
	}

	srand(time(NULL) * rank); // Set different seed for every process
	for (int j = 0; j < N; ++j) {
		A[j] = -1.0
				+ static_cast<double>(rand())
						/ static_cast<double>(RAND_MAX / 2); // random double number from -1 to 1
	}

	double min = A[0];
	for (int j = 1; j < N; ++j) {
		if (A[j] < min) {
			min = A[j];
		}
	}

	cout << "Process " << rank << " local result: " << min << endl;

	if (size > 1) {
		if (rank == 0) {
			MPI_Send((unsigned char *) &min, sizeof(double), MPI_DOUBLE, 1, 0,
			MPI_COMM_WORLD);
			MPI_Recv((unsigned char *) &min, sizeof(double), MPI_DOUBLE, 1, 1,
			MPI_COMM_WORLD, &st1);
		} else {
			double buf;
			MPI_Recv((unsigned char *) &buf, sizeof(double), MPI_DOUBLE,
					rank - 1, 0,
					MPI_COMM_WORLD, &st1);
			if (buf < min) {
				min = buf;
			}
			if (rank != size - 1) {
				MPI_Send((unsigned char *) &min, sizeof(double), MPI_DOUBLE,
						rank + 1, 0,
						MPI_COMM_WORLD);
				MPI_Recv((unsigned char *) &min, sizeof(double), MPI_DOUBLE,
						rank + 1, 1,
						MPI_COMM_WORLD, &st2);
				MPI_Send((unsigned char *) &min, sizeof(double), MPI_DOUBLE,
						rank - 1, 1,
						MPI_COMM_WORLD);
			} else {
				MPI_Send((unsigned char *) &min, sizeof(double), MPI_DOUBLE,
						rank - 1, 1,
						MPI_COMM_WORLD);
			}
		}
	}

	cout << "Process " << rank << " result:" << min << endl;
	if( rank == 0 ){
		cout << "Executed in " << MPI_Wtime() - time_begin << " s" << endl;
	}

	delete A;

	MPI_Finalize();
	return 0;
}

