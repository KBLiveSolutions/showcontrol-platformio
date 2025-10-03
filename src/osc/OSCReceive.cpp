#include "OSCReceive.h"
#include "ethernetManager.h"
#include "../core/mainPageParser.h"
#include "../core/settingsParser.h"
#include "../core/globalParser.h"
#include "../display/colors.h"
#include "../display/pages/globalPage.h"
#include "../core/utils.h"

volatile bool oscMsgProcessing = false;
unsigned long lastOscTime = 0;
void receiveOSCMsg(char* _packetBuffer, int packetSize) {
    OSCMessage msg;
    
    // CORRECTION : Utiliser la taille du paquet UDP au lieu de chercher '\0'
    for (int i = 0; i < packetSize; i++) {
        msg.fill((uint8_t)_packetBuffer[i]);
    }

    if (!msg.hasError()) {
        String address = msg.getAddress();
        
        // DEBUG_OSC_LOG("OSC Message Received from: ");
        // DEBUG_OSC_LOG(showcontrolUdp.remoteIP());
        // DEBUG_OSC_LOG(" / ");
        // DEBUG_OSC_LOG(showcontrolUdp.remotePort());
        // DEBUG_OSC_LOG(" : ");
        // DEBUG_OSC_LOG(msg.getAddress());
        // DEBUG_OSC_LOG(" | UDP size: ");
        // DEBUG_OSC_LOG(packetSize);
        // DEBUG_OSC_LOG(" | OSC size: ");
        // DEBUG_OSC_LOGLN(msg.size());

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
          global.setTempo(msg.getFloat(0));
        });
        
        msg.route("/global/isPlaying", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Is Playing received");
            global.setIsPlaying(msg.getInt(0));
        });
        
        msg.route("/setlist/loopEnabled", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Loop enabled received");
            global.setLoopEnabled(msg.getInt(0));
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
            DEBUG_OSC_LOGLN(msg.getInt(0));
            // if(msg.getInt(0) == 0) mainParser.onNewSetLoaded();
        });
        
        msg.route("/global/isConnected", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOG("Ableset connected: ");
            DEBUG_OSC_LOGLN(msg.getInt(0));
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
          DEBUG_OSC_LOGLN(size);
          mainParser.songsListSize = size;
          for (int j = 0; j < size; j++) {
            char strBuf[MAX_SONG_NAME];
            msg.getString(j, strBuf, MAX_SONG_NAME);
            for (int i = 0; i < MAX_SONG_NAME; i++) {
              mainParser.songsList[j][i] = strBuf[i];
            }
            DEBUG_OSC_LOG("songs: ");
            DEBUG_OSC_LOGLN(mainParser.songsList[j]);
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
            DEBUG_OSC_LOGLN(size);
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
            mainParser.setActiveSongName(strBuf);
        });
        
        msg.route("/setlist/activeSongColor", [](OSCMessage &msg, int addrOffset) {
            char strBuf[32];
            msg.getString(1, strBuf, 32);
            mainParser.setActiveSongColor(hexStringtoRGB565(strBuf));
        });
        
        msg.route("/setlist/activeSongIndex", [](OSCMessage &msg, int addrOffset) {
            mainParser.setActiveSongIndex(msg.getFloat(0));
        });
        
        msg.route("/setlist/activeSongStart", [](OSCMessage &msg, int addrOffset) {
            mainParser.setActiveSongStart(msg.getFloat(0));
        });
        
        msg.route("/setlist/activeSongEnd", [](OSCMessage &msg, int addrOffset) {
            mainParser.setActiveSongEnd(msg.getFloat(0));
        });
        
        msg.route("/setlist/activeSongDuration", [](OSCMessage &msg, int addrOffset) {
            mainParser.setActiveSongDuration(msg.getFloat(0));
        });
        
        msg.route("/setlist/activeSongProgress", [](OSCMessage &msg, int addrOffset) {
            mainParser.setActiveSongProgress(msg.getFloat(0));
        });
        
        msg.route("/setlist/activeSectionName", [](OSCMessage &msg, int addrOffset) {
            char strBuf[128];  // Buffer plus grand pour accepter n'importe quelle chaîne
            msg.getString(0, strBuf, 128);
            mainParser.setActiveSectionName(strBuf);
            DEBUG_OSC_LOG(F("activeSectionName changed: "));
            DEBUG_OSC_LOGLN(mainParser.activeSectionName);
        });
        
        msg.route("/setlist/activeSectionIndex", [](OSCMessage &msg, int addrOffset) {
            mainParser.activeSectionIndex = (int) msg.getFloat(0);
            DEBUG_OSC_LOG(F("activeSectionIndex changed: "));
            DEBUG_OSC_LOGLN(mainParser.activeSectionIndex);
        });
        
        msg.route("/setlist/nextSongName", [](OSCMessage &msg, int addrOffset) {
            char strBuf[128];  // Buffer plus grand pour accepter n'importe quelle chaîne
            msg.getString(0, strBuf, 128);
            mainParser.setNextSongName(strBuf);
        });
        
        msg.route("/setlist/nextSectionName", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOGLN("Next section name received");
        });
        
        msg.route("/setlist/remainingTimeInSong", [](OSCMessage &msg, int addrOffset) {
            mainParser.setRemainingTimeInSong(msg.getFloat(0));
        });
        
        msg.route("/setlist/remainingTimeInSet", [](OSCMessage &msg, int addrOffset) {
            mainParser.setRemainingTimeInSet(msg.getFloat(0));
        });
        
        msg.route("/subscribed", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOG("Subscribed message received from: ");
            DEBUG_OSC_LOG(showcontrolUdp.remoteIP());
            DEBUG_OSC_LOG(":");
            DEBUG_OSC_LOGLN(showcontrolUdp.remotePort());
            
            // Chercher le service Ableset existant et le marquer comme souscrit
            bool serviceFound = false;
            bool wasFirstSubscription = false;
            
            for (int i = 0; i < ethernet.totalServiceCount; i++) {
                if (ethernet.discoveredServices[i].serviceType == ABLESETSRV && 
                    ethernet.discoveredServices[i].ip == showcontrolUdp.remoteIP()) {
                    // Vérifier si c'est la première souscription
                    if (!ethernet.discoveredServices[i].subscribed) {
                        wasFirstSubscription = true;
                        ethernet.discoveredServices[i].subscribed = true; // Mark the service as subscribed
                        DEBUG_OSC_LOGLN("Ableset service marked as subscribed for the first time");
                    }
                    ethernet.discoveredServices[i].lastSeen = millis(); // Update last seen
                    serviceFound = true;
                    break;
                }
            }
            
            if (!serviceFound) {
                DEBUG_OSC_LOGLN("WARNING: /subscribed received but no Ableset service found!");
                // Ajouter le service depuis /subscribed (cas de fallback)
                ethernet.addService(ABLESETSRV, showcontrolUdp.remoteIP(), showcontrolUdp.remotePort());
                wasFirstSubscription = true;
            }
            
            // Lancer l'interface utilisateur seulement pour le premier service Ableset souscrit
            if (wasFirstSubscription && ethernet.serviceCounts[0] == 1) {
                DEBUG_OSC_LOGLN("First Ableset subscription confirmed, calling getItStarted from /subscribed");
                settings.getItStarted();
                DEBUG_OSC_LOGLN("Subscribed to Ableset - UI started");
            } else {
                DEBUG_OSC_LOGLN("Ableset subscription confirmed but UI already initialized");
            }
        });
        
        msg.route("/heartbeat", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOG("Heartbeat received from: ");
            DEBUG_OSC_LOG(showcontrolUdp.remoteIP());
            DEBUG_OSC_LOG(":");
            DEBUG_OSC_LOGLN(showcontrolUdp.remotePort());
            
            bool serviceFound = false;
            
            // Chercher d'abord dans les services Ableset (comparer seulement IP et type)
            for (int i = 0; i < ethernet.totalServiceCount; i++) {
                if (ethernet.discoveredServices[i].serviceType == ABLESETSRV && 
                    ethernet.discoveredServices[i].ip == showcontrolUdp.remoteIP()) {
                    ethernet.discoveredServices[i].lastSeen = millis();
                    DEBUG_OSC_LOGLN("Ableset service heartbeat - updating last seen");
                    serviceFound = true;
                    
                    // Fallback: si c'est le premier service Ableset et que l'UI n'est pas encore lancée,
                    // lancer l'UI maintenant (au cas où le message /subscribed n'est pas reçu)
                    if (ethernet.serviceCounts[0] == 1 && !settings.isRunning) {
                        DEBUG_OSC_LOGLN("First Ableset heartbeat received and UI not running, calling getItStarted as fallback");
                        settings.getItStarted();
                    }
                    break;
                }
            }
            
            // Si pas trouvé dans Ableset, chercher dans ShowControl
            if (!serviceFound) {
                for (int i = 0; i < ethernet.totalServiceCount; i++) {
                    if (ethernet.discoveredServices[i].serviceType == SHOWCONTROLSRV && 
                        ethernet.discoveredServices[i].ip == showcontrolUdp.remoteIP()) {
                        ethernet.discoveredServices[i].lastSeen = millis();
                        DEBUG_OSC_LOGLN("ShowControl service heartbeat updated");
                        serviceFound = true;
                        break;
                    }
                }
            }
            
            // Si aucun service trouvé, ajouter comme service Ableset (par défaut)
            // IMPORTANT: C'est ici que les services Ableset sont re-ajoutés après une reconnexion
            if (!serviceFound) {
                DEBUG_OSC_LOG("Service not found, adding Ableset service from heartbeat: ");
                DEBUG_OSC_LOG(showcontrolUdp.remoteIP());
                DEBUG_OSC_LOG(":");
                DEBUG_OSC_LOGLN(showcontrolUdp.remotePort());
                ethernet.addService(ABLESETSRV, showcontrolUdp.remoteIP(), showcontrolUdp.remotePort());
                ethernet.listAllServices(); // Debug: lister tous les services après ajout
                
                // Note: getItStarted() sera appelé quand on recevra le message /subscribed
                // qui confirme que l'abonnement fonctionne correctement
            }
        });
        
        // Handler pour les réponses ping de ShowControl
        msg.route("/showcontrol/pong", [](OSCMessage &msg, int addrOffset) {
            DEBUG_OSC_LOG("ShowControl pong received from: ");
            DEBUG_OSC_LOGLN(showcontrolUdp.remoteIP());
            
            bool serviceFound = false;
            for (int i = 0; i < ethernet.totalServiceCount; i++) {
                if (ethernet.discoveredServices[i].serviceType == SHOWCONTROLSRV && 
                    ethernet.discoveredServices[i].ip == showcontrolUdp.remoteIP()) {
                    ethernet.discoveredServices[i].lastSeen = millis();
                    DEBUG_OSC_LOGLN("ShowControl service pong updated");
                    serviceFound = true;
                    break;
                }
            }
            
            if (!serviceFound) {
                DEBUG_OSC_LOG("ShowControl service not found, adding: ");
                DEBUG_OSC_LOGLN(showcontrolUdp.remoteIP());
                ethernet.addService(SHOWCONTROLSRV, showcontrolUdp.remoteIP(), showcontrolUdp.remotePort());
                // Mise à jour immédiate de l'affichage
                globalPage.showEthSprite(settings.MIDIConnected, ethernet.serviceCounts[0]);
            }
        });

    } else {
        DEBUG_OSC_LOG("Error OSC message - UDP size: ");
        DEBUG_OSC_LOG(packetSize);
        DEBUG_OSC_LOG(" | Error code: ");
        DEBUG_OSC_LOGLN(msg.getError());
    }
  lastOscTime = millis();

}