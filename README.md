View this project on [CADLAB.io](https://cadlab.io/project/1200). 

# NixieClock
Nixie Tube Clock based project based on existing code and boards by Ogilumen and RobotPirate

The current project uses an Arduino Uno and the Arduinix Shield from RobotPirate. The code is an extension of the sample code
provided by the latter. The PCB for 6 nixie tubes was designed based on the .pdf schematic available in [Arduinix](http://www.arduinix.com).

As of 2019, the current design uses a bluetooth module (HC-06 or HM-10). The use of HM-10 was chosen since it's a Bluetooth 4.0+ module that allows it to work with both Android and iOS apps.

The iOS app was implemented with the work from [hoiberg](https://github.com/hoiberg/HM10-BluetoothSerial-iOS) as a template. His explanation of his project can be found [here](http://www.hangar42.nl/hm10) 
