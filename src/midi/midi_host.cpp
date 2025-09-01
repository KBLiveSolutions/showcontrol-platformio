#include <Arduino.h>
#include <stdint.h>
#include "midi_host.h"
#include "tusb.h"

Adafruit_USBH_Host USBHost;
dev_info_t dev_info[CFG_TUH_DEVICE_MAX] = { 0 };
MidiHost midiHost;


MidiHost::MidiHost() {
    
}


void MidiHost::begin() {
  //while ( !Serial ) delay(10);   // wait for native usb
  // configure pio-usb: defined in usbh_helper.h
  rp2040_configure_pio_usb();

  // run host stack on controller (rhport) 1
  // Note: For rp2040 pico-pio-usb, calling USBHost.begin() on core1 will have most of the
  // host bit-banging processing works done in core1 to free up core0 for other works
  USBHost.begin(1);
  // Initialisation éventuelle
}

void MidiHost::task() {
    
  // static uint32_t last_debug = 0;
  // static uint32_t loop_count = 0;
  USBHost.task();
  // loop_count++;
  // La lecture MIDI se fait désormais dans le callback tuh_midi_rx_cb
  // delay(1); // Limite la charge CPU et évite de saturer la stack USB
  // Affiche un message toutes les 2 secondes pour vérifier que la boucle tourne et compter les passages
  // if (millis() - last_debug > 2000) {
  //   Serial.printf("[DEBUG] loop1 tourne, host actif, count = %lu\r\n", loop_count);
  //   Serial.flush();
  //   last_debug = millis();
  // }
}

void MidiHost::handleDeviceConnected(uint8_t daddr) {
    // Gestion de la connexion d'un périphérique MIDI
    Serial.printf("[MIDI] Device connected: %d\r\n", daddr);
}

void MidiHost::handleDeviceDisconnected(uint8_t daddr) {
    // Gestion de la déconnexion d'un périphérique MIDI
    Serial.printf("[MIDI] Device disconnected: %d\r\n", daddr);
}


//--------------------------------------------------------------------+
// TinyUSB Host callbacks
//--------------------------------------------------------------------+

// Callback TinyUSB Host MIDI : appelé lors du montage d'un device MIDI
// Signature attendue par Adafruit TinyUSB

extern "C" void tuh_midi_mount_cb(uint8_t idx, const tuh_midi_mount_cb_t* mount_cb_data) {
  Serial.print("[MIDI HOST] Device MIDI monté, idx = ");
  Serial.println(idx);
}

// Callback TinyUSB Host MIDI : appelé lors du démontage d'un device MIDI
extern "C" void tuh_midi_umount_cb(uint8_t idx) {
  Serial.print("[MIDI HOST] Device MIDI démonté, idx = ");
  Serial.println(idx);
}

// Callback TinyUSB Host MIDI : appelé quand des données MIDI sont reçues
extern "C" void tuh_midi_rx_cb(uint8_t dev_addr, uint32_t xferred_bytes) {
  uint8_t packet[4];
  // Lecture des paquets MIDI reçus (signature standard TinyUSB)
  while (tuh_midi_packet_read(dev_addr, packet)) {
    // Filtre : ignorer les messages Timing Clock (0xF8) et Active Sensing (0xFE)
    if (packet[1] == 0xF8 || packet[1] == 0xFE) {
      continue;
    }
    // Pack les 4 bytes dans un uint32_t pour transfert FIFO
    uint32_t midi_word = ((uint32_t)packet[0]) |
                        ((uint32_t)packet[1] << 8) |
                        ((uint32_t)packet[2] << 16) |
                        ((uint32_t)packet[3] << 24);
    write_to_other_core(midi_word);
    Serial.printf("[MIDI HOST] Message MIDI reçu sur device idx=%u, octets transférés = %lu\r\n", dev_addr, xferred_bytes);
    Serial.printf("[MIDI DATA] dev=%u : ", dev_addr);
    for (int i = 0; i < 4; i++) {
      Serial.printf("%02X ", packet[i]);
    }
    Serial.println();
  }
}

void print_device_descriptor(tuh_xfer_t *xfer);

void utf16_to_utf8(uint16_t *temp_buf, size_t buf_len);

void print_lsusb(void) {
  bool no_device = true;
  for (uint8_t daddr = 1; daddr < CFG_TUH_DEVICE_MAX + 1; daddr++) {
    // TODO can use tuh_mounted(daddr), but tinyusb has an bug
    // use local connected flag instead
    dev_info_t *dev = &dev_info[daddr - 1];
    if (dev->mounted) {
      Serial.printf("Device %u: ID %04x:%04x %s %s\r\n", daddr,
                    dev->desc_device.idVendor, dev->desc_device.idProduct,
                    (char *) dev->manufacturer, (char *) dev->product);

      no_device = false;
    }
  }

  if (no_device) {
    Serial.println("No device connected (except hub)");
  }
}

// Invoked when device is mounted (configured)
void tuh_mount_cb(uint8_t daddr) {
  Serial.printf("[DEBUG] Device attached, address = %d\r\n", daddr);
  Serial.flush();

  dev_info_t *dev = &dev_info[daddr - 1];
  dev->mounted = true;

  // Get Device Descriptor
  tuh_descriptor_get_device(daddr, &dev->desc_device, 18, print_device_descriptor, 0);
}

/// Invoked when device is unmounted (bus reset/unplugged)
void tuh_umount_cb(uint8_t daddr) {
  Serial.printf("[DEBUG] Device removed, address = %d\r\n", daddr);
  Serial.flush();
  dev_info_t *dev = &dev_info[daddr - 1];
  dev->mounted = false;

  // print device summary
  print_lsusb();
}

void print_device_descriptor(tuh_xfer_t *xfer) {
  if (XFER_RESULT_SUCCESS != xfer->result) {
    Serial.printf("Failed to get device descriptor\r\n");
    return;
  }

  uint8_t const daddr = xfer->daddr;
  dev_info_t *dev = &dev_info[daddr - 1];
  tusb_desc_device_t *desc = &dev->desc_device;

  Serial.printf("Device %u: ID %04x:%04x\r\n", daddr, desc->idVendor, desc->idProduct);
  Serial.printf("Device Descriptor:\r\n");
  Serial.printf("  bLength             %u\r\n"     , desc->bLength);
  Serial.printf("  bDescriptorType     %u\r\n"     , desc->bDescriptorType);
  Serial.printf("  bcdUSB              %04x\r\n"   , desc->bcdUSB);
  Serial.printf("  bDeviceClass        %u\r\n"     , desc->bDeviceClass);
  Serial.printf("  bDeviceSubClass     %u\r\n"     , desc->bDeviceSubClass);
  Serial.printf("  bDeviceProtocol     %u\r\n"     , desc->bDeviceProtocol);
  Serial.printf("  bMaxPacketSize0     %u\r\n"     , desc->bMaxPacketSize0);
  Serial.printf("  idVendor            0x%04x\r\n" , desc->idVendor);
  Serial.printf("  idProduct           0x%04x\r\n" , desc->idProduct);
  Serial.printf("  bcdDevice           %04x\r\n"   , desc->bcdDevice);

  // Get String descriptor using Sync API
  Serial.printf("  iManufacturer       %u     ", desc->iManufacturer);
  if (XFER_RESULT_SUCCESS ==
      tuh_descriptor_get_manufacturer_string_sync(daddr, LANGUAGE_ID, dev->manufacturer, sizeof(dev->manufacturer))) {
    utf16_to_utf8(dev->manufacturer, sizeof(dev->manufacturer));
    Serial.printf((char *) dev->manufacturer);
  }
  Serial.printf("\r\n");

  Serial.printf("  iProduct            %u     ", desc->iProduct);
  if (XFER_RESULT_SUCCESS ==
      tuh_descriptor_get_product_string_sync(daddr, LANGUAGE_ID, dev->product, sizeof(dev->product))) {
    utf16_to_utf8(dev->product, sizeof(dev->product));
    Serial.printf((char *) dev->product);
  }
  Serial.printf("\r\n");

  Serial.printf("  iSerialNumber       %u     ", desc->iSerialNumber);
  if (XFER_RESULT_SUCCESS ==
      tuh_descriptor_get_serial_string_sync(daddr, LANGUAGE_ID, dev->serial, sizeof(dev->serial))) {
    utf16_to_utf8(dev->serial, sizeof(dev->serial));
    Serial.printf((char *) dev->serial);
  }
  Serial.printf("\r\n");

  Serial.printf("  bNumConfigurations  %u\r\n", desc->bNumConfigurations);

  // print device summary
  print_lsusb();
}

//--------------------------------------------------------------------+
// String Descriptor Helper
//--------------------------------------------------------------------+

static void _convert_utf16le_to_utf8(const uint16_t *utf16, size_t utf16_len, uint8_t *utf8, size_t utf8_len) {
  // TODO: Check for runover.
  (void) utf8_len;
  // Get the UTF-16 length out of the data itself.

  for (size_t i = 0; i < utf16_len; i++) {
    uint16_t chr = utf16[i];
    if (chr < 0x80) {
      *utf8++ = chr & 0xff;
    } else if (chr < 0x800) {
      *utf8++ = (uint8_t) (0xC0 | (chr >> 6 & 0x1F));
      *utf8++ = (uint8_t) (0x80 | (chr >> 0 & 0x3F));
    } else {
      // TODO: Verify surrogate.
      *utf8++ = (uint8_t) (0xE0 | (chr >> 12 & 0x0F));
      *utf8++ = (uint8_t) (0x80 | (chr >> 6 & 0x3F));
      *utf8++ = (uint8_t) (0x80 | (chr >> 0 & 0x3F));
    }
    // TODO: Handle UTF-16 code points that take two entries.
  }
}

// Count how many bytes a utf-16-le encoded string will take in utf-8.
static int _count_utf8_bytes(const uint16_t *buf, size_t len) {
  size_t total_bytes = 0;
  for (size_t i = 0; i < len; i++) {
    uint16_t chr = buf[i];
    if (chr < 0x80) {
      total_bytes += 1;
    } else if (chr < 0x800) {
      total_bytes += 2;
    } else {
      total_bytes += 3;
    }
    // TODO: Handle UTF-16 code points that take two entries.
  }
  return total_bytes;
}

void utf16_to_utf8(uint16_t *temp_buf, size_t buf_len) {
  // Sécurisation : vérifier la taille du buffer et la valeur du descripteur
  size_t total_len = temp_buf[0] & 0xFF;
  if (total_len < 2) return; // rien à convertir
  size_t utf16_len = (total_len - 2) / sizeof(uint16_t);
  // Limiter la taille pour ne pas dépasser le buffer fourni
  if (utf16_len > (buf_len/2 - 1)) utf16_len = (buf_len/2 - 1);
  size_t utf8_len = _count_utf8_bytes(temp_buf + 1, utf16_len);
  if (utf8_len >= buf_len) utf8_len = buf_len - 1;

  _convert_utf16le_to_utf8(temp_buf + 1, utf16_len, (uint8_t *) temp_buf, buf_len);
  ((uint8_t *) temp_buf)[utf8_len] = '\0';
}


// INTER CORE

void MidiHost::read(){
    // Lecture des messages MIDI reçus depuis core1
  static uint32_t midi_count = 0;
  static uint32_t last_debug = 0;
  uint32_t midi_word;
  uint8_t packet[4];
  // Utilisation du FIFO Arduino style (rp2040.fifo)
  while (rp2040.fifo.available()) {
    midi_word = rp2040.fifo.pop();
    midi_count++;
    decode_word(midi_word, packet);
    Serial.print("[CORE0 MIDI] ");
    for (int i = 0; i < 4; i++) {
      Serial.printf("%02X ", packet[i]);
    }
    Serial.println();
  }
  // Affiche le nombre de messages reçus toutes les 2 secondes
  // if (millis() - last_debug > 2000) {
  //   Serial.printf("[DEBUG] core0 a recu %lu messages MIDI\r\n", midi_count);
  //   last_debug = millis();
  // }
}


bool write_to_other_core(uint32_t data) {
  if (rp2040.fifo.push_nb(data) == false) {
    Serial.println((char *)"Failed to push\n");
    return false;
  }
  // core_debug((char*)"pushed\n");
  return true;
}


bool MidiHost::receive(uint32_t* midi_word) {
  if (multicore_fifo_rvalid()) {
    *midi_word = multicore_fifo_pop_blocking();
    return true;
  }
  return false;
}

bool MidiHost::receive_nb(uint32_t* midi_word) {
  if (multicore_fifo_rvalid()) {
    *midi_word = multicore_fifo_pop_blocking();
    return true;
  }
  return false;
}

void MidiHost::decode_word(uint32_t midi_word, uint8_t* packet) {
  packet[0] = midi_word & 0xFF;
  packet[1] = (midi_word >> 8) & 0xFF;
  packet[2] = (midi_word >> 16) & 0xFF;
  packet[3] = (midi_word >> 24) & 0xFF;
}

