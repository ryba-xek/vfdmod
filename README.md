# About
Vfdmod is a LinuxCNC userspace component designed to control any VFD what supports MODBUS RTU protocol.

# Features
- All settings are stored in a single plain text INI-file. This file describes communication settings, spindle limits, VFD's control registers to run spindle cw/ccw, any count of user defined parameters such as output current, frequency, temperature, etc.
- Vfdmod has additional HAL pins to track RS485 communication status: is VFD connected (or not), total count of communication errors and last error code.
- Built-in PyVcp generator to make a simple control panel, what can be attached to Axis main window.

# Screenshots

![](https://raw.githubusercontent.com/aekhv/vfdmod/master/images/hc1-cplus-axis.png) ![](https://raw.githubusercontent.com/aekhv/vfdmod/master/images/hc1-cplus-hal.png)

# Download \*.deb
Latest build for Debian Wheezy & Stretch: [https://drive.google.com/drive/folders/1h3QomeaVBhC2EMr2kg6_Duv5fGrwGm8s](https://drive.google.com/drive/folders/1h3QomeaVBhC2EMr2kg6_Duv5fGrwGm8s)

# Usage & support
English: [https://forum.linuxcnc.org/24-hal-components/38733-vfdmod-an-easy-vfd-control-over-modbus-rtu](https://forum.linuxcnc.org/24-hal-components/38733-vfdmod-an-easy-vfd-control-over-modbus-rtu)

Russian: [http://www.cnc-club.ru/forum/viewtopic.php?p=557679](http://www.cnc-club.ru/forum/viewtopic.php?p=557679)
