#include "globalParser.h"
#include "../display/pages/globalPage.h"
#include "pages.h"
#include "../leds/leds.h"


Global global;

void Global::setLoopEnabled(bool enabled){
    loopEnabled = enabled;
    if (enabled) {
        ETHERNET_DEBUG_LOGLN("Loop enabled");
    } else {
        ETHERNET_DEBUG_LOGLN("Loop disabled");
    }   
};
void Global::setLoopStart(float start){
    loopStart = start;
    ETHERNET_DEBUG_LOG("Loop start set to: ");
    ETHERNET_DEBUG_LOGLN(loopStart);  
};
void Global::setLoopEnd(float end){
    loopEnd = end;
    ETHERNET_DEBUG_LOG("Loop end set to: ");
    ETHERNET_DEBUG_LOGLN(loopEnd);  
};
void Global::setBeatsPosition(float position){
    beatsPosition = position;
    ETHERNET_DEBUG_LOG("Beats position set to: ");
    ETHERNET_DEBUG_LOGLN(beatsPosition);  
};
void Global::setCurrentBar(int bar){
    current_bar = bar;
    ETHERNET_DEBUG_LOG("Current bar set to: ");
    ETHERNET_DEBUG_LOGLN(current_bar);  
};
void Global::setCurrentBeat(int beat){
    current_beat = beat;
    ETHERNET_DEBUG_LOG("Current beat set to: ");
    ETHERNET_DEBUG_LOGLN(current_beat);  
    globalPage.showCounter();
};
void Global::setSigNumerator(int numerator){
    sig_numerator = numerator;
    ETHERNET_DEBUG_LOG("Signature numerator set to: ");
    ETHERNET_DEBUG_LOGLN(sig_numerator);  
};
void Global::setSigDenominator(int denominator){
    sig_denominator = denominator;
    ETHERNET_DEBUG_LOG("Signature denominator set to: ");
    ETHERNET_DEBUG_LOGLN(sig_denominator);
};
void Global::setTempo(int newTempo){
    tempo = newTempo;
    snprintf(tempoStr, sizeof(tempoStr), "%d", tempo);
    globalPage.showTempo();
    ETHERNET_DEBUG_LOG("Tempo set to: ");
    ETHERNET_DEBUG_LOGLN(tempoStr);
};
void Global::setIsPlaying(bool playing){
    isPlaying = playing;
    if (playing) {
        ETHERNET_DEBUG_LOGLN("Playback started");
    } else {
        ETHERNET_DEBUG_LOGLN("Playback stopped");
    }  
    if(activePage == pages[0]){
        if (global.isPlaying) l[1].show_green();
        else l[1].show_lightGreen();
        }
    l[2].r = 100;
    l[2].g = 0;
    l[2].b = 80;
    l[2].show_color();
};

