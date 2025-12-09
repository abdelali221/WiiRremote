# WiiRremote

Use your Wii sensor bar as a remote control!

# How to use :

The app requires a code file (codes.dat) which you neeed to create yourself.

The file is structured like the following :

The first 4 bytes of the file is the header and it contains how many codes are in the file.
After the header you have to put the codes which are made of a 8 bytes header (containing a name) and the RAW data of the IR code to transmit.

Note: The app (for now) can only transmit the codes with the NEC protocol, so make sure to verify that your device uses that before starting to complain.

When you get the codes.dat file ready, put it in the same folder as the .dol file (generally apps/WiiRremote) and launch the app on your console.

Note: The app requires both a real console and a wired ir bar, otherwise it won't work.

# Credits :

- [The Arduino-IRremote library](https://github.com/Arduino-IRremote/Arduino-IRremote/).
