# About
Vfdmod is a LinuxCNC userspace component designed to control any VFD that supports standard MODBUS RTU protocol.

# Features
- Setting command speed with MODBUS function codes 0x06 and 0x10.
- Setting control word with MODBUS function codes 0x05, 0x06, 0x0F and 0x10.
- Reading any count of user defined parameters with MODBUS function codes 0x01 and 0x03.
- User defined parameters of four types are supported: bit, float, s32 and u32.
- Built-in PyVcp generator.
- RS485 bus monitoring including total error count and last error code.
- Auto reconnection attempts when USB-to-RS485 adapter was physically re-plugged.
- *Huanyang VFD's are not supported because of using non-standard MODBUS function codes.*

# Screenshots

![](https://raw.githubusercontent.com/aekhv/vfdmod/master/images/hc1-cplus-axis.png) ![](https://raw.githubusercontent.com/aekhv/vfdmod/master/images/hc1-cplus-hal.png)

# Documentation
See vfdmod wiki: [https://github.com/aekhv/vfdmod/wiki](https://github.com/aekhv/vfdmod/wiki)

# Download DEB
Latest Debian build: [https://github.com/aekhv/vfdmod/releases](https://github.com/aekhv/vfdmod/releases)

# Support
In english: [https://forum.linuxcnc.org/24-hal-components/38733-vfdmod-an-easy-vfd-control-over-modbus-rtu](https://forum.linuxcnc.org/24-hal-components/38733-vfdmod-an-easy-vfd-control-over-modbus-rtu)

In russian: [http://www.cnc-club.ru/forum/viewtopic.php?p=557679](http://www.cnc-club.ru/forum/viewtopic.php?p=557679)

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
