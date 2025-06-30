#include "globalParser.h"
#include "../display/pages/globalPage.h"
#include "pages.h"
#include "../leds/leds.h"


Global global;

void Global::setLoopEnabled(bool enabled){
    loopEnabled = enabled;
    if (enabled) {
        Serial.println("Loop enabled");
    } else {
        Serial.println("Loop disabled");
    }   
};
void Global::setLoopStart(float start){
    loopStart = start;
    Serial.print("Loop start set to: ");
    Serial.println(loopStart);  
};
void Global::setLoopEnd(float end){
    loopEnd = end;
    Serial.print("Loop end set to: ");
    Serial.println(loopEnd);  
};
void Global::setBeatsPosition(float position){
    beatsPosition = position;
    Serial.print("Beats position set to: ");
    Serial.println(beatsPosition);  
};
void Global::setCurrentBar(int bar){
    current_bar = bar;
    Serial.print("Current bar set to: ");
    Serial.println(current_bar);  
};
void Global::setCurrentBeat(int beat){
    current_beat = beat;
    Serial.print("Current beat set to: ");
    Serial.println(current_beat);  
    globalPage.showCounter();
};
void Global::setSigNumerator(int numerator){
    sig_numerator = numerator;
    Serial.print("Signature numerator set to: ");
    Serial.println(sig_numerator);  
};
void Global::setSigDenominator(int denominator){
    sig_denominator = denominator;
    Serial.print("Signature denominator set to: ");
    Serial.println(sig_denominator);
};
void Global::setTempo(int newTempo){
    tempo = newTempo;
    snprintf(tempoStr, sizeof(tempoStr), "%d", tempo);
    globalPage.showTempo();
    Serial.print("Tempo set to: ");
    Serial.println(tempoStr);
};
void Global::setIsPlaying(bool playing){
    isPlaying = playing;
    if (playing) {
        Serial.println("Playback started");
    } else {
        Serial.println("Playback stopped");
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

