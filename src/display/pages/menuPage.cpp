#include "menuPage.h"
#include "../../display/sprites.h"
#include "../../config/jsonManager.h"
#include "../colors.h"
#include "../../config/consts.h"
#include "../../core/utils.h"
#include "../fonts/fonts.h"

MenuPage menuPage;

void MenuPage::showMenuEntry(const char* optionText, uint8_t i, uint8_t menuItemIndex){
    // Validation des paramètres d'entrée
    if (!optionText) {
        DEBUG_LOGLN("showMenuEntry: optionText is null");
        return;
    }
    if (i >= MAX_MENU_ITEMS) {
        DEBUG_LOG_VALUE("showMenuEntry: invalid sprite index: ", i);
        return;
    }
    if (spriteW <= 0 || spriteH <= 0) {
        DEBUG_LOGLN("showMenuEntry: invalid sprite dimensions");
        return;
    }
    int16_t padding = 5;
    uint8_t border = 0;
    uint16_t activeBGColor = _Green;
    uint16_t inactiveBGColor = _White;
    uint16_t color = inactiveBGColor;
    uint8_t spriteY = spriteH;

    mainmenuSprites[i].setColorDepth(16);
    mainmenuSprites[i].createSprite(spriteW, spriteH);
    mainmenuSprites[i].setTextDatum(1);
    mainmenuSprites[i].setTextColor(_White, defaultBgColor);
    mainmenuSprites[i].loadFont(FONT20);

    if (menuItemIndex == activeMenuItem) border = 3;
    int rectX = max(padding, 0);
    int rectY = max(padding, 0);
    int rectW = max(spriteW - padding * 2 - 2, 1);
    int rectH = max(spriteH - padding * 2 - 2, 1);
    mainmenuSprites[i].drawSmoothRoundRect(rectX, rectY, RADIUS, RADIUS-border, rectW, rectH, _White);

    // Coupe le texte si trop long pour le sprite et ajoute "..." si tronqué
    char truncatedText[MAX_SONG_NAME+4];
    strncpy(truncatedText, optionText, MAX_SONG_NAME+3);
    truncatedText[MAX_SONG_NAME+3] = '\0';
    int textWidth = mainmenuSprites[i].textWidth(truncatedText, GFXFF);
    if (textWidth > spriteW - 2*padding) {
        int len = strlen(truncatedText);
        while (len > 0 && mainmenuSprites[i].textWidth(truncatedText, GFXFF) > spriteW - 2*padding - 20) {
            len--;
            truncatedText[len] = '\0';
        }
        if (len > 3) {
            strncpy(&truncatedText[len-3], "...", 3);
            truncatedText[len] = '\0';
        }
    }
    mainmenuSprites[i].drawString(truncatedText, spriteW / 2, 3 * padding, GFXFF);
    mainmenuSprites[i].unloadFont();
    mainmenuSprites[i].pushSprite(WIDTH / 2 - spriteW / 2, spriteH * (i+1));
    mainmenuSprites[i].deleteSprite();
}

void MenuPage::updateMainMenu() {
    // Validation de activeMenuSize
    if (activeMenuSize <= 0) {
        DEBUG_LOGLN("updateMainMenu: invalid activeMenuSize");
        return;
    }

    // Calcul du startIndex pour le scroll
    if (activeMenuSize > MAX_MENU_ITEMS) {
        if (activeMenuItem >= startIndex + MAX_MENU_ITEMS) {
            startIndex = activeMenuItem - (MAX_MENU_ITEMS - 1);
        } else if (activeMenuItem < startIndex) {
            startIndex = activeMenuItem;
        }
    } else {
        startIndex = 0;
    }
    if (startIndex < 0) startIndex = 0;
    if (startIndex > activeMenuSize - MAX_MENU_ITEMS) startIndex = activeMenuSize - MAX_MENU_ITEMS;
    if (startIndex < 0) startIndex = 0;

    // Recopie les bons noms dans les buffers
    for (uint8_t i = 0; i < MAX_MENU_ITEMS; i++) {
        uint8_t menuItemIndex = startIndex + i;
        if (menuItemIndex >= activeMenuSize) {
            strncpy(menuItems[i], "", MAX_SONG_NAME-1);
            menuItems[i][MAX_SONG_NAME-1] = '\0';
            continue;
        }
        // Source dynamique selon le menu
        const char* optionText = nullptr;
        if (activeMenu == SONG_MENU) {
            if (menuItemIndex < sizeof(_main.songsList)/sizeof(_main.songsList[0]) && _main.songsList[menuItemIndex]) {
                optionText = _main.songsList[menuItemIndex];
            } else {
                snprintf(menuItems[i], MAX_SONG_NAME, "Song %d", menuItemIndex+1);
                continue;
            }
        } else if (activeMenu == SETLIST_MENU) {
            if (menuItemIndex < sizeof(_main.setlistsList)/sizeof(_main.setlistsList[0]) && _main.setlistsList[menuItemIndex]) {
                optionText = _main.setlistsList[menuItemIndex];
            } else {
                snprintf(menuItems[i], MAX_SONG_NAME, "Setlist %d", menuItemIndex+1);
                continue;
            }
        } else if (activeMenu == MAIN_MENU) {
            if (menuItemIndex < sizeof(modesList)/sizeof(modesList[0]) && modesList[menuItemIndex]) {
                optionText = modesList[menuItemIndex];
            } else {
                snprintf(menuItems[i], MAX_SONG_NAME, "Mode %d", menuItemIndex);
                continue;
            }
        }
        if (optionText) {
            strncpy(menuItems[i], optionText, MAX_SONG_NAME-1);
            menuItems[i][MAX_SONG_NAME-1] = '\0';
        }
    }

    // Affichage des sprites
    for (uint8_t i = 0; i < MAX_MENU_ITEMS; i++) {
        showMenuEntry(menuItems[i], i, startIndex + i);
    }
}

void MenuPage::changeSelectedItem(int value){
    // Validation de la valeur d'entrée
    if (value < -100 || value > 100) {
        DEBUG_LOG_VALUE("changeSelectedItem: extreme value capped: ", value);
        value = (value < 0) ? -1 : 1;
    }
    
    int prevActiveMenuItem = activeMenuItem;
    activeMenuItem = activeMenuItem + value;
    activeMenuItem = max(activeMenuItem, 0);
    
    // Protection contre activeMenuSize invalide
    if (activeMenuSize <= 0) {
        DEBUG_LOGLN("changeSelectedItem: invalid activeMenuSize");
        activeMenuItem = 0;
    } else {
        activeMenuItem = min(activeMenuItem, activeMenuSize - 1);  // Correction: -1 pour index valide
    }
    
    DEBUG_LOG_VALUE("Active menu item: ", activeMenuItem);
    if (prevActiveMenuItem != activeMenuItem) updateMainMenu();
}

void MenuPage::showPage() {
    DEBUG_LOGLN("MenuPage::showPage() starting");
    
    int maxItems;  // Déclaration au début pour éviter les erreurs de scope
    switch (activeMenu) {
        case MAIN_MENU:
        {
            // Validation de settings.userPagesAmount
            if (settings.userPagesAmount < 0 || settings.userPagesAmount > 8) {
                DEBUG_LOG_VALUE("showPage: invalid userPagesAmount, capping: ", settings.userPagesAmount);
                settings.userPagesAmount = max(0, min(settings.userPagesAmount, 8));
            }
            
            activeMenuSize = settings.userPagesAmount + 2;
            DEBUG_LOG_VALUE("Active menu size: ", activeMenuSize);
            
            // Protection contre débordement du tableau menuItems
            maxItems = min(activeMenuSize, (int)(sizeof(menuItems)/sizeof(menuItems[0])));
            
            for (uint8_t i = 0; i < maxItems - 1; i++) {
                if (i == 0) {
                    strncpy(menuItems[i], "Setlist", sizeof(menuItems[i]) - 1);
                    menuItems[i][sizeof(menuItems[i]) - 1] = '\0';
                    // TO DO: check if AbleSet is available
                    // if (ethernet.serviceCounts[0] > 0)  strcpy(menuItems[i], "AbleSet");
                    // else  strcpy(menuItems[i], "Setlist");
                } 
                else {
                    // Protection contre l'accès hors limites de modesList
                    if (i < sizeof(modesList)/sizeof(modesList[0]) && modesList[i]) {
                        strncpy(menuItems[i], modesList[i], sizeof(menuItems[i]) - 1);
                        menuItems[i][sizeof(menuItems[i]) - 1] = '\0';
                    } else {
                        DEBUG_LOG_VALUE("showPage: modesList index out of bounds: ", i);
                        snprintf(menuItems[i], sizeof(menuItems[i]), "Mode %d", i);
                    }
                }
            }
            
            if (maxItems > 0) {
                strncpy(menuItems[maxItems-1], "Settings", sizeof(menuItems[maxItems-1]) - 1);
                menuItems[maxItems-1][sizeof(menuItems[maxItems-1]) - 1] = '\0';
            }
            break;
        }

        case SONG_MENU:
        {
            // Validation de _main.songsListSize
            if (_main.songsListSize < 0 || _main.songsListSize > 100) {
                DEBUG_LOG_VALUE("showPage: invalid songsListSize: ", _main.songsListSize);
                _main.songsListSize = max(0, min(_main.songsListSize, 100));
            }
            
            activeMenuSize = _main.songsListSize;
            maxItems = min(activeMenuSize, (int)(sizeof(menuItems)/sizeof(menuItems[0])));
            
            for (uint8_t i = 0; i < maxItems; i++) {
                DEBUG_LOG_VALUE("showPage: processing song index: ", i);    
                // Protection contre l'accès hors limites de _main.songsList
                if (i < sizeof(_main.songsList)/sizeof(_main.songsList[0]) && _main.songsList[i]) {
                    DEBUG_LOG_VALUE("showPage: copying song name: ", _main.songsList[i]);
                    strncpy(menuItems[i], _main.songsList[i], sizeof(menuItems[i]) - 1);
                    menuItems[i][sizeof(menuItems[i]) - 1] = '\0';
                } else {
                    DEBUG_LOG_VALUE("showPage: songsList index out of bounds: ", i);
                    snprintf(menuItems[i], sizeof(menuItems[i]), "Song %d", i + 1);
                }
            }
            break;
        }

        case SETLIST_MENU:
        {
            // Validation de _main.setlistsListSize
            if (_main.setlistsListSize < 0 || _main.setlistsListSize > 100) {
                DEBUG_LOG_VALUE("showPage: invalid setlistsListSize: ", _main.setlistsListSize);
                _main.setlistsListSize = max(0, min(_main.setlistsListSize, 100));
            }
            
            activeMenuSize = _main.setlistsListSize;
            maxItems = min(activeMenuSize, (int)(sizeof(menuItems)/sizeof(menuItems[0])));
            
            for (uint8_t i = 0; i < maxItems; i++) {
                // Protection contre l'accès hors limites de _main.setlistsList
                if (i < sizeof(_main.setlistsList)/sizeof(_main.setlistsList[0]) && _main.setlistsList[i]) {
                    strncpy(menuItems[i], _main.setlistsList[i], sizeof(menuItems[i]) - 1);
                    menuItems[i][sizeof(menuItems[i]) - 1] = '\0';
                } else {
                    DEBUG_LOG_VALUE("showPage: setlistsList index out of bounds: ", i);
                    snprintf(menuItems[i], sizeof(menuItems[i]), "Setlist %d", i + 1);
                }
            }
            break;
        }
            
        default:
        {
            DEBUG_LOG_VALUE("showPage: unknown activeMenu: ", activeMenu);
            activeMenuSize = 1;
            strncpy(menuItems[0], "Error", sizeof(menuItems[0]) - 1);
            menuItems[0][sizeof(menuItems[0]) - 1] = '\0';
            break;
        }
    }
    
    showCancelSprite(true);
    updateMainMenu();
    DEBUG_LOGLN("MenuPage::showPage() completed");
}

void MenuPage::clearPage() {
    DEBUG_LOGLN("MenuPage::clearPage() starting");
    
    // Validation des dimensions
    if (spriteW <= 0 || spriteH <= 0) {
        DEBUG_LOGLN("clearPage: invalid sprite dimensions");
        return;
    }
    
    for (int i = 0; i < 6; i++) {
        mainmenuSprites[i].createSprite(spriteW, spriteH);
        mainmenuSprites[i].fillSprite(defaultBgColor);
        
        // Protection contre coordonnées négatives
        int pushX = max(WIDTH / 2 - spriteW / 2, 0);
        int pushY = max(spriteH * (i + 1), 0);
        
        mainmenuSprites[i].pushSprite(pushX, pushY);
        mainmenuSprites[i].deleteSprite();
    }
    showCancelSprite(false);
    DEBUG_LOGLN("MenuPage::clearPage() completed");
}

void MenuPage::showCancelSprite(bool show) {
    DEBUG_LOGLN("showCancelSprite: starting");
    
    
    uint16_t color = show ? _LightGray : defaultBgColor;
    // Nettoyage sécurisé du bouton Cancel
    CancelButtonSprite->sprite.createSprite(CancelButtonSprite->width, CancelButtonSprite->height);
    // CancelButtonSprite->sprite.fillSmoothRoundRect(0, 0, lockSprite->width, lockSprite->height, lockSprite->padding, show ? _Gray : defaultBgColor, TFT_BLACK);

        // CancelButtonSprite->sprite.fillRect(bodyX, arcY- thickness, thickness, thickness, _Red);
        // CancelButtonSprite->sprite.fillRect(bodyX + bodyW -thickness, arcY - thickness, thickness, thickness, _Red);
        // // Base du cadenas (rectangle)
        // CancelButtonSprite->sprite.fillRect(bodyX, bodyY, bodyW + 1, bodyH, _Red);
    CancelButtonSprite->sprite.drawCircle(15, 15, 15, color);
    CancelButtonSprite->sprite.drawCircle(15, 15, 14, color);
    CancelButtonSprite->sprite.setTextColor(color, color);
    CancelButtonSprite->sprite.loadFont(FONT12);
    CancelButtonSprite->sprite.drawString(show ? "X" : "", 8, 6, GFXFF);
    CancelButtonSprite->sprite.unloadFont();

        CancelButtonSprite->sprite.pushSprite(CancelButtonSprite->positionX, CancelButtonSprite->positionY);
        CancelButtonSprite->sprite.deleteSprite();
    
    DEBUG_LOGLN("showCancelSprite: completed");
}