#include "OSCReceive.h"
#include "ethernetManager.h"
#include "../core/mainPageParser.h"
#include "../core/settingsParser.h"
#include "../core/globalParser.h"
#include "../display/colors.h"
#include "../display/pages/globalPage.h"
#include "../core/utils.h"
#include "serviceManager.h"

volatile bool oscMsgProcessing = false;
unsigned long lastOscTime = 0;
void receiveOSCMsg(char* _packetBuffer, int packetSize) {
    // Ignorer les paquets venant de ShowControl si au moins un serveur Ableset est détecté
    if (serviceManager.serviceCounts[0] > 0) {
        if (showcontrolUdp.remotePort() == showcontrolAppInPort) {
            DEBUG_OSC_LOG("Message ShowControl ignoré (Ableset détecté, port ShowControl)");
            return;
        }
    }

    OSCMessage msg;
    
    // CORRECTION : Utiliser la taille du paquet UDP au lieu de chercher '\0'
    for (int i = 0; i < packetSize; i++) {
        msg.fill((uint8_t)_packetBuffer[i]);
    }

    if (!msg.hasError()) {
        String address = msg.getAddress();
        
        // DEBUG_OSC_LOG("OSC Message Received from: ");
        // DEBUG_OSC_LOG_CONT(showcontrolUdp.remoteIP());
        // DEBUG_OSC_LOG_CONT(" / ");
        // DEBUG_OSC_LOG_CONT(showcontrolUdp.remotePort());
        // DEBUG_OSC_LOG_CONT(" : ");
        // DEBUG_OSC_LOG_CONT(msg.getAddress());
        // DEBUG_OSC_LOG_CONT(" | UDP size: ");
        // DEBUG_OSC_LOG_CONT(packetSize);
        // DEBUG_OSC_LOG_CONT(" | OSC size: ");
        // DEBUG_OSC_LOG_CONT_LN(msg.size());

        // Routes OSC avec callbacks inline
        msg.route("/global/beatsPosition", [](OSCMessage &msg, int addrOffset) {
          global.beatsPosition = msg.getFloat(0);
          global.current_bar = int(global.beatsPosition / 4) + 1;
          global.current_beat = (int)global.beatsPosition % 4 + 1;
          global.setCurrentBar(global.current_bar);
          global.setCurrentBeat(global.current_beat);
        });
        
        msg.route("/global/tempo", [](OSCMessage &msg, int addrOffset) {
          DEBUG_OSC_LOGLN("Tempo received");
          global.onTempo(msg.getFloat(0));
        });
        
        msg.route("/global/isPlaying", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Is Playing received");
            global.onIsPlaying(msg.getInt(0));
        });
        
        msg.route("/setlist/loopEnabled", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Loop enabled received");
            global.onLoopEnabled(msg.getInt(0));
        });

        msg.route("/global/timeSignature", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Signature received");
            // int global.sig_numerator = msg.getInt(0);
            // int global.sig_denominator = msg.getInt(1);
            // sprintf(itemToDisplay[12], "%d/%d", global.sig_numerator, global.sig_denominator);
            // globalPage.showTempo();
        });
        
        msg.route("/global/humanPosition", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Human position received");
        });
        
        msg.route("/global/isLoadingFile", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOG("Ableset loading file: ");
            DEBUG_OSC_LOG_CONT_LN(msg.getInt(0));
            // if(msg.getInt(0) == 0) mainParser.onNewSetLoaded();
        });
        
        msg.route("/global/isConnected", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOG("Ableset connected: ");
            DEBUG_OSC_LOG_CONT_LN(msg.getInt(0));
        });
        
        msg.route("/global/isRecording", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Ableset recording state");
        });
        
        msg.route("/global/isSyncingToRemoteTick", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Ableset sync state");
        });
        
        msg.route("/global/quantization", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Ableset quantization");
        });
        
        msg.route("/audioInterfaces/connected", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Audio interfaces connected");
        });
        
        msg.route("/audioInterfaces/count", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Audio interfaces count");
        });
        
        msg.route("/audioInterfaces/names", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Audio interfaces names");
        });
        
        msg.route("/audioInterfaces/scenes", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Audio interfaces scenes");
        });

        // msg.route("/global/finePosition", [](OSCMessage &msg, int addrOffset) {
        //     DEBUG_OSC_LOGLN("/setlist/finePosition received");
        //     mainParser.updateSongProgress();
        // });

        msg.route("/setlist/name", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Setlist Name received");
            // char strBuf[32];
            // msg.getString(0, strBuf, 32);
            // showSprite(itemToDisplay[indexOfItem(ActiveSetlist)], _LightGray, titleSprite);
        });
        
        msg.route("/setlist/songs", [](OSCMessage &msg, int addrOffset) {
          DEBUG_OSC_LOGLN("Songs List received");
          int size = msg.size();
          DEBUG_OSC_LOG("songs size: ");
          DEBUG_OSC_LOG_CONT_LN(size);
          mainParser.songsListSize = size;
          for (int j = 0; j < size; j++) {
            char strBuf[MAX_SONG_NAME];
            msg.getString(j, strBuf, MAX_SONG_NAME);
            for (int i = 0; i < MAX_SONG_NAME; i++) {
              mainParser.songsList[j][i] = strBuf[i];
            }
            DEBUG_OSC_LOG("songs: ");
            DEBUG_OSC_LOG_CONT_LN(mainParser.songsList[j]);
          }
        });
        
        msg.route("/setlist/songColors", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Song Colors received");
            // int size = msg.size();
            // DEBUG_OSC_LOG("colors size: ");
            // DEBUG_OSC_LOGLN(size);
        });
        
        msg.route("/setlist/songDurations", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Song Durations received");
            // int size = msg.size();
            // DEBUG_OSC_LOG("songs durations: ");
            // DEBUG_OSC_LOGLN(size);
        });
        
        msg.route("/setlist/sections", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Sections List received");
            int size = msg.size();
            DEBUG_OSC_LOG("onSections size: ");
            DEBUG_OSC_LOG_CONT_LN(size);
            for (int j = 0; j < size; j++) {
                char strBuf[MAX_SONG_NAME];
                msg.getString(j, strBuf, MAX_SONG_NAME);
                strcpy(mainParser.sections[j], strBuf);
                DEBUG_OSC_LOG("mainParser.sections: ");
                DEBUG_OSC_LOGLN(mainParser.sections[j]);
            }
        });
        
        msg.route("/setlist/sectionColors", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Section colors received");
        });
        
        msg.route("/setlist/activeSongName", [](OSCMessage &msg, int addrOffset) {
            char strBuf[128];  // Buffer plus grand pour accepter n'importe quelle chaîne
            msg.getString(0, strBuf, 128);
            mainParser.onActiveSongName(strBuf);
        });
        
        msg.route("/setlist/activeSongColor", [](OSCMessage &msg, int addrOffset) {
            char strBuf[32];
            msg.getString(1, strBuf, 32);
            mainParser.onActiveSongColor(hexStringtoRGB565(strBuf));
        });
        
        msg.route("/setlist/activeSongIndex", [](OSCMessage &msg, int addrOffset) {
            mainParser.onActiveSongIndex(msg.getFloat(0));
        });
        
        msg.route("/setlist/activeSongStart", [](OSCMessage &msg, int addrOffset) {
            mainParser.onActiveSongStart(msg.getFloat(0));
        });
        
        msg.route("/setlist/activeSongEnd", [](OSCMessage &msg, int addrOffset) {
            mainParser.onActiveSongEnd(msg.getFloat(0));
        });
        
        msg.route("/setlist/activeSongDuration", [](OSCMessage &msg, int addrOffset) {
            mainParser.onActiveSongDuration(msg.getFloat(0));
        });
        
        msg.route("/setlist/activeSongProgress", [](OSCMessage &msg, int addrOffset) {
            mainParser.onActiveSongProgress(msg.getFloat(0));
        });
        
        msg.route("/setlist/activeSectionName", [](OSCMessage &msg, int addrOffset) {
            char strBuf[128];  // Buffer plus grand pour accepter n'importe quelle chaîne
            msg.getString(0, strBuf, 128);
            mainParser.onActiveSectionName(strBuf);
            DEBUG_OSC_LOG(F("activeSectionName changed: "));
            DEBUG_OSC_LOG_CONT_LN(mainParser.activeSectionName);
        });
        
        msg.route("/setlist/activeSectionIndex", [](OSCMessage &msg, int addrOffset) {
            mainParser.activeSectionIndex = (int) msg.getFloat(0);
            DEBUG_OSC_LOG(F("activeSectionIndex changed: "));
            DEBUG_OSC_LOG_CONT_LN(mainParser.activeSectionIndex);
        });
        
        msg.route("/setlist/nextSongName", [](OSCMessage &msg, int addrOffset) {
            char strBuf[128];  // Buffer plus grand pour accepter n'importe quelle chaîne
            msg.getString(0, strBuf, 128);
            mainParser.onNextSongName(strBuf);
        });
        
        msg.route("/setlist/nextSectionName", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Next section name received");
        });
        
        msg.route("/setlist/remainingTimeInSong", [](OSCMessage &msg, int addrOffset) {
            mainParser.onRemainingTimeInSong(msg.getFloat(0));
        });
        
        msg.route("/setlist/remainingTimeInSet", [](OSCMessage &msg, int addrOffset) {
            mainParser.onRemainingTimeInSet(msg.getFloat(0));
        });
        
        msg.route("/subscribed", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOG("Subscribed message received from: ");
            DEBUG_OSC_LOG_CONT(showcontrolUdp.remoteIP());
            DEBUG_OSC_LOG_CONT(":");
            DEBUG_OSC_LOG_CONT_LN(showcontrolUdp.remotePort());
            settings.getItStarted();
        });
        
        msg.route("/heartbeat", [](OSCMessage &msg, int addrOffset) {
            // DEBUG_OSC_LOG("Heartbeat received from: ");
            // DEBUG_OSC_LOG_CONT(showcontrolUdp.remoteIP());
            // DEBUG_OSC_LOG_CONT(":");
            // DEBUG_OSC_LOG_CONT_LN(showcontrolUdp.remotePort());
            int isShowControl = msg.getInt(0); // 1 si ShowControl, 0 si Ableset
            bool serviceFound = false;
            
            // Chercher d'abord dans les services Ableset (comparer seulement IP et type)
            for (int i = 0; i < serviceManager.totalServiceCount; i++) {
                if (serviceManager.discoveredServices[i].ip == showcontrolUdp.remoteIP()) {
                    serviceManager.discoveredServices[i].lastSeen = millis();
                    // DEBUG_OSC_LOGLN("Running service heartbeat - updating last seen");
                    serviceFound = true;
                    // Fallback: si c'est le premier service Ableset et que l'UI n'est pas encore lancée,
                    // lancer l'UI maintenant (au cas où le message /subscribed n'est pas reçu)
                    if (serviceManager.totalServiceCount == 1 && !settings.isRunning) {
                        DEBUG_OSC_LOGLN("First Ableset heartbeat received and UI not running, calling getItStarted as fallback");
                        settings.getItStarted();
                    }
                    break;
                }
            }

            // Si aucun service trouvé, ajouter nouveau service 
            // IMPORTANT: C'est ici que les services sont re-ajoutés après une reconnexion
            if (!serviceFound) {
                DEBUG_OSC_LOG("Service not found, adding service from heartbeat: ");
                DEBUG_OSC_LOG(showcontrolUdp.remoteIP());
                DEBUG_OSC_LOG(":");
                DEBUG_OSC_LOG_CONT_LN(showcontrolUdp.remotePort());
                serviceManager.addService(isShowControl == 0 ? ABLESETSRV : SHOWCONTROLSRV, showcontrolUdp.remoteIP(), showcontrolUdp.remotePort());
                serviceManager.listAllServices(); // Debug: lister tous les services après ajout
                
            }
        });

        msg.route("/showcontrol/sceneName", [](OSCMessage &msg, int addrOffset) {
            char strBuf[128];  // Buffer plus grand pour accepter n'importe quelle chaîne
            msg.getString(0, strBuf, 128);
            mainParser.onSceneName(strBuf);
        });
        msg.route("/showcontrol/trackName", [](OSCMessage &msg, int addrOffset) {
            char strBuf[128];  // Buffer plus grand pour accepter n'importe quelle chaîne
            msg.getString(0, strBuf, 128);
            mainParser.onTrackName(strBuf);
        });
                msg.route("/showcontrol/leftMarkerName", [](OSCMessage &msg, int addrOffset) {
            char strBuf[128];  // Buffer plus grand pour accepter n'importe quelle chaîne
            msg.getString(0, strBuf, 128);
            mainParser.onLeftMarkerName(strBuf);
        });
                msg.route("/showcontrol/rightMarkerName", [](OSCMessage &msg, int addrOffset) {
            char strBuf[128];  // Buffer plus grand pour accepter n'importe quelle chaîne
            msg.getString(0, strBuf, 128);
            mainParser.onRightMarkerName(strBuf);
        });
        msg.route("/showcontrol/looperName", [](OSCMessage &msg, int addrOffset) {
            char strBuf[128];  // Buffer plus grand pour accepter n'importe quelle chaîne
            msg.getString(0, strBuf, 128);
            // mainParser.onLooperName(strBuf);
        });
        msg.route("/sysex", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Sysex message received");
            int length = msg.getInt(0);
            if (length > 0 && length <= 256) { // Limite la taille pour éviter les débordements
                uint8_t sysex_msg[256];
                for (int i = 0; i < length; i++) {
                    sysex_msg[i] = (uint8_t)msg.getInt(i + 1);
                }
                osc.sendSysex(sysex_msg, length);
            } else {
                DEBUG_OSC_LOGLN("Invalid sysex length");
            }
        });
        msg.route("/controlChange", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Control Change message received");
            DEBUG_OSC_LOG("Args: ");
            for (int i = 0; i < msg.size(); i++) {
                DEBUG_OSC_LOG_CONT(msg.getInt(i));
                DEBUG_OSC_LOG_CONT(" ");
            }
            DEBUG_OSC_LOGLN("");
            int channel = msg.getInt(0);
            int controller = msg.getInt(1);
            int value = msg.getInt(2);
            mainParser.onCCReceived(channel, controller, value);
        });
    }
    else {
        DEBUG_OSC_LOG("Error OSC message - UDP size: ");
        DEBUG_OSC_LOG(packetSize);
        DEBUG_OSC_LOG(" | Error code: ");
        DEBUG_OSC_LOGLN(msg.getError());
    }
  lastOscTime = millis();

}