#include <stdio.h>
#include <stdint.h>
#include <emmintrin.h>
typedef union
{
    __m128i dato;
    struct
    {
        uint64_t bajo;
        uint64_t alto;
    };
} Numero128;
typedef struct
{
    uint64_t r0;
    uint64_t r1;
    uint64_t r2;
    uint64_t r3;
} Numero256;
Numero256 multiplicar(Numero128 A, Numero128 B)
{
    Numero256 R;
    uint64_t a0 = A.bajo;
    uint64_t a1 = A.alto;
    uint64_t b0 = B.bajo;
    uint64_t b1 = B.alto;
    unsigned long long p00 = a0 * b0;
    unsigned long long p01 = a0 * b1;
    unsigned long long p10 = a1 * b0;
    unsigned long long p11 = a1 * b1;
    R.r0 = p00;
    R.r1 = p01 + p10;
    R.r2 = p11;
    R.r3 = 0;
    return R;
}
void mostrar128(Numero128 X)
{
    printf("\n0x%016llX%016llX\n",(unsigned long long)X.alto,(unsigned long long)X.bajo);
}

void mostrar256(Numero256 X)
{
    printf("\n0x%016llX%016llX%016llX%016llX\n",(unsigned long long)X.r3,(unsigned long long)X.r2,(unsigned long long)X.r1,(unsigned long long)X.r0);
}
int main()
{
	 printf ("\nEste codigo fue escrito por Abraham Sinai Ortega Romero\n");
    Numero128 A;
    Numero128 B;
    Numero256 R;
    A.alto = 2;
    A.bajo = 3;
    B.alto = 4;
    B.bajo = 5;
    printf("A = ");
    mostrar128(A);
    printf("B = ");
    mostrar128(B);
    R = multiplicar(A,B);
    printf("\nResultado:\n");
    mostrar256(R);
    return 0;
}
