#pragma once

class Global{
    public:
    // GLOBAL VARIABLES
        bool loopEnabled = false;
        bool isPlaying = false;
        float loopStart, beatsPosition = 0, loopEnd;
        int current_bar, current_beat = 1, sig_numerator = 4, sig_denominator = 4, tempo = 120;
        char tempoStr[7];
        void onLoopEnabled(bool enabled);
        void setLoopStart(float start);
        void setLoopEnd(float end);
        void setBeatsPosition(float position);
        void setCurrentBar(int bar);
        void setCurrentBeat(int beat);
        void setSigNumerator(int numerator);
        void setSigDenominator(int denominator);
        void onTempo(int newTempo);
        void onIsPlaying(bool playing);
};

extern Global global;