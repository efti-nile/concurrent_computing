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

using namespace std;
 
int main(int argc, char *argv[]) {
	int N, size, rank, chunk, local_chunk;
	MPI_Status st;
	double *A, *B, *C, time_begin;

	if(argc < 2){
		cout << "specify length of vectors as first arg." << endl;
		exit(1);
	}else{
		N = strtol(argv[1], NULL, 10);
		if(N < 1){
			cout << "length of vectors less than one OR non-numerical first argument" << endl;
			exit(1);
		}
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0){
		time_begin = MPI_Wtime();
	}

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	chunk = N / size;

	cout << "Hi! I'm procces " << rank << " of " << size << endl;

	if(rank == 0){
		local_chunk = N;
	}else if(rank < size - 1){
		local_chunk = chunk;
	}else{
		local_chunk = N - chunk * (size - 1);
	}

	A = new double[local_chunk];
	B = new double[local_chunk];
	C = new double[local_chunk];

	if(rank == 0){
		for(int i = 0; i < chunk; ++i){
			A[i] = i * sin(i * M_PI / N);
			B[i] = i / pow(N, 2) + pow(N, 2) / (i + 1);
			C[i] = A[i] * B[i];
		}		
	}else{
		for(int j = 0; j < local_chunk; ++j){
			int i = rank * chunk + j;
			A[j] = i * sin(i * M_PI / N);
			B[j] = i / pow(N, 2) + pow(N, 2) / (i + 1);
			C[j] = A[i] * B[i];
		}
	}

	if(rank == 0){
		for(int r = 1; r < size; ++r){
			int remote_chunk = r != size - 1 ? chunk : N - chunk * (size - 1);
			MPI_Recv(A + chunk * r, remote_chunk, MPI_DOUBLE, r, 0, MPI_COMM_WORLD, &st);
			MPI_Recv(B + chunk * r, remote_chunk, MPI_DOUBLE, r, 1, MPI_COMM_WORLD, &st);
			MPI_Recv(C + chunk * r, remote_chunk, MPI_DOUBLE, r, 2, MPI_COMM_WORLD, &st);
		}
	}else{
		MPI_Send(A, local_chunk, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		MPI_Send(B, local_chunk, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		MPI_Send(C, local_chunk, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
	}

	if(rank == 0){
		for(int r = 0; r < size; ++r){
			cout << "First 4 elements for rank = " << r << endl;
			for(int i = 0; i < 4 && i < N; i++){
				cout << fixed \
					 << "A[" << i + r * chunk << "] = " << A[i + r * chunk] \
					 <<	"   B[" << i + r * chunk << "] = " << B[i + r * chunk] \
					 << "   C[" << i + r * chunk << "] = " << C[i + r * chunk] << endl;
			}
		}
		cout << "Master process executed for N = " << N << " in " << MPI_Wtime() - time_begin << " s" << endl;
	}

	MPI_Finalize();
	return 0;
}

