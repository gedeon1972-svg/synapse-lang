// salida_metal.c - Generado por Synapse Compilador
// Lenguaje: Synapse v1.0 (#lang: es)
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>

typedef struct { int longitud; const char* datos; } CadenaSegura;

typedef struct { uint32_t filas; uint32_t columnas; float* datos; } Tensor;

typedef struct { FILE* stream; int es_valido; } Canal;

Canal abrir(CadenaSegura ruta, CadenaSegura modo) {
    Canal _c;
    _c.stream = fopen(ruta.datos, modo.datos);
    _c.es_valido = (_c.stream != NULL) ? 1 : 0;
    if (!_c.es_valido) {
        fprintf(stderr, "ESCAPA_DEL_ALCANCE: fopen fallo en abrir()\n");
    }
    return _c;
}

CadenaSegura leer(Canal canal) {
    if (!canal.es_valido) { return (CadenaSegura){ .longitud = 0, .datos = "" }; }
    fseek(canal.stream, 0, SEEK_END);
    long _tam = ftell(canal.stream);
    rewind(canal.stream);
    char* _buf = (char*)malloc(_tam + 1);
    if (!_buf) { return (CadenaSegura){ .longitud = 0, .datos = "" }; }
    size_t _leido = fread(_buf, 1, _tam, canal.stream);
    _buf[_leido] = '\0';
    return (CadenaSegura){ .longitud = (int)_leido, .datos = (const char*)_buf };
}

void escribir(Canal canal, CadenaSegura contenido) {
    if (!canal.es_valido) { return; }
    fwrite(contenido.datos, 1, contenido.longitud, canal.stream);
    fflush(canal.stream);
}

void cerrar(Canal canal) {
    if (canal.stream) {
        fclose(canal.stream);
    }
}

void principal(void) {
    Canal f = abrir((CadenaSegura){ .longitud = 9, .datos = "datos.txt" }, (CadenaSegura){ .longitud = 1, .datos = "r" });
    printf("abri pero no cierre\n");
    /* ADVERTENCIA: canal 'f' no fue cerrado explicitamente */
    if (f.stream) { fclose(f.stream); f.es_valido = 0; }
}

int main() {
    principal();
    return 0;
}