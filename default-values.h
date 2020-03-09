#ifndef DEFAULTVALUES_H
#define DEFAULTVALUES_H

/* Config file default values */
#define GROUP_RS485             "RS485"
#define GROUP_SPINDLE_LIMITS    "SpindleLimits"

#define KEY_SLAVE_ADDRESS       "SlaveAddress"
#define KEY_SERIAL_DEVICE       "SerialDevice"
#define KEY_BAUD_RATE           "BaudRate"
#define KEY_DATA_BITS           "DataBits"
#define KEY_PARITY              "Parity"
#define KEY_STOP_BITS           "StopBits"
#define KEY_LOOP_DELAY_MS       "LoopDelayMS"
#define KEY_PROTOCOL_DELAY      "ProtocolDelay"
#define KEY_MAX_SPEED_RPM       "MaxSpeedRPM"
#define KEY_MIN_SPEED_RPM       "MinSpeedRPM"

#define VALUE_SLAVE_ADDRESS     "1"
#define VALUE_SERIAL_DEVICE     "/dev/ttyS0"
#define VALUE_BAUD_RATE         "9600"
#define VALUE_DATA_BITS         "8"
#define VALUE_PARITY            "N"
#define VALUE_STOP_BITS         "1"
#define VALUE_LOOP_DELAY_MS     "100"
#define VALUE_PROTOCOL_DELAY    "4"
#define VALUE_MAX_SPEED_RPM     "24000"
#define VALUE_MIN_SPEED_RPM     "3000"

#define KEY_ADDRESS             "Address"
#define KEY_DIVIDER             "Divider"
#define KEY_MULTIPLIER          "Multuplier"
#define KEY_PIN_TYPE            "PinType"
#define KEY_PIN_NAME            "PinName"

#define VALUE_DIVIDER           "1"
#define VALUE_MULTIPLIER        "1"

#endif // DEFAULTVALUES_H
