#include "globalParser.h"
#include "../display/pages/globalPage.h"
#include "pages.h"
#include "../leds/leds.h"


Global global;

void Global::setLoopEnabled(bool enabled){
    loopEnabled = enabled;
    if (enabled) {
        if(activePage == pages[0]){
            l[5].setColor(80, 0); // Set to yellow
            l[5].show_color();
        }
        DEBUG_LOGLN("Loop enabled");
    } 
    else {
            l[5].setColor(81, 0); // Set to yellow
            l[5].show_color();
    }   
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
    globalPage.showCounter();
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
    if (playing) {
        DEBUG_LOGLN("Playback started");
    } else {
        DEBUG_LOGLN("Playback stopped");
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

