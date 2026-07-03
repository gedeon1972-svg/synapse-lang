import os, json, glob
import pytest

from conftest import DIR_VALID, compilar_texto, ast_a_canonico_test


def _listar_fixtures_validas():
    return sorted(glob.glob(os.path.join(DIR_VALID, '*.syn')))


@pytest.mark.parametrize('ruta_syn', _listar_fixtures_validas())
def test_ast_coincide_con_expectativa(ruta_syn):
    with open(ruta_syn, 'r', encoding='utf-8') as f:
        fuente = f.read()

    ast, diag = compilar_texto(fuente)

    assert not diag.hay_errores(), (
        f"Se esperaba 0 errores en {os.path.basename(ruta_syn)}, "
        f"se obtuvieron {diag.contar()}"
    )

    ruta_json = ruta_syn.rsplit('.', 1)[0] + '.expected.json'
    if not os.path.exists(ruta_json):
        pytest.fail(f"Archivo de expectativa faltante: {ruta_json}")

    with open(ruta_json, 'r', encoding='utf-8') as f:
        esperado = json.load(f)

    obtenido = json.loads(ast_a_canonico_test(ast))
    assert obtenido == esperado, (
        f"AST no coincide con expectativa en {os.path.basename(ruta_syn)}\n"
        f"Esperado:\n{json.dumps(esperado, indent=2, ensure_ascii=False)}\n"
        f"Obtenido:\n{json.dumps(obtenido, indent=2, ensure_ascii=False)}"
    )
