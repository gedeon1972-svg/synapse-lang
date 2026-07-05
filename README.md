# Synapse v1.3.0 — Lenguaje políglota de sistemas para inferencia local

## Características Clave
- **Pool Allocator seguro para hilos (Mutex):** Gestión de memoria determinista con bloques fijos y protección pthread, sin GC ni malloc/free caótico.
- **Compilación directa a C nativo:** Sin dependencias pesadas. El binario del compilador es autocontenido y genera C puro vinculable a un runtime propio.
- **Evaluación lógica de cortocircuito:** Los operadores `y`/`o` evalúan en cortocircuito, optimizando flujos booleanos sin efectos secundarios innecesarios.
- **Tensor como ciudadano de primera clase:** Tipo `Tensor` 2D con producto punto, suma y ReLU como operaciones nativas del lenguaje.

## Instalación (Quickstart)
1. Extrae la carpeta `dist/` en cualquier ubicación.
2. Ejecuta `install.ps1` (clic derecho → "Ejecutar con PowerShell").
3. Reinicia tu terminal. Ahora `synapse` está disponible globalmente.

## Uso en VS Code
- La extensión de sintaxis Synapse está en `dist/vscode-synapse/`. Instálala desde la carpeta o copia a `~/.vscode/extensions/`.
- Atajo de compilación: `Ctrl+Shift+B` ejecuta la tarea "Build Synapse" definida en `.vscode/tasks.json`.

## Arquitectura Interna
- **Frontend:** Python (lexer, parser, generador de AST canónico en JSON).
- **Backend:** Generación de C puro + runtime propio (`synapse_rt.o`) con pool allocator, tensores y concurrencia pthread.
