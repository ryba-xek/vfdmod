#ifndef STRUCTURES_H
#define STRUCTURES_H

/* Required by hal.h */
#define ULAPI

#include <hal.h>
#include <QVector>

#define MODBUS_FUNC_WRITE_SINGLE_COIL                   0x05
#define MODBUS_FUNC_WRITE_MULTIPLE_COILS                0x0F
#define MODBUS_FUNC_WRITE_SINGLE_HOLDING_REGISTER       0x06
#define MODBUS_FUNC_WRITE_MULTIPLE_HOLDING_REGISTERS    0x10

/*** INI STRUCTURES ***/

/* Common settings */
typedef struct {
    QString componentName;
    int maxSpeedRpm;
    int minSpeedRpm;
    double atSpeedThreshold;
} common_config_t;

/* Communication settings */
typedef struct {
    int slaveAddress;
    QString serialDevice;
    int baudRate;
    int dataBits;
    QString parity;
    int stopBits;
    // Delay in milliseconds after all user parameters have been transferred, 100...200 ms should be fine.
    int loopDelay;
    // Delay in characters at beginning of every Modbus request. Modbus RTU specification recommends
    // at least 3.5 characters, so minimum value must be 4.
    int protocolDelay;
    // How many successfull Modbus requests should be completed to set HAL isConnected pin.
    int isConnectedDelay;
    // Delay in millisecods before reconnection attempt.
    int connectionDelay;
    // Error codes what call reconnection attempt.
    QVector<int> criticalErrors;

} rs485_config_t;

/* Spindle control settings */
typedef struct {
    int functionCode;
    int address;
    int runFwdValue;
    int runRevValue;
    int stopValue;
    int faultResetValue;
} control_config_t;

/* Spindle IN settings */
typedef struct {
    int functionCode;
    int address;
    int multiplier;
    int divider;
} spindle_in_config_t;

/* Spindle OUT settings */
typedef struct {
    int address;
    int multiplier;
    int divider;
} spindle_out_config_t;

/* Main INI-file settings */
typedef struct {
    common_config_t common;
    rs485_config_t rs485;
    control_config_t control;
    spindle_in_config_t rpmIn;
    spindle_out_config_t rpmOut;
} main_config_t;

/* User parameter structure */
typedef struct {
    QString groupName;
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
    hal_bit_t   *faultReset;
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
