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
} rs485_config_t;

/* Spindle control settings */
typedef struct {
    int address;
    int runFwdValue;
    int runRevValue;
    int stopValue;
} control_config_t;

/* Spindle IN settings */
typedef struct {
    int address;
    int multiplier;
    int divider;
    int maxSpeedRpm;
    int minSpeedRpm;
} spindle_in_config_t;

/* Spindle OUT settings */
typedef struct {
    int address;
    int multiplier;
    int divider;
    double atSpeedThreshold;
} spindle_out_config_t;

/* Main INI-file settings */
typedef struct {
    QString componentName;
    rs485_config_t rs485;
    control_config_t control;
    spindle_in_config_t rpmIn;
    spindle_out_config_t rpmOut;
} main_config_t;

/* User parameter structure */
typedef struct {
    int address;
    int multiplier;
    int divider;
    hal_type_t pinType;
    QString pinName;
} user_config_t;


/*** HAL STRUCTURES ***/

typedef struct {
    /* Communication pins */
    hal_bit_t   *isConnected;
    hal_s32_t   *errorCount;
    hal_s32_t   *lastError;

    /* Spindle control pins */
    hal_bit_t   *runForward;
    hal_bit_t   *runReverse;
    hal_bit_t   *atSpeed;
    hal_float_t *spindleRpmIn;
    hal_float_t *spindleRpmOut;
} hal_main_data_t;

/* Three type of data for an each user parameter */
typedef struct {
    hal_u32_t   *u32Pin;
    hal_s32_t   *s32Pin;
    hal_float_t *floatPin;
} hal_user_data_t;

#endif // STRUCTURES_H
