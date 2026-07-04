# ARQUITECTURA POLIGLOTA - SYNAPSE v2.0

## Principio Fundamental

Synapse ya no es un lenguaje con keywords fijas. Es un **AST Universal** que puede
expresarse textualmente en cualquier idioma. El repositorio almacena la
representacion canonica (JSON), no el texto en un idioma particular.

```
┌──────────────────────────────────────────────────────────────────┐
│                    CAPA DE INTERFAZ HUMANA                       │
│  (Entrada/Salida en cualquier idioma: es, en, fr, pt, de, it, ...) │
└──────────────────────────┬───────────────────────────────────────┘
                           │
┌──────────────────────────▼───────────────────────────────────────┐
│  LEXER POLIGLOTA                                                 │
│  - Detecta #lang: es|en|fr|pt|de|it|... en la primera linea      │
│  - Selecciona el DICCIONARIO correspondiente                     │
│  - Mapea palabras a TokenID universales                          │
│  - Produce tokens agnosticos al idioma                           │
└──────────────────────────┬───────────────────────────────────────┘
                           │
┌──────────────────────────▼───────────────────────────────────────┐
│  CANONICAL ENCODER                                               │
│  - Convierte AST universal a JSON canonico                       │
│  - Formato de almacenamiento en repositorio GitHub               │
│  - NO contiene palabras de ningun idioma                         │
│  - TAMPOCO contiene TokenID como strings, solo estructura AST    │
└──────────────────────────┬───────────────────────────────────────┘
                           │
              ┌────────────▼────────────┐
              │  .syn.json (en repo)    │
              │  AST Universal Canonico │
              └────────────┬────────────┘
                           │
┌──────────────────────────▼───────────────────────────────────────┐
│  PARSER (AGNOSTICO)                                              │
│  - Trabaja sobre tokens universales (TokenID)                    │
│  - NO sabe que idioma original se uso                           │
│  - Produce AST universal                                         │
└──────────────────────────┬───────────────────────────────────────┘
                           │
┌──────────────────────────▼───────────────────────────────────────┐
│  GENERADOR C (AGNOSTICO)                                         │
│  - Trabaja sobre AST universal                                   │
│  - NO sabe que idioma original se uso                           │
│  - Produce codigo C compilable                                   │
└──────────────────────────┬───────────────────────────────────────┘
                           │
                    ┌──────▼──────┐
                    │ salida.c    │
                    │ (compilar)  │
                    └─────────────┘
```

## Tabla de Diccionarios

### Estructura

```python
DICCIONARIOS: Dict[str, Dict[str, TokenID]] = {
    'es': {
        'si':        TokenID.IF,
        'sino':      TokenID.ELSE,
        'funcion':   TokenID.FUNCTION,
        'retornar':  TokenID.RETURN,
        'lanzar':    TokenID.SPAWN,
        'recuperar': TokenID.RECOVER,
        'escuchar':  TokenID.LISTEN,
        'mientras':  TokenID.WHILE,
        'importar':  TokenID.IMPORT,
    },
    'en': {
        'if':        TokenID.IF,
        'else':      TokenID.ELSE,
        'function':  TokenID.FUNCTION,
        'return':    TokenID.RETURN,
        'spawn':     TokenID.SPAWN,
        'recover':   TokenID.RECOVER,
        'listen':    TokenID.LISTEN,
        'while':     TokenID.WHILE,
        'import':    TokenID.IMPORT,
    },
    'fr': {
        'si':        TokenID.IF,
        'sinon':     TokenID.ELSE,
        'fonction':  TokenID.FUNCTION,
        'retourner': TokenID.RETURN,
        'lancer':    TokenID.SPAWN,
        'recuperer': TokenID.RECOVER,
        'ecouter':   TokenID.LISTEN,
        'tantque':   TokenID.WHILE,
        'importer':  TokenID.IMPORT,
    },
    'pt': {
        'se':        TokenID.IF,
        'senao':     TokenID.ELSE,
        'funcao':    TokenID.FUNCTION,
        'retornar':  TokenID.RETURN,
        'lancar':    TokenID.SPAWN,
        'recuperar': TokenID.RECOVER,
        'escutar':   TokenID.LISTEN,
        'enquanto':  TokenID.WHILE,
        'importar':  TokenID.IMPORT,
    },
    'de': {
        'wenn':      TokenID.IF,
        'sonst':     TokenID.ELSE,
        'funktion':  TokenID.FUNCTION,
        'rueckgabe': TokenID.RETURN,
        'starten':   TokenID.SPAWN,
        'wiederherstellen': TokenID.RECOVER,
        'hoeren':    TokenID.LISTEN,
        'waehrend':  TokenID.WHILE,
        'importieren': TokenID.IMPORT,
    },
    'it': {
        'se':        TokenID.IF,
        'altrimenti': TokenID.ELSE,
        'funzione':  TokenID.FUNCTION,
        'restituisci': TokenID.RETURN,
        'lancia':    TokenID.SPAWN,
        'recupera':  TokenID.RECOVER,
        'ascolta':   TokenID.LISTEN,
        'mentre':    TokenID.WHILE,
        'importa':   TokenID.IMPORT,
    },
}
```

### Contrato

1. Cada **idioma** tiene un codigo (ej: `es`, `en`, `fr`, `pt`, `de`, `it`)
2. Cada **palabra clave** se mapea a un `TokenID` universal
3. `TokenID` es un Enum con nombres en ingles NEUTRO (no es "ingles del lenguaje",
   es el identificador interno del compilador)
4. Para anadir un nuevo idioma, solo se necesita un nuevo mapa en `DICCIONARIOS`

## Formato Canonico (.syn.json)

El archivo que se almacena en el repositorio. No tiene keywords de ningun idioma.

```json
{
  "synapse": "2.0",
  "ast": {
    "_tipo": "Programa",
    "sentencias": [
      {
        "_tipo": "DefinicionFuncion",
        "nombre": "sumar",
        "parametros": [
          { "nombre": "a", "tipo": "int" },
          { "nombre": "b", "tipo": "int" }
        ],
        "tipo_retorno": "int",
        "cuerpo": [
          {
            "_tipo": "SentenciaRetornar",
            "expr": {
              "_tipo": "OpBinaria",
              "operador": "+",
              "izquierdo": { "_tipo": "Identificador", "nombre": "a" },
              "derecho":   { "_tipo": "Identificador", "nombre": "b" }
            }
          }
        ]
      }
    ]
  }
}
```

### Ventajas

- **Unico source of truth**: el AST es identico para todos los idiomas
- **Diff universal**: un cambio en el JSON se ve igual en cualquier PR
- **Sin conflictos de idioma**: dos developers en diferentes idiomas producen el mismo `.syn.json`
- **Versionable**: el formato JSON es diffable y mergeable

## CLI (command line)

```
# Compilar desde español (entrada) -> C + canónico
python compilador.py programa.syn

# Compilar desde inglés -> C + canónico
python compilador.py programa.en.syn

# Leer canónico y generar C
python compilador.py programa.syn.json

# Leer canónico y re-generar texto en español
python compilador.py programa.syn.json --lang es

# Leer canónico y re-generar texto en inglés
python compilador.py programa.syn.json --lang en

# Leer canónico y re-generar texto en francés
python compilador.py programa.syn.json --lang fr

# Leer canónico y re-generar texto en portugués
python compilador.py programa.syn.json --lang pt

# Leer canónico y re-generar texto en alemán
python compilador.py programa.syn.json --lang de

# Leer canónico y re-generar texto en italiano
python compilador.py programa.syn.json --lang it
```

## Flujo de Trabajo del Desarrollador

### Escritura (cualquier idioma)
```
editor -> programa.syn (#lang: es) -> python compilador.py -> programa.syn.json (repo)
editor -> program.syn  (#lang: en) -> python compilador.py -> program.syn.json (repo)
```

### Lectura (cualquier idioma)
```
git pull -> programa.syn.json -> python compilador.py programa.syn.json --lang es -> programa.es.syn
git pull -> program.syn.json  -> python compilador.py program.syn.json  --lang en -> program.en.syn
git pull -> programme.syn.json -> python compilador.py programme.syn.json --lang fr -> programme.fr.syn
git pull -> programa.syn.json  -> python compilador.py programa.syn.json  --lang pt -> programa.pt.syn
git pull -> programm.syn.json  -> python compilador.py programm.syn.json  --lang de -> programm.de.syn
git pull -> programma.syn.json -> python compilador.py programma.syn.json --lang it -> programma.it.syn
```

### El AST canonico es SIEMPRE identico
Los archivos `.syn.json` generados desde `programa.syn` (es) y `programa.en.syn` (en)
son ESTRUCTURALMENTE IDENTICOS, diferenciandose solo en:
- Nombres de identificadores definidos por el usuario (`resultado` vs `result`)
- Literales (`"Fusion..."` vs `"Module fusion..."`)
- Tipos escritos como strings (`entero` vs `int`)

## Componentes Modificados

### compilador.py

| Componente | Cambio |
|---|---|
| `TokenID` (ex `TipoToken`) | Renombrado. Los valores del Enum son universales. |
| `DICCIONARIOS` | **NUEVO**. Mapa idioma -> (palabra -> TokenID) |
| `DICCIONARIOS_INVERSO` | **NUEVO**. Mapa idioma -> (TokenID -> palabra) para pretty-print |
| `Lexer.__init__` | Acepta `diccionario` y `idioma` opcionales |
| `Lexer._detectar_idioma` | Lee `#lang:` y selecciona diccionario automaticamente |
| `Lexer._tokenizar_linea` | Usa `self.diccionario` en vez de `PALABRAS_CLAVE` |
| `ast_a_canonico` | **NUEVO**. Serializa AST a JSON canonico |
| `canonico_a_ast` | **NUEVO**. Deserializa JSON canonico a AST |
| `ast_a_texto` | **NUEVO**. Pretty-printer multilenguaje |
| `compilar_desde_texto` | Reemplaza `compilar_archivo`. Maneja deteccion de idioma. |
| `compilar_desde_canonico` | **NUEVO**. Lee .syn.json directamente |
| `ejecutar_compilador` | Genera C + JSON canonico + texto opcional en cualquier idioma |
| `Parser` | **SIN CAMBIOS**. Ya era agnostico. |
| `GeneradorC` | **SIN CAMBIOS**. Ya era agnostico. |
