//============================================================================
// Name        : main.cu
// Author      : efti-nile
// Version     :
// Copyright   : WTFPL
// Description : 
//============================================================================

#define SIZE 1000

#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <omp.h>

using namespace std;

__global__ void func(double *d_A, double *d_B, double *d_C){
	int i = threadIdx.x;
	double di = (double) i;
	d_A[i] = di * sin((di / (double)SIZE) * (double)M_PI);
	d_B[i] = di / (double)SIZE / (double)SIZE + (double)SIZE * (double)SIZE / (di + 1);
	d_C[i] = d_A[i] * d_B[i];
}

int main(int argc, char *argv[]) {
	double *d_A, *d_B, *d_C, *A, *B, *C;

	A = new double[SIZE];
	B = new double[SIZE];
	C = new double[SIZE];

	cudaMalloc(&d_A, SIZE * sizeof(double));
	cudaMalloc(&d_B, SIZE * sizeof(double));
	cudaMalloc(&d_C, SIZE * sizeof(double));

	if(   A == NULL ||   B == NULL ||   C == NULL || \
		d_A == NULL || d_B == NULL || d_C == NULL){
		cout << "Problem allocating memory" << endl;
		if(A != NULL) delete A;
		if(B != NULL) delete B;
		if(C != NULL) delete C;
		if(d_A != NULL) cudaFree(d_A);
		if(d_B != NULL) cudaFree(d_B);
		if(d_C != NULL) cudaFree(d_C);
		exit(1);
	}

	cout << "Allocated total in VRAM and RAM: " << \
			sizeof(double) * 3 * 2 * SIZE / (1024.0 * 1024.0) << " MiB" << endl;

	clock_t begin = clock();

	func<<< 1, SIZE >>>(d_A, d_B, d_C);

	clock_t exec_time = clock() - begin;

	cudaMemcpy(A, d_A, SIZE * sizeof(double), cudaMemcpyDeviceToHost);
	cudaMemcpy(B, d_B, SIZE * sizeof(double), cudaMemcpyDeviceToHost);
	cudaMemcpy(C, d_C, SIZE * sizeof(double), cudaMemcpyDeviceToHost);

	cout << "First 4 elements:" << endl;
	for(int i = 0; i < 4 && i < SIZE; i++){
		cout << fixed \
			 << "A[" << i << "] = " << A[i] \
			 <<	"   B[" << i << "] = " << B[i] \
			 << "   C[" << i << "] = " << C[i] << endl;
	}

	cout << "Executed for N = " << SIZE << " in " << (exec_time * 1000 / CLOCKS_PER_SEC) << " ms" << endl;

	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);

	delete A;
	delete B;
	delete C;

	return 0;
}
