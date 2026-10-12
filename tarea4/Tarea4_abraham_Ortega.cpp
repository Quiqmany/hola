#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#define TAM 10

typedef struct n {
    uint32_t clave;
    int valor;
    struct n *next;
} nodo;

nodo *tabla[TAM];

uint32_t hash32(uint32_t clave) {
    clave ^= clave >> 16;
    clave *= 0x7FEB352D;
    clave ^= clave >> 15;
    clave *= 0x846CA68B;
    clave ^= clave >> 16;

    return clave;
}

int insertar(uint32_t clave, int valor) {
    uint32_t posicion;
    nodo *actual;
    nodo *nuevo;
    int opcion;

    posicion = hash32(clave) % TAM;
    actual = tabla[posicion];

    while (actual != NULL) {
        if (actual->clave == clave) {
            if (actual->valor == valor) {
                printf("La clave %u ya tiene el valor %d. No se modifica.\n",
                       (unsigned int)clave, valor);
                return 0;
            } else {
                printf("La clave %u ya existe con valor %d.\n",
                       (unsigned int)clave, actual->valor);
                printf("Nuevo valor propuesto: %d\n", valor);
                printf("Deseas actualizar? 1=Si, 0=No: ");
                scanf("%d", &opcion);

                if (opcion == 1) {
                    actual->valor = valor;
                    printf("Valor actualizado correctamente.\n");
                    return 1;
                } else {
                    printf("Se conserva el valor anterior.\n");
                    return 0;
                }
            }
        }

        actual = actual->next;
    }

    nuevo = (nodo *)malloc(sizeof(nodo));

    if (nuevo == NULL) {
        printf("Error de memoria.\n");
        return -1;
    }

    nuevo->clave = clave;
    nuevo->valor = valor;
    nuevo->next = tabla[posicion];

    tabla[posicion] = nuevo;

    printf("Insertada clave %u con valor %d en posicion %u.\n",
           (unsigned int)clave, valor, (unsigned int)posicion);

    return 1;
}

int buscar(uint32_t clave) {
    uint32_t posicion;
    nodo *actual;

    posicion = hash32(clave) % TAM;
    actual = tabla[posicion];

    while (actual != NULL) {
        if (actual->clave == clave) {
            printf("Clave: %u, valor: %d\n",
                   (unsigned int)actual->clave, actual->valor);
            return 1;
        }

        actual = actual->next;
    }

    printf("La clave %u no existe.\n", (unsigned int)clave);
    return 0;
}

void imprimir_tabla() {
    int i;
    nodo *actual;

    for (i = 0; i < TAM; i++) {
        printf("\nPosicion %d: ", i);
        actual = tabla[i];

        if (actual == NULL) {
            printf("Vacia");
        }

        while (actual != NULL) {
            printf("[%u, %d] -> ",
                   (unsigned int)actual->clave, actual->valor);
            actual = actual->next;
        }

        printf("NULL\n");
    }
}

void liberar_tabla() {
    int i;
    nodo *actual;
    nodo *temp;

    for (i = 0; i < TAM; i++) {
        actual = tabla[i];

        while (actual != NULL) {
            temp = actual;
            actual = actual->next;
            free(temp);
        }

        tabla[i] = NULL;
    }
}

int main() {
    int i;

    for (i = 0; i < TAM; i++) {
        tabla[i] = NULL;
    }

    insertar(30, 100);
    insertar(15, 200);
    insertar(45, 300);

    insertar(30, 100);
    insertar(30, 999);

    insertar(25, 500);

    printf("\nContenido de la tabla hash:\n");
    imprimir_tabla();

    printf("\nBusquedas:\n");
    buscar(30);
    buscar(45);
    buscar(99);

    liberar_tabla();

    return 0;
}
