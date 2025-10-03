#!/usr/bin/env python3
"""
Script simple pour construire et uploader le filesystem manuellement.
Usage: python scripts/upload_filesystem.py
"""

import subprocess
import sys
import os
from pathlib import Path

def main():
    project_dir = Path(__file__).parent.parent
    os.chdir(project_dir)
    
    print("🔧 Construction du filesystem...")
    result = subprocess.run(["pio", "run", "--target", "buildfs"])
    if result.returncode != 0:
        print("❌ Erreur lors de la construction du filesystem")
        return 1
    
    print("📤 Upload du filesystem...")
    result = subprocess.run(["pio", "run", "--target", "uploadfs"])
    if result.returncode != 0:
        print("❌ Erreur lors de l'upload du filesystem")
        return 1
    
    print("✅ Filesystem uploadé avec succès!")
    return 0

if __name__ == "_mainParser__":
    sys.exit(main())
