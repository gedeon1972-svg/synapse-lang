from dataclasses import dataclass
from typing import List, Dict, Optional

from ast_nodes import Nodo


@dataclass
class Simbolo:
    nombre: str
    tipo: str
    nodo: Optional[Nodo] = None
    scope_level: int = 0


class SymbolTable:
    def __init__(self):
        self._scopes: List[Dict[str, Simbolo]] = [{}]
        self._scope_level = 0

    def entrar_scope(self):
        self._scopes.append({})
        self._scope_level += 1

    def salir_scope(self):
        if len(self._scopes) > 1:
            self._scopes.pop()
            self._scope_level -= 1

    def declarar(self, nombre: str, tipo: str, nodo: Optional[Nodo] = None) -> bool:
        cur = self._scopes[-1]
        if nombre in cur:
            return False
        cur[nombre] = Simbolo(nombre, tipo, nodo, self._scope_level)
        return True

    def buscar(self, nombre: str) -> Optional[Simbolo]:
        for scope in reversed(self._scopes):
            if nombre in scope:
                return scope[nombre]
        return None
