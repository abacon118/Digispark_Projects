## Digispark Volume

This volume knob is based off [The Knob - Micro USB Media Controller Digispark Arduino by Frank Lumien on Thingiverse](https://www.thingiverse.com/thing:2970774) . The code provided didn't function properly on the Digispark and Rotary encoder I was using.  I used [Adafruit's Trinket USB Volume Knob code](https://learn.adafruit.com/trinket-usb-volume-knob/add-a-mute-button) and added additional functionality like the button press options present in Lumien's code along with the capability to mute with a double press of the button.


### Pinout
| Digispark | KY-040 Rotary Encoder | Notes                                                                                |
|-----------|-----------------------|--------------------------------------------------------------------------------------|
| 5V        | GND                   | This operates with an active high.  Swapping GND and 5V corrects this on the button. |
| GND       | +                     |                                                                                      |
| P0        | CLK                   |                                                                                      |
| P1        | SW                    | The LED on the Digispark acts as a pull up resistor                                  |
| P2        | DT                    |                                                                                      |
