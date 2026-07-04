#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "librerias/embedded_libs.h"

typedef struct {int longitud; const char *datos;} CadenaSegura;
typedef struct {FILE *stream; int es_valido; int es_virtual; const char* virtual_data; int virtual_len;} Canal;

typedef struct Programa { CadenaSegura tipo; void *sentencias; } Programa;

Canal abrir(CadenaSegura ruta, CadenaSegura modo) {
    Canal _c = {0};
    _c.es_virtual = 0;
    if (strcmp(ruta.datos, "librerias/compiler/ast_nodes.syn") == 0) { _c.es_virtual = 1; _c.virtual_data = LIB_AST; _c.virtual_len = (int)strlen(LIB_AST); _c.es_valido = 1; return _c; }
    if (strcmp(ruta.datos, "librerias/compiler/lexer.syn") == 0) { _c.es_virtual = 1; _c.virtual_data = LIB_LEXER; _c.virtual_len = (int)strlen(LIB_LEXER); _c.es_valido = 1; return _c; }
    if (strcmp(ruta.datos, "librerias/compiler/parser.syn") == 0) { _c.es_virtual = 1; _c.virtual_data = LIB_PARSER; _c.virtual_len = (int)strlen(LIB_PARSER); _c.es_valido = 1; return _c; }
    if (strcmp(ruta.datos, "librerias/compiler/generator.syn") == 0) { _c.es_virtual = 1; _c.virtual_data = LIB_GENERATOR; _c.virtual_len = (int)strlen(LIB_GENERATOR); _c.es_valido = 1; return _c; }
    if (strcmp(ruta.datos, "librerias/std/io.syn") == 0) { _c.es_virtual = 1; _c.virtual_data = LIB_IO; _c.virtual_len = (int)strlen(LIB_IO); _c.es_valido = 1; return _c; }
    if (strcmp(ruta.datos, "librerias/std/mem.syn") == 0) { _c.es_virtual = 1; _c.virtual_data = LIB_MEM; _c.virtual_len = (int)strlen(LIB_MEM); _c.es_valido = 1; return _c; }
    if (strcmp(ruta.datos, "librerias/std/math.syn") == 0) { _c.es_virtual = 1; _c.virtual_data = LIB_MATH; _c.virtual_len = (int)strlen(LIB_MATH); _c.es_valido = 1; return _c; }
    _c.stream = fopen(ruta.datos, modo.datos);
    _c.es_valido = (_c.stream != NULL) ? 1 : 0;
    if (!_c.es_valido) {
        fprintf(stderr, "ESCAPA_DEL_ALCANCE: fopen fallo en abrir()\n");
    }
    return _c;
}

CadenaSegura leer(Canal canal) {
    if (!canal.es_valido) { return (CadenaSegura){ .longitud = 0, .datos = "" }; }
    if (canal.es_virtual) {
        char* _buf = (char*)malloc(canal.virtual_len + 1);
        if (!_buf) { return (CadenaSegura){ .longitud = 0, .datos = "" }; }
        memcpy(_buf, canal.virtual_data, canal.virtual_len);
        _buf[canal.virtual_len] = '\0';
        return (CadenaSegura){ .longitud = canal.virtual_len, .datos = (const char*)_buf };
    }
    fseek(canal.stream, 0, SEEK_END);
    long _tam = ftell(canal.stream);
    rewind(canal.stream);
    char* _buf = (char*)malloc(_tam + 1);
    if (!_buf) { return (CadenaSegura){ .longitud = 0, .datos = "" }; }
    size_t _leido = fread(_buf, 1, _tam, canal.stream);
    _buf[_leido] = '\0';
    return (CadenaSegura){ .longitud = (int)_leido, .datos = (const char*)_buf };
}

void cerrar(Canal canal) {
    if (canal.es_virtual) { return; }
    if (canal.stream) {
        fclose(canal.stream);
    }
}

struct Programa parsear(CadenaSegura fuente) {
    (void)fuente;
    struct Programa prog = {0};
    return prog;
}

int generar(struct Programa programa, CadenaSegura ruta) {
    (void)programa; (void)ruta;
    return 0;
}
