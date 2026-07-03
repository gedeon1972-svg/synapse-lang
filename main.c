#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

// Basic types
typedef struct {int longitud; const char* datos;} CadenaSegura;

typedef struct {uint32_t filas; uint32_t columnas; float* datos;} Tensor;

typedef struct {FILE* stream; int es_valido;} Canal;

#define nulo NULL

// AST type definitions (generated from .syn files)
typedef struct Token {
    int tipo;
    CadenaSegura lexema;
    int linea;
    int columna;
} Token;

typedef struct Nodo {
    CadenaSegura tipo;
} Nodo;

typedef struct ListaNodo {
    Nodo* cabeza;
    struct ListaNodo* cola;
} ListaNodo;

typedef struct Programa {
    CadenaSegura tipo;
    ListaNodo* sentencias;
} Programa;

typedef struct Identificador {
    CadenaSegura tipo;
    CadenaSegura nombre;
} Identificador;

typedef struct LiteralNumero {
    CadenaSegura tipo;
    int valor;
} LiteralNumero;

typedef struct LiteralCadena {
    CadenaSegura tipo;
    CadenaSegura valor;
} LiteralCadena;

typedef struct OpBinaria {
    CadenaSegura tipo;
    Nodo* izquierdo;
    Token* operador;
    Nodo* derecho;
} OpBinaria;

typedef struct OpUnaria {
    CadenaSegura tipo;
    Token* operador;
    Nodo* expr;
} OpUnaria;

typedef struct LlamadaFuncion {
    CadenaSegura tipo;
    CadenaSegura nombre;
    ListaNodo* argumentos;
} LlamadaFuncion;

typedef struct ExprAccesoCampo {
    CadenaSegura tipo;
    Nodo* objeto;
    CadenaSegura nombre_campo;
} ExprAccesoCampo;

typedef struct AsignacionVariable {
    CadenaSegura tipo;
    CadenaSegura nombre;
    Nodo* expresion;
} AsignacionVariable;

typedef struct AsignacionCampo {
    CadenaSegura tipo;
    Nodo* objeto;
    CadenaSegura nombre_campo;
    Nodo* expresion;
} AsignacionCampo;

typedef struct SentenciaSi {
    CadenaSegura tipo;
    Nodo* condicion;
    ListaNodo* cuerpo;
    ListaNodo* cuerpo_sino;
} SentenciaSi;

typedef struct SentenciaMientras {
    CadenaSegura tipo;
    Nodo* condicion;
    ListaNodo* cuerpo;
} SentenciaMientras;

typedef struct SentenciaRetornar {
    CadenaSegura tipo;
    Nodo* expr;
} SentenciaRetornar;

typedef struct SentenciaExpr {
    CadenaSegura tipo;
    Nodo* expr;
} SentenciaExpr;

typedef struct LogLlamada {
    CadenaSegura tipo;
    ListaNodo* argumentos;
} LogLlamada;

typedef struct Parametro {
    CadenaSegura tipo;
    CadenaSegura nombre;
    CadenaSegura tipo_param;
    int es_transferencia;
} Parametro;

typedef struct ListaParametro {
    Parametro* cabeza;
    struct ListaParametro* cola;
} ListaParametro;

typedef struct DefinicionFuncion {
    CadenaSegura tipo;
    CadenaSegura nombre;
    ListaParametro* parametros;
    CadenaSegura tipo_retorno;
    ListaNodo* cuerpo;
} DefinicionFuncion;

typedef struct DefinicionEstructura {
    CadenaSegura tipo;
    CadenaSegura nombre;
    ListaParametro* campos;
} DefinicionEstructura;

typedef struct SentenciaRomper { CadenaSegura tipo; } SentenciaRomper;

typedef struct SentenciaSiguiente { CadenaSegura tipo; } SentenciaSiguiente;

typedef struct SentenciaLanzar { CadenaSegura tipo; Nodo* llamada; } SentenciaLanzar;

typedef struct SentenciaRecuperar { CadenaSegura tipo; Nodo* accion_critica; Nodo* plan_b; } SentenciaRecuperar;

typedef struct SentenciaEscuchar { CadenaSegura tipo; Nodo* canal; Nodo* respuesta; } SentenciaEscuchar;

typedef struct ExprTensor { CadenaSegura tipo; Nodo* filas; Nodo* columnas; } ExprTensor;

typedef struct ExprIndice { CadenaSegura tipo; Nodo* expr; Nodo* indice; } ExprIndice;

typedef struct ArgumentoTransferido { CadenaSegura tipo; Nodo* expr; } ArgumentoTransferido;

typedef struct SentenciaImportar { CadenaSegura tipo; CadenaSegura ruta; } SentenciaImportar;

static int _g_argc;
static char** _g_argv;

int _argc(){ return _g_argc; }
CadenaSegura _argv(int i){
    if(i<0 || i>=_g_argc) return (CadenaSegura){0, ""};
    return (CadenaSegura){.longitud = (int)strlen(_g_argv[i]), .datos = _g_argv[i]};
}

void salir(int c){ exit(c); }

static inline CadenaSegura cs_from_literal(const char *s){
    int len = (int)strlen(s);
    return (CadenaSegura){.longitud = len, .datos = s};
}

/* Forward declarations of core compiler functions (implemented in embedded libraries) */
struct Programa parsear(CadenaSegura fuente);
void _expr_a_c(struct Nodo nodo, CadenaSegura buf);
void _visitar_nodo(struct Nodo nodo, Canal out);
void _visitar_lista(struct ListaNodo lista, Canal out);
void _visitar_programa(struct Programa programa, Canal out);
int generar(struct Programa programa, CadenaSegura ruta);
Canal abrir(CadenaSegura ruta, CadenaSegura modo);
CadenaSegura leer(Canal canal);
void escribir(Canal canal, CadenaSegura contenido);
void cerrar(Canal canal);

/* Embedded library strings */
#include "librerias/embedded_libs.h"

void principal(){
    if(_argc() < 2){
        fprintf(stderr, "Usage: synapse <source.syn>\n");
        salir(1);
    }
    CadenaSegura ruta = _argv(1);
    Canal canal = abrir(ruta, cs_from_literal("r"));
    CadenaSegura fuente = leer(canal);
    cerrar(canal);
    struct Programa prog = parsear(fuente);
    int errores = generar(prog, ruta);
    if(errores > 0){
        fprintf(stderr, "Compilation errors: %d\n", errores);
        salir(errores);
    }
}

int main(int argc, char** argv){
    _g_argc = argc;
    _g_argv = argv;
    principal();
    return 0;
}
