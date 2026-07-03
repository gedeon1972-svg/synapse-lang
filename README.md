# Synapse

**Lenguaje de programación poliglota con IA tensorial nativa. Sin dependencias externas.**

---

## ¿Por qué Synapse?

- **Cero dependencias** — El compilador es un único binario autocontenido. No requiere Python, Node, ni ninguna biblioteca del sistema. `synapse.exe` lo incluye todo.
- **Memoria propia** — Sistema de *pool allocation* con bloques fijos y bitmap de asignación. Sin malloc/free caótico, sin GC, sin fragmentación. Rendimiento predecible en entornos embebidos y de IA.
- **Matemáticas nativas para IA** — Tensores 2D, multiplicación matricial, ReLU y suma de vectores son instrucciones de primera clase del lenguaje. No se necesita instalar NumPy, PyTorch ni ninguna dependencia.
- **Poliglota por diseño** — Un mismo AST canónico puede serializarse a código fuente en español, inglés o cualquier idioma soportado. Ideal para equipos multilingües y educación.
- **Sin punteros visibles** — Structs, tensores y canales se manejan con sintaxis de alto nivel. El compilador gestiona la memoria automáticamente con semántica de transferencia de ownership.

---

## Muestra rápida

```synapse
importar std.io
importar std.math

funcion principal() -> nulo:
    entrada = crear_tensor(1, 4)
    pesos = crear_tensor(4, 3)
    oculta = producto_punto(entrada, pesos)
    activacion = relu(oculta)
    escribir_linea("Forward Pass completado.")
```

Compila y ejecuta:

```bash
synapse.exe red.syn
gcc -o red.exe red.c && ./red.exe
```

Sin imports pesados. Sin frameworks. Sin dependencias.

---

## Instalación y uso

### Opción 1 — Usar el binario precompilado

```bash
./synapse.exe programa.syn      # Genera programa.c + programa.syn.json
gcc -o programa.exe programa.c   # Compila el C generado
./programa.exe                   # Ejecuta
```

### Opción 2 — Compilar desde la fuente (Python)

```bash
git clone https://github.com/tu-usuario/synapse
cd synapse
python main.py programa.syn
gcc -o programa.exe programa.c
./programa.exe
```

### Opciones del compilador

| Flag | Descripción |
|------|-------------|
| `--tokens` | Muestra la secuencia de tokens generada |
| `--lang es` | Genera código fuente en español |
| `--lang en` | Genera código fuente en inglés |

---

## Filosofía de diseño

Synapse nace de una convicción: **el código fuente no debería depender de un ecosistema de herramientas que cambia cada seis meses**.

En lugar de construir sobre capas y capas de abstracciones (transpiladores, empaquetadores, gestores de dependencias), Synapse mantiene su AST en un formato canónico JSON — legible por máquina y por humano — y genera C directamente. Esto significa:

- **Portabilidad real**: cualquier plataforma con un compilador de C puede ejecutar Synapse.
- **Verificabilidad**: el AST canónico es diffable, mergeable y auditable sin ejecutar nada.
- **Rendimiento nativo**: los bucles de multiplicación de tensores se traducen a `for` en C. Sin overhead de intérprete ni JIT.

El sistema de memoria propio (pool de bloques fijos con bitmap) no es una decisión arbitraria: en cargas de trabajo de IA, donde se asignan y liberan cientos de tensores por segundo, un malloc/free genérico fragmenta el heap en milisegundos. Synapse evita esto desde el diseño del lenguaje.

---

## Licencia

MIT
