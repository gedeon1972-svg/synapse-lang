import os
import shutil
import stat

PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))
DIST_DIR = os.path.join(PROJECT_ROOT, "dist")

def remove_readonly(func, path, excinfo):
    os.chmod(path, stat.S_IWRITE)
    func(path)

def clean_create_dir(path):
    if os.path.exists(path):
        shutil.rmtree(path, onerror=remove_readonly)
    os.makedirs(path)

def generate_install_ps1(dist_path):
    ps1_content = r"""$distDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$binPath = Join-Path $distDir "bin"

$currentPath = [Environment]::GetEnvironmentVariable("Path", "User")
if ($currentPath -split ";" -notcontains $binPath) {
    $newPath = $currentPath + ";" + $binPath
    [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
    Write-Host "Synapse instalado correctamente. Reinicia tu terminal."
} else {
    Write-Host "Synapse ya se encuentra en el PATH."
}
"""
    ps1_path = os.path.join(dist_path, "install.ps1")
    with open(ps1_path, "w", encoding="utf-8") as f:
        f.write(ps1_content)
    return ps1_path

def build():
    clean_create_dir(DIST_DIR)

    bin_dir = os.path.join(DIST_DIR, "bin")
    lib_dir = os.path.join(DIST_DIR, "lib")
    os.makedirs(bin_dir)
    os.makedirs(lib_dir)

    # Copy compiler executable
    shutil.copy2(os.path.join(PROJECT_ROOT, "synapse.exe"),
                 os.path.join(bin_dir, "synapse.exe"))

    # Copy runtime object and header
    shutil.copy2(os.path.join(PROJECT_ROOT, "synapse_rt.o"),
                 os.path.join(lib_dir, "synapse_rt.o"))
    shutil.copy2(os.path.join(PROJECT_ROOT, "synapse_rt.h"),
                 os.path.join(lib_dir, "synapse_rt.h"))

    # Copy runtime headers (preserving relative structure for #include "librerias/...")
    lib_librerias = os.path.join(lib_dir, "librerias")
    shutil.copytree(os.path.join(PROJECT_ROOT, "librerias"), lib_librerias)

    # Generate install script
    generate_install_ps1(DIST_DIR)

    print(f"Distribucion generada en: {DIST_DIR}")

if __name__ == "__main__":
    build()
