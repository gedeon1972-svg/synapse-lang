"""
Inquisidor — Punto único de entrada para verificar la estabilidad del sistema.
Delega en tests/test_runner.py para ejecutar los tests E2E.
"""

import os
import sys
import subprocess

TEST_RUNNER = os.path.join(os.path.dirname(__file__), "tests", "test_runner.py")

def main():
    print("=== INICIANDO AUDITORÍA SYNAPSE QA ===")
    print("[INQUISIDOR] Delegando en test_runner E2E...\n")

    result = subprocess.run(
        [sys.executable, TEST_RUNNER],
        cwd=os.path.dirname(__file__),
        text=True,
        capture_output=True,
    )

    print(result.stdout, end="")
    if result.stderr.strip():
        print(result.stderr, end="")

    if result.returncode != 0:
        print("\n>>> [QA INQUISIDOR] EXIT 1: ESTABILIDAD COMPROMETIDA. <<<")
        sys.exit(1)

    print("\n>>> [QA INQUISIDOR] EXIT 0: TODO VERDE. EL CÓDIGO ES ESTABLE. <<<")
    sys.exit(0)

if __name__ == "__main__":
    main()