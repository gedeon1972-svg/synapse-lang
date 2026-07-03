// salida_metal.c - Generado por Synapse Compilador
// Lenguaje: Synapse v1.0 (#lang: es)
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>

typedef struct { int longitud; const char* datos; } CadenaSegura;

typedef struct { uint32_t filas; uint32_t columnas; float* datos; } Tensor;

Tensor reserva(int tamano) {
    Tensor _bloque;
    _bloque.filas = tamano;
    _bloque.columnas = 1;
    _bloque.datos = (float*)malloc(tamano);
    if (!_bloque.datos) {
        fprintf(stderr, "ESCAPA_DEL_ALCANCE: malloc fallo en reserva()\n");
        exit(1);
    }
    return _bloque;
}

void libera(Tensor bloque) {
    if (bloque.datos) {
        free(bloque.datos);
    }
}

void principal(void) {
    Tensor bloque = reserva(64);
    printf("memoria reservada\n");
    libera(bloque);
    printf("memoria liberada\n");
}

int main() {
    principal();
    return 0;
}