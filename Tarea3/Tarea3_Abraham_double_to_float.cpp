#include <time.h>
#include <iostream>
#include <immintrin.h>
#include <iomanip>
using namespace std;
typedef unsigned long long bench_t;
static bench_t before;
static bench_t after;
static inline bench_t cycles (void){
	unsigned int hi,lo;
		__asm__ __volatile__ ("rdtsc\n\t":"=a" (lo), "=d"(hi));
   	return ((bench_t) lo) | (((bench_t) hi) << 32);
}
float horner(float X, float *coef, long size){
	float ACC = 0.0f;
	int i;
	for(i = 0; i < size; i++){
		ACC = (ACC + coef[i]) * X;
	}
	return ACC;	
}
float horner_intrinsic(float X, float *coef, long size){
	float R[4];
	float P;
	int i;
	__m128 xmm0;
	__m128 X128;
	__m128 Y;
	X128 = _mm_set1_ps(X * X * X * X);
	Y = _mm_set1_ps(0.0f);
	for(i = 0; i < size / 4 - 1; i++){
		xmm0 = _mm_loadu_ps(&coef[i * 4]);
		Y = _mm_add_ps(Y, xmm0);
		Y = _mm_mul_ps(Y, X128);
	}
	xmm0 = _mm_loadu_ps(&coef[i * 4]);
	Y = _mm_add_ps(Y, xmm0);
	_mm_storeu_ps(R, Y);
	P = R[3] * X;
	P += R[2] * X * X;
	P += R[1] * X * X * X;
	P += R[0] * X * X * X * X;
	return P;
}

int main(){
	float X = 0.9f;
	float R;
	int i;
	int num_trails = 100000;
	clock_t t1, t2;
	srand(time(NULL));	
	float *coeficientes;
	int j;
	coeficientes = (float *)_mm_malloc(10000 * sizeof(float), 16);
	for(j = 0; j < 1; j++){
		for(i = 0; i < 10000; i++){
			coeficientes[i] = (float)(rand() % 1000) / 1000.0f;
			if(i < 10)
				cout << coeficientes[i] << endl;
		}
		cout << fixed << setprecision(2);
		R = horner(X, coeficientes, 10000);
		cout << R << endl;
		R = horner_intrinsic(X, coeficientes, 10000);
		cout << R << endl;		
	}
	
	t1 = clock();
	for(j = 0; j < num_trails; j++)
		R = horner(X, coeficientes, 10000);
	t2 = clock();
	float diff = ((float)t2 - (float)t1) / CLOCKS_PER_SEC;
	cout << "Tiempo horner normal: " << diff << endl;
	t1 = clock();
	for(j = 0; j < num_trails; j++)
		R = horner_intrinsic(X, coeficientes, 10000);
	t2 = clock();
	diff = ((float)t2 - (float)t1) / CLOCKS_PER_SEC;
	cout << "Tiempo horner usando intrinsecos: " << diff << endl;
	_mm_free(coeficientes);
	return 0;
}
