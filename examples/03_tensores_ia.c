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

// Pool de memoria de bloques fijos
#define POOL_BLOQUES 64
#define TAMANO_BLOQUE 4096
typedef struct {
    uint8_t* pool_base;
    uint32_t* bitmap;
    uint32_t total_blocks;
    uint32_t block_size;
} MemoryPool;
static MemoryPool _g_pool;

void pool_init(uint32_t total_blocks, uint32_t block_size) {
    _g_pool.total_blocks = total_blocks;
    _g_pool.block_size = block_size;
    _g_pool.pool_base = (uint8_t*)malloc(total_blocks * block_size);
    uint32_t _words = (total_blocks + 31) / 32;
    _g_pool.bitmap = (uint32_t*)calloc(_words, sizeof(uint32_t));
    if (!_g_pool.pool_base || !_g_pool.bitmap) {
        fprintf(stderr, "ESCAPA_DEL_ALCANCE: pool_init fallo\n");
        exit(1);
    }
}

void* pool_alloc() {
    uint32_t _words = (_g_pool.total_blocks + 31) / 32;
    for (uint32_t _w = 0; _w < _words; _w++) {
        if (_g_pool.bitmap[_w] != 0xFFFFFFFF) {
            uint32_t _bits = ~_g_pool.bitmap[_w];
            uint32_t _b = 0;
            while (!(_bits & (1u << _b))) { _b++; }
            uint32_t _index = _w * 32 + _b;
            if (_index >= _g_pool.total_blocks) break;
            _g_pool.bitmap[_w] |= (1u << _b);
            return _g_pool.pool_base + _index * _g_pool.block_size;
        }
    }
    return NULL;
}

void pool_free(void* ptr) {
    if (ptr >= (void*)_g_pool.pool_base
        && ptr < (void*)(_g_pool.pool_base + _g_pool.total_blocks * _g_pool.block_size)) {
        uint32_t _index = (uint32_t)((uint8_t*)ptr - _g_pool.pool_base) / _g_pool.block_size;
        uint32_t _w = _index / 32;
        uint32_t _b = _index % 32;
        _g_pool.bitmap[_w] &= ~(1u << _b);
    } else {
        free(ptr);
    }
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

static int _g_argc;
static char** _g_argv;
int _argc() { return _g_argc; }

CadenaSegura _argv(int i) {
    if (i < 0 || i >= _g_argc) return (CadenaSegura){0, ""};
    return (CadenaSegura){ .longitud = (int)strlen(_g_argv[i]), .datos = _g_argv[i] };
}

void salir(int codigo) { exit(codigo); }

CadenaSegura concat(CadenaSegura a, CadenaSegura b) {
    int _tl = a.longitud + b.longitud;
    char* _buf = (char*)malloc(_tl + 1);
    if (!_buf) { fprintf(stderr,"ESCAPA_DEL_ALCANCE: malloc fallo en concat\n"); exit(1); }
    memcpy(_buf, a.datos, a.longitud);
    memcpy(_buf + a.longitud, b.datos, b.longitud);
    _buf[_tl] = 0;
    CadenaSegura _r = { .longitud = _tl, .datos = _buf };
    return _r;
}

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

void principal(void) {
    escribir_linea((CadenaSegura){ .longitud = 32, .datos = "=== Forward Pass: Capa Densa ===" });
    escribir_linea((CadenaSegura){ .longitud = 19, .datos = "Creando tensores..." });
    Tensor entrada = crear_tensor(1, 4);
    Tensor pesos = crear_tensor(4, 3);
    escribir_linea((CadenaSegura){ .longitud = 42, .datos = "Multiplicando: entrada (1x4) * pesos (4x3)" });
    Tensor oculta = producto_punto(entrada, pesos);
    escribir_linea((CadenaSegura){ .longitud = 28, .datos = "Aplicando activacion ReLU..." });
    Tensor activacion = relu(oculta);
    escribir_linea((CadenaSegura){ .longitud = 34, .datos = "Forward Pass completado con exito." });
    escribir_linea((CadenaSegura){ .longitud = 28, .datos = "=== Fin de la simulacion ===" });
    pool_free(oculta.datos);
    pool_free(pesos.datos);
    pool_free(entrada.datos);
    pool_free(activacion.datos);
}

int main(int argc, char** argv) {
    _g_argc = argc;
    _g_argv = argv;
    pool_init(POOL_BLOQUES, TAMANO_BLOQUE);
    principal();
    return 0;
}