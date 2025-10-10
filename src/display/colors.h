#pragma once

#include <stdint.h>


extern uint16_t topBarBgColor;
extern uint16_t defaultTxtColor;
extern uint16_t topBarBgColor;
extern uint16_t defaultBgColor;

// Fonctions de conversion couleur
uint16_t RRRGB888toRGB565(uint8_t r, uint8_t g, uint8_t b);
uint16_t hexStringtoRGB565(char* hex);
uint16_t hexStringtoRGB565Shade(char* hex);

// Définitions de couleurs
#define _SetlistBlue RRRGB888toRGB565(25, 104, 200)
#define _DarkGray RRRGB888toRGB565(140, 140, 140)
#define _Gray RRRGB888toRGB565(69, 69, 69)
#define _LightGray RRRGB888toRGB565(200, 200, 200)
#define _Red RRRGB888toRGB565(180, 0, 0)
#define _Orange RRRGB888toRGB565(210, 120, 0)
#define _Amber RRRGB888toRGB565(200, 140, 0)
#define _Yellow RRRGB888toRGB565(200, 200, 0)
#define _Lime RRRGB888toRGB565(160, 200, 50)
#define _Green RRRGB888toRGB565(0, 180, 0)
#define _Emerald RRRGB888toRGB565(0, 200, 180)
#define _Teal RRRGB888toRGB565(10, 140, 90)
#define _Cyan RRRGB888toRGB565(10, 140, 70)
#define _Sky RRRGB888toRGB565(5, 84, 160)
#define _Blue RRRGB888toRGB565(0, 0, 255)
#define _Indigo RRRGB888toRGB565(160, 80, 200)
#define _Violet RRRGB888toRGB565(200, 5, 180)
#define _Purple RRRGB888toRGB565(200, 20, 170)
#define _PurpleStop RRRGB888toRGB565(120, 20, 180)
#define _Fuchsia RRRGB888toRGB565(200, 20, 120)
#define _Pink RRRGB888toRGB565(200, 5, 80)
#define _Rose RRRGB888toRGB565(108, 5, 5)
#define _Black RRRGB888toRGB565(0, 0, 0)
#define _White RRRGB888toRGB565(255, 255, 255)
#define _YellowLoop RRRGB888toRGB565(103, 75, 0)

