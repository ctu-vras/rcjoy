//RPi pico code 
//Waveshare RP2040 zero
//https://www.waveshare.com/wiki/RP2040-Zero

#include <Adafruit_TinyUSB.h>
//Change USB Stack to "Adafruit TinyUSB"

#include "sbus.h"
//https://github.com/bolderflight/sbus 
//v 8.1.4
#include <Adafruit_NeoPixel.h>
//https://github.com/adafruit/Adafruit_NeoPixel
//V1.12.0

extern "C" {
#include <hardware/watchdog.h>
};
//https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__watchdog.html#gaa73ccd43d6d755817e71bc3d7eb1fda6

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_GAMEPAD()
};

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, 2, false);

// Report payload defined in src/class/hid/hid.h
// - For Gamepad Button Bit Mask see  hid_gamepad_button_bm_t
// - For Gamepad Hat    Bit Mask see  hid_gamepad_hat_t
hid_gamepad_report_t gp;






/* SbusRx object on Serial1 */
bfs::SbusRx sbus_rx(&Serial1);
//std::array<int16_t, bfs::SbusRx::NUM_CH()> sbus_data;
bfs::SbusData sbus_data;

uint16_t buttmp1, buttmp2;
uint32_t buttmp;
uint8_t button1[8], button2[8];

Adafruit_NeoPixel pixels(1, 16, NEO_RGB + NEO_KHZ800);
uint8_t r, g, b;

uint64_t lastRXtime;
uint32_t timeout = 1000;
uint32_t timeout_count = 10;

void setup() {
  TinyUSBDevice.setSerialDescriptor("rcjoy");
  Serial.begin(115200);
  // Serial to display the received data

#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  TinyUSB_Device_Init(0);
#endif


  usb_hid.begin();

  // wait until device mounted
  while (!TinyUSBDevice.mounted()) delay(1);

  gp.x = 0;
  gp.y = 0;
  gp.z = -127;
  gp.rx = 0;
  gp.ry = 0;
  gp.rz = -127;
  gp.hat = 0;
  gp.buttons = 0;  //Maybe DMS should be forced to push???
  usb_hid.sendReport(0, &gp, sizeof(gp));

  //Serial.begin(115200);
  //while (!Serial) {}

  /* Begin the SBUS communication */
  sbus_rx.Begin();


  Serial.println("rcjoy Init");
  //pinMode(25, OUTPUT);

  pixels.begin();
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();
  watchdog_enable(100000, 1);
}

void loop() {

  if (lastRXtime + timeout < millis()) {
    Serial.println("Timeout!");
    r = 255;
    g = 0;
    gp.x = 0;
    gp.y = 0;
    gp.z = -127;
    gp.rx = 0;
    gp.ry = 0;
    gp.rz = -127;
    gp.hat = 0;
    gp.buttons = 0;  //Maybe DMS should be forced to push???
    usb_hid.sendReport(0, &gp, sizeof(gp));
    delay(100);
  } else {
    watchdog_update();
    r = 0;
    g = 255;
  }

  if (!usb_hid.ready()) {
    //return;
    b = 255;
  } else {
    b = 0;
  }
  pixels.setPixelColor(0, pixels.Color(r, g, b));
  pixels.show();

  //if ((sbus_rx.Read()) && (sbus_data.ch[8]==32)) {
  if (sbus_rx.Read()) {
    lastRXtime = millis();
    sbus_data = sbus_rx.data();
    /* Display the received data */

    for (int i = 0; i < 9; i++) {
      Serial.print(sbus_data.ch[i] / 8);
      Serial.print(",");
    }
    Serial.println();

    buttmp1 = sbus_data.ch[6] / 8;
    for (uint8_t b = 0; b < 8; b++) {
      button1[b] = buttmp1 % 2;
      buttmp1 /= 2;
      Serial.print(button1[b]);
      Serial.print("\t");
    }

    buttmp2 = ((sbus_data.ch[7] / 8) & 0b1111);
    for (uint8_t b = 0; b < 8; b++) {
      button2[b] = buttmp2 % 2;
      buttmp2 /= 2;
      Serial.print(button2[b]);
      Serial.print("\t");
    }

    Serial.println("");
    //Serial.println("");


    buttmp = 1 * button1[0] + 2 * button1[1] + 4 * button1[2] + 8 * button1[3] + 16 * button1[4] + 32 * button1[5] + 64 * button1[6] + 128 * button1[7] + 256 * button2[0] + 512 * button2[1] + 1024 * button2[2] + 2048 * button2[3] + 4096 * button2[4] + 8192 * button2[5] + 16384 * button2[6] + 32768 * button2[7];

    //if (!sbus_data.failsafe) {
    //digitalWrite(25, HIGH);
    if (sbus_data.ch[8]==32) { //Sbus data are generated by transmitter program 
    gp.x = map(sbus_data.ch[0] / 8, 0, 255, -127, 127);
    gp.y = map(sbus_data.ch[1] / 8, 0, 255, -127, 127);
    gp.z = map(sbus_data.ch[2] / 8, 0, 255, -127, 127);
    gp.rx = map(sbus_data.ch[3] / 8, 0, 255, -127, 127);
    gp.ry = map(sbus_data.ch[4] / 8, 0, 255, -127, 127);
    gp.rz = map(sbus_data.ch[5] / 8, 0, 255, -127, 127);
    gp.hat = sbus_data.ch[7] / 128;  //8*16!!!!
    gp.buttons = buttmp;
    }
    /* } else {
      //digitalWrite(25, LOW);
      gp.x = 0;
      gp.y = 0;
      gp.z = -127;
      gp.rx = 0;
      gp.ry = 0;
      gp.rz = -127;
      gp.hat = 0;
      gp.buttons = 0;  //DMS ???
    }
    Serial.print("FAILSAFE: ");
    Serial.println(sbus_data.failsafe);
    Serial.print("LOST FRAME: ");
    Serial.println(sbus_data.lost_frame);
    */
    usb_hid.sendReport(0, &gp, sizeof(gp));
  }
}
