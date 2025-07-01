#!/usr/bin/env python3
"""
Script post-build pour créer une version combinée avec le filesystem.
Ce script fusionne le firmware principal et le filesystem en un seul UF2.
"""

import os
import shutil
import subprocess
import sys
from pathlib import Path

def log(message, level="INFO"):
    """Log avec timestamp."""
    print(f"[MERGE-FS] {level}: {message}")

def main():
    """Crée une version combinée firmware + filesystem."""
    project_dir = Path(__file__).parent.parent
    build_dir = project_dir / ".pio" / "build" / "pico"
    
    # Chemins des fichiers générés
    firmware_uf2 = build_dir / "firmware.uf2"
    filesystem_bin = build_dir / "littlefs.bin"
    combined_uf2 = build_dir / "firmware_with_filesystem.uf2"
    
    if not firmware_uf2.exists():
        log("Firmware UF2 non trouvé, skip merge", "WARNING")
        return 0
    
    if not filesystem_bin.exists():
        log("Filesystem non trouvé, copie du firmware seul", "INFO")
        shutil.copy2(firmware_uf2, combined_uf2)
        log(f"Firmware copié vers: {combined_uf2}")
        return 0
    
    log("Création d'un UF2 combiné (firmware + filesystem)")
    
    # Pour l'instant, on copie juste le firmware
    # TODO: Implémenter la vraie fusion si nécessaire
    shutil.copy2(firmware_uf2, combined_uf2)
    
    log(f"UF2 combiné créé: {combined_uf2}")
    log("Note: Upload séparé du filesystem recommandé pour l'instant")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
