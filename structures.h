#ifndef STRUCTURES_H
#define STRUCTURES_H

/* Required by hal.h */
#define ULAPI

#include <hal.h>

/*** INI STRUCTURES ***/

/* Communication settings */
typedef struct {
    int slaveAddress;
    QString serialDevice;
    int baudRate;
    int dataBits;
    QString parity;
    int stopBits;
    int loopDelayMs;   // Delay in milliseconds after all user parameters have been read.
    int protocolDelay; // Modbus RTU specification recommends at least 3.5 characters, so minimum value should be 4.
} ini_rs485_t;

/*  */
typedef struct {

} ini_main_t;

/* User pin type can be float, s32, or u32. */
enum pin_type_t { PIN_TYPE_FLOAT, PIN_TYPE_S32, PIN_TYPE_U32 };

/* Each user parameter has this structure */
typedef struct {
    int address;
    int multiplier;
    int divider;
    pin_type_t pinType; // see above
    QString pinName;
} ini_user_t;


/*** HAL STRUCTURES ***/

/* Communication pins */
typedef struct {
    hal_bit_t   *is_connected;
    hal_s32_t   *error_count;
    hal_s32_t   *last_error;
} hal_rs485_t;

/* Spindle control pins */
typedef struct {
    hal_bit_t   *run_forward;
    hal_bit_t   *run_reverse;
    hal_bit_t   *at_speed;
    hal_float_t *spindle_rpm_in;
    hal_float_t *spindle_rpm_out;

} hal_main_t;

/* Three type of data for an each user parameter */
typedef struct {
    hal_u32_t   *u32Pin;
    hal_s32_t   *s32Pin;
    hal_float_t *floatPin;
} hal_user_t;

#endif // STRUCTURES_H
