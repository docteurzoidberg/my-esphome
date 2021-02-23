# My personnal esphome config / devices

(this readme is a work in progress)

## alarmkeypad.yaml

//TODO: insert pictures / videos

A homeassistant physical alarm keypad + display
Using keypad and ht16k33 custom components from ssied

- using mechanical keyboard switchs
- adressable keypad backlight leds
- ht16k33 quad alphanumerical displays x2
- underglow (case) leds
- pn532 rfid reader
- pcf8574a for keypad matrix input

#ht16k33alpha #pn532 #pcf8574 #keypad #fastled

## zilloscope.yaml

//TODO: insert pictures / videos

A 32x8 led matrix display using adressable_led_display by //TODO: find name

Cf custom component 'zilloscope' for features

#custom_components #display #addressable_led_display #fastled

## zillominee.yaml

//TODO: insert pictures / videos

A bigger custom made 35x25 rgb led matrix display. using the zilloscope component for features

Cf custom component 'zilloscope' for features

#custom_components #display #addressable_led_display #fastled #gpio

## tentionlatete.yaml

//TODO: insert pictures / videos

(early stages)

#max7219_matrix #display #ultrasound #fastled #dfplayer

## others

//TODO: write about other significants esphome projects

# Gif for Esphome from Aseprite*

Aseprite seems to not export gifs that esphome can convert nicelly, prefer exporting animation as multiple pngs, then converting it to gif using imagemagick.

  convert -delay 20 -loop 0 $(ls -1 *.png | sort -V)  boot1.gif

converts *png to gif, but keeping numerical order