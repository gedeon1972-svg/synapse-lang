# EVALUACION PROFESIONAL: Synapse Compiler v1.0

## 1. ANALISIS DEL LENGUAJE (Comprension profunda)

### Filosofia
Synapse es un lenguaje imperativo con sintaxis indentada (Python-like) pero con keywords en espanol, disenado para compilar a C. Su propuesta es ser un "metalenguaje" accesible que abstrae conceptos complejos (concurrencia, manejo de errores, IPC) detras de primitivas simples.

### Gramatica descubierta

```
programa     := (sentencia)*
sentencia    := funcion | si | mientras | lanzar | recuperar | escuchar
               | retornar | importar | asignacion | expresion
funcion      := "funcion" ID "(" params ")" "->" tipo ":" bloque
params       := (ID ":" tipo ("," ID ":" tipo)*)?
tipo         := "int" | "entero" | "vacio" | "nulo" | "decimal" | "real"
               | "Tensor" | "texto" | "cadena" | "booleano" | "logico"
si           := "si" expr ":" bloque ("sino" ":" bloque)?
mientras     := "mientras" expr ":" bloque
lanzar       := "lanzar" llamada
recuperar    := expr "recuperar" ":" expr
escuchar     := "escuchar" expr "->" llamada
retornar     := "retornar" ["->"] expr?
importar     := "importar" ID ("." ID)*
asignacion   := ID "=" expr
expr         := comparacion ((">" | "<" | "==" | "=") comparacion)*
comparacion  := adicion (("+" | "-") adicion)*
adicion      := multiplicacion (("*" | "/" | "%") multiplicacion)*
primario     := NUM | STRING | ID ["(" args ")"] | "(" expr ")"
tensor       := "tensor" "(" expr "," expr ")"
log          := "log" "(" args ")"
```

### Caracteristicas implementadas (verificadas ejecutando)

| Caracteristica | Estado | Notas |
|---|---|---|
| Lexer completo | ✅ | tokens ES, indentacion, comentarios |
| Funciones con params | ✅ | tipos mapean a C |
| If/else | ✅ | anidamiento correcto |
| While | ✅ | |
| Operadores binarios | ✅ | + - * / % > < == |
| Llamadas a funciones | ✅ | recursion incluida |
| Asignacion de variables | ✅ | inferencia de tipo basica |
| log() | ✅ | printf nativo con formatos |
| importar modulo | ✅ | resolucion recursiva con anti-ciclos |
| tensor() | ✅ | alloc en heap, free automatico |
| lanzar (spawn) | ✅ | pthreads detached |
| recuperar (recover) | ✅ | codigos de retorno |
| escuchar (listen) | ✅ | file polling en thread |
| retornar con transferencia | ✅ | transferencia de propiedad tensor |
| Generacion C | ✅ | limpio, compilable con gcc |

### Bugs y carencias detectadas (encontradas experimentalmente)

| # | Problema | Archivo:Linea | Severidad |
|---|---|---|---|
| 1 | `<=` y `>=` no existen como tokens | compilador.py:89-92 | ALTA |
| 2 | `!=` no existe como token | compilador.py:89-92 | ALTA |
| 3 | `-` unario no soportado (`-1`, `-x`) | compilador.py:594-607 | ALTA |
| 4 | `=` incluido como operador de comparacion (confunde asignacion) | compilador.py:581 | MEDIA |
| 5 | `_tipo_de_expr` busca funciones en `_variables` en vez de tabla de funciones | compilador.py:1114-1116 | ALTA |
| 6 | Sin tipo booleano nativo (usa `int` 0/1) | compilador.py:825-837 | BAJA |
| 7 | No hay `break`/`continue` en ciclos | -- | MEDIA |
| 8 | `escuchar` usa file polling bloqueante infinito | compilador.py:1036-1044 | MEDIA |
| 9 | No hay `for` loop | -- | BAJA |
| 10 | Sin sistema de tipos compuesto (struct, array) | -- | BAJA |

---

## 2. ARQUITECTURA ACTUAL

```
programa.syn (o cualquier .syn)
    │
    ▼
┌─────────────────────┐
│  LEXER             │  Tokeniza: palabras clave ES, indentacion, literales
│  (Lexer)           │  Salida: List[Token]
└─────────┬───────────┘
          ▼
┌─────────────────────┐
│  PARSER            │  Recursive descent, AST tipado
│  (Parser)          │  Salida: Programa (AST)
└─────────┬───────────┘
          ▼
┌─────────────────────┐
│  IMPORT RESOLVER   │  Resolucion recursiva de imports, fusion AST
│  (compilar_archivo)│  Proteccion contra ciclos
└─────────┬───────────┘
          ▼
┌─────────────────────┐
│  C GENERATOR       │  Visitor -> codigo C
│  (GeneradorC)      │  Salida: salida_metal.c
└─────────────────────┘
          ▼
      salida_metal.c  (compilable con gcc/clang)
```

### Metricas del codigo
- **1252 lineas** en un unico archivo (`compilador.py`)
- **3 clases principales**: Lexer (~127 loc), Parser (~334 loc), GeneradorC (~332 loc)
- **18 tipos de nodos AST**
- **10 palabras clave** en espanol
- **0 tests automatizados**

---

## 3. PLAN DE TRABAJO Y MEJORA

### FASE 0: Correccion inmediata (bugs)
| Tarea | Descripcion | Archivos a modificar | Esfuerzo |
|---|---|---|---|
| 0.1 | Anadir `<=`, `>=`, `!=` a TOKEN_BICARACTER y al parser | compilador.py:89-92, 581 | 30 min |
| 0.2 | Implementar operador unario `-` y `!` | compilador.py:594-622 | 1 hr |
| 0.3 | Separar `=` de los operadores de comparacion | compilador.py:581 | 15 min |
| 0.4 | Crear registro de funciones con tipo de retorno | compilador.py:1103-1119, 928-951 | 1 hr |

### FASE 1: Refactor arquitectonico
| Tarea | Descripcion | Archivos | Esfuerzo |
|---|---|---|---|
| 1.1 | Dividir `compilador.py` en modulos | `lexer.py`, `parser.py`, `ast.py`, `generador.py`, `main.py` | 2 hr |
| 1.2 | Separar configuracion (tipos, keywords) en JSON | `sinapsis.json` | 1 hr |
| 1.3 | Anyadir manejo de errores con recuperacion en parser | `parser.py` | 3 hr |
| 1.4 | Agregar tests unitarios (pytest) | `tests/` | 3 hr |

### FASE 2: Sistema de tipos y semantica
| Tarea | Descripcion | Esfuerzo |
|---|---|---|
| 2.1 | Implementar tabla de simbolos con scopes | 4 hr |
| 2.2 | Type checker / validador semanticos | 6 hr |
| 2.3 | Tipos booleanos nativos con `verdadero`/`falso` | 1 hr |
| 2.4 | Tipos arrays/lista `[tipo]` | 4 hr |
| 2.5 | Inferencia de tipos mejorada (retorno de funciones) | 2 hr |

### FASE 3: Lenguaje (nuevas caracteristicas)
| Tarea | Descripcion | Prioridad | Esfuerzo |
|---|---|---|---|
| 3.1 | `para` loop (for) con iteracion | Alta | 2 hr |
| 3.2 | `romper` (break) y `continuar` (continue) | Alta | 1 hr |
| 3.3 | Estructuras compuestas `tipo ...: ...` (struct) | Media | 4 hr |
| 3.4 | Operadores `+=`, `-=`, `*=` compuestos | Media | 1 hr |
| 3.5 | Modulo `y` (and), `o` (or), `no` (not) logicos | Alta | 2 hr |
| 3.6 | `elegir` (match/switch) | Baja | 3 hr |
| 3.7 | Closures o funciones anonimas | Baja | 6 hr |

### FASE 4: Runtime y backend
| Tarea | Descripcion | Prioridad | Esfuerzo |
|---|---|---|---|
| 4.1 | Reemplazar `escuchar` file polling por senales IPC reales | Media | 4 hr |
| 4.2 | Sincronizacion entre hilos (mutex, canales) | Media | 4 hr |
| 4.3 | Operaciones tensoriales reales (suma, multiplicacion matricial) | Baja | 6 hr |
| 4.4 | Recolector de basura basico o arena allocator | Baja | 6 hr |
| 4.5 | Backend LLVM como alternativa a C | Baja | 20 hr+ |

### FASE 5: DX y ecosistema
| Tarea | Descripcion | Esfuerzo |
|---|---|---|
| 5.1 | CLI con argumentos (--output, --tokens, --ast) | 1 hr |
| 5.2 | Coloreado de errores con numero de linea y contexto | 2 hr |
| 5.3 | REPL / modo interactivo | 4 hr |
| 5.4 | Formateador de codigo ("synfmt") | 6 hr |
| 5.5 | LSP server para editores | 20 hr+ |
| 5.6 | Documentacion del lenguaje en README.md y ejemplos/ | 3 hr |

---

## 4. ROADMAP RECOMENDADO (Priorizado por impacto)

```
SEMANA 1:      FASE 0 (bugs) + FASE 1.1 (modularizar)
SEMANA 2:      FASE 1.2 + 1.3 + 1.4 (tests)
SEMANA 3:      FASE 2.1 + 2.2 (type system)
SEMANA 4:      FASE 2.3 + 2.4 + 2.5
SEMANA 5:      FASE 3.1 + 3.2 + 3.5 (loops + logicos)
SEMANA 6:      FASE 3.3 + 3.4 + 3.6
SEMANA 7:      FASE 4.1 + 4.2 (runtime)
SEMANA 8:      FASE 5.1 + 5.2 + 5.3 (DX)
```

### Quick wins (impacto inmediato, < 2 hr c/u)
1. Anadir `<=`, `>=`, `!=` al lexer y parser
2. Separar `=` del conjunto de comparacion
3. Anadir `not`/`!` unario
4. CLI basico con `argparse`
5. Tests de humo para lexer + parser
6. Registro de tipos de retorno de funciones
7. `romper` y `continuar` en ciclos

---

## 5. CONCLUSION

Synapse es un proyecto con **vision solida y ejecucion funcional**. Tiene un pipeline compilador completo que efectivamente traduce un lenguaje custom a C compilable, con caracteristicas no triviales (concurrencia, manejo de errores, IPC, modularidad, tensores).

Las **debilidades principales** son:
1. Monolito de 1252 lineas sin separacion de responsabilidades
2. Ausencia total de tests
3. Sistema de tipos fragil (inferencia incorrecta en llamadas a funciones)
4. Operadores faltantes (`<=`, `>=`, `!=`, unario `-`, logicos)

El proyecto esta en un punto ideal para una refactorizacion que lo lleve de "prototipo funcional" a "lenguaje mantenible". Las Fases 0 y 1 deberian priorizarse porque habilitan todo lo demas.
