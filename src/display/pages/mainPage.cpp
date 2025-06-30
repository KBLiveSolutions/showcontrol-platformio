
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
    showSprite(text, txtColor, mainSprite);
};

void MainPage::showSub1Sprite(const char* text, uint16_t txtColor){
    showSprite(text, txtColor, sub1Sprite);
};

void MainPage::showSub2Sprite(const char* text, uint16_t txtColor){
    showSprite(text, txtColor, sub2Sprite);
};

void MainPage::showMainUserSprite(const char* text, uint16_t txtColor){
    showSprite(text, txtColor, mainUserSprite);
};

void MainPage::showSub1UserSprite(const char* text, uint16_t txtColor){
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
    int totalSeconds = static_cast<int>(seconds); 
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;
    snprintf(buffer, sizeof(buffer), "%d:%02d", minutes, secs); 
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
    if (!(_main.songPercentage >= 0.0f && _main.songPercentage <= 1.0f)) {
        DEBUG_LOGLN("Warning: songPercentage is NaN or out of bounds, resetting to 0.");
        _main.songPercentage = 0.0f;
    }
    progressBarSprite->sprite.createSprite(progressBarSprite->width, progressBarSprite->height);
    const int16_t width = WIDTH * 0.8;
    const int16_t barX = (WIDTH - width) / 2;
    int bar_width = _main.songPercentage * width;
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
  const char* currentTime = formatTime(_main.currentSeconds);
  showSpriteColor(currentTime,  defaultTxtColor, _main.activeSongColor, timeSprite, false);
}

void MainPage::showSongsCounter(bool show) {
  if(!show) {
    showSpriteColor("", defaultBgColor, defaultBgColor, songsCountSprite, false);
    return;
  }
  char buffer[20];
  sprintf(buffer, "%d/%d", _main.activeSongIndex+1, _main.songsListSize);
  DEBUG_LOG_VALUE("mainPage.showSongsCounter ", buffer);
  showSpriteColor(buffer,  defaultTxtColor, _SetlistBlue, songsCountSprite, true);
}


void MainPage::showButtonSprite(bool buttonState, int num, const char* text, uint16_t bgColor, bool luminance) {
    // uint16_t txtColor = buttonState ? bgColor : defaultBgColor;
    // int luminance = (l[num+1].r + l[num+1].g + l[num+1].b) > 100;
    uint16_t txtColor = luminance ? defaultBgColor : defaultTxtColor;
    int width = WIDTH/6 + 10;
    userButtonSprite->sprite.createSprite(width, HEIGHT/5);
    
    // if (buttonState) spriteName->sprite.drawSmoothRoundRect(0, 0, RADIUS, RADIUS-2, width, 56, bgColor, _Black);
    // else 
    userButtonSprite->sprite.fillSmoothRoundRect(0, 0, width, 56, RADIUS, bgColor, _Black);
    if (buttonState) userButtonSprite->sprite.drawSmoothRoundRect(2, 2, RADIUS, RADIUS-2, width-4, 56-4, txtColor, _Black);
    else userButtonSprite->sprite.drawSmoothRoundRect(1, 1, RADIUS, RADIUS-2, width-1, 56-1, bgColor, _Black);


    userButtonSprite->sprite.setTextDatum(4); 
    userButtonSprite->sprite.setTextColor(txtColor, defaultBgColor);
    userButtonSprite->sprite.setTextSize(2);

    char line1[20] = {0};  // Initialise à zéro
    char line2[20] = {0};  // Initialise à zéro
    int wordsRead = sscanf(text, "%19s %19s", line1, line2);  // Limite la lecture à éviter les dépassements

    int x = (width) / 2;
    int y1 = (HEIGHT/5) / 3; 
    int y2 = (HEIGHT/5) * 2 / 3;

    userButtonSprite->sprite.drawString(line1, x, y1, GFXFF);
    if (wordsRead == 2) {  // N'affiche line2 que s'il y a un deuxième mot
        userButtonSprite->sprite.drawString(line2, x, y2, GFXFF);
    }

    userButtonSprite->sprite.unloadFont();
    userButtonSprite->sprite.pushSprite((WIDTH/5 * num) + 5, HEIGHT-HEIGHT/5);
    userButtonSprite->sprite.deleteSprite();
}



