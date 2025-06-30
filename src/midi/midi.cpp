#include "midi.h"
Adafruit_USBD_MIDI usb_midi(2);
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);


void midi::setup() {
  USBDevice.setManufacturerDescriptor("CLR");
  USBDevice.setProductDescriptor("showcontrol");
  usb_midi.setCableName(1, "DAW");
  usb_midi.setCableName(2, "Custom MIDI");
  usb_midi.setCableName(3, "USB MIDI");
  usb_midi.begin();
  // MIDI_DIN.begin();
  // MIDI_DIN.setHandleNoteOn(onSerialNoteOn);
}
