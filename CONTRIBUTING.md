# Contribuciones a Synapse

Synapse es un proyecto de infraestructura crítica. La estabilidad y el minimalismo son nuestras prioridades. No buscamos "funcionalidad a cualquier precio", sino **eficiencia clínica**.

### Reglas de Oro
1. **Prioridad al Rendimiento:** Si una contribución añade complejidad a costa de ciclos de CPU o fragmentación de memoria, será rechazada.
2. **Cero Dependencias:** No aceptaremos PRs que introduzcan dependencias externas.
3. **Calidad sobre Cantidad:** Preferimos una solución elegante de 10 líneas que un *hack* de 100.
4. **Respeto a la Filosofía:** Si tu propuesta rompe el *pool allocator* o la sintaxis minimalista, no será fusionada.

Antes de abrir un *Issue* o *Pull Request*, asegúrate de que el compilador siga compilándose limpiamente con `make` (o tu comando base).
