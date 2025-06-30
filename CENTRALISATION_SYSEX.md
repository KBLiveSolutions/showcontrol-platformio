# Centralisation des constantes SysEx dans midi.h

## Résumé des modifications

Les constantes SysEx qui étaient dupliquées dans `midi_in.cpp` et `midi_out.cpp` ont été centralisées dans `midi.h` pour améliorer la maintenabilité et éviter la duplication.

## ✅ Constantes centralisées

### Constantes de base SysEx
```cpp
static constexpr uint8_t SYSEX_START_BYTE = 0xF0;
static constexpr uint8_t SYSEX_END_BYTE = 0xF7;
static constexpr uint8_t SYSEX_MANUFACTURER_ID = 122;
static constexpr uint8_t SYSEX_DEVICE_ID = 29;
```

### Family IDs selon la direction
```cpp
static constexpr uint8_t SYSEX_FAMILY_ID_SEND = 1;  // Pour l'envoi (midi_out)
static constexpr uint8_t SYSEX_FAMILY_ID_RECV = 19; // Pour la réception (midi_out commands)
static constexpr uint8_t SYSEX_FAMILY_ID_IN = 1;    // Pour la réception (midi_in)
```

### Constantes pour les commandes SysEx (réception - midi_in)
```cpp
static constexpr uint8_t CMD_IN_HANDSHAKE_REQUEST = 1;
static constexpr uint8_t CMD_IN_SETTINGS_REQUEST = 3;
static constexpr uint8_t CMD_IN_MIDI_CONNECTED = 4;
static constexpr uint8_t CMD_IN_LIVE_UPDATE_REQUEST = 5;
static constexpr uint8_t CMD_IN_SEND_CONTROLS = 6;
static constexpr uint8_t CMD_IN_USER_PAGES_REQUEST = 7;
static constexpr uint8_t CMD_IN_CONFIGURE_PEDAL = 12;
static constexpr uint8_t CMD_IN_DISPLAY_ITEM = 51;
static constexpr uint8_t CMD_IN_ARRAY_ITEM = 52;
static constexpr uint8_t CMD_IN_SET_PEDAL_TYPE = 55;
static constexpr uint8_t CMD_IN_SET_IP = 56;
static constexpr uint8_t CMD_IN_SET_DNS = 57;
static constexpr uint8_t CMD_IN_SET_GATEWAY = 58;
static constexpr uint8_t CMD_IN_SET_SUBNET = 59;
static constexpr uint8_t CMD_IN_SET_DHCP = 61;
static constexpr uint8_t CMD_IN_SET_PORT = 62;
static constexpr uint8_t CMD_IN_SET_PAGES_AMOUNT = 63;
static constexpr uint8_t CMD_IN_REMOVE_USER_PAGE = 64;
static constexpr uint8_t CMD_IN_SET_BRIGHTNESS = 65;
static constexpr uint8_t CMD_IN_SET_BRIGHTNESS_ALT = 66;
```

### Constantes pour les commandes SysEx (envoi)
```cpp
static constexpr uint8_t CMD_HANDSHAKE = 4;
static constexpr uint8_t CMD_LIVE_UPDATE = 2;
static constexpr uint8_t CMD_SEND_PAGES_AMOUNT = 7;
static constexpr uint8_t CMD_CONFIGURE_BUTTON = 10;
static constexpr uint8_t CMD_CONFIGURE_LED = 14;
static constexpr uint8_t CMD_CONFIGURE_DISPLAY = 18;
static constexpr uint8_t CMD_SEND_SETTINGS_IP = 56;
static constexpr uint8_t CMD_SEND_SETTINGS_DISPLAY = 61;
static constexpr uint8_t CMD_SEND_SETTINGS_NIGHT = 62;
static constexpr uint8_t CMD_SEND_SETTINGS_PORT = 62;
static constexpr uint8_t CMD_SEND_SETTINGS_LED = 65;
static constexpr uint8_t CMD_SETLIST_CHANGE = 74;
```

### Tailles des buffers SysEx
```cpp
static constexpr uint8_t SYSEX_HEADER_SIZE = 6;
static constexpr uint8_t SYSEX_BUTTON_CONFIG_SIZE = 14;
static constexpr uint8_t SYSEX_LED_CONFIG_SIZE = 14;
static constexpr uint8_t SYSEX_DISPLAY_CONFIG_SIZE = 12;
static constexpr uint8_t SYSEX_SIMPLE_MSG_SIZE = 7;
static constexpr uint8_t SYSEX_MSG_WITH_VALUE_SIZE = 8;
```

### Constantes pour midi_in
```cpp
static constexpr uint8_t MIDI_MAX_PACKET_SIZE = 4;
static constexpr size_t SYSEX_BUFFER_SIZE = (MIDI_MAX_PACKET_SIZE * 10);
```

## Modifications apportées

### 📄 midi.h
**AVANT :** Contenait seulement les déclarations de base
**APRÈS :** Contient toutes les constantes SysEx centralisées avec documentation claire

### 📄 midi_out.cpp
**AVANT :**
```cpp
// Constantes pour les messages SysEx
static constexpr uint8_t SYSEX_START_BYTE = 0xF0;
static constexpr uint8_t SYSEX_END_BYTE = 0xF7;
// ... 20+ lignes de constantes dupliquées
```

**APRÈS :**
```cpp
#include "midi.h"
// Toutes les constantes sont maintenant dans midi.h
```

### 📄 midi_in.cpp
**AVANT :**
```cpp
#define MIDI_MAX_PACKET_SIZE 4
#define SYSEX_START_BYTE 0xF0
#define SYSEX_END_BYTE 0xF7
// ... constantes dupliquées avec #define
```

**APRÈS :**
```cpp
#include "midi.h"
// Utilise les constantes centralisées de midi.h
```

## Bénéfices obtenus

### ✅ **Élimination de la duplication**
- Les constantes SysEx ne sont plus définies dans plusieurs fichiers
- Une seule source de vérité pour toutes les valeurs SysEx
- Réduction de 25+ lignes de code dupliqué

### ✅ **Maintienabilité améliorée**
- Modification d'une constante dans un seul endroit
- Moins de risque d'incohérence entre les modules
- Documentation centralisée avec commentaires explicatifs

### ✅ **Cohérence des types**
- Remplacement des `#define` par `static constexpr`
- Typage strict pour une meilleure sécurité
- Validation au moment de la compilation

### ✅ **Organisation claire**
- Séparation logique des constantes par usage
- Commentaires explicatifs pour chaque groupe
- Structure claire et documentée

### ✅ **Compatibilité**
- Aucun changement fonctionnel
- Compilation réussie sans erreur
- Comportement identique du système

## Structure finale

```
midi.h
├── Constantes de base SysEx (start, end, manufacturer, device)
├── Family IDs (send, recv, in)
├── Commandes SysEx (toutes les CMD_*)
├── Tailles des buffers (sizes standardisées)
└── Constantes spécifiques midi_in

midi_out.cpp
├── Utilise toutes les constantes de midi.h
└── Plus de duplication

midi_in.cpp  
├── Utilise toutes les constantes de midi.h
└── Plus de duplication
```

## Validation

- ✅ **Compilation réussie** sans erreurs ni warnings SysEx
- ✅ **Fonctionnalité préservée** - aucun changement de comportement
- ✅ **Code plus propre** avec élimination de 25+ lignes dupliquées
- ✅ **Maintenabilité améliorée** avec source unique des constantes

Cette centralisation améliore significativement la structure du code et facilite la maintenance future du protocole SysEx.

## Application dans la fonction onSysEx

### 📄 midi_in.cpp - Fonction onSysEx optimisée

**AVANT :**
```cpp
switch (command) {
  case 1: { // Handshake avec l'éditeur
    // Magic numbers partout
    const uint8_t editor_handshake[] = { 
      240, SYSEX_MANUFACTURER_ID, SYSEX_DEVICE_ID, SYSEX_FAMILY_ID_IN, 19, 1, 
      FIRMWARE_MAJOR_VERSION, FIRMWARE_MINOR_VERSION, 247 
    };
    break;
  }
  case 56: case 57: case 58: case 59: { // IP addresses
    // Magic numbers non documentés
  }
}
```

**APRÈS :**
```cpp
switch (command) {
  case CMD_IN_HANDSHAKE_REQUEST: { // Handshake avec l'éditeur
    // Constantes nommées et explicites
    const uint8_t editor_handshake[] = { 
      SYSEX_START_BYTE, SYSEX_MANUFACTURER_ID, SYSEX_DEVICE_ID, 
      SYSEX_FAMILY_ID_IN, 19, 1, 
      FIRMWARE_MAJOR_VERSION, FIRMWARE_MINOR_VERSION, SYSEX_END_BYTE 
    };
    break;
  }
  case CMD_IN_SET_IP: 
  case CMD_IN_SET_DNS: 
  case CMD_IN_SET_GATEWAY: 
  case CMD_IN_SET_SUBNET: { // IP addresses
    // Constantes explicites et auto-documentées
  }
}
```

### ✅ Améliorations apportées à onSysEx

#### **Élimination des Magic Numbers**
- Tous les numéros de commande (1, 3, 4, 5, etc.) remplacés par des constantes nommées
- Messages SysEx utilisent les constantes `SYSEX_START_BYTE` et `SYSEX_END_BYTE`
- Code auto-documenté et plus lisible

#### **Documentation intégrée**
- Chaque commande a un nom explicite (`CMD_IN_HANDSHAKE_REQUEST` vs `1`)
- Séparation claire entre commandes d'envoi et de réception
- Intention du code immédiatement compréhensible

#### **Maintenabilité améliorée**
- Modification d'une commande = une seule ligne dans `midi.h`
- Réduction des erreurs de frappe dans les switch statements
- Code plus robuste et facile à étendre
