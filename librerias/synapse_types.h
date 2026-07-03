// synapse_types.h – shared type definitions for the compiler runtime
#ifndef SYNAPSE_TYPES_H
#define SYNAPSE_TYPES_H

#include <stdio.h>

// Basic secure string type
typedef struct {
    int longitud;
    const char *datos;
} CadenaSegura;

// Tensor type
typedef struct {
    uint32_t filas;
    uint32_t columnas;
    float *datos;
} Tensor;

// Canal (I/O stream) type
typedef struct {
    FILE *stream;
    int es_valido;
} Canal;

#endif // SYNAPSE_TYPES_H
