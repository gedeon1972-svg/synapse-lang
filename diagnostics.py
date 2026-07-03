import sys
from enum import Enum, auto
from typing import Optional, List, Dict

from ast_nodes import Token, TokenID


class ErrorCodes(Enum):
    ERR_SYNTAX_EXPECTED_TOKEN = auto()
    ERR_SYNTAX_UNEXPECTED_TOKEN = auto()
    ERR_SYNTAX_UNEXPECTED_EXPR = auto()
    ERR_SYNTAX_EXPECTED_NEWLINE = auto()
    ERR_LANG_MISSING = auto()
    ERR_LANG_UNSUPPORTED = auto()
    ERR_INDENT_INVALID = auto()
    ERR_INDENT_INCONSISTENT = auto()
    ERR_STRING_UNCLOSED = auto()
    ERR_LEX_CHAR_UNEXPECTED = auto()
    ERR_FILE_NOT_FOUND = auto()
    ERR_CANONICAL_FORMAT = auto()
    ERR_SEM_VAR_NO_DECLARADA = auto()
    ERR_SEM_TIPO_INCOMPATIBLE = auto()
    ERR_SEM_TIPO_RETORNO = auto()
    ERR_SEM_FUNC_NO_DEFINIDA = auto()
    ERR_SEM_REDEFINICION = auto()
    ERR_SEM_ARGUMENTOS_INVALIDOS = auto()
    ERR_SEM_ESTRUCTURA_NO_DEFINIDA = auto()
    ERR_SEM_CAMPO_NO_EXISTE = auto()


ERROR_MESSAGES: Dict[str, Dict[ErrorCodes, str]] = {
    'es': {
        ErrorCodes.ERR_SYNTAX_EXPECTED_TOKEN: "Se esperaba {esperado}, se encontró '{encontrado}'",
        ErrorCodes.ERR_SYNTAX_UNEXPECTED_TOKEN: "Token inesperado '{tok_name}' tras expresión",
        ErrorCodes.ERR_SYNTAX_UNEXPECTED_EXPR: "Expresión inesperada: '{tipo}'",
        ErrorCodes.ERR_SYNTAX_EXPECTED_NEWLINE: "Se esperaba nueva línea después de '{construccion}'",
        ErrorCodes.ERR_LANG_MISSING: "Falta declaración de idioma '#lang: <codigo>' en la línea 1",
        ErrorCodes.ERR_LANG_UNSUPPORTED: "Idioma '{idioma}' no soportado. Idiomas disponibles: {soportados}",
        ErrorCodes.ERR_INDENT_INVALID: "La indentación debe ser múltiplo de 4 espacios",
        ErrorCodes.ERR_INDENT_INCONSISTENT: "Nivel de indentación inconsistente",
        ErrorCodes.ERR_STRING_UNCLOSED: "Cadena sin cerrar",
        ErrorCodes.ERR_LEX_CHAR_UNEXPECTED: "Caracter inesperado '{char}'",
        ErrorCodes.ERR_FILE_NOT_FOUND: "Archivo no encontrado: {archivo}",
        ErrorCodes.ERR_CANONICAL_FORMAT: "Formato canónico no reconocido o corrupto",
        ErrorCodes.ERR_SEM_VAR_NO_DECLARADA: "Variable '{nombre}' no declarada en este ámbito",
        ErrorCodes.ERR_SEM_TIPO_INCOMPATIBLE: "Tipos incompatibles: no se puede usar '{tipo1}' con '{tipo2}' en '{operacion}'",
        ErrorCodes.ERR_SEM_TIPO_RETORNO: "Tipo de retorno incorrecto: se esperaba '{esperado}', se obtuvo '{obtenido}'",
        ErrorCodes.ERR_SEM_FUNC_NO_DEFINIDA: "Función '{nombre}' no definida",
        ErrorCodes.ERR_SEM_REDEFINICION: "Redefinición de '{nombre}' en el mismo ámbito",
        ErrorCodes.ERR_SEM_ARGUMENTOS_INVALIDOS: "Cantidad de argumentos inválida para '{nombre}': se esperaban {esperados}",
        ErrorCodes.ERR_SEM_ESTRUCTURA_NO_DEFINIDA: "Estructura '{nombre}' no definida",
        ErrorCodes.ERR_SEM_CAMPO_NO_EXISTE: "La estructura '{struct}' no tiene un campo '{campo}'",
    },
    'en': {
        ErrorCodes.ERR_SYNTAX_EXPECTED_TOKEN: "Expected {esperado}, found '{encontrado}'",
        ErrorCodes.ERR_SYNTAX_UNEXPECTED_TOKEN: "Unexpected token '{tok_name}' after expression",
        ErrorCodes.ERR_SYNTAX_UNEXPECTED_EXPR: "Unexpected expression: '{tipo}'",
        ErrorCodes.ERR_SYNTAX_EXPECTED_NEWLINE: "Expected newline after '{construccion}'",
        ErrorCodes.ERR_LANG_MISSING: "Missing language declaration '#lang: <code>' at line 1",
        ErrorCodes.ERR_LANG_UNSUPPORTED: "Language '{idioma}' not supported. Available: {soportados}",
        ErrorCodes.ERR_INDENT_INVALID: "Indentation must be a multiple of 4 spaces",
        ErrorCodes.ERR_INDENT_INCONSISTENT: "Inconsistent indentation level",
        ErrorCodes.ERR_STRING_UNCLOSED: "Unclosed string literal",
        ErrorCodes.ERR_LEX_CHAR_UNEXPECTED: "Unexpected character '{char}'",
        ErrorCodes.ERR_FILE_NOT_FOUND: "File not found: {archivo}",
        ErrorCodes.ERR_CANONICAL_FORMAT: "Unrecognized or corrupted canonical format",
        ErrorCodes.ERR_SEM_VAR_NO_DECLARADA: "Variable '{nombre}' not declared in this scope",
        ErrorCodes.ERR_SEM_TIPO_INCOMPATIBLE: "Incompatible types: cannot use '{tipo1}' with '{tipo2}' in '{operacion}'",
        ErrorCodes.ERR_SEM_TIPO_RETORNO: "Incorrect return type: expected '{esperado}', got '{obtenido}'",
        ErrorCodes.ERR_SEM_FUNC_NO_DEFINIDA: "Function '{nombre}' not defined",
        ErrorCodes.ERR_SEM_REDEFINICION: "Redefinition of '{nombre}' in the same scope",
        ErrorCodes.ERR_SEM_ARGUMENTOS_INVALIDOS: "Invalid argument count for '{nombre}': expected {esperados}",
        ErrorCodes.ERR_SEM_ESTRUCTURA_NO_DEFINIDA: "Struct '{nombre}' not defined",
        ErrorCodes.ERR_SEM_CAMPO_NO_EXISTE: "Struct '{struct}' has no field '{campo}'",
    },
}


class DiagnosticManager:
    def __init__(self, idioma: str = 'es', fuente_lineas: Optional[List[str]] = None,
                 ruta_archivo: str = '<entrada>'):
        self.errores: List[dict] = []
        self.idioma = idioma if idioma in ERROR_MESSAGES else 'es'
        self.fuente_lineas = fuente_lineas or []
        self.ruta_archivo = ruta_archivo
        self._mensajes = ERROR_MESSAGES[self.idioma]

    def _obtener_linea_contexto(self, linea_num: int) -> str:
        if not self.fuente_lineas or linea_num < 1 or linea_num > len(self.fuente_lineas):
            return ""
        return self.fuente_lineas[linea_num - 1].rstrip('\n')

    def reportar(self, codigo: ErrorCodes, token: Optional[Token] = None,
                 **contexto) -> None:
        linea = token.linea if token else 0
        columna = token.columna if token else 0
        plantilla = self._mensajes.get(codigo, f"Error {codigo.name}")
        mensaje = plantilla.format(**contexto) if contexto else plantilla

        loc = f"{self.ruta_archivo}:{linea}:{columna}" if linea else self.ruta_archivo
        entry = f"[Synapse] {loc} - {mensaje}"

        linea_ctx = self._obtener_linea_contexto(linea)
        if linea_ctx:
            entry += f"\n  --> {linea_ctx}"
            if columna > 0:
                entry += f"\n      {' ' * (columna - 1)}^"

        print(entry, file=sys.stderr)
        self.errores.append({
            'codigo': codigo,
            'linea': linea,
            'columna': columna,
            'mensaje': mensaje,
        })

    def hay_errores(self) -> bool:
        return len(self.errores) > 0

    def contar(self) -> int:
        return len(self.errores)

    def codigo_salida(self) -> int:
        return 1 if self.hay_errores() else 0

    def resumen(self) -> str:
        if not self.hay_errores():
            return "0 errores"
        return f"{self.contar()} error{'es' if self.contar() > 1 else ''} encontrado{'s' if self.contar() > 1 else ''}"
