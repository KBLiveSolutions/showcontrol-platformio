

#pragma once
namespace midi {
    void clearSysExBuffer();
    void onSerialNoteOn(byte channel, byte note, byte vel);
    void onControlChange(uint8_t channel, uint8_t control, uint8_t value);
    void onSysEx(uint8_t *data, unsigned int _length);
    void onUSBSysEx(uint8_t *data, unsigned int _length);

    void handleMIDIDAWMessage(uint8_t *packet);
    void handleSysExMessage(uint8_t *packet);
    void handleSysExStart(uint8_t *packet);
    void handleSysExContinuation(uint8_t *packet);
    // void parseArrayItem(uint8_t itemType, char* strBuf, uint8_t listIndex, uint8_t listLength);
    void parseDisplayItem(uint8_t itemType, char* strBuf, int arg2);
    void read();
}