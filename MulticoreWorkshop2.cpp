/*
*	In His Exalted Name
*	Matrix Addition - Sequential Code
*	Ahmad Siavashi, Email: siavashi@aut.ac.ir
*	15/04/2018
*/

// Let it be.
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

typedef struct {
	int* A, * B, * C, * C2;
	int n, m;
} DataSet;

void fillDataSet(DataSet* dataSet);
void printDataSet(DataSet dataSet);
void closeDataSet(DataSet dataSet);
double add_s(DataSet dataSet);
void add(DataSet dataSet, double stime);
bool check_res(DataSet dataSet);


void omp_check() {
	printf("------------ Info -------------\n");
#ifdef _DEBUG
	printf("[!] Configuration: Debug.\n");
#pragma message ("Change configuration to Release for a fast execution.")
#else
	printf("[-] Configuration: Release.\n");
#endif // _DEBUG
#ifdef _M_X64
	printf("[-] Platform: x64\n");
#elif _M_IX86 
	printf("[-] Platform: x86\n");
#pragma message ("Change platform to x64 for more memory.")
#endif // _M_IX86 
#ifdef _OPENMP
	printf("[-] OpenMP is on.\n");
	printf("[-] OpenMP version: %d\n", _OPENMP);
#else
	printf("[!] OpenMP is off.\n");
	printf("[#] Enable OpenMP.\n");
#endif // _OPENMP
	printf("[-] Maximum threads: %d\n", omp_get_max_threads());
	printf("[-] Nested Parallelism: %s\n", omp_get_nested() ? "On" : "Off");
#pragma message("Enable nested parallelism if you wish to have parallel region within parallel region.")
	printf("===============================\n");
}

int main(int argc, char* argv[]) {

	omp_check();

	DataSet dataSet;
	if (argc < 3) {
		printf("[-] Invalid No. of arguments.\n");
		printf("[-] Try -> <n> <m> \n");
		printf(">>> ");
		int temp;
		scanf("%d", &temp);
		dataSet.n = temp;
		dataSet.m = temp;
	}
	else {
		dataSet.n = atoi(argv[1]);
		dataSet.m = atoi(argv[2]);
	}
	fillDataSet(&dataSet);
	double stime = add_s(dataSet);
	add(dataSet, stime);
	// printDataSet(dataSet);
	closeDataSet(dataSet);
	system("PAUSE");
	return EXIT_SUCCESS;
}

void fillDataSet(DataSet* dataSet) {
	int i, j;

	dataSet->A = (int*)malloc(sizeof(int) * dataSet->n * dataSet->m);
	dataSet->B = (int*)malloc(sizeof(int) * dataSet->n * dataSet->m);
	dataSet->C = (int*)malloc(sizeof(int) * dataSet->n * dataSet->m);
	dataSet->C2 = (int*)malloc(sizeof(int) * dataSet->n * dataSet->m);

	srand(time(NULL));

	for (i = 0; i < dataSet->n; i++) {
		for (j = 0; j < dataSet->m; j++) {
			dataSet->A[i * dataSet->m + j] = rand() % 100;
			dataSet->B[i * dataSet->m + j] = rand() % 100;
		}
	}
}

bool check_res(DataSet dataSet) {
	int i, j;
	for (i = 0; i < dataSet.n; i++) {
		for (j = 0; j < dataSet.m; j++) {
			if (dataSet.C[i * dataSet.m + j] != dataSet.C2[i * dataSet.m + j]) {
				return false;
			}
		}
	}
	return true;
}

void printDataSet(DataSet dataSet) {
	int i, j;

	printf("[-] Matrix A\n");
	for (i = 0; i < dataSet.n; i++) {
		for (j = 0; j < dataSet.m; j++) {
			printf("%-4d", dataSet.A[i * dataSet.m + j]);
		}
		putchar('\n');
	}

	printf("[-] Matrix B\n");
	for (i = 0; i < dataSet.n; i++) {
		for (j = 0; j < dataSet.m; j++) {
			printf("%-4d", dataSet.B[i * dataSet.m + j]);
		}
		putchar('\n');
	}

	printf("[-] Matrix C\n");
	for (i = 0; i < dataSet.n; i++) {
		for (j = 0; j < dataSet.m; j++) {
			printf("%-8d", dataSet.C[i * dataSet.m + j]);
		}
		putchar('\n');
	}
}

void closeDataSet(DataSet dataSet) {
	free(dataSet.A);
	free(dataSet.B);
	free(dataSet.C);
}


double add_s(DataSet dataSet) {
	int i, j;
	double starttime, elapsedtime;
	starttime = omp_get_wtime();
	for (i = 0; i < dataSet.n; i++) {
		for (j = 0; j < dataSet.m; j++) {
			dataSet.C2[i * dataSet.m + j] = dataSet.A[i * dataSet.m + j] + dataSet.B[i * dataSet.m + j];
		}
	}
	elapsedtime = omp_get_wtime() - starttime;
	return elapsedtime;
}


void add(DataSet dataSet, double stime) {

	double avg_time = 0;

	for (int k = 0; k < 10; k++) {

		int i, j;
		int max_thread_num = 1;
		int temp;
		double starttime, elapsedtime;

		starttime = omp_get_wtime();

#pragma omp parallel for collapse(2) private(i, j) num_threads(8)
		for (i = 0; i < dataSet.n; i++) {
			temp = omp_get_thread_num();
			if (temp > max_thread_num) {
				max_thread_num = temp;
			}
			for (j = 0; j < dataSet.m; j++) {
				dataSet.C[i * dataSet.m + j] = dataSet.A[i * dataSet.m + j] + dataSet.B[i * dataSet.m + j];
			}
		}

		elapsedtime = omp_get_wtime() - starttime;

		avg_time += elapsedtime;


		printf("Time Elapsed: %f Secs - Max Thread Num: %d - Speed up: %f - ",
			elapsedtime, max_thread_num, stime - elapsedtime);
		if (check_res(dataSet)) {
			printf("Result is Correct.\n");
		}
		else {
			printf("Result is Incorrect !!!\n");
		}
	
	}

	printf(".........................................\n");
	avg_time /= 10;
	double speed_up = stime / avg_time;
	printf("Serial Time: %f\n", stime);
	printf("Avg Parallel Time: %f\n", avg_time);
	printf("Speed up: %f\n\n", speed_up);

}
