/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2019 Ha Thach for Adafruit Industries
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#ifndef USBH_HELPER_H
#define USBH_HELPER_H
#include "pio_usb.h"
#define PIN_USB_HOST_DP  3
  
#include "Adafruit_TinyUSB.h"

extern Adafruit_USBH_Host USBHost;

  // Native USB Host such as rp2040
  // (défini dans midi_host.cpp)
  

//--------------------------------------------------------------------+
// Helper Functions
//--------------------------------------------------------------------+

static void rp2040_configure_pio_usb(void) {
  //while ( !Serial ) delay(10);   // wait for native usb
  Serial.println("Core1 setup to run TinyUSB host with pio-usb");

  // Check for CPU frequency, must be multiple of 12 Mhz for bit-banging USB
  uint32_t cpu_hz = clock_get_hz(clk_sys);
  if (cpu_hz % 12000000UL) {
    while (!Serial) {
      delay(10);   // wait for native usb
    }
    Serial.printf("Error: CPU Clock = %lu, PIO USB require CPU clock must be multiple of 12 Mhz\r\n", cpu_hz);
    Serial.printf("Change your CPU Clock to 12*n Mhz in Menu->CPU Speed \r\n");
    while (1) {
      delay(1);
    }
  }

  pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
  pio_cfg.pin_dp = PIN_USB_HOST_DP;

  USBHost.configure_pio_usb(1, &pio_cfg);
}

#endif
