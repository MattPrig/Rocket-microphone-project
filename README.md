# Rocket-microphone-project
This repository gives all the necessary files to build a stereo microphone system that was put onboard of several experimental rockets over the 2023-2025 period. The point of doing a stereophonic capture of the sound with 2 microphones put 10cm apart is to be able to put headphones post-flight and get the audio as if we were onboard. 
This system was developped by building first a board with the 2 microphones, capacitors for filtering and a TLV320ADC6120IRTER as dual channel ADC. it was supposed to be linked to a external board throught I2C and I2S, but in the end, a second board was built specifically for this subsystem. The second board is made of an ESP32 and a microSD card. The whole system needs an external 3.3V source for power, of maximum 100mA (due to microSD card writing cycles pulling current spikes).

The files for the two boards can be found in the repo and are meant to be edited on EasyEDA.
The C++ code for the ESP32 was wrote using PlatformIO as IDE. It works by first settings the parameters of the ADC by writing in its registers, and then simply reading the I2S line and writing bursts of datas on the microSD card in a .bin file.
Finally, the extractsound.py file is a file for converting the .bin to wav files that you can listen to.
Python for audio post-processing.

The audio of the flight done in 2025 onboard a 2 stages rocket can be found [here](https://youtu.be/YBQxnALEHxU). The audio is of poor quality during the ascending phase of the flight, because of the saturation of the microphones during this phase. One way to prevent this would be to lower the gain of the ADC by a lot, or to use a "dynamic gain" parameter on the ADC for it to automatically adjust the gain according to the volume of what is listened.

Have fun !

