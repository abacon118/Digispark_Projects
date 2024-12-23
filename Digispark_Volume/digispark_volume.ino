// Adafruit https://learn.adafruit.com/trinket-usb-volume-knob/add-a-mute-button
// Modified to include button press actions and double press Andrew Bowman 2024
// Inspired by https://www.thingiverse.com/thing:2970774

#include "TrinketHidCombo.h"

#define PIN_ENCODER_A 2
#define PIN_ENCODER_B 0
#define TRINKET_PINx  PINB
#define PIN_ENCODER_SWITCH 1

#define LEFT_ACTION MMKEY_VOL_DOWN
#define LEFT_ACTION_PRESSED MMKEY_SCAN_PREV_TRACK

#define RIGHT_ACTION MMKEY_VOL_UP
#define RIGHT_ACTION_PRESSED MMKEY_SCAN_NEXT_TRACK
#define BUTTON_ACTION_SINGLE MMKEY_PLAYPAUSE
#define BUTTON_ACTION_DOUBLE MMKEY_MUTE
#define DOUBLE_PRESS_TIME 250 //value in ms for double press

static uint8_t enc_prev_pos   = 0;
static uint8_t enc_flags      = 0;
static char    sw_was_pressed = 0;
unsigned long last_press_time = 0;
unsigned long press_count = 0;

void setup()
{
  // set pins as input with internal pull-up resistors enabled
  pinMode(PIN_ENCODER_A, INPUT);
  pinMode(PIN_ENCODER_B, INPUT);
  digitalWrite(PIN_ENCODER_A, HIGH);
  digitalWrite(PIN_ENCODER_B, HIGH);

  pinMode(PIN_ENCODER_SWITCH, INPUT);
  // the switch is active-high, not active-low
  // since it shares the pin with Trinket's built-in LED
  // the LED acts as a pull-down resistor
  digitalWrite(PIN_ENCODER_SWITCH, LOW);

  TrinketHidCombo.begin(); // start the USB device engine and enumerate

  // get an initial reading on the encoder pins
  if (digitalRead(PIN_ENCODER_A) == LOW) {
    enc_prev_pos |= (1 << 0);   }
  if (digitalRead(PIN_ENCODER_B) == LOW) {
    enc_prev_pos |= (1 << 1);   }
}

void loop()
{
  int8_t enc_action = 0; // 1 or -1 if moved, sign is direction

  // note: for better performance, the code will now use
  // direct port access techniques
  // http://www.arduino.cc/en/Reference/PortManipulation
  uint8_t enc_cur_pos = 0;
  // read in the encoder state first
  if (bit_is_clear(TRINKET_PINx, PIN_ENCODER_A)) {
    enc_cur_pos |= (1 << 0);
  }
  if (bit_is_clear(TRINKET_PINx, PIN_ENCODER_B)) {
    enc_cur_pos |= (1 << 1);
  }

  // if any rotation at all
  if (enc_cur_pos != enc_prev_pos)
  {
    if (enc_prev_pos == 0x00)
    {
      // this is the first edge
      if (enc_cur_pos == 0x01) {
        enc_flags |= (1 << 0);
      }
      else if (enc_cur_pos == 0x02) {
        enc_flags |= (1 << 1);
      }
    }

    if (enc_cur_pos == 0x03)
    {
      // this is when the encoder is in the middle of a "step"
      enc_flags |= (1 << 4);
    }
    else if (enc_cur_pos == 0x00)
    {
      // this is the final edge
      if (enc_prev_pos == 0x02) {
        enc_flags |= (1 << 2);
      }
      else if (enc_prev_pos == 0x01) {
        enc_flags |= (1 << 3);
      }

      // check the first and last edge
      // or maybe one edge is missing, if missing then require the middle state
      // this will reject bounces and false movements
      if (bit_is_set(enc_flags, 0) && (bit_is_set(enc_flags, 2) || bit_is_set(enc_flags, 4))) {
        enc_action = 1;
      }
      else if (bit_is_set(enc_flags, 2) && (bit_is_set(enc_flags, 0) || bit_is_set(enc_flags, 4))) {
        enc_action = 1;
      }
      else if (bit_is_set(enc_flags, 1) && (bit_is_set(enc_flags, 3) || bit_is_set(enc_flags, 4))) {
        enc_action = -1;
      }
      else if (bit_is_set(enc_flags, 3) && (bit_is_set(enc_flags, 1) || bit_is_set(enc_flags, 4))) {
        enc_action = -1;
      }

      enc_flags = 0; // reset for next time
    }
  }

  enc_prev_pos = enc_cur_pos;
if(!bit_is_set(TRINKET_PINx, PIN_ENCODER_SWITCH)){  //If the button is not pressed
  if (enc_action > 0) {
    TrinketHidCombo.pressMultimediaKey(RIGHT_ACTION);
  }
  else if (enc_action < 0) {
    TrinketHidCombo.pressMultimediaKey(LEFT_ACTION);
  }}
  if(bit_is_set(TRINKET_PINx, PIN_ENCODER_SWITCH)){
  if (enc_action > 0) {
    TrinketHidCombo.pressMultimediaKey(RIGHT_ACTION_PRESSED);
  }
  else if (enc_action < 0) {
    TrinketHidCombo.pressMultimediaKey(LEFT_ACTION_PRESSED);
  }}

  // remember that the switch is active-high
  if (bit_is_set(TRINKET_PINx, PIN_ENCODER_SWITCH)) {
    if (!sw_was_pressed) { // Button was just pressed
      unsigned long current_time = millis();

      if (current_time - last_press_time < DOUBLE_PRESS_TIME) {
        // Detected a double press
        press_count++;
        if (press_count == 2) {
          TrinketHidCombo.pressMultimediaKey(BUTTON_ACTION_DOUBLE);
          press_count = 0; // Reset press count after double press
        }
      } else {
        // Detected a single press or the interval has passed
        press_count = 1;
        TrinketHidCombo.pressMultimediaKey(BUTTON_ACTION_SINGLE);
      }

      last_press_time = current_time;
      delay(5); // Debounce delay
    }
    sw_was_pressed = true;
  } else {
    if (sw_was_pressed) {
      delay(5); // Debounce delay
    }
    sw_was_pressed = false;
  }


  TrinketHidCombo.poll(); // check if USB needs anything done
}