#ifndef STRUCTURES_H
#define STRUCTURES_H

/* Required by hal.h */
#define ULAPI

#include <hal.h>

/* Connection settings */
typedef struct {
    int slaveAddress;
    QString serialDevice;
    int baudRate;
    int dataBits;
    QString parity;
    int stopBits;
    int loopDelayMs;   // Delay in milliseconds after all user parameters have been read.
    int protocolDelay; // Modbus RTU specification recommends at least 3.5 characters, so minimum value should be 4.
} rs485_config_t;

/* Three type of data for an each user parameter */
typedef struct {
    hal_u32_t   *u32Pin;
    hal_s32_t   *s32Pin;
    hal_float_t *floatPin;
} hal_parameter_t;

/* User pin type can be float, s32, or u32. */
enum pin_type_t { PIN_TYPE_FLOAT = 0, PIN_TYPE_S32, PIN_TYPE_U32 };

/* Each user parameter has this structure */
typedef struct {
    int address;
    int multiplier;
    int divider;
    pin_type_t pinType;
    QString pinName;
    hal_parameter_t *pinData;
} user_parameter_t;

#endif // STRUCTURES_H
