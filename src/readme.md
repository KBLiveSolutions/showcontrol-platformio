##Loop:
- checks inputs
    - buttons
    - encoder
    - pedals
- listens to ethernet
- listens to midi:
    - from usb
    - from host

##Setup:
- creates midi
    ⮑ creates midi_in
    ⮑ creates midi_out
- creates ethenet
    ⮑ creates osc
    ⮑ creates osc parser
- creates display
    ⮑ creates sprites
    ⮑ creates display pages
- creates pages
    ⮑ creates json
    ⮑ creates setlist
- creates leds
- creates buttons
- creates encoder
- creates pedals

Trucs à afficher:
**Top bar**:
- tempo
- counter
- setlist
- user mode
- ethernet icon
- scene A/B
**Main Page**
- Main Sprite
- Sub1 Sprite
- Sub2 Sprite
- Progress Bar
- songs count
- song time
- setlist time
 
Bien sûr ! Voici une proposition d’**architecture modulaire optimisée** pour ton projet, avec un découpage clair et des dépendances minimales entre modules.  
L’objectif :  
- **Chaque module ne dépend que de ce dont il a besoin**  
- **Pas de dépendances croisées inutiles**  
- **Un point d’entrée central (main.ino)**

---

## **Architecture proposée**

```
main.ino
│
├── config/
│   ├── consts.h
│   ├── state.h
│   └── jsonManager.{h,cpp}
│
├── core/
│   ├── pages.{h,cpp}
│   └── setlist.{h,cpp}
│
├── display/
│   ├── DisplayManager.{h,cpp}
│   ├── sprites.{h,cpp}
│   ├── colors.{h,cpp}
│   └── pages/
│       ├── mainPage.{h,cpp}
│       ├── settingsPage.{h,cpp}
│       └── Menu.{h,cpp}
│
├── input/
│   ├── Buttons.{h,cpp}
│   ├── encoder.{h,cpp}
│   └── pedals.{h,cpp}
│
├── leds/
│   ├── Leds.{h,cpp}
│   └── led_utils.{h,cpp}
│
├── midi/
│   ├── midi.{h,cpp}
│   ├── midi_in.{h,cpp}
│   └── midi_out.{h,cpp}
│
├── network/
│   ├── ethernet.{h,cpp}
│
├── osc/
│   ├── OSCManager.{h,cpp}
│   └── OSCParser.{h,cpp}
```

---

## **Dépendances minimales entre modules**

### **1. config/**
- **consts.h** : définitions globales, inclus partout si besoin.
- **state.h** : variables d’état globales, inclus là où nécessaire.
- **jsonManager** : ne dépend que de `ArduinoJson` et éventuellement de `LittleFS`.

### **2. core/**
- **pages.h/cpp** : dépend de consts.h, sprites.h (pour les pointeurs de sprites), et de `mainPageElements.h` si besoin.
- **mainPageElements.h/cpp** : dépend de consts.h.

### **3. display/**
- **DisplayManager** : dépend de `sprites.h`, `colors.h`, et éventuellement des pages display.
- **sprites.h/cpp** : dépend de `TFT_eSPI.h` et consts.h.
- **colors.h/cpp** : indépendant, utilisé partout où tu manipules des couleurs.
- **pages/** : chaque page display dépend de `sprites.h` et de ce dont elle a besoin.

### **4. input/**
- **Buttons, encoder, pedals** : dépendent de consts.h et éventuellement de pages.h pour notifier les changements.

### **5. leds/**
- **Leds, led_utils** : dépendent de consts.h et éventuellement de colors.h.

### **6. midi/**
- **midi, midi_in, midi_out** : dépendent de la lib MIDI, de consts.h, et éventuellement de pages.h pour notifier les actions.

### **7. network/**
- **ethernet** : dépend de la lib Ethernet, de consts.h.

### **8. osc/**
- **OSCManager, OSCParser** : dépendent de la lib OSC, de `network/ethernet.h`, et éventuellement de pages.h pour notifier les actions.

---

## **Schéma des dépendances (simplifié)**

```
main.ino
│
├─ config/consts.h
├─ core/settingsParser.h
├─ config/jsonManager.h
│
├─ core/pages.h ──> display/sprites.h
│                └─> core/mainPageParser.h
│
├─ display/pages/mainPage.h ──> display/sprites.h
│                            └─> display/colors.h
│
├─ display/sprites.h ──> TFT_eSPI.h
│
├─ display/colors.h
│
├─ input/Buttons.h ──> config/consts.h
│
├─ leds/leds.h ──> config/consts.h
│
├─ midi/midi.h ──> config/consts.h
│
├─ network/ethernet.h ──> config/consts.h
│
├─ osc/OSCManager.h ──> network/ethernet.h
```

---

## **Conseils pour réduire les dépendances**

- **Inclure uniquement ce qui est nécessaire** dans chaque module.
- **Ne pas inclure un header juste parce qu’un autre module l’inclut** : inclure explicitement si tu utilises ses types/fonctions/variables.
- **Utiliser des pointeurs ou des références** pour éviter les inclusions circulaires.
- **Centraliser les variables globales** dans `state.h` ou dans des modules bien identifiés.
- **Pas de logique métier dans les headers** : uniquement des déclarations.

---

## **Exemple de main.ino minimal**

````cpp
#include "midi/midi.h"
#include "network/ethernet.h"
#include "osc/OSCManager.h"
#include "display/pages/mainPage.h"
#include "core/pages.h"
#include "config/jsonManager.h"
#include "core/mainPageParser.h"
#include "leds/leds.h"
#include "input/Buttons.h"
#include "input/encoder.h"
#include "input/pedals.h"

void setup() {
  setup();
  setup();
  setupOSC();
  setupDisplay();
  setupPages();
  setupJSON();
  setupSetlist();
  setupLeds();
  setupButtons();
  setupEncoder();
  setupPedals();
}

void loop() {
  readButtons();
  readEncoder();
  readPedals();
  ethernetLoop();
  oscLoop();
  MIDI.read();
  updateDisplay();
  updateLeds();
}
````

---

## **Conclusion**

- **Chaque module doit être le plus indépendant possible.**
- **Inclue uniquement ce dont tu as besoin dans chaque fichier.**
- **Centralise l’orchestration dans le `.ino` ou un fichier principal.**

---

Si tu veux un schéma graphique ou des exemples de fichiers précis, demande !