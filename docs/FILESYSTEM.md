# Gestion du Filesystem dans ShowControl

Ce projet utilise LittleFS pour stocker les données de configuration sur le RP2040.

## Configuration Automatique

Le filesystem est automatiquement construit lors de la compilation grâce au script `scripts/build_filesystem.py`. Les fichiers dans le dossier `data/` sont inclus dans l'image du filesystem.

## Commandes PlatformIO

### Compilation avec Filesystem

```bash
# Build normal - inclut automatiquement le filesystem
pio run

# Build explicite du filesystem seulement
pio run --target buildfs

# Upload du firmware seulement
pio run --target upload

# Upload du filesystem seulement
pio run --target uploadfs
```

### Upload Complet

```bash
# Méthode recommandée: upload séquentiel
pio run --target buildfs    # Construit le filesystem
pio run --target uploadfs   # Upload le filesystem
pio run --target upload     # Upload le firmware
```

Ou en une commande :
```bash
# Script personnalisé pour upload complet
python scripts/upload_filesystem.py
```

## Impact sur les Performances

### ✅ Avantages
- **Configuration persistante** : Les paramètres survivent aux redémarrages
- **Mise à jour facile** : Modification des fichiers sans recompilation
- **Séparation des données** : Le code et les données sont séparés

### ⚠️ Inconvénients
- **Temps de compilation** : +5-10 secondes pour construire le filesystem
- **Temps d'upload** : +10-15 secondes pour uploader le filesystem
- **Espace flash** : 512KB réservés pour le filesystem (configuré dans platformio.ini)

### 🚀 Optimisations

1. **Développement** : Uploadez le filesystem une seule fois, puis seulement le code
   ```bash
   pio run --target uploadfs  # Une seule fois
   pio run --target upload    # À chaque modification du code
   ```

2. **Production** : Upload complet pour la distribution
   ```bash
   pio run --target uploadfs && pio run --target upload
   ```

3. **CI/CD** : Le filesystem est automatiquement inclus dans les builds

## Structure des Données

```
data/
└── data.json          # Configuration principale (options, pages utilisateur)
```

Le fichier `data.json` contient :
- Configuration réseau (IP, DHCP, port OSC)
- Pages utilisateur personnalisées
- Paramètres d'affichage et LEDs
- Configuration MIDI/OSC

## Utilisation dans le Code

```cpp
#include "SPIFFS.h"  // ou LittleFS.h

void setup() {
  // Initialiser le filesystem
  if (!LittleFS.begin()) {
    Serial.println("Erreur montage filesystem");
    return;
  }
  
  // Lire un fichier
  File file = LittleFS.open("/data.json", "r");
  if (file) {
    String content = file.readString();
    file.close();
    // Parser JSON...
  }
}
```

## Recommandations

1. **Pour le développement quotidien** : Désactivez l'upload automatique du filesystem
2. **Pour les releases** : Activez l'upload complet
3. **Pour le debug** : Le filesystem peut être mis à jour indépendamment du code
4. **Taille des fichiers** : Gardez les fichiers de données petits (<100KB total)

## Configuration dans platformio.ini

```ini
[env:pico]
# Taille du filesystem (0.5MB = 512KB)
board_build.filesystem_size = 0.5m
board_build.filesystem = littlefs

# Scripts automatiques
extra_scripts = 
    pre:scripts/build_filesystem.py
```

Cette configuration équilibre performance et fonctionnalité pour un développement efficace.
