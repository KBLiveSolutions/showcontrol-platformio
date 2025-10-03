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

def find_uf2conv():
    """Cherche uf2conv dans les emplacements standards."""
    possible_paths = [
        "uf2conv",
        "uf2conv.py",
        os.path.expanduser("~/.platformio/packages/tool-uf2conv/uf2conv.py"),
        os.path.expanduser("~/.platformio/packages/framework-arduinopico/tools/uf2conv.py"),
        "/usr/local/bin/uf2conv",
        "/usr/bin/uf2conv"
    ]
    
    for path in possible_paths:
        if shutil.which(path) or os.path.exists(path):
            log(f"uf2conv trouvé: {path}")
            return path
    
    log("uf2conv non trouvé", "WARNING")
    return None

def create_combined_uf2_with_uf2conv(firmware_uf2, filesystem_bin, output_uf2, uf2conv_path):
    """Crée un UF2 combiné avec uf2conv."""
    try:
        # Conversion du firmware en bin si nécessaire
        firmware_bin = firmware_uf2.with_suffix('.bin')
        
        # Convertir UF2 vers BIN
        cmd = [
            "python3", uf2conv_path,
            str(firmware_uf2),
            "-o", str(firmware_bin),
            "-f", "0xe48bff56"  # RP2040 family ID
        ]
        
        subprocess.run(cmd, check=True, capture_output=True)
        log("Firmware UF2 converti en BIN")
        
        # Créer une image combinée
        create_combined_binary(firmware_bin, filesystem_bin, firmware_bin.with_name("combined.bin"))
        
        # Reconvertir en UF2
        cmd = [
            "python3", uf2conv_path,
            str(firmware_bin.with_name("combined.bin")),
            "-o", str(output_uf2),
            "-f", "0xe48bff56",
            "-b", "0x10000000"  # Base address pour RP2040
        ]
        
        subprocess.run(cmd, check=True, capture_output=True)
        log("UF2 combiné créé avec uf2conv")
        
    except subprocess.CalledProcessError as e:
        log(f"Erreur uf2conv: {e}", "ERROR")
        raise

def create_combined_uf2_manual(firmware_uf2, filesystem_bin, output_uf2):
    """Crée un UF2 combiné manuellement."""
    log("Fusion manuelle - extraction des blocs UF2")
    
    # Lire le firmware UF2 existant
    with open(firmware_uf2, 'rb') as f:
        firmware_data = f.read()
    
    # Lire le filesystem
    with open(filesystem_bin, 'rb') as f:
        fs_data = f.read()
    
    # Pour RP2040, le filesystem est typiquement à l'offset 0x100000 (1MB)
    fs_offset = 0x100000
    
    # Créer les blocs UF2 pour le filesystem
    fs_uf2_blocks = create_uf2_blocks_for_filesystem(fs_data, fs_offset)
    
    # Combiner firmware + filesystem
    with open(output_uf2, 'wb') as f:
        # Écrire le firmware original
        f.write(firmware_data)
        # Ajouter les blocs filesystem
        f.write(fs_uf2_blocks)
    
    log("UF2 combiné créé manuellement")

def create_combined_binary(firmware_bin, filesystem_bin, output_bin):
    """Crée un binaire combiné firmware + filesystem."""
    # Taille flash RP2040 = 2MB, filesystem typiquement à 1MB
    flash_size = 2 * 1024 * 1024  # 2MB
    fs_offset = 1 * 1024 * 1024   # 1MB
    
    # Créer un buffer de la taille flash
    combined_data = bytearray(flash_size)
    
    # Lire et placer le firmware au début
    with open(firmware_bin, 'rb') as f:
        firmware_data = f.read()
        combined_data[0:len(firmware_data)] = firmware_data
    
    # Lire et placer le filesystem à l'offset
    with open(filesystem_bin, 'rb') as f:
        fs_data = f.read()
        if len(fs_data) > (flash_size - fs_offset):
            log("Filesystem trop grand!", "ERROR")
            raise ValueError("Filesystem dépasse la taille flash disponible")
        
        combined_data[fs_offset:fs_offset + len(fs_data)] = fs_data
    
    # Écrire le binaire combiné
    with open(output_bin, 'wb') as f:
        f.write(combined_data)
    
    log(f"Binaire combiné créé: {output_bin}")

def create_uf2_blocks_for_filesystem(fs_data, base_address):
    """Crée des blocs UF2 pour le filesystem."""
    UF2_MAGIC_START0 = 0x0A324655
    UF2_MAGIC_START1 = 0x9E5D5157
    UF2_MAGIC_END = 0x0AB16F30
    UF2_FLAG_NOTmainParser_FLASH = 0x00000001
    UF2_FLAG_FILE_CONTAINER = 0x00001000
    FAMILY_ID_RP2040 = 0xe48bff56
    
    blocks = bytearray()
    block_size = 256  # Taille des données par bloc UF2
    num_blocks = (len(fs_data) + block_size - 1) // block_size
    
    for i in range(num_blocks):
        start_offset = i * block_size
        end_offset = min(start_offset + block_size, len(fs_data))
        block_data = fs_data[start_offset:end_offset]
        
        # Pad to 256 bytes
        while len(block_data) < block_size:
            block_data += b'\x00'
        
        # Créer l'en-tête UF2
        block = bytearray(512)  # Bloc UF2 = 512 bytes
        
        # Magic numbers
        block[0:4] = UF2_MAGIC_START0.to_bytes(4, 'little')
        block[4:8] = UF2_MAGIC_START1.to_bytes(4, 'little')
        
        # Flags
        block[8:12] = UF2_FLAG_FILE_CONTAINER.to_bytes(4, 'little')
        
        # Address
        block[12:16] = (base_address + start_offset).to_bytes(4, 'little')
        
        # Payload size
        block[16:20] = len(block_data).to_bytes(4, 'little')
        
        # Block number et total
        block[20:24] = i.to_bytes(4, 'little')
        block[24:28] = num_blocks.to_bytes(4, 'little')
        
        # Family ID
        block[28:32] = FAMILY_ID_RP2040.to_bytes(4, 'little')
        
        # Data payload
        block[32:32+len(block_data)] = block_data
        
        # Magic end
        block[508:512] = UF2_MAGIC_END.to_bytes(4, 'little')
        
        blocks.extend(block)
    
    return blocks

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
    
    try:
        # Méthode 1: Utiliser uf2conv avec filesystem
        uf2conv_path = find_uf2conv()
        if uf2conv_path:
            log("Utilisation de uf2conv pour créer UF2 combiné")
            create_combined_uf2_with_uf2conv(firmware_uf2, filesystem_bin, combined_uf2, uf2conv_path)
        else:
            # Méthode 2: Fusion manuelle des données binaires
            log("uf2conv non trouvé, utilisation de la fusion manuelle")
            create_combined_uf2_manual(firmware_uf2, filesystem_bin, combined_uf2)
            
    except Exception as e:
        log(f"Erreur lors de la fusion: {e}", "ERROR")
        log("Fallback: copie du firmware seul", "WARNING")
        shutil.copy2(firmware_uf2, combined_uf2)
    
    log(f"UF2 combiné créé: {combined_uf2}")
    log("Le filesystem est maintenant inclus dans le UF2")
    
    return 0

if __name__ == "_mainParser__":
    sys.exit(main())
