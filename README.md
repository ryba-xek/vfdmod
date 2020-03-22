# About
Vfdmod is a LinuxCNC HAL userspace interface to control any VFD what supports MODBUS RTU protocol.

# Features
- Vfdmod takes settings from a single INI file, so you don't need to specify a lot of command line arguments after a 'loadusr -W'.
- Vfdmod provides full control of your VFD: start spindle forward/reverse, stop spindle, output speed monitoring and 'at-speed' signal conditioning.
- Vfdmod allows to monitor any count of VFD's registers and turns return value to float or signed/unsigned integer at user's choice.
- Vfdmod constantly monitors RS485 bus and allows to count an errors and shows last error code.

# PyVcp screens
**Tecorp HC1-CPlus**

![](https://raw.githubusercontent.com/aekhv/vfdmod/master/images/hc1-cplus-1.png) ![](https://raw.githubusercontent.com/aekhv/vfdmod/master/images/hc1-cplus-2.png)

# Download \*.deb
Latest build for Debian Wheezy & Stretch: [https://drive.google.com/drive/folders/1h3QomeaVBhC2EMr2kg6_Duv5fGrwGm8s](https://drive.google.com/drive/folders/1h3QomeaVBhC2EMr2kg6_Duv5fGrwGm8s)
