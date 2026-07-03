import os

# Files to embed (relative to project root)
files = {
    "LIB_AST": "librerias/compiler/ast_nodes.syn",
    "LIB_LEXER": "librerias/compiler/lexer.syn",
    "LIB_PARSER": "librerias/compiler/parser.syn",
    "LIB_GENERATOR": "librerias/compiler/generator.syn",
    "LIB_IO": "librerias/std/io.syn",
    "LIB_MEM": "librerias/std/mem.syn",
}

header_path = os.path.join("librerias", "embedded_libs.h")

def c_string_literal(text: str) -> str:
    """Return a C string literal with escaped newlines (\\n), backslashes and double quotes."""
    escaped_lines = []
    for line in text.splitlines():
        escaped = line.replace('\\', '\\\\\\').replace('"', '\\"')
        escaped_lines.append(escaped)
    # Join lines with escaped newline sequence and ensure trailing newline
    return "\\n".join(escaped_lines) + "\\n"

lines = []
lines.append("// Auto‑generated header embedding .syn files as string literals")
lines.append("#pragma once")
lines.append("")

for const_name, rel_path in files.items():
    with open(rel_path, "r", encoding="utf-8") as f:
        content = f.read()
    # Remove leading #lang directive if present
    if content.startswith("#lang"):
        content = "\n".join(content.splitlines()[1:])
    c_literal = c_string_literal(content)
    # Embed the literal inside quotes
    lines.append(f"static const char {const_name}[] = \"{c_literal}\";")

with open(header_path, "w", encoding="utf-8") as out:
    out.write("\n".join(lines))

print(f"Embedded header generated at {header_path}")
