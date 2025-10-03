#!/usr/bin/env python3
"""
Script SCons pour construire automatiquement le filesystem LittleFS.
"""

from pathlib import Path

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

# Dans le contexte SCons
try:
    Import("env")
    env.AddPreAction("$BUILD_DIR/firmware.bin", before_build)
    print("✅ [FILESYSTEM] Script de pré-build configuré")
except:
    # Exécution directe pour test
    if __name__ == "_mainParser__":
        print("⚠️  Ce script doit être exécuté dans le contexte PlatformIO")
        print("Utilisation: pio run")
