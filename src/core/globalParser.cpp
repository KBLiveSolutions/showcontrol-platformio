#include "globalParser.h"
#include "../display/pages/globalPage.h"
#include "pages.h"
#include "../leds/leds.h"
#include "../display/pages/mainPage.h"


Global global;


void Global::setLoopEnabled(bool enabled){
    loopEnabled = enabled;
    if(activePage == &pages[0]) activePage->updateLoopSprite();
};

void Global::setLoopStart(float start){
    loopStart = start;
    DEBUG_LOG("Loop start set to: ");
    DEBUG_LOGLN(loopStart);  
};
void Global::setLoopEnd(float end){
    loopEnd = end;
    DEBUG_LOG("Loop end set to: ");
    DEBUG_LOGLN(loopEnd);  
};
void Global::setBeatsPosition(float position){
    beatsPosition = position;
    DEBUG_LOG("Beats position set to: ");
    DEBUG_LOGLN(beatsPosition);  
};
void Global::setCurrentBar(int bar){
    current_bar = bar;
    DEBUG_LOG("Current bar set to: ");
    DEBUG_LOGLN(current_bar);  
};
void Global::setCurrentBeat(int beat){
    current_beat = beat;
    DEBUG_LOG("Current beat set to: ");
    DEBUG_LOGLN(current_beat);  
    DEBUG_LOG("Beats position set to: ");   
    DEBUG_LOGLN(global.beatsPosition);
    globalPage.showCounter();
    mainPage.updateProgressBar();
};
void Global::setSigNumerator(int numerator){
    sig_numerator = numerator;
    DEBUG_LOG("Signature numerator set to: ");
    DEBUG_LOGLN(sig_numerator);  
};
void Global::setSigDenominator(int denominator){
    sig_denominator = denominator;
    DEBUG_LOG("Signature denominator set to: ");
    DEBUG_LOGLN(sig_denominator);
};
void Global::setTempo(int newTempo){
    tempo = newTempo;
    snprintf(tempoStr, sizeof(tempoStr), "%d", tempo);
    globalPage.showTempo();
    DEBUG_LOG("Tempo set to: ");
    DEBUG_LOGLN(tempoStr);
};
void Global::setIsPlaying(bool playing){
    isPlaying = playing;
    if(activePage == &pages[0]) activePage->updatePlaySprite();
};

