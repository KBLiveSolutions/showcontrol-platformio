
// ====================================
// ACTIONS
// ====================================

// Helper function pour traiter les boutons SETLIST
void Page::handleSetlistButton(uint8_t buttonNum) {
  switch (buttonNum) {
    case 0:
      sendOSCAbleset("/global/play");
      break;
    case 1:
      sendOSCAbleset("/global/stop");
      break;
    case 2:
      sendOSCAbleset("/setlist/jumpBySongs", -1);
      break;
    case 3:
      sendOSCAbleset("/setlist/jumpBySongs", 1);
      break;
    case 4:
      sendOSCAbleset(global.loopEnabled ? "/loop/escape" : "/loop/enable");
      break;
    default:
      break;
  }
}

void Page::buttonLongPress(uint8_t buttonNum) {
  if (buttonNum >= 5) return;  // Protection contre l'accès hors limites
  sendOSCShowControl("/showcontrol/longPress", control_cc[buttonNum], 127, control_ch[buttonNum] + 1);
}

void Page::buttonDoublePress(uint8_t buttonNum) {
  if (buttonNum >= 5) return;  // Protection contre l'accès hors limites
  sendOSCShowControl("/showcontrol/doublePress", control_cc[buttonNum], 127, control_ch[buttonNum] + 1);
}

void Page::onButtonShortPress(uint8_t buttonNum) {
  // Vérification de sécurité pour buttonPressed[]
  if (buttonNum >= 8) return;  // Protection pour le tableau buttonPressed[8]
  
  buttonPressed[buttonNum] = true;
  
  if (buttonNum < 5) {
    // Variables locales pour éviter les accès répétés aux tableaux
    uint8_t cc = control_cc[buttonNum];
    uint8_t ch = control_ch[buttonNum];
    control_type_t type = control_type[buttonNum];
    
    if (control_toggle[buttonNum] == 0) {
      sendOSCShowControl("/showcontrol/control_id", cc, 127, ch + 1);
      midi::sendToDAWPort(type, ch, cc, 127);
      midi::sendToDAWPort(type, ch, cc, 0);
    } 
    else {
      buttonState[buttonNum] = !buttonState[buttonNum];  // Plus efficace que modulo
      uint8_t value = buttonState[buttonNum] * 127;
      sendOSCShowControl("/showcontrol/control_id", cc, value, ch + 1);
      midi::sendToDAWPort(type, ch, cc, value);
    }
    
    // Traitement spécifique selon le type de page
    if (pageType == SETLIST) {
      handleSetlistButton(buttonNum);
    }
    // Note: USER case commenté dans l'original
  } 
  else {
    press_pedal(buttonNum - 6);
  }
}

void Page::press_button(uint8_t buttonNum) {
}

void Page::release_button(uint8_t buttonNum) {
  // buttonPressed[buttonNum] = false;
  // if (buttonNum < 5) {
  //   l[buttonNum + 1].show_color();
  //   if (pageType == USER) 
  //   {
  //     mainPage.showButton(buttonPressed[buttonNum], buttonNum, getActionName(pageNumber, buttonNum), getActionColor(pageNumber, buttonNum), getLuminance(buttonNum));
  //   }
  //   if (control_toggle[buttonNum] == 0 && control_type[buttonNum] != PC) {
  //     sendOSCShowControl("/showcontrol/control_id", control_cc[buttonNum], 0, control_ch[buttonNum] + 1);
  //     midi::sendToDAWPort(control_type[buttonNum], control_ch[buttonNum], control_cc[buttonNum], 0);
  //   }
  // } 
  // else release_pedal(buttonNum - 6);
}



void Page::doubleClick(uint8_t buttonNum) {
}

// ====================================
// PEDALS
// ====================================


void Page::expressionPedal(uint8_t pedalNum, uint8_t value) {
  // TO DO: Implement expression pedal handling
  // midi::sendToDAWPort(CC, pedal_ch[pedalNum], pedal_cc[pedalNum], value);
}

void Page::press_pedal(uint8_t pedalNum) {
  // if (pedal_toggle[pedalNum] == 0) {
  //   sendOSCShowControl("/showcontrol/control_id", pedal_cc[pedalNum], 127, pedal_ch[pedalNum]);
  //   midi::sendToDAWPort(pedal_type[pedalNum], pedal_ch[pedalNum], pedal_cc[pedalNum], 127);
  // } else {
  //   pedalState[pedalNum] = (pedalState[pedalNum] + 1) % 2;
  //   sendOSCShowControl("/showcontrol/control_id", pedal_cc[pedalNum], pedalState[pedalNum] * 127, pedal_ch[pedalNum]);
  //   midi::sendToDAWPort(pedal_type[pedalNum], pedal_ch[pedalNum], pedal_cc[pedalNum], pedalState[pedalNum] * 127);
  // }
}

void Page::release_pedal(uint8_t pedalNum) {
  // if (pedal_toggle[pedalNum] == 0) {
  //   sendOSCShowControl("/showcontrol/control_id", pedal_cc[pedalNum], 0, pedal_ch[pedalNum]);
  //   midi::sendToDAWPort(pedal_type[pedalNum], pedal_ch[pedalNum], pedal_cc[pedalNum], 0);
  // }
}

// Méthodes de Page (copie/colle le contenu de chaque méthode ici)
// ... (toutes les méthodes de la struct Page, à copier depuis ton .h)
