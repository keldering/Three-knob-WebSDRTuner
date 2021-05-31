# KWT
 KiwiSDR Three knob Webtuner
This software was at first initiated by PA1KE 7 may 2021
badsed on an article in the Dutch Ham magazine "Electron"
April 2021 where an indication of use of one (optical) Rotary encoder 
for Web SDR control was suggested for a limited set of functions for 
several WebSDR's.

The first ALFA release of this software was done on Github at 26 mai2021 
The idea of the Three Knob (Kiwi)SDRTuner is to access  
all the functions that are made accessible by keystrokes in the original
KiwiSDR software. This is done by means of a small XIAO (SAMD21), Arduino like)
processor board. Besides the XIAO only three ordinary (cheap) rotary encoders with an OLED dsplay 128x64 
are the only components used. 

The software generates keyboard codes and sends them via the USB (C) port
of the XIAO to the device that runs the websdr software in a browser thus emulating a real keyboard connection.
The keys are sent to the device regardless the software it runs, so the TKKT (Three Knob KIWI Tuner) starts
in a locked mode where actually nothing is sent to the device running the browser with the SDR running in it.
(This to prevent unwanted keystrokes into to whatever software runs at the moment the TKKT is connected to the USB port).

Opposite to optical encoders these ordinary rotary encoders need to be debounced.
There is a very elegant solution to do this in software at:
https://www.best-microcontroller-projects.com/rotary-encoder.html
I used this software and it works very well so no need for the rather encombrent
optical ones.


