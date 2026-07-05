# Protocolo QA-Driven

1. **Tests obligatorios:** Todo Pull Request DEBE incluir uno o más tests en `tests/e2e/` que cubran el cambio propuesto.
2. **Validación local:** Antes de abrir el PR, el código debe pasar `tests/test_runner.py` con código de salida `EXIT 0`.
3. **Sin excepciones:** Cualquier contribución que no cumpla estos dos puntos será rechazada sin revisión de contenido.
