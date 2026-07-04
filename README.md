# Synapse
**Lenguaje de programación de sistemas con soberanía de datos y IA tensorial nativa.**

Synapse es una arquitectura de programación diseñada para entornos donde la privacidad, la latencia y la eficiencia son innegociables. Sin dependencias externas, sin basura de gestión de memoria, sin burocracia de runtime.

## ¿Por qué Synapse?
- **Cero dependencias:** El compilador es un binario autocontenido. No requiere Python, Node, ni librerías del sistema.
- **Memoria determinista:** Sistema de *pool allocation* con bloques fijos. Sin `malloc/free` caótico, sin *Garbage Collector*. Rendimiento predecible para IA y sistemas embebidos.
- **IA nativa:** Tensores 2D, producto punto y funciones de activación (ReLU) son instrucciones de primera clase. Inferencia de alto rendimiento sin bibliotecas externas.
- **Poliglota por diseño:** El AST canónico se serializa a código fuente en español, inglés, francés, portugués, alemán o italiano (ej. `#lang:es`, `#lang:fr`, `#lang:de`).
- **Soberanía de datos:** Diseñado para orquestar agentes y modelos que operan estrictamente en el dispositivo.

## Estado del Proyecto
> **Fase:** Alpha | **Estabilidad:** En desarrollo activo.

| Módulo | Estado | Notas |
| :--- | :--- | :--- |
| Lexer/Parser | Estable | Procesamiento base operativo |
| Generador C | Estable | Emisión de código nativo |
| Pool Allocator | Estable | Gestión de memoria eficiente |
| Operaciones Tensoriales | Estable | Producto punto, suma, ReLU |
| Lógica Booleana | Estable | `y`, `o`, `no` operativos |
| Concurrencia (`lanzar`) | Estable | Threads vía pthread |
| Sistema Poliglota | Estable | 6 idiomas: es, en, fr, pt, de, it |

## Inicio Rápido
**Compilación:**
```bash
# Genera código C desde fuente Synapse
./synapse.exe programa.syn

# Compila el ejecutable nativo
gcc -o programa.exe programa.c
./programa.exe

# Re-genera fuente Synapse en otro idioma desde el AST canónico
python main.py programa.syn.json --lang fr
```

Ejemplo:

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

## Filosofía
Synapse nace de una necesidad táctica: el código fuente no debería depender de un ecosistema que cambia cada seis meses. La estabilidad es una característica, no un accidente. Synapse mantiene su AST en formato canónico JSON —auditable y verificable— generando C puro.

## Licencia
MIT
