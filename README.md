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

Le projet a bénéficié d'optimisations majeures :

### ✅ **Sécurité et robustesse**
- Validation stricte des paramètres d'entrée
- Protection contre les débordements de buffer
- Gestion d'erreur robuste avec logs détaillés

### ✅ **Performance**
- Constantes `constexpr` pour l'optimisation compile-time
- Réduction de la duplication de code
- Gestion mémoire optimisée

### ✅ **Maintenabilité**
- Centralisation des constantes SysEx dans `midi.h`
- Fonctions spécialisées et modulaires
- Documentation complète avec commentaires Doxygen

### ✅ **Debug unifié**
- Macros de debug centralisées dans `utils.h`
- Messages préfixés par module
- Contrôle global du niveau de debug

## 📚 Documentation

- [Guide d'optimisation MIDI](OPTIMISATIONS_MIDI_OUT.md)
- [Centralisation SysEx](CENTRALISATION_SYSEX.md)
- [Optimisations Settings Parser](OPTIMISATIONS_SETTINGS_PARSER.md)
- [Optimisations MIDI In](OPTIMISATIONS_MIDI_IN.md)

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
