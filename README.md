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

# History
Vfdmod 0.3.2:
- User defined HAL pins may be boolean (bit type):
  - Function code 0x01 (read coils) supported.
  - Function code 0x03 (read holding registers, used by default) may be used to get any single bit within returned 16-bit data.

Vfdmod 0.3.1:
- Function codes 0x05 (write single coil) and 0x0F (write multiple coils) are supported.
- PyVcp generator: fault reset button fixed.
- Blank config file now includes all parameters and short description of each.

Vfdmod 0.3.0:
- Reconnection feature added, it's useful when serial device has been re-attached.
- Fault reset added.
- Function codes 0x06 (write single register) and 0x10 (write multiple registers) are supported.

Vfdmod 0.2.0:
- PyVcp generator added.

Vfdmod 0.1.0:
- First release.
