This repository contains all the necessary files to build a stereo microphone system that was mounted onboard several experimental rockets between 2023 and 2025.

The purpose of using a stereophonic setup—with two microphones placed 10 cm apart—is to allow post-flight playback through headphones, recreating the sensation of being onboard the rocket.

The system was developed by first designing a board equipped with two microphones, filtering capacitors, and a TLV320ADC6120IRTER dual-channel ADC. It was initially intended to interface with an external control board via I²C and I²S, but a dedicated second board was eventually designed specifically for this subsystem.

This second board integrates an ESP32 microcontroller and a microSD card. The complete system requires an external 3.3 V power source capable of supplying up to 100 mA, mainly due to current spikes during SD card write operations.

All design files for the two boards are available in this repository and can be edited with EasyEDA. The C++ firmware for the ESP32 was written using PlatformIO. The program first configures the ADC by writing to its registers, then continuously reads data from the I²S interface and writes it in bursts to the microSD card as a .bin file.

The provided extractsound.py script converts these .bin recordings into .wav files for playback and further audio post-processing in Python.

An example recording from a 2025 flight onboard a two-stage rocket can be found [here](https://youtu.be/YBQxnALEHxU). The audio quality during ascent is relatively poor due to microphone saturation at high sound pressure levels. This issue could be mitigated by significantly lowering the ADC gain or by implementing a dynamic gain control feature that automatically adjusts the input gain based on the detected volume.

Enjoy exploring the project! 🚀🎧


