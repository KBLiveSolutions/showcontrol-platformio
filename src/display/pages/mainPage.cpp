#include <Arduino.h>
#include "../../config/consts.h"
#include "../../core/utils.h"
#include "../../core/mainPageParser.h"

#include "mainPage.h"
#include "../fonts/fonts.h"
#include "../sprites.h"
#include "../colors.h"


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
    // if (_main.setlistTotalTime <= 0) {
    //     Serial.println("Warning: setlistTotalTime is 0 or negative, cannot format time.");
    //     return;
    // }
    // const char* setlistTotalTime = formatTime(_main.setlistTotalTime);
    // if (_main.setlistTotalTime != _main.previousSetlistTotalTime) {
    //     showSpriteColor(setlistTotalTime, defaultTxtColor, _SetlistBlue, totalTimeSprite, true);
    //     _main.previousSetlistTotalTime = _main.setlistTotalTime;
    // }
  // showSpriteColor(formatTime(setlistTotalTime), defaultTxtColor, _SetlistBlue, totalTimeSprite, true);
}

void MainPage::updateProgressBarFine(bool show) {
    if(!show) {
        showSpriteColor("", defaultBgColor, defaultBgColor, progressBarSprite, false);
        return;
    }
    
    // Vérification du pointeur sprite
    if (!progressBarSprite) {
        DEBUG_LOGLN("updateProgressBarFine: progressBarSprite is null");
        return;
    }
    
    // Protection contre les valeurs invalides
    if (!(_main.songPercentage >= 0.0f && _main.songPercentage <= 1.0f)) {
        DEBUG_LOG_VALUE("updateProgressBarFine: invalid songPercentage: ", _main.songPercentage);
        _main.songPercentage = 0.0f;
    }
    
    // Vérification des dimensions
    if (progressBarSprite->width <= 0 || progressBarSprite->height <= 0) {
        DEBUG_LOGLN("updateProgressBarFine: invalid sprite dimensions");
        return;
    }
    
    progressBarSprite->sprite.createSprite(progressBarSprite->width, progressBarSprite->height);
    const int16_t width = WIDTH * 0.8;
    const int16_t barX = (WIDTH - width) / 2;
    int bar_width = _main.songPercentage * width;
    
    // Protection contre les dimensions négatives
    if (width <= 0 || bar_width < 0) {
        DEBUG_LOGLN("updateProgressBarFine: invalid calculated dimensions");
        progressBarSprite->sprite.deleteSprite();
        return;
    }
    
    progressBarSprite->sprite.fillSmoothRoundRect(barX, 0, width,  progressBarSprite->height, progressBarRadius, _Gray, TFT_BLACK);
    progressBarSprite->sprite.fillSmoothRoundRect(barX, 0, bar_width,  progressBarSprite->height, progressBarRadius , _main.activeSongColor, TFT_BLACK);
    progressBarSprite->sprite.pushSprite(progressBarSprite->positionX, progressBarSprite->positionY);
    progressBarSprite->sprite.deleteSprite();
}

void MainPage::showRemainingTimeInSong(bool show) {
  if(!show) {
    showSpriteColor("",  defaultBgColor, defaultBgColor, timeSprite, false);
    return;
  }
  
  // Protection contre les valeurs invalides
  if (isnan(_main.currentSeconds) || isinf(_main.currentSeconds) || _main.currentSeconds < 0) {
    DEBUG_LOG_VALUE("showRemainingTimeInSong: invalid currentSeconds: ", _main.currentSeconds);
    _main.currentSeconds = 0.0f;
  }
  
  const char* currentTime = formatTime(_main.currentSeconds);
  if (!currentTime) {
    DEBUG_LOGLN("showRemainingTimeInSong: formatTime returned null");
    return;
  }
  
  showSpriteColor(currentTime,  defaultTxtColor, _main.activeSongColor, timeSprite, false);
}

void MainPage::showSongsCounter(bool show) {
  if(!show) {
    showSpriteColor("", defaultBgColor, defaultBgColor, songsCountSprite, false);
    return;
  }
  
  // Protection contre les valeurs invalides
//   if (_main.activeSongIndex < 0) || _main.songsListSize <= 0) {
//     DEBUG_LOG_VALUE("showSongsCounter: invalid activeSongIndex: ", _main.activeSongIndex);
//     DEBUG_LOG_VALUE("showSongsCounter: invalid songsListSize: ", _main.songsListSize);
//     showSpriteColor("0/0", defaultTxtColor, _SetlistBlue, songsCountSprite, true);
//     return;
//   }
  if (_main.activeSongIndex < 0) _main.activeSongIndex = 0;
  if (_main.songsListSize < 0) _main.songsListSize = 0;

  // Protection contre overflow d'index
//   if (_main.activeSongIndex >= _main.songsListSize) {
//     DEBUG_LOGLN("showSongsCounter: activeSongIndex >= songsListSize, capping");
//     _main.activeSongIndex = _main.songsListSize - 1;
//   }
  
  char buffer[20];
  int result = snprintf(buffer, sizeof(buffer), "%d/%d", _main.activeSongIndex+1, _main.songsListSize);
  
  // Protection contre overflow du buffer
  if (result < 0 || result >= sizeof(buffer)) {
    DEBUG_LOGLN("showSongsCounter: buffer overflow");
    strcpy(buffer, "?/?");
  }
  
  DEBUG_LOG_VALUE("mainPage.showSongsCounter ", buffer);
  showSpriteColor(buffer,  defaultTxtColor, _SetlistBlue, songsCountSprite, true);
}


void MainPage::showButtonSprite(bool buttonState, int num, const char* text, uint16_t bgColor, bool luminance) {
    // Vérifications critiques de sécurité
    if (!text) {
        DEBUG_LOGLN("showButtonSprite: null text pointer");
        return;
    }
    
    if (!userButtonSprite) {
        DEBUG_LOGLN("showButtonSprite: userButtonSprite is null");
        return;
    }
    
    // Protection contre les index invalides
    if (num < 0 || num >= 5) {
        DEBUG_LOG_VALUE("showButtonSprite: invalid button number: ", num);
        return;
    }
    
    // Protection contre les dimensions invalides
    if (WIDTH <= 0 || HEIGHT <= 0) {
        DEBUG_LOGLN("showButtonSprite: invalid screen dimensions");
        return;
    }
    
    uint16_t txtColor = luminance ? defaultBgColor : defaultTxtColor;
    int width = WIDTH/6 + 10;
    
    // Vérification que les dimensions calculées sont valides
    if (width <= 0 || HEIGHT/5 <= 0) {
        DEBUG_LOGLN("showButtonSprite: calculated dimensions are invalid");
        return;
    }
    
    userButtonSprite->sprite.createSprite(width, HEIGHT/5);
    
    // Protection contre les coordonnées négatives
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

    char line1[20] = {0};  // Initialise à zéro
    char line2[20] = {0};  // Initialise à zéro
    
    // Protection contre les chaînes trop longues et sscanf
    size_t textLen = strlen(text);
    if (textLen >= 200) {  // Protection contre les chaînes extrêmement longues
        DEBUG_LOGLN("showButtonSprite: text too long, truncating");
        strncpy(line1, text, 19);
        line1[19] = '\0';
    } else {
        int wordsRead = sscanf(text, "%19s %19s", line1, line2);  // Limite la lecture pour éviter les dépassements
        if (wordsRead < 1) {
            strcpy(line1, "?");  // Fallback si sscanf échoue
        }
    }

    int x = width / 2;
    int y1 = (HEIGHT/5) / 3; 
    int y2 = (HEIGHT/5) * 2 / 3;

    // Vérification que les coordonnées sont valides
    if (x < 0 || y1 < 0 || y2 < 0) {
        DEBUG_LOGLN("showButtonSprite: invalid text coordinates");
        userButtonSprite->sprite.deleteSprite();
        return;
    }

    userButtonSprite->sprite.drawString(line1, x, y1, GFXFF);
    if (strlen(line2) > 0) {  // N'affiche line2 que s'il y a un deuxième mot
        userButtonSprite->sprite.drawString(line2, x, y2, GFXFF);
    }

    userButtonSprite->sprite.unloadFont();
    
    // Protection des coordonnées de push
    int pushX = (WIDTH/5 * num) + 5;
    int pushY = HEIGHT - HEIGHT/5;
    
    if (pushX < 0 || pushY < 0) {
        DEBUG_LOGLN("showButtonSprite: invalid push coordinates");
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
        playSprite->sprite.fillRect(playSprite->width/2 - 8, playSprite->height/2 - 12, 24, 24, _Purple);
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
    int thickness = 4;
    int margin = 8;
    int margin_v = 12;
    int w = loopSprite->width;
    int h = loopSprite->height;

    // Dessin du rectangle "boucle" (forme carrée)
    // 1. Trait haut
    loopSprite->sprite.fillRect(margin, margin_v, w - 2*margin, thickness, color);
    // 3. Trait bas
    loopSprite->sprite.fillRect(margin, h - margin_v - thickness, w - 2*margin, thickness, color);
    // 2. Trait droit
    loopSprite->sprite.fillRect(w - margin - thickness, margin_v, thickness, h - 2*margin_v, color);
    // 4. Trait gauche
    loopSprite->sprite.fillRect(margin, margin_v, thickness, h - 2*margin_v - thickness, color);
    // 5. Petit "pont" pour la flèche
    int bridgeW = w/4;
    int bridgeH = thickness;
    loopSprite->sprite.fillRect(w/2 - bridgeW/2, h - margin_v - thickness, bridgeW, thickness, _Black);

    // Triangle (pointe de flèche) en bas au centre
    int triBase = thickness * 2;
    int triHeight = thickness * 2;
    int triX = w/2 + bridgeW/2;
    int triY = h - margin_v - thickness/2;
    loopSprite->sprite.fillTriangle(triX, triY + 2*thickness, triX , triY - 2*thickness, triX - 2*thickness, triY, color);

    if (loopSprite->positionX < 0 || loopSprite->positionY < 0) {
        DEBUG_LOGLN("showLoopSprite: invalid sprite position");
        loopSprite->sprite.deleteSprite();
        return;
    }
    loopSprite->sprite.pushSprite(loopSprite->positionX, loopSprite->positionY);
    loopSprite->sprite.deleteSprite();
}