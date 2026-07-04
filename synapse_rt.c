// synapse_rt.c — Runtime precompilado para Synapse
// Compilar una sola vez: gcc -c synapse_rt.c -o synapse_rt.o
// Linkear: gcc programa.c synapse_rt.o -o programa -lpthread

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include "librerias/embedded_libs.h"

// --- Type definitions (deben coincidir exactamente con las emitidas por el generador) ---
typedef struct { int longitud; const char* datos; } CadenaSegura;
typedef struct { uint32_t filas; uint32_t columnas; float* datos; } Tensor;
typedef struct { FILE* stream; int es_valido; int es_virtual; const char* virtual_data; int virtual_len; } Canal;

// --- Memory pool ---
#define POOL_BLOQUES 64
#define TAMANO_BLOQUE 4096

typedef struct {
    uint8_t* pool_base;
    uint32_t* bitmap;
    uint32_t total_blocks;
    uint32_t block_size;
} MemoryPool;

static MemoryPool _g_pool;
static pthread_mutex_t _g_pool_mutex = PTHREAD_MUTEX_INITIALIZER;

void pool_init(uint32_t total_blocks, uint32_t block_size) {
    pthread_mutex_lock(&_g_pool_mutex);
    _g_pool.total_blocks = total_blocks;
    _g_pool.block_size = block_size;
    _g_pool.pool_base = (uint8_t*)malloc(total_blocks * block_size);
    uint32_t _words = (total_blocks + 31) / 32;
    _g_pool.bitmap = (uint32_t*)calloc(_words, sizeof(uint32_t));
    if (!_g_pool.pool_base || !_g_pool.bitmap) {
        fprintf(stderr, "ESCAPA_DEL_ALCANCE: pool_init fallo\n");
        pthread_mutex_unlock(&_g_pool_mutex);
        exit(1);
    }
    pthread_mutex_unlock(&_g_pool_mutex);
}

void* pool_alloc() {
    pthread_mutex_lock(&_g_pool_mutex);
    uint32_t _words = (_g_pool.total_blocks + 31) / 32;
    for (uint32_t _w = 0; _w < _words; _w++) {
        if (_g_pool.bitmap[_w] != 0xFFFFFFFF) {
            uint32_t _bits = ~_g_pool.bitmap[_w];
            uint32_t _b = 0;
            while (!(_bits & (1u << _b))) { _b++; }
            uint32_t _index = _w * 32 + _b;
            if (_index >= _g_pool.total_blocks) break;
            _g_pool.bitmap[_w] |= (1u << _b);
            pthread_mutex_unlock(&_g_pool_mutex);
            return _g_pool.pool_base + _index * _g_pool.block_size;
        }
    }
    pthread_mutex_unlock(&_g_pool_mutex);
    return NULL;
}

void pool_free(void* ptr) {
    pthread_mutex_lock(&_g_pool_mutex);
    if (ptr >= (void*)_g_pool.pool_base
        && ptr < (void*)(_g_pool.pool_base + _g_pool.total_blocks * _g_pool.block_size)) {
        uint32_t _index = (uint32_t)((uint8_t*)ptr - _g_pool.pool_base) / _g_pool.block_size;
        uint32_t _w = _index / 32;
        uint32_t _b = _index % 32;
        _g_pool.bitmap[_w] &= ~(1u << _b);
    } else {
        free(ptr);
    }
    pthread_mutex_unlock(&_g_pool_mutex);
}

static inline float* _pool_malloc(size_t tamano) {
    if (tamano <= TAMANO_BLOQUE) {
        float* _p = (float*)pool_alloc();
        if (_p) return _p;
        fprintf(stderr, "ADVERTENCIA: pool agotado, usando malloc\n");
    }
    float* _p = (float*)malloc(tamano);
    if (!_p) {
        fprintf(stderr, "ESCAPA_DEL_ALCANCE: malloc fallo\n");
        exit(1);
    }
    return _p;
}

// --- std.io ---
void escribir(CadenaSegura contenido) {
    fwrite(contenido.datos, 1, contenido.longitud, stdout);
    fflush(stdout);
}

void escribir_linea(CadenaSegura contenido) {
    fwrite(contenido.datos, 1, contenido.longitud, stdout);
    fwrite("\n", 1, 1, stdout);
    fflush(stdout);
}

CadenaSegura leer_linea() {
    static char _buf[4096];
    if (fgets(_buf, 4096, stdin)) {
        int _len = (int)strlen(_buf);
        if (_len > 0 && _buf[_len - 1] == '\n') { _buf[_len - 1] = '\0'; _len--; }
        char* _dup = (char*)malloc(_len + 1);
        if (!_dup) { return (CadenaSegura){ .longitud = 0, .datos = "" }; }
        memcpy(_dup, _buf, _len + 1);
        return (CadenaSegura){ .longitud = _len, .datos = _dup };
    }
    return (CadenaSegura){ .longitud = 0, .datos = "" };
}

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

// --- std.math ---
Tensor crear_tensor(int filas, int columnas) {
    Tensor r;
    r.filas = filas;
    r.columnas = columnas;
    r.datos = _pool_malloc(filas * columnas * sizeof(float));
    memset(r.datos, 0, filas * columnas * sizeof(float));
    return r;
}

Tensor suma_tensor(Tensor a, Tensor b) {
    if (a.filas != b.filas || a.columnas != b.columnas) {
        fprintf(stderr, "ESCAPA_DEL_ALCANCE: dimensiones incompatibles en suma_tensor()\n");
        return (Tensor){ .filas = 0, .columnas = 0, .datos = NULL };
    }
    Tensor r;
    r.filas = a.filas;
    r.columnas = a.columnas;
    r.datos = _pool_malloc(r.filas * r.columnas * sizeof(float));
    for (int _i = 0; _i < r.filas * r.columnas; _i++) {
        r.datos[_i] = a.datos[_i] + b.datos[_i];
    }
    pool_free(a.datos);
    pool_free(b.datos);
    return r;
}

Tensor producto_punto(Tensor a, Tensor b) {
    if (a.columnas != b.filas) {
        fprintf(stderr, "ESCAPA_DEL_ALCANCE: dimensiones incompatibles en producto_punto()\n");
        return (Tensor){ .filas = 0, .columnas = 0, .datos = NULL };
    }
    Tensor r;
    r.filas = a.filas;
    r.columnas = b.columnas;
    r.datos = (float*)calloc(r.filas * r.columnas, sizeof(float));
    for (int _i = 0; _i < r.filas; _i++) {
        for (int _j = 0; _j < r.columnas; _j++) {
            float _sum = 0;
            for (int _k = 0; _k < a.columnas; _k++) {
                _sum += a.datos[_i * a.columnas + _k] * b.datos[_k * b.columnas + _j];
            }
            r.datos[_i * r.columnas + _j] = _sum;
        }
    }
    pool_free(a.datos);
    pool_free(b.datos);
    return r;
}

Tensor relu(Tensor a) {
    Tensor r;
    r.filas = a.filas;
    r.columnas = a.columnas;
    r.datos = _pool_malloc(a.filas * a.columnas * sizeof(float));
    for (int _i = 0; _i < a.filas * a.columnas; _i++) {
        r.datos[_i] = (a.datos[_i] > 0) ? a.datos[_i] : 0.0f;
    }
    pool_free(a.datos);
    return r;
}

// --- std.math (alias) ---
Tensor suma(Tensor a, Tensor b) {
    return suma_tensor(a, b);
}

Tensor producto(Tensor a, Tensor b) {
    return producto_punto(a, b);
}

// --- std.mem ---
Tensor reserva(int tamano) {
    Tensor _bloque;
    _bloque.filas = tamano;
    _bloque.columnas = 1;
    _bloque.datos = _pool_malloc(tamano);
    return _bloque;
}

void libera(Tensor bloque) {
    if (bloque.datos) {
        pool_free(bloque.datos);
    }
}

// --- std.conv ---
int texto_a_entero(CadenaSegura str) {
    if (str.datos == NULL || str.longitud == 0) return 0;
    return (int)strtol(str.datos, NULL, 10);
}

float texto_a_decimal(CadenaSegura str) {
    if (str.datos == NULL || str.longitud == 0) return 0.0f;
    return (float)strtod(str.datos, NULL);
}

CadenaSegura decimal_a_texto(float n) {
    char buf[64];
    int len = snprintf(buf, sizeof(buf), "%f", n);
    char* data = (char*)malloc(len + 1);
    if (!data) { fprintf(stderr, "ESCAPA_DEL_ALCANCE: malloc fallo en decimal_a_texto\n"); exit(1); }
    memcpy(data, buf, len + 1);
    return (CadenaSegura){ .longitud = len, .datos = data };
}

CadenaSegura entero_a_texto(int n) {
    char buf[64];
    int len = snprintf(buf, sizeof(buf), "%d", n);
    char* data = (char*)malloc(len + 1);
    if (!data) { fprintf(stderr, "ESCAPA_DEL_ALCANCE: malloc fallo en entero_a_texto\n"); exit(1); }
    memcpy(data, buf, len + 1);
    return (CadenaSegura){ .longitud = len, .datos = data };
}
