# ShowControl - Advanced MIDI Control System

![PlatformIO](https://img.shields.io/badge/PlatformIO-orange.svg)
![Raspberry Pi Pico](https://img.shields.io/badge/Raspberry%20Pi-Pico-red.svg)
![C++](https://img.shields.io/badge/Language-C++-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

## 📋 Description

ShowControl est un système de contrôle MIDI avancé conçu pour les performances live, construit avec PlatformIO et optimisé pour le Raspberry Pi Pico. Il offre un contrôle complet des équipements MIDI, des interfaces utilisateur personnalisées et une gestion réseau robuste.

## ✨ Fonctionnalités

### 🎵 MIDI
- **Entrée/Sortie MIDI USB** - Communication bidirectionnelle avec les DAW
- **Messages SysEx** - Protocole personnalisé pour les commandes avancées
- **Control Change** - Gestion complète des messages CC
- **Handshake intelligent** - Connexion automatique avec les applications

### 🖥️ Interface Utilisateur
- **Écran TFT couleur** - Interface graphique intuitive
- **Contrôles tactiles** - Boutons et encodeurs rotatifs
- **Pages utilisateur** - Navigation entre différents modes
- **Thèmes personnalisables** - Mode jour/nuit

### 🌐 Connectivité
- **Ethernet** - Connexion réseau filaire
- **OSC (Open Sound Control)** - Communication avec les logiciels audio
- **Configuration IP** - DHCP ou adresse statique
- **Gestion DNS/Gateway** - Configuration réseau complète

### 💡 LEDs et Affichage
- **FastLED** - Contrôle avancé des LEDs RGB
- **Sprites personnalisés** - Graphiques optimisés
- **Luminosité adaptive** - Ajustement automatique
- **Animations fluides** - Transitions visuelles
- **Module LED optimisé** - Gestion efficace des couleurs avec structures typées
- **Palette de 128 couleurs** - Stockage PROGMEM pour économiser la RAM
- **Correction gamma** - Rendu naturel des couleurs LED
- **Interpolation de couleurs** - Transitions fluides et effets visuels
- **Configuration par rangées** - Organisation logique des LEDs

### 💾 Gestion de Configuration
- **JSONManager optimisé** - Gestionnaire de configuration robuste et performant
- **Validation étendue** - Vérification des paramètres avec valeurs par défaut sécurisées
- **Opérations batch** - Regroupement des modifications pour meilleures performances
- **Gestion d'erreurs avancée** - Retours d'erreur détaillés avec structure `JsonResult`
- **Sauvegarde intelligente** - Système "dirty flag" pour éviter écritures inutiles
- **ArduinoJson v7** - Utilisation de la dernière version avec `JsonDocument`
- **Création automatique** - Configuration par défaut si fichier corrompu/absent
- **Diagnostics intégrés** - Outils de débogage et surveillance mémoire

### 💾 Système de fichiers
- **LittleFS intégré** - Stockage persistant sur flash (512KB)
- **Build automatique** - Construction du filesystem à chaque compilation
- **Accès rapide** - Lecture/écriture de fichiers de configuration
- **Scripts optimisés** - Upload simplifié firmware + données

## 🛠️ Technologies utilisées

- **Framework:** Arduino sur PlatformIO
- **Microcontrôleur:** Raspberry Pi Pico (RP2040)
- **Librairies principales:**
  - MIDI Library 5.0.2
  - Adafruit TinyUSB Library 3.7.0
  - TFT_eSPI 2.5.43
  - FastLED 3.9.9
  - ArduinoJson 7.4.2
  - Ethernet 2.0.2

## 🚀 Installation

### Prérequis
- [PlatformIO](https://platformio.org/) installé
- [Visual Studio Code](https://code.visualstudio.com/) (recommandé)
- Un Raspberry Pi Pico

### Étapes d'installation

1. **Cloner le repository**
   ```bash
   git clone https://github.com/votre-username/showcontrol-platformio.git
   cd showcontrol-platformio
   ```

2. **Ouvrir dans PlatformIO**
   ```bash
   pio project init --ide vscode
   ```

3. **Installer les dépendances**
   ```bash
   pio lib install
   ```

4. **Compiler et uploader**
   ```bash
   pio run --target upload
   ```

## 📁 Structure du projet

```
showcontrol/
├── src/
│   ├── config/
│   │   ├── consts.h              # Constantes globales
│   │   ├── jsonManager.cpp       # Gestion JSON
│   │   └── jsonManager.h
│   ├── core/
│   │   ├── globalParser.cpp      # Parseur global
│   │   ├── mainPageParser.cpp    # Parseur page principale
│   │   ├── pages.cpp             # Gestion des pages
│   │   ├── settingsParser.cpp    # Parseur paramètres
│   │   └── utils.cpp             # Utilitaires
│   ├── display/
│   │   ├── colors.cpp            # Gestion couleurs
│   │   ├── displaySetup.cpp      # Configuration affichage
│   │   ├── sprites.cpp           # Sprites graphiques
│   │   ├── fonts/                # Polices personnalisées
│   │   └── pages/                # Pages d'interface
│   ├── input/
│   │   ├── actions.cpp           # Actions utilisateur
│   │   ├── buttons.cpp           # Gestion boutons
│   │   ├── encoder.cpp           # Encodeurs rotatifs
│   │   └── expressionPedal.cpp   # Pédales d'expression
│   ├── leds/
│   │   ├── leds.cpp              # Contrôle LEDs
│   │   └── led_utils.cpp         # Utilitaires LEDs
│   ├── midi/
│   │   ├── midi.h                # Constantes MIDI/SysEx
│   │   ├── midi_in.cpp           # Réception MIDI
│   │   └── midi_out.cpp          # Envoi MIDI
│   ├── osc/
│   │   ├── ethernetManager.cpp   # Gestion Ethernet
│   │   ├── OSCManager.cpp        # Gestionnaire OSC
│   │   └── OSCReceive.cpp        # Réception OSC
│   └── showcontrol_refactor.ino  # Fichier principal
├── data/
│   └── data.json                 # Configuration
├── platformio.ini                # Configuration PlatformIO
└── README.md                     # Ce fichier
```

## 🗂️ Filesystem et Configuration

Ce projet utilise LittleFS pour stocker les données de configuration de manière persistante :

### Configuration Automatique ✅

Le filesystem est **automatiquement construit** lors de la compilation grâce au script `scripts/build_filesystem.py`. Les fichiers dans le dossier `data/` sont intégrés dans l'image du filesystem.

### Structure des Données

```
data/
└── data.json          # Configuration complète (réseau, pages, contrôles)
```

### Commandes de Build

```bash
# Build normal avec filesystem automatique
pio run

# Build du filesystem uniquement 
pio run --target buildfs

# Upload du firmware seulement
pio run --target upload

# Upload du filesystem seulement
pio run --target uploadfs

# Upload complet (recommandé pour la première utilisation)
python scripts/upload_filesystem.py
```

### Performance

| Opération | Temps | Impact |
|-----------|-------|---------|
| Build normal | +2-3s | Filesystem inclus automatiquement |
| Upload filesystem | +10-15s | Une seule fois nécessaire |
| Upload firmware | Normal | Pas d'impact |

**Recommandation** : En développement, uploadez le filesystem une seule fois, puis seulement le code.

### Tâches VS Code

Utilisez `Ctrl+Shift+P` → "Tasks: Run Task" :
- **ShowControl: Complete Build** - Build complet (défaut)
- **PlatformIO: Build Filesystem** - Build filesystem uniquement
- **PlatformIO: Upload All** - Upload complet

📖 **Documentation complète** : [docs/FILESYSTEM.md](docs/FILESYSTEM.md)

## ⚙️ Configuration

### Configuration réseau
Le système supporte la configuration automatique (DHCP) ou manuelle :

```cpp
// Configuration IP statique
IPAddress ip(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
```

### Configuration MIDI
Le protocole SysEx utilise :
- **Manufacturer ID:** 122
- **Device ID:** 29
- **Family ID:** 1 (réception) / 19 (envoi)

## 🔧 Optimisations récentes

Le projet a bénéficié d'optimisations majeures dans plusieurs modules clés :

### 🎛️ **JSONManager (Module de Configuration)**
- **Gestion d'erreurs robuste** avec structure `JsonResult` pour retours détaillés
- **Validation étendue** de tous les paramètres avec valeurs par défaut sécurisées
- **Opérations batch** pour regrouper les modifications et améliorer les performances
- **Migration ArduinoJson v7** avec `JsonDocument` moderne et API mise à jour
- **Sauvegarde intelligente** avec système "dirty flag" évitant les écritures inutiles
- **Diagnostics intégrés** pour surveillance mémoire et débogage système
- **Sauvegardes atomiques** avec fichiers temporaires pour éviter la corruption

### 💡 **LED Utilities (Gestion LED Optimisée)**
- **Structures typées** `RGBColor` et `LedRow` pour meilleure lisibilité et sécurité
- **Stockage PROGMEM** pour palette de 128 couleurs (économie RAM significative)
- **Correction gamma** intégrée pour rendu naturel des couleurs LED
- **Interpolation de couleurs** pour transitions fluides et effets visuels
- **Organisation par rangées** pour configuration logique des LEDs

### ✅ **Sécurité et robustesse globale**
- Validation stricte des paramètres d'entrée (CC MIDI 0-127, canaux 1-16)
- Protection contre les débordements de buffer
- Gestion d'erreur robuste avec logs détaillés
- Création automatique de configuration par défaut si corruption

### ✅ **Performance optimisée**
- Constantes `constexpr` pour l'optimisation compile-time
- Réduction de la duplication de code
- Gestion mémoire optimisée avec allocations intelligentes
- Sauvegarde conditionnelle pour éviter I/O inutiles

### ✅ **Maintenabilité améliorée**
- Centralisation des constantes SysEx dans `midi.h`
- Fonctions spécialisées et modulaires
- Documentation complète avec commentaires Doxygen
- Code organisé en sections logiques claires

### ✅ **Debug unifié**
- Macros de debug centralisées dans `utils.h`
- Messages préfixés par module
- Contrôle global du niveau de debug

## 📚 Documentation

### Optimisations modules
- [**JSONManager optimisé**](docs/JSONMANAGER_OPTIMIZATIONS.md) - Gestionnaire de configuration robuste
- [**LED Utilities optimisé**](docs/LED_OPTIMIZATIONS.md) - Module LED avec structures typées
- [**Filesystem intégré**](docs/FILESYSTEM.md) - Système de fichiers automatique

### Guides techniques
- [Guide d'optimisation MIDI](OPTIMISATIONS_MIDI_OUT.md)
- [Centralisation SysEx](CENTRALISATION_SYSEX.md)
- [Optimisations Settings Parser](OPTIMISATIONS_SETTINGS_PARSER.md)
- [Optimisations MIDI In](OPTIMISATIONS_MIDI_IN.md)

### Exemples d'utilisation
- [**JSONManager usage**](examples/jsonManager_usage_example.cpp) - Utilisation du gestionnaire JSON
- [**LED utilities usage**](examples/led_usage_example.cpp) - Exemples d'utilisation des LEDs

## 🤝 Contribution

Les contributions sont les bienvenues ! Merci de :

1. Fork le projet
2. Créer une branche feature (`git checkout -b feature/AmazingFeature`)
3. Commit vos changements (`git commit -m 'Add some AmazingFeature'`)
4. Push la branche (`git push origin feature/AmazingFeature`)
5. Ouvrir une Pull Request

## 📜 License

Ce projet est sous licence MIT. Voir le fichier [LICENSE](LICENSE) pour plus de détails.

## 👥 Auteurs

- **Votre Nom** - *Développement initial* - [VotreGitHub](https://github.com/votre-username)

## 🙏 Remerciements

- Communauté PlatformIO pour l'excellent framework
- Raspberry Pi Foundation pour le Pico
- Contributeurs des librairies open source utilisées

---

⭐ **N'hésitez pas à donner une étoile si ce projet vous aide !**
