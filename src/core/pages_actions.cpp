#include "pages.h"
#include "utils.h"
#include "../config/consts.h"
#include "../display/pages/mainPage.h"
// #include "../display/pages/settingsPage.h"
// #include "../display/pages/menuPage.h"
// #include "../display/pages/splashPage.h"
// #include "../display/pages/globalPage.h"
#include "../display/colors.h"
#include "../leds/leds.h"
#include "../osc/OSCManager.h"
#include "../midi/midi_out.h"
// #include "mainPageParser.h"
// #include "globalParser.h"

// ====================================
// ACTIONS
// ====================================

// Helper function pour traiter les boutons SETLIST
void Page::handleSetlistButton(uint8_t buttonNum)
{
  switch (buttonNum)
  {
  case 0:
    osc.sendOSC("/global/play");
    break;
  case 1:
    osc.sendOSC("/global/stop");
    break;
  case 2:
    osc.sendOSC("/setlist/jumpBySongs", -1);
    break;
  case 3:
    osc.sendOSC("/setlist/jumpBySongs", 1);
    break;
  case 4:
    osc.sendOSC(global.loopEnabled ? "/loop/escape" : "/loop/enable");
    break;
  default:
    break;
  }
}

void Page::buttonLongPress(uint8_t buttonNum)
{
  DEBUG_LOG_VALUE("Page Button LongPress", buttonNum);
  if (buttonNum >= 5)
    return; // Protection contre l'accès hors limites
  osc.sendOSC("/showcontrol/longPress", controls[buttonNum].cc, 127, controls[buttonNum].ch + 1);
}

void Page::buttonDoublePress(uint8_t buttonNum)
{
  if (buttonNum >= 5)
    return; // Protection contre l'accès hors limites
  osc.sendOSC("/showcontrol/doublePress", controls[buttonNum].cc, 127, controls[buttonNum].ch + 1);
}

void Page::onButtonShortPress(uint8_t buttonNum)
{
  // Vérification de sécurité pour buttonPressed[]
  if (buttonNum >= 8)
    return; // Protection pour le tableau buttonPressed[8]
  // DEBUG_LOG_VALUE("Page Button ShortPress", buttonNum);
}

void Page::press_button(uint8_t buttonNum)
{
  DEBUG_LOG_VALUE("Page Button press_button", buttonNum);

  buttonPressed[buttonNum] = true;
  int offset = settings.shiftPressed ? 5 : 0;
  int controlNum = buttonNum + offset;
  DEBUG_LOG_VALUE("Control Number Calculated", controlNum);
  if (buttonNum < 5)
  {
    // Variables locales pour éviter les accès répétés aux tableaux
    uint8_t cc = controls[controlNum].cc;
    uint8_t ch = controls[controlNum].ch;
    control_type_t type = controls[controlNum].type;
    if (controls[controlNum].toggle == 0)
    {
      osc.sendOSC("/showcontrol/control_id", cc, 127, ch + 1);
      midi::sendToDAWPort(type, ch, cc, 127);
    }
    else
    {
      buttonState[buttonNum] = !buttonState[buttonNum]; // Plus efficace que modulo
      uint8_t value = buttonState[buttonNum] * 127;
      osc.sendOSC("/showcontrol/control_id", cc, value, ch + 1);
      midi::sendToDAWPort(type, ch, cc, value);
    }
    if (pageType == USER)mainPage.showButtonSprite(true, buttonNum, controls[controlNum].actionName, controls[controlNum].color, controls[controlNum].luminance);
    else if (pageType == SETLIST) handleSetlistButton(buttonNum);
  }
  else{
    press_pedal(buttonNum - 6);
  }
}

void Page::release_button(uint8_t buttonNum)
{

  buttonPressed[buttonNum] = false;
  int offset = settings.shiftPressed ? 5 : 0;
  int controlNum = buttonNum + offset;
  if (buttonNum < 5)
  {
    // Variables locales pour éviter les accès répétés aux tableaux
    uint8_t cc = controls[controlNum].cc;
    uint8_t ch = controls[controlNum].ch;
    control_type_t type = controls[controlNum].type;

    if (controls[controlNum].toggle == 0)
    {
      osc.sendOSC("/showcontrol/control_id", cc, 0, ch + 1);
      midi::sendToDAWPort(type, ch, cc, 0);
    }
  }

  if (buttonNum < 6)
  {
    updateLed(controlNum, 127);
    if (pageType == USER) mainPage.showButtonSprite(false, buttonNum, controls[controlNum].actionName, controls[controlNum].color, controls[controlNum].luminance);
    
    // if (control_toggle[buttonNum] == 0 && control_type[buttonNum] != PC) {
    //   osc.sendOSC("/showcontrol/control_id", control_cc[buttonNum], 0, control_ch[buttonNum] + 1);
    //   midi::sendToDAWPort(control_type[buttonNum], control_ch[buttonNum], control_cc[buttonNum], 0);
    // }
  }
  // else release_pedal(buttonNum - 6);
}

void Page::doubleClick(uint8_t buttonNum)
{
}

// ====================================
// PEDALS
// ====================================

void Page::expressionPedal(uint8_t pedalNum, uint8_t value)
{
  // TO DO: Implement expression pedal handling
  // midi::sendToDAWPort(CC, pedal_ch[pedalNum], pedal_cc[pedalNum], value);
}

void Page::press_pedal(uint8_t pedalNum)
{
  // if (pedal_toggle[pedalNum] == 0) {
  //   osc.sendOSC("/showcontrol/control_id", pedal_cc[pedalNum], 127, pedal_ch[pedalNum]);
  //   midi::sendToDAWPort(pedal_type[pedalNum], pedal_ch[pedalNum], pedal_cc[pedalNum], 127);
  // } else {
  //   pedalState[pedalNum] = (pedalState[pedalNum] + 1) % 2;
  //   osc.sendOSC("/showcontrol/control_id", pedal_cc[pedalNum], pedalState[pedalNum] * 127, pedal_ch[pedalNum]);
  //   midi::sendToDAWPort(pedal_type[pedalNum], pedal_ch[pedalNum], pedal_cc[pedalNum], pedalState[pedalNum] * 127);
  // }
}

void Page::release_pedal(uint8_t pedalNum)
{
  // if (pedal_toggle[pedalNum] == 0) {
  //   osc.sendOSC("/showcontrol/control_id", pedal_cc[pedalNum], 0, pedal_ch[pedalNum]);
  //   midi::sendToDAWPort(pedal_type[pedalNum], pedal_ch[pedalNum], pedal_cc[pedalNum], 0);
  // }
}

// Méthodes de Page (copie/colle le contenu de chaque méthode ici)
// ... (toutes les méthodes de la struct Page, à copier depuis ton .h)
