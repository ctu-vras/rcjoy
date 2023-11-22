# rcjoy
Boost gamepad control of ROS robots to kilometer ranges and 100% reliability. Throw away all your nano dongles :)

# How it works
The rcjoy is intended to be a drop in replacement of a wireless gamepad like xbox controller or Logitech F710.  
The transmitter mcu (Teensy 4.0) acts as an usb host reading data from PS4 controller connected by wire, converts them to packets and send them via SBUS to tx module (ImmersionRC Ghost lite).  
The receiverer mcu (RP2040) acts as usb hid joystick emulator, reads SBUS packets decodes them and mimic gamepad controller connected to robot's PC.  
At this moment is communication one directional.

#BoM
ImmersionRC Ghost xLite Module
https://shop.orqafpv.com/products/immersionrc-immersionrc-ghost-xlite-module
ImmersionRC Ghost Atto Receiver
https://shop.orqafpv.com/products/immersionrc-immersionrc-ghost-atto-receiver
Teensy® 4.0 Development Board
https://www.pjrc.com/store/teensy40.html
Waveshare RP2040-Zero
https://www.waveshare.com/rp2040-zero.htm
Powerbank module  
https://www.laskakit.cz/nabijecka-boost-pro-usb-powerbank-5v--usb-c/
Two LiIon 18650 cells
