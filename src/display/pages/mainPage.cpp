#include <Arduino.h>
#include "../../config/consts.h"
#include "../../core/utils.h"
#include "../../core/mainPageParser.h"
#include "../../core/globalParser.h"
#include "../../core/pages.h"

#include "mainPage.h"
#include "../fonts/fonts.h"
#include "../sprites.h"
#include "../colors.h"

float lastSongPercentage = 0.0f;
float lastRemainingTimeInSong = 0.0f;
unsigned long lastUpdateMillis = 0;
float lastSongDuration = 1.0f; // Pour éviter division par zéro

MainPage mainPage;

void MainPage::showMainSprite(const char* text, uint16_t txtColor){
    if (!text) {
        DEBUG_LOGLN("showMainSprite: null text pointer");
        return;
    }
    showSprite(text, txtColor, mainSprite);
};

void MainPage::showSub1Sprite(const char* text, uint16_t txtColor){
    if (!text) {
        DEBUG_LOGLN("showSub1Sprite: null text pointer");
        return;
    }
    showSprite(text, txtColor, sub1Sprite);
};

void MainPage::showSub2Sprite(const char* text, uint16_t txtColor){
    if (!text) {
        DEBUG_LOGLN("showSub2Sprite: null text pointer");
        return;
    }
    showSprite(text, txtColor, sub2Sprite);
};

void MainPage::showInfoSprite(const char* text, uint16_t txtColor, bool show) {
    if (!text) {
        DEBUG_LOGLN("showInfoSprite: null text pointer");
        return;
    }
    if(show) showSpriteColor(text, txtColor, _Orange, infoSprite, true);
    else showSpriteColor(" ", defaultBgColor, defaultBgColor, infoSprite, true);
};

void MainPage::showLockSprite(bool show) {
    if (!lockSprite) {
        DEBUG_LOGLN("showLockSprite: lockSprite is null");
        return;
    }
    
    if (show) {
        lockSprite->sprite.createSprite(lockSprite->width, lockSprite->height);
        lockSprite->sprite.fillSmoothRoundRect(0, 0, lockSprite->width, lockSprite->height, lockSprite->padding, _Gray, TFT_BLACK);

        // Dessin du cadenas rouge
        int thickness = 2;
        int w = lockSprite->width;
        int h = lockSprite->height;
        int centerX = w / 2;
        int centerY = h / 2;
        int bodyW = w / 2;
        int bodyH = h / 3;
        int bodyX = centerX - bodyW / 2;
        int bodyY = centerY;
        int arcR = bodyW / 2;
        int arcY = bodyY - thickness ; //- arcR + 2;
        // Anse du cadenas (demi-cercle)
        lockSprite->sprite.drawSmoothArc(centerX, arcY, arcR, arcR-thickness, 90, 270, _Red, _Gray);

        lockSprite->sprite.fillRect(bodyX, arcY- thickness, thickness, thickness, _Red);
        lockSprite->sprite.fillRect(bodyX + bodyW -thickness, arcY - thickness, thickness, thickness, _Red);
        // Base du cadenas (rectangle)
        lockSprite->sprite.fillRect(bodyX, bodyY, bodyW + 1, bodyH, _Red);


        lockSprite->sprite.pushSprite(lockSprite->positionX, lockSprite->positionY);
        lockSprite->sprite.deleteSprite();
    } else {
        showSprite("", defaultBgColor, lockSprite);
    }
};

void MainPage::showMainUserSprite(const char* text, uint16_t txtColor){
    if (!text) {
        DEBUG_LOGLN("showMainUserSprite: null text pointer");
        return;
    }
    showSprite(text, txtColor, mainUserSprite);
};

void MainPage::showSub1UserSprite(const char* text, uint16_t txtColor){
    if (!text) {
        DEBUG_LOGLN("showSub1UserSprite: null text pointer");
        return;
    }
    showSprite(text, txtColor, sub1UserSprite);
};

void MainPage::showNextSprite(bool show){
    if(!show)  showSprite("", _SetlistBlue, nextSprite);
    else showSprite("Next:", _SetlistBlue, nextSprite);
}

void MainPage::showButton(bool , int , const char* , uint16_t , bool ){

};

const char* formatTime(float seconds) {
    static char buffer[16]; // "mm:ss" format plus null terminator
    
    // Protection contre les valeurs invalides
    if (isnan(seconds) || isinf(seconds) || seconds < 0) {
        DEBUG_LOG_VALUE("formatTime: invalid seconds value: ", seconds);
        strcpy(buffer, "0:00");
        return buffer;
    }
    
    // Limitation à une valeur raisonnable (24h max)
    if (seconds > 86400.0f) {
        DEBUG_LOGLN("formatTime: seconds value too large, capping to 24h");
        seconds = 86400.0f;
    }
    
    int totalSeconds = static_cast<int>(seconds); 
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;
    
    // Protection contre overflow du buffer
    int result = snprintf(buffer, sizeof(buffer), "%d:%02d", minutes, secs);
    if (result < 0 || result >= sizeof(buffer)) {
        DEBUG_LOGLN("formatTime: buffer overflow, using default");
        strcpy(buffer, "0:00");
    }
    
    return buffer;
}

void MainPage::showRemainingTimeInSet(bool show) {
    if(!show) {
        showSpriteColor("", defaultBgColor, defaultBgColor, totalTimeSprite, false);
        return;
    }
    // if (mainParser.setlistTotalTime <= 0) {
    //     Serial.println("Warning: setlistTotalTime is 0 or negative, cannot format time.");
    //     return;
    // }
    // const char* setlistTotalTime = formatTime(mainParser.setlistTotalTime);
    // if (mainParser.setlistTotalTime != mainParser.previousSetlistTotalTime) {
    //     showSpriteColor(setlistTotalTime, defaultTxtColor, _SetlistBlue, totalTimeSprite, true);
    //     mainParser.previousSetlistTotalTime = mainParser.setlistTotalTime;
    // }
  // showSpriteColor(formatTime(setlistTotalTime), defaultTxtColor, _SetlistBlue, totalTimeSprite, true);
}



void MainPage::updateProgressBar() {
    // Calcul de la progression en fonction de la position courante
    float songStart = mainParser.activeSongStart; // début en ticks ou secondes
    float songEnd = mainParser.activeSongEnd;     // fin en ticks ou secondes
    float position = global.beatsPosition;        // position courante (même unité)

    float duration = songEnd - songStart;
    float progress = position - songStart;

    float percentage = 0.0f;
    if (duration > 0.0f && progress >= 0.0f) {
        percentage = progress / duration;
        if (percentage < 0.0f) percentage = 0.0f;
        if (percentage > 1.0f) percentage = 1.0f;
    }

    mainParser.songPercentage = percentage;
    if(activePage->pageType == SETLIST) updateProgressBarFine(true);
}

void MainPage::updateProgressBarFine(bool show) {
    if(!show) {
        showSpriteColor("", defaultBgColor, defaultBgColor, progressBarSprite, false);
        return;
    }
    if (!progressBarSprite) {
        DEBUG_LOGLN("updateProgressBarFine: progressBarSprite is null");
        return;
    }
    if (!(mainParser.songPercentage >= 0.0f && mainParser.songPercentage <= 1.0f)) {
        DEBUG_LOG_VALUE("updateProgressBarFine: invalid songPercentage: ", mainParser.songPercentage);
        mainParser.songPercentage = 0.0f;
    }
    if (progressBarSprite->width <= 0 || progressBarSprite->height <= 0) {
        DEBUG_LOGLN("updateProgressBarFine: invalid sprite dimensions");
        return;
    }

    progressBarSprite->sprite.createSprite(progressBarSprite->width, progressBarSprite->height);
    const int16_t width = WIDTH * 0.8;
    const int16_t barX = (WIDTH - width) / 2;
    int bar_width = mainParser.songPercentage * width;

    // Fond arrondi
    int arcCenterX = barX + progressBarRadius-2;
    int arcCenterY = progressBarSprite->height / 2;        
    int arcRadius = progressBarRadius - 2;

    progressBarSprite->sprite.fillSmoothRoundRect(barX, 0, width, progressBarSprite->height, progressBarRadius, _Gray, defaultBgColor);

    // Barre rectangulaire
    if (bar_width > 0) {
        progressBarSprite->sprite.fillRect(barX, 0, bar_width, progressBarSprite->height, mainParser.activeSongColor);

        // Arc gauche (bord arrondi, 180°)
        progressBarSprite->sprite.drawSmoothArc(
            arcCenterX, arcCenterY,
            arcRadius + 8, arcRadius,
            360, 180, // arc de gauche (bas à haut)
            defaultBgColor, defaultBgColor
        );
    }

    // Arc droit (bord arrondi, position fixe à la fin du fond)
    int arcRightX = barX + width - progressBarRadius;
    progressBarSprite->sprite.drawSmoothArc(
        arcRightX, arcCenterY,
        arcRadius + 8, arcRadius,
        180, 0, // arc de droite (haut à bas)
       defaultBgColor, defaultBgColor
    );

    progressBarSprite->sprite.pushSprite(progressBarSprite->positionX, progressBarSprite->positionY);
    progressBarSprite->sprite.deleteSprite();
}

void MainPage::showRemainingTimeInSong(bool show) {
  if(!show) {
    showSpriteColor("",  defaultBgColor, defaultBgColor, timeSprite, false);
    return;
  }
  
  // Protection contre les valeurs invalides
  if (isnan(mainParser.currentSeconds) || isinf(mainParser.currentSeconds) || mainParser.currentSeconds < 0) {
    DEBUG_LOG_VALUE("showRemainingTimeInSong: invalid currentSeconds: ", mainParser.currentSeconds);
    mainParser.currentSeconds = 0.0f;
  }
  
  const char* currentTime = formatTime(mainParser.currentSeconds);
  if (!currentTime) {
    DEBUG_LOGLN("showRemainingTimeInSong: formatTime returned null");
    return;
  }
  
  showSpriteColor(currentTime,  defaultTxtColor, mainParser.activeSongColor, timeSprite, false);
}

void MainPage::showSongsCounter(bool show) {
  if(!show) {
    showSpriteColor("", defaultBgColor, defaultBgColor, songsCountSprite, false);
    return;
  }
  
  if (mainParser.activeSongIndex < 0) mainParser.activeSongIndex = 0;
  if (mainParser.songsListSize < 0) mainParser.songsListSize = 0;

  char buffer[20];
  int result = snprintf(buffer, sizeof(buffer), "%d/%d", mainParser.activeSongIndex+1, mainParser.songsListSize);
  
  // Protection contre overflow du buffer
  if (result < 0 || result >= sizeof(buffer)) {
    DEBUG_LOGLN("showSongsCounter: buffer overflow");
    strcpy(buffer, "?/?");
  }
  if (mainParser.songsListSize == 0) {
    strcpy(buffer, "- / -");
  }
  
  DEBUG_LOG_VALUE("mainPage.showSongsCounter ", buffer);
  uint16_t color = (mainParser.songsListSize == 0) ? defaultBgColor : _SetlistBlue;
  showSpriteColor(buffer,  defaultTxtColor, color, songsCountSprite, true);
}


void MainPage::showButtonSprite(bool buttonState, int num, const char* text, uint16_t bgColor, bool luminance) {

    if (!text) return;
    if (!userButtonSprite) return;
    if (num < 0 || num >= 5) return;
    if (WIDTH <= 0 || HEIGHT <= 0) return;

    uint16_t txtColor = luminance ? defaultBgColor : defaultTxtColor;
    int width = WIDTH/6 + 10;
    if (width <= 0 || HEIGHT/5 <= 0) return;

    userButtonSprite->sprite.createSprite(width, HEIGHT/5);
    userButtonSprite->sprite.loadFont(FONT10); 

    const int rectWidth = width > 4 ? width - 4 : 1;
    const int rectHeight = 56 > 4 ? 56 - 4 : 1;

    userButtonSprite->sprite.fillSmoothRoundRect(0, 0, width, 56, RADIUS, bgColor, _Black);

    if (buttonState) {
        userButtonSprite->sprite.drawSmoothRoundRect(2, 2, RADIUS, RADIUS-2, rectWidth, rectHeight, txtColor, _Black);
    } else {
        userButtonSprite->sprite.drawSmoothRoundRect(1, 1, RADIUS, RADIUS-2, width > 1 ? width-1 : 1, 56 > 1 ? 56-1 : 1, bgColor, _Black);
    }

    userButtonSprite->sprite.setTextDatum(4); 
    userButtonSprite->sprite.setTextColor(txtColor, defaultBgColor);
    userButtonSprite->sprite.setTextSize(2);

    char line1[20] = {0};
    char line2[20] = {0};

    size_t textLen = strlen(text);

    if (textLen >= 200) {
        strncpy(line1, text, 19);
        line1[19] = '\0';
    } else {
        int wordsRead = sscanf(text, "%19s %19s", line1, line2);
        if (wordsRead < 1) {
            strcpy(line1, "");
        }
    }

    int x = width / 2;
    int y1 = (HEIGHT/5) / 3; 
    int y2 = (HEIGHT/5) * 2 / 3;
    if (x < 0 || y1 < 0 || y2 < 0) {
        userButtonSprite->sprite.deleteSprite();
        return;
    }

    userButtonSprite->sprite.drawString(line1, x, y1, GFXFF);
    if (strlen(line2) > 0) {
        userButtonSprite->sprite.drawString(line2, x, y2, GFXFF);
    }

    userButtonSprite->sprite.unloadFont();

    int pushX = (WIDTH/5 * num) + 5;
    int pushY = HEIGHT - HEIGHT/5;
    if (pushX < 0 || pushY < 0) {
        userButtonSprite->sprite.deleteSprite();
        return;
    }

    userButtonSprite->sprite.pushSprite(pushX, pushY);
    userButtonSprite->sprite.deleteSprite();
}

void MainPage::showPlaySprite(bool _isPlaying,bool show) {
    if(!show) {
        showSpriteColor("", defaultBgColor, defaultBgColor, playSprite, false);
        return;
    }
    
    // Vérification du pointeur sprite
    if (!playSprite) {
        DEBUG_LOGLN("showPlaySprite: playSprite is null");
        return;
    }
    
    playSprite->sprite.createSprite(playSprite->width, playSprite->height);
    if(_isPlaying) {
        playSprite->sprite.fillRoundRect(0, 0, playSprite->width, playSprite->height, 4, _Black);
        playSprite->sprite.fillTriangle(playSprite->width/2 - 10, playSprite->height/2 - 10,
                                        playSprite->width/2 - 10, playSprite->height/2 + 10,
                                        playSprite->width/2 + 10, playSprite->height/2, _Green);
    } else {
        // Affichage de l'icône de pause
        playSprite->sprite.fillRoundRect(0, 0, playSprite->width, playSprite->height, 4, _Black);
        playSprite->sprite.fillRect(playSprite->width/2 - 8, playSprite->height/2 - 12, 24, 24, _PurpleStop);
    }   
    // playSprite->sprite.fillRoundRect(0, 0, playSprite->width, playSprite->height, 4, _Black);
    
    // Protection contre les coordonnées négatives
    if (playSprite->positionX < 0 || playSprite->positionY < 0) {
        DEBUG_LOGLN("showPlaySprite: invalid sprite position");
        playSprite->sprite.deleteSprite();
        return;
    }
    
    playSprite->sprite.pushSprite(playSprite->positionX, playSprite->positionY);
    playSprite->sprite.deleteSprite();
}   

void MainPage::showLoopSprite(bool _isLooping, bool show) {
    if(!show) {
        showSpriteColor("", defaultBgColor, defaultBgColor, loopSprite, false);
        return;
    }
    if (!loopSprite) {
        DEBUG_LOGLN("showLoopSprite: loopSprite is null");
        return;
    }
    loopSprite->sprite.createSprite(loopSprite->width, loopSprite->height);
    loopSprite->sprite.fillRoundRect(0, 0, loopSprite->width, loopSprite->height, 4, _Black);

    int color = _isLooping ? _Yellow : _DarkGray ;
    int radius = 3;
    int padding = 4;
    int thickness = 4;
    int margin = 8;
    int margin_v = 12;
    int w = loopSprite->width - 2*padding;
    int h = loopSprite->height - 2*padding;
    //  Grand rectangle arrondi (contour de la boucle)
    loopSprite->sprite.fillRoundRect(margin, margin + padding, w - margin, h - 2*margin, radius, color);

    // Petit rectangle arrondi (intérieur, couleur de fond)
    loopSprite->sprite.fillRoundRect(margin + thickness, margin + padding + thickness, w - 2*(padding + thickness), h - 2*(margin + thickness), radius, defaultBgColor);

    int bridgeW = w/4;
    int bridgeH = thickness;
    loopSprite->sprite.fillRect(w/2 - bridgeW/2, h - margin, bridgeW, thickness, defaultBgColor);

    // Triangle (pointe de flèche) en bas au centre
    int triBase = thickness * 2;
    int triHeight = thickness * 2;
    int triX = w/2 + bridgeW/2;
    int triY = h - margin_v - thickness/2 + margin - 1;
    loopSprite->sprite.fillTriangle(triX, triY + 2*thickness, triX , triY - 2*thickness, triX - 2*thickness, triY, color);

    if (loopSprite->positionX < 0 || loopSprite->positionY < 0) {
        DEBUG_LOGLN("showLoopSprite: invalid sprite position");
        loopSprite->sprite.deleteSprite();
        return;
    }
    loopSprite->sprite.pushSprite(loopSprite->positionX, loopSprite->positionY);
    loopSprite->sprite.deleteSprite();
}