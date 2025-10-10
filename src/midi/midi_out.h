
#pragma once
#include "../core/utils.h"

namespace midi {
    void sendControls(uint8_t pageNum);
    void forwardMessageToUSBMIDI(uint8_t *packet);
    void sendSysexToDAW(const uint8_t *data, uint16_t length);
    void sendToDAWPort(control_type_t type, uint8_t channel, uint8_t data1, uint8_t data2);
    void sendActiveSetlistChange(uint8_t setlistIndex);
    void sendToUSBMIDIPort(uint8_t *array);
    void sendUserPageAmount();
    void sendSettings();
    void sendLiveHandshake();
    void sendLiveUpdateRequest();
    
    // Nouvelles fonctions utilitaires
    bool sendControlConfigurations(uint8_t pageNum);
    bool sendButtonConfiguration(uint8_t pageNum, uint8_t control_index);
    bool sendLedConfiguration(uint8_t pageNum, uint8_t control_index);
    bool sendDisplayConfigurations(uint8_t pageNum);
    bool sendLedBrightnessSetting();
    bool sendNightModeSetting();
    bool sendDisplayBrightnessSetting();
    bool sendManualIPSetting();
    bool sendPortSetting();
    void encodeIPOctet(uint8_t ip_octet, uint8_t *msb, uint8_t *lsb);
}