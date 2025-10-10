#!/usr/bin/env python3
"""
Script SCons pour construire automatiquement le filesystem LittleFS.
"""

from pathlib import Path
import subprocess

def after_buildfs(source, target, env):
    build_dir = Path(env.subst("$BUILD_DIR"))
    bin_path = build_dir / "littlefs.bin"
    uf2_path = build_dir / "littlefs.uf2"
    uf2conv = Path(env.subst("$PROJECT_DIR")) / "scripts" / "uf2conv.py"
    if bin_path.exists():
        print(f"🔄 Conversion {bin_path} → {uf2_path}")
        subprocess.run(
            ["python3", str(uf2conv), str(bin_path), "--base", "0x100000", "--output", str(uf2_path), "--verbose"],
            check=True
        )
        if uf2_path.exists():
            print(f"✅ Fichier UF2 généré : {uf2_path}")
        else:
            print(f"❌ Fichier UF2 NON généré : {uf2_path}")
    else:
        print("❌ Fichier littlefs.bin introuvable")

def before_build(source, target, env):
    """Fonction appelée avant le build principal."""
    project_dir = Path(env.subst("$PROJECT_DIR"))
    data_dir = project_dir / "data"
    
    print("🔧 [FILESYSTEM] Vérification du dossier data...")
    
    if not data_dir.exists():
        print("⚠️  [FILESYSTEM] Dossier data non trouvé, pas de filesystem à construire")
        return
    
    files = list(data_dir.glob("*"))
    if not files:
        print("⚠️  [FILESYSTEM] Dossier data vide")
        return
    
    print(f"✅ [FILESYSTEM] {len(files)} fichier(s) trouvé(s) dans data/")
    for file in files:
        print(f"   - {file.name}")
    
    print("📦 [FILESYSTEM] Le filesystem sera construit automatiquement par PlatformIO")

# --- Contexte PlatformIO/SCons ---
Import("env")
env.AddPostAction("buildfs", after_buildfs)
env.AddPreAction("$BUILD_DIR/firmware.bin", before_build)
print("✅ [FILESYSTEM] Script de pré-build et post-build configuré")
