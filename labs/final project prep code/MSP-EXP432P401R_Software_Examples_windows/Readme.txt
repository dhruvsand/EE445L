MSP-EXP432P401R Software Examples v3.00.00
Texas Instruments, Inc.


********************************************************************************
Welcome to the MSP-EXP432P401R LaunchPad!
********************************************************************************

In this package, you will find software examples and source codes that can help
you quickly get started on developing your own project.

The firmware example projects are located in the 'Firmware/Source' folder:
 1. 430BOOST-SENSE1_CapTouch_MSP432P401R        | CapTouch Example for 430BOOST-SENSE1
 2. 430BOOST-SHARP96_CapTouch_MSP432P401R       | CapTouch Example for 430BOOST-SHARP96
 3. 430BOOST-SHARP96_GrlibExample_MSP432P401R   | Grlib Example for 430BOOST-SHARP96
 4. BlinkLED_MSP432P401R                        | Blink LED example source code
 5. BOOSTXL-BATPAKMKII_FuelGauge_MSP432P401R    | Fuel Gauge demo on BOOSTXL-BATPAKMKII
 6. BOOSTXL-EDUMKII_Accelerometer_MSP432P401R   | Accelerometer demo on BOOSTXL-EDUMKII
 7. BOOSTXL-EDUMKII_JoyStick_MSP432P401R        | Joystick demo on BOOSTXL-EDUMKII
 8. BOOSTXL-EDUMKII_LightSensor_MSP432P401R     | Light Sensor demo on BOOSTXL-EDUMKII
 9. BOOSTXL-EDUMKII_MicrophoneFFT_MSP432P401R   | Mic + FFT demo using CMSIS DSP lib on BOOSTXL-EDUMKII
10. BOOSTXL-EDUMKII_Temperature_MSP432P401R     | Temperature sensor demo on BOOSTXL-EDUMKII
11. BOOSTXL-K350QVG-S1_GrlibExample_MSP432P401R | Grlib Example for BOOSTXL-K350QVG-S1
12. BOOSTXL-SENSORS_SensorGUI_MSP432P401R       | Sensors GUI demo using BOOSTXL-SENSORS
13. BOOSTXL-SENSORS_TI_RTOS_SensorsGUI_MSP432P401R | TI-RTOS Sensors GUI demo using BOOSTXL-SENSORS
13. CC3100BOOST_MQTT-TwitterLED_MSP432P401R     | MQTT Example for CC3100BOOST
14. OutOfBox_MSP432P401R                        | The LaunchPad's Out Of Box Demo

In addition, prebuilt binary outputs for each firmware example are located in the
'Firmware/Binary' folder. These filess are ready to be downloaded onto your
LaunchPad using a MSP432 programming interface.

The Out of Box Demo GUI can be launched by running the executable inside the
'GUI/Binary' folder. Source for the GUI can also be found in a zip within the
'GUI/Source' folder.

LaunchPad Drivers are necessary on Windows platform before you can download the
prebuilt firmware images or communicate with the LaunchPad using the Out of Box
Demo GUI. Follow the link inside /Drivers/Emulation/ to download and install the
XDS Emulation Software Package. Make sure to right click on the installer and click
"Run as administrator". The drivers may already be installed on your computer if
you've already used TI CCS, IAR, Keil, or Energia to program MSP-EXP432P401R.

You can also find various web links in the 'Documentation' folder pointing to
the LaunchPad's Homepage, Quick Start Guide, User's Guide, and Hardware Design Files.

For more resources, be sure to visit www.ti.com/tool/msp-exp43p401r


********************************************************************************
Revision History
********************************************************************************
Ver 3.00.00 - June 3rd, 2016
- New Rev2.0 PCB hardware design
- Added BOOSTXL-BATPAKMKII and BOOSTXL-SENSORS demos

Ver 2.10.00 - February 17th, 2016
- Header file, Driver Library updates

Ver 2.00.00 - November 4th, 2015
- Updated demo projects for CMSIS header file updates
- Added CapTouch BoosterPack demos

Ver 1.05.00 - November 4th, 2015
- Bug fixes to demo projects

Ver 1.04.00 - August 24th, 2015
- Fixed Educational Boosterpack MKII example projectSpec include path error in Linux

Ver 1.03.00 - June 22nd, 2015
- Added MacOSX + Linux support for OOB GUI and DSLite
- Added Educational Boosterpack MKII example projects

Ver 1.02.00 - May 15th, 2015
- Fixed OOB GUI path length issue in Windows
- Added DSLite utility for one-click example firmware download
- Reorganized Software file structure

Ver 1.01.02 - April 14th, 2015
- Bug fixes for OutOfBox demo
- Added KEIL project files to software examples
- Updated IAR project files
- Added Blink LED example

Ver 1.00.05 - March 24th, 2015
- Initial Release


********************************************************************************
Project File Listing
********************************************************************************
|-Documentation                                   : Folder containing links to more resources
|-Driver                                          : Download Link to emulation drivers
|-Firmware                                        : Folder containing example firmwares
  |-Binary                                        : Prebuilt example firmware images
  |-Source                                        : Example firmware source projects
    |-430BOOST-SENSE1_CapTouch_MSP432P401R        : Grlib example with 430BOOST-SHARP96
    |-430BOOST-SHARP96_CapTouch_MSP432P401R       : Grlib example with 430BOOST-SHARP96
    |-430BOOST-SHARP96_GrlibExample_MSP432P401R   : Grlib example with 430BOOST-SHARP96
    |-BlinkLED_MSP432P401R                        : Blink LED example source code
    |-BOOSTXL-BATPAKMKII_FuelGauge_MSP432P401R    : Fuel Gauge demo on BOOSTXL-BATPAKMKII
    |-BOOSTXL-EDUMKII_Accelerometer_MSP432P401R   : Accelerometer demo on BOOSTXL-EDUMKII
    |-BOOSTXL-EDUMKII_JoyStick_MSP432P401R        : Joystick demo on BOOSTXL-EDUMKII
    |-BOOSTXL-EDUMKII_LightSensor_MSP432P401R     : Light Sensor demo on BOOSTXL-EDUMKII
    |-BOOSTXL-EDUMKII_MicrophoneFFT_MSP432P401R   : Mic + FFT demo using CMSIS DSP lib on BOOSTXL-EDUMKII
    |-BOOSTXL-EDUMKII_Temperature_MSP432P401R     : Temperature sensor demo on BOOSTXL-EDUMKII
    |-BOOSTXL-K350QVG-S1_GrlibExample_MSP432P401R : Grlib example with BOOSTXL-K350QVG-S1
    |-BOOSTXL-SENSORS_SensorGUI_MSP432P401R       : Sensors GUI demo using BOOSTXL-SENSORS
    |-BOOSTXL-SENSORS_TI_RTOS_SensorsGUI_MSP432P401R : TI-RTOS Sensors GUI demo using BOOSTXL-SENSORS
    |-CC3100BOOST_MQTT-TwitterLED_MSP432P401R     : MQTT demo source code with CC3100BOOST
    |-dsplib-msp432                               : Generates CMSIS DSP Software Library .lib
    |-OutOfBox_MSP432P401R                        : Out of box demo source code
|-GUI                                             : Folder containing GUI related files
  |-Binary                                        : GUI executables
  |-Source                                        : GUI Source
    |-OutOfBox_MSP432P401R_GUI.zip                : Out of box demo GUI source code
|-Readme.txt                                      : This file
