/*
 ============================================================================
 Name        : pc_lab1_mpi.cpp
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

double func(double x) {
	return 1 / pow(sin(x + 1), 2) * tan(x + 1);
}

int main(int argc, char *argv[]) {
	int N, size, rank, chunk, local_chunk;
	MPI_Status st;
	double time_begin;

	if (argc < 2) {
		cout
				<< "specify number of points for numerical integration as first argument"
				<< endl;
		exit(1);
	} else {
		N = strtol(argv[1], NULL, 10);
		if (N < 1) {
			cout
					<< "number of points less than one OR non-numerical first argument"
					<< endl;
			exit(1);
		}
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		clock_t begin = clock();
	}

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	cout << "Hi! I'm procces " << rank << " of " << size << endl;

	chunk = N / size;
	double a = M_PI / 4 - 1, b = M_PI / 3 - 1;
	double step = (b - a) / (N + 1);
	double s = 0.;

	if (rank < size - 1) {
		local_chunk = chunk;
	} else {
		local_chunk = N - chunk * (size - 1);
	}

	for (int j = 0; j < local_chunk; ++j) {
		int i = rank * chunk + j; // global index
		double x = (i + 0.5) * step;
		s += step * func(x);
	}

	cout << "Process " << rank << " result: " << s << endl;

	if (rank == 0) {
		for (int r = 1; r < size; ++r) {
			double buf;
			MPI_Recv((unsigned char *) &buf, sizeof(double), MPI_DOUBLE, r, 0,
					MPI_COMM_WORLD, &st);
			s += buf;
		}
	} else {
		MPI_Send((unsigned char *) &s, sizeof(double), MPI_DOUBLE, 0, 0,
				MPI_COMM_WORLD);
	}

	if (rank == 0) {
		cout << "Total result: " << s << endl;
		cout << "Master process executed for N = " << N << " in "
				<< (clock() - time_begin) * 1000 / CLOCKS_PER_SEC << " ms" << endl;
	}

	MPI_Finalize();
	return 0;
}

