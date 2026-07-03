"""
Smoke Test: recorre todos los archivos .syn bajo tests/fixtures/ y verifica que
el compilador termine con código 0 (éxito) para los válidos y distinto de 0 para
los inválidos.
"""

import os, sys, glob

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

from conftest import DIR_VALID, DIR_INVALID, compilar_texto


VERDE = '\033[92m'
ROJO = '\033[91m'
AMARILLO = '\033[93m'
RESET = '\033[0m'


def run_smoke():
    errores = 0
    total = 0

    categorias = [
        (DIR_VALID, 'validos', 0),
        (DIR_INVALID, 'invalidos', None),
    ]

    for directorio, nombre, codigo_esperado in categorias:
        archivos = sorted(glob.glob(os.path.join(directorio, '*.syn')))
        for ruta in archivos:
            base = os.path.basename(ruta)
            total += 1
            with open(ruta, 'r', encoding='utf-8') as f:
                fuente = f.read()
            ast, diag = compilar_texto(fuente)
            codigo = diag.codigo_salida()

            if codigo_esperado is None:
                ok = codigo != 0
            else:
                ok = codigo == codigo_esperado

            estado = f'{VERDE}OK{RESET}' if ok else f'{ROJO}FAIL{RESET}'
            if ok:
                print(f'  [{estado}] {base}')
            else:
                print(f'  [{estado}] {base}  (código={codigo}, esperado={codigo_esperado})')
                errores += 1

    print(f'\n{"=" * 40}')
    if errores == 0:
        print(f'{VERDE}{total}/{total} pruebas pasaron{RESET}')
    else:
        print(f'{ROJO}{errores} de {total} pruebas fallaron{RESET}')

    return 1 if errores else 0


if __name__ == '__main__':
    sys.exit(run_smoke())
