#ifndef DEFAULTVALUES_H
#define DEFAULTVALUES_H

/* DEFUALT CONFIG FILE GROUPS, KEYS & VALUES */

/* Common group */
#define GROUP_COMMON                "Common"
#define KEY_COMPONENT_NAME          "ComponentName"
#define KEY_MAX_SPEED_RPM           "MaxSpeedRPM"
#define KEY_MIN_SPEED_RPM           "MinSpeedRPM"
#define KEY_AT_SPEED_THRESHOLD      "AtSpeedThreshold"
#define VALUE_AT_SPEED_THRESHOLD    "0.05"

/* Communication group */
#define GROUP_RS485                 "RS485"
#define KEY_SLAVE_ADDRESS           "SlaveAddress"
#define KEY_SERIAL_DEVICE           "SerialDevice"
#define KEY_BAUD_RATE               "BaudRate"
#define KEY_DATA_BITS               "DataBits"
#define KEY_PARITY                  "Parity"
#define KEY_STOP_BITS               "StopBits"
#define KEY_LOOP_DELAY              "LoopDelay"
#define KEY_PROTOCOL_DELAY          "ProtocolDelay"
#define KEY_IS_CONNECTED_DELAY      "IsConnectedDelay"
#define KEY_CONNECTION_ERROR_LIST   "ConnectionErrorList"
#define KEY_CONNECTION_DELAY        "ConnectionDelay"
#define VALUE_DATA_BITS             "8"
#define VALUE_PARITY                "N"
#define VALUE_STOP_BITS             "1"
#define VALUE_LOOP_DELAY            "200"
#define VALUE_PROTOCOL_DELAY        "4"
#define VALUE_IS_CONNECTED_DELAY    "10"
#define VALUE_CONNECTION_DELAY      "1000"

/* Spindle control group */
#define GROUP_CONTROL               "Control"
// valid for function codes 0x06 & 0x10
#define KEY_RUN_FWD                 "RunForwardValue"
#define KEY_RUN_REV                 "RunReverseValue"
#define KEY_STOP                    "StopValue"
#define KEY_FAULT_RESET             "FaultResetValue"
// valid for function codes 0x05 & 0x0F
#define KEY_RUN_COIL                "RunCoil"
#define KEY_DIRECTION_COIL          "DirectionCoil"
#define KEY_FAULT_RESET_COIL        "FaultResetCoil"

/* Spindle RPM-in group */
#define GROUP_SPINDLE_IN            "SpindleRpmIn"

/* Spindle RPM-out group */
#define GROUP_SPINDLE_OUT           "SpindleRpmOut"

/* User parameter group */
#define KEY_BIT_MASK                "BitMask"
#define KEY_PIN_TYPE                "PinType"
#define KEY_PIN_NAME                "PinName"

/* Common keys & values */
#define KEY_FUNCTION_CODE           "FunctionCode"
#define KEY_ADDRESS                 "Address"
#define KEY_DIVIDER                 "Divider"
#define KEY_MULTIPLIER              "Multiplier"
#define VALUE_DIVIDER               "1"
#define VALUE_MULTIPLIER            "1"

/* Default HAL groups & pins */
#define HAL_GROUP_RS485             "rs485"
#define HAL_GROUP_USER_PARAMETERS   "parameters"
#define HAL_GROUP_SPINDLE           "spindle"
#define HAL_GROUP_CONTROL           "control"
#define HAL_PIN_AT_SPEED            "at-speed"
#define HAL_PIN_RPM_IN              "rpm-in"
#define HAL_PIN_RPM_OUT             "rpm-out"
#define HAL_PIN_FAULT_RESET         "fault-reset"
#define HAL_PIN_RUN_FORWARD         "run-forward"
#define HAL_PIN_RUN_REVERSE         "run-reverse"
#define HAL_PIN_IS_CONNECTED        "is-connected"
#define HAL_PIN_ERROR_COUNT         "error-count"
#define HAL_PIN_LAST_ERROR          "last-error"

/* Default PyVcp name to make postgui HAL file */
#define PYVCP                       "pyvcp"


/* MODBUS */

/* Function codes */
#define MODBUS_FUNC_READ_MULTIPLE_COILS                 0x01
#define MODBUS_FUNC_READ_MULTIPLE_HOLDING_REGISTERS     0x03
#define MODBUS_FUNC_WRITE_SINGLE_COIL                   0x05
#define MODBUS_FUNC_WRITE_MULTIPLE_COILS                0x0F
#define MODBUS_FUNC_WRITE_SINGLE_HOLDING_REGISTER       0x06
#define MODBUS_FUNC_WRITE_MULTIPLE_HOLDING_REGISTERS    0x10

#define INACTIVE_FLAG                                   0x1FFFF

#endif // DEFAULTVALUES_H
