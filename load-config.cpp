#include <QFile>
#include <QSettings>
#include <QStringList>
#include "default-values.h"
#include "structures.h"
#include <QDebug>

extern int checkFlag;
extern QString exeName;

/*
 * QString::toInt(bool *ok, int base = 10) is not available in Qt4,
 * so I will use my own converter from a hex string to an integer.
 */
int hex_to_int(QString s, bool *ok)
{
    int base = 1;
    int result = 0;
    QString digits = "0123456789abcdef";

    if (s.toLower().startsWith("0x"))
        s.remove(0, 2);

    if (s.isEmpty()) {
        *ok = false;
        return -1;
    }

    for (int i = s.length() - 1; i >= 0; --i) {
        *ok = false;
        for (int j = 0; j < digits.length(); ++j) {
            if (s.toLower().at(i) == digits.at(j)) {
                *ok = true;
                result += j * base;
                continue;
            }
        }
        if (!*ok)
            return -1;
        base *= 16;
    }

    return result;
}

int load_common_group(QSettings &ini, common_config_t &common)
{
    bool ok;
    QString key;

    ini.beginGroup(GROUP_COMMON);
    if (checkFlag)
        printf("[%s]\n", qPrintable(GROUP_COMMON));

    /* Component name */
    common.componentName = ini.value(KEY_COMPONENT_NAME, exeName).toString();

    if (common.componentName.isEmpty())
        common.componentName = exeName;

    if(checkFlag)
        printf("%s\t\t: %s\n", KEY_COMPONENT_NAME, qPrintable(common.componentName));

    /* Max speed RPM */
    common.maxSpeedRpm = ini.value(key = KEY_MAX_SPEED_RPM, "").toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if (common.maxSpeedRpm <= 0)
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t: %d\n", KEY_MAX_SPEED_RPM, common.maxSpeedRpm);

    /* Min speed RPM */
    common.minSpeedRpm = ini.value(key = KEY_MIN_SPEED_RPM, "").toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if ((common.minSpeedRpm <= 0)
            || (common.minSpeedRpm > common.maxSpeedRpm))
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t: %d\n", KEY_MIN_SPEED_RPM, common.minSpeedRpm);

    /* At speed threshold */
    common.atSpeedThreshold = ini.value(key = KEY_AT_SPEED_THRESHOLD, VALUE_AT_SPEED_THRESHOLD).toDouble(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if ((common.atSpeedThreshold < 0) || (common.atSpeedThreshold > 1.0))
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t: %.2f\n", KEY_AT_SPEED_THRESHOLD, common.atSpeedThreshold);

    ini.endGroup();
    return 0;
fail_invalid_parameter:
    fprintf(stderr, "[%s]:%s - parameter is wrong or missing!\n", GROUP_RS485, qPrintable(key));
    return -1;
fail_out_of_range:
    fprintf(stderr, "[%s]:%s - parameter is out of range!\n", GROUP_RS485, qPrintable(key));
    return -1;
}

int load_rs485_group(QSettings &ini, rs485_config_t &rs485)
{
    bool ok;
    QString key, value;
    QStringList errors;

    ini.beginGroup(GROUP_RS485);
    if (checkFlag)
        printf("\n[%s]\n", qPrintable(GROUP_RS485));

    /* Slave address */
    value = ini.value(key = KEY_SLAVE_ADDRESS, "").toString();

    if (value.toLower().startsWith("0x"))
        rs485.slaveAddress = hex_to_int(value, &ok);
    else
        rs485.slaveAddress = value.toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if ((rs485.slaveAddress < 0) || (rs485.slaveAddress > 0xFF))
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t: 0x%02X (%d)\n", KEY_SLAVE_ADDRESS, rs485.slaveAddress, rs485.slaveAddress);

    /* Serial device path */
    rs485.serialDevice = ini.value(key = KEY_SERIAL_DEVICE, "").toString();

    if (rs485.serialDevice.isEmpty())
        goto fail_invalid_parameter;

    if (checkFlag)
        printf("%s\t\t: %s\n", KEY_SERIAL_DEVICE, qPrintable(rs485.serialDevice));

    /* Baud rate */
    rs485.baudRate = ini.value(key = KEY_BAUD_RATE, "").toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if (rs485.baudRate <= 0)
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t: %d\n", KEY_BAUD_RATE, rs485.baudRate);

    /* Data bits */
    rs485.dataBits = ini.value(key = KEY_DATA_BITS, VALUE_DATA_BITS).toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if (rs485.dataBits != 8)
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t: %d\n", KEY_DATA_BITS, rs485.dataBits);

    /* Parity */
    rs485.parity = ini.value(key = KEY_PARITY, VALUE_PARITY).toString().toUpper();

    if (rs485.parity.isEmpty())
        goto fail_invalid_parameter;

    if ((rs485.parity != "N")
            && (rs485.parity != "E")
            && (rs485.parity != "O"))
        goto fail_invalid_parameter;

    if (checkFlag)
        printf("%s\t\t\t: %s\n", KEY_PARITY, qPrintable(rs485.parity));

    /* Stop bits */
    rs485.stopBits = ini.value(key = KEY_STOP_BITS, VALUE_STOP_BITS).toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if ((rs485.stopBits != 1) && (rs485.stopBits != 2))
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t: %d\n", KEY_STOP_BITS, rs485.stopBits);

    /* Loop delay */
    rs485.loopDelay = ini.value(key = KEY_LOOP_DELAY, VALUE_LOOP_DELAY).toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if ((rs485.loopDelay < 0) || (rs485.loopDelay > 10000))
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t: %d\n", KEY_LOOP_DELAY, rs485.loopDelay);

    /* Protocol delay */
    rs485.protocolDelay = ini.value(key = KEY_PROTOCOL_DELAY, VALUE_PROTOCOL_DELAY).toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if ((rs485.protocolDelay < 0) || (rs485.protocolDelay > 100))
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t: %d\n", KEY_PROTOCOL_DELAY, rs485.protocolDelay);

    /* Is connected delay */
    rs485.isConnectedDelay = ini.value(key = KEY_IS_CONNECTED_DELAY, VALUE_IS_CONNECTED_DELAY).toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if ((rs485.isConnectedDelay < 1) || (rs485.isConnectedDelay > 100))
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t: %d\n", KEY_IS_CONNECTED_DELAY, rs485.isConnectedDelay);

    /* Connection errors */
    errors = ini.value(key = KEY_CONNECTION_ERROR_LIST, "").toStringList();

    foreach (QString error, errors) {
        if (error.isEmpty())
            continue;

        int e;

        if (error.toLower().startsWith("0x"))
            e = hex_to_int(error, &ok);
        else
            e = error.toInt(&ok);

        if (!ok)
            goto fail_invalid_parameter;

        rs485.criticalErrors.append(e);
    }

    if (checkFlag && !rs485.criticalErrors.isEmpty()) {
        printf("%s\t: ", KEY_CONNECTION_ERROR_LIST);
        for (int i = 0; i < rs485.criticalErrors.count(); i++) {
            if (i != rs485.criticalErrors.count() - 1)
                printf("%d, ", rs485.criticalErrors.at(i));
            else
                printf("%d\n", rs485.criticalErrors.at(i));
        }
    }

    /* Connection delay */
    rs485.connectionDelay = ini.value(key = KEY_CONNECTION_DELAY, VALUE_CONNECTION_DELAY).toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if ((rs485.connectionDelay < 0) || (rs485.connectionDelay > 10000))
        goto fail_out_of_range;

    if (checkFlag && !rs485.criticalErrors.isEmpty())
        printf("%s\t\t: %d\n", KEY_CONNECTION_DELAY, rs485.connectionDelay);

    ini.endGroup();
    return 0;
fail_invalid_parameter:
    fprintf(stderr, "[%s]:%s - parameter is wrong or missing!\n", GROUP_RS485, qPrintable(key));
    return -1;
fail_out_of_range:
    fprintf(stderr, "[%s]:%s - parameter is out of range!\n", GROUP_RS485, qPrintable(key));
    return -1;
}

int load_ctrl_group(QSettings &ini, control_config_t &control)
{
    bool ok;
    QString key, value;

    ini.beginGroup(GROUP_CONTROL);
    if (checkFlag)
        printf("\n[%s]\n", qPrintable(GROUP_CONTROL));

    /* Function code */
    value = ini.value(key = KEY_FUNCTION_CODE, QString("%1").arg(MODBUS_FUNC_WRITE_SINGLE_HOLDING_REGISTER)).toString();

    if (value.toLower().startsWith("0x"))
        control.functionCode = hex_to_int(value, &ok);
    else
        control.functionCode = value.toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if ((control.functionCode != MODBUS_FUNC_WRITE_SINGLE_HOLDING_REGISTER)
            && (control.functionCode != MODBUS_FUNC_WRITE_MULTIPLE_HOLDING_REGISTERS)
            && (control.functionCode != MODBUS_FUNC_WRITE_SINGLE_COIL)
            && (control.functionCode != MODBUS_FUNC_WRITE_MULTIPLE_COILS))
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t: 0x%02X (%d)\n", KEY_FUNCTION_CODE, control.functionCode, control.functionCode);

    /* LOADING PARAMETERS FOR FUNCTION CODES 0x06 AND 0x10 */
    if ((control.functionCode == MODBUS_FUNC_WRITE_SINGLE_HOLDING_REGISTER)
            || (control.functionCode == MODBUS_FUNC_WRITE_MULTIPLE_HOLDING_REGISTERS)) {

        /* Address */
        value = ini.value(key = KEY_ADDRESS, "").toString();

        if (value.toLower().startsWith("0x"))
            control.address = hex_to_int(value, &ok);
        else
            control.address = value.toInt(&ok);

        if (!ok)
            goto fail_invalid_parameter;

        if ((control.address < 0) || (control.address > 0xFFFF))
            goto fail_out_of_range;

        if (checkFlag)
            printf("%s\t\t\t: 0x%04X (%d)\n", KEY_ADDRESS, control.address, control.address);

        /* Run forward value */
        value = ini.value(key = KEY_RUN_FWD, "").toString();

        if (value.toLower().startsWith("0x"))
            control.runFwdValue = hex_to_int(value, &ok);
        else
            control.runFwdValue = value.toInt(&ok);

        if (!ok)
            goto fail_invalid_parameter;

        if ((control.runFwdValue < 0) || (control.runFwdValue > 0xFFFF))
            goto fail_out_of_range;

        if (checkFlag)
            printf("%s\t\t: 0x%04X (%d)\n", KEY_RUN_FWD, control.runFwdValue, control.runFwdValue);

        /* Run reverse value */
        value = ini.value(key = KEY_RUN_REV, "").toString();

        if (value.toLower().startsWith("0x"))
            control.runRevValue = hex_to_int(value, &ok);
        else
            control.runRevValue = value.toInt(&ok);

        if (!ok)
            goto fail_invalid_parameter;

        if ((control.runRevValue < 0) || (control.runRevValue > 0xFFFF))
            goto fail_out_of_range;

        if (checkFlag)
            printf("%s\t\t: 0x%04X (%d)\n", KEY_RUN_REV, control.runRevValue, control.runRevValue);

        /* Fault reset value */
        value = ini.value(key = KEY_FAULT_RESET, QString("%1").arg(INACTIVE_FLAG)).toString();

        if (value.toLower().startsWith("0x"))
            control.faultResetValue = hex_to_int(value, &ok);
        else
            control.faultResetValue = value.toInt(&ok);

        if (!ok)
            goto fail_invalid_parameter;

        if (((control.faultResetValue < 0) || (control.faultResetValue > 0xFFFF))
                && (control.faultResetValue != INACTIVE_FLAG))
            goto fail_out_of_range;

        if ((checkFlag) && (control.faultResetValue != INACTIVE_FLAG))
            printf("%s\t\t: 0x%04X (%d)\n", KEY_FAULT_RESET, control.faultResetValue, control.faultResetValue);

        /* Stop value */
        value = ini.value(key = KEY_STOP, "").toString();

        if (value.toLower().startsWith("0x"))
            control.stopValue = hex_to_int(value, &ok);
        else
            control.stopValue = value.toInt(&ok);

        if (!ok)
            goto fail_invalid_parameter;

        if ((control.stopValue < 0) || (control.stopValue > 0xFFFF))
            goto fail_out_of_range;

        if (checkFlag)
            printf("%s\t\t: 0x%04X (%d)\n", KEY_STOP, control.stopValue, control.stopValue);

    }

    /* LOADING PARAMETERS FOR FUNCTION CODES 0x05 AND 0x0F */
    if ((control.functionCode == MODBUS_FUNC_WRITE_SINGLE_COIL)
            || (control.functionCode == MODBUS_FUNC_WRITE_MULTIPLE_COILS)) {

        /* Run coil */
        value = ini.value(key = KEY_RUN_COIL, "").toString();

        if (value.toLower().startsWith("0x"))
            control.runCoil = hex_to_int(value, &ok);
        else
            control.runCoil = value.toInt(&ok);

        if (!ok)
            goto fail_invalid_parameter;

        if ((control.runCoil < 0) || (control.runCoil > 0xFFFF))
            goto fail_out_of_range;

        if (checkFlag)
            printf("%s\t\t\t: 0x%04X (%d)\n", KEY_RUN_COIL, control.runCoil, control.runCoil);

        /* Direction coil */
        value = ini.value(key = KEY_DIRECTION_COIL, "").toString();

        if (value.toLower().startsWith("0x"))
            control.directionCoil = hex_to_int(value, &ok);
        else
            control.directionCoil = value.toInt(&ok);

        if (!ok)
            goto fail_invalid_parameter;

        if ((control.directionCoil < 0) || (control.directionCoil > 0xFFFF))
            goto fail_out_of_range;

        if (checkFlag)
            printf("%s\t\t: 0x%04X (%d)\n", KEY_DIRECTION_COIL, control.directionCoil, control.directionCoil);

        /* Fault reset coil */
        value = ini.value(key = KEY_FAULT_RESET_COIL, QString("%1").arg(INACTIVE_FLAG)).toString();

        if (value.toLower().startsWith("0x"))
            control.faultResetCoil = hex_to_int(value, &ok);
        else
            control.faultResetCoil = value.toInt(&ok);

        if (!ok)
            goto fail_invalid_parameter;

        if (((control.faultResetCoil < 0) || (control.faultResetCoil > 0xFFFF))
                && (control.faultResetCoil != INACTIVE_FLAG))
            goto fail_out_of_range;

        if ((checkFlag) && (control.faultResetCoil != INACTIVE_FLAG))
            printf("%s\t\t: 0x%04X (%d)\n", KEY_FAULT_RESET_COIL, control.faultResetCoil, control.faultResetCoil);

    }

    ini.endGroup();
    return 0;
fail_invalid_parameter:
    fprintf(stderr, "[%s]:%s - parameter is wrong or missing!\n", GROUP_CONTROL, qPrintable(key));
    return -1;
fail_out_of_range:
    fprintf(stderr, "[%s]:%s - parameter is out of range!\n", GROUP_CONTROL, qPrintable(key));
    return -1;
}

int load_rpm_in_group(QSettings &ini, spindle_in_config_t &spindle)
{
    bool ok;
    QString key, value;

    ini.beginGroup(GROUP_SPINDLE_IN);
    if (checkFlag)
        printf("\n[%s]\n", qPrintable(GROUP_SPINDLE_IN));

    /* Function code */
    value = ini.value(key = KEY_FUNCTION_CODE, QString("%1").arg(MODBUS_FUNC_WRITE_SINGLE_HOLDING_REGISTER)).toString();

    if (value.toLower().startsWith("0x"))
        spindle.functionCode = hex_to_int(value, &ok);
    else
        spindle.functionCode = value.toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if ((spindle.functionCode != MODBUS_FUNC_WRITE_SINGLE_HOLDING_REGISTER)
            && (spindle.functionCode != MODBUS_FUNC_WRITE_MULTIPLE_HOLDING_REGISTERS))
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t: 0x%02X (%d)\n", KEY_FUNCTION_CODE, spindle.functionCode, spindle.functionCode);

    /* Address */
    value = ini.value(key = KEY_ADDRESS, "").toString();

    if (value.toLower().startsWith("0x"))
        spindle.address = hex_to_int(value, &ok);
    else
        spindle.address = value.toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if ((spindle.address < 0) || (spindle.address > 0xFFFF))
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t\t: 0x%04X (%d)\n", KEY_ADDRESS, spindle.address, spindle.address);

    /* Multiplier */
    spindle.multiplier = ini.value(key = KEY_MULTIPLIER, VALUE_MULTIPLIER).toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if (spindle.multiplier <= 0)
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t: %d\n", KEY_MULTIPLIER, spindle.multiplier);

    /* Divider */
    spindle.divider = ini.value(key = KEY_DIVIDER, VALUE_DIVIDER).toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if (spindle.divider <= 0)
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t\t: %d\n", KEY_DIVIDER, spindle.divider);

    ini.endGroup();
    return 0;
fail_invalid_parameter:
    fprintf(stderr, "[%s]:%s - parameter is wrong or missing!\n", GROUP_SPINDLE_IN, qPrintable(key));
    return -1;
fail_out_of_range:
    fprintf(stderr, "[%s]:%s - parameter is out of range!\n", GROUP_SPINDLE_IN, qPrintable(key));
    return -1;
}

int load_rpm_out_group(QSettings &ini, spindle_out_config_t &spindle)
{
    bool ok;
    QString key, value;

    ini.beginGroup(GROUP_SPINDLE_OUT);
    if (checkFlag)
        printf("\n[%s]\n", qPrintable(GROUP_SPINDLE_OUT));

    /* Address */
    value = ini.value(key = KEY_ADDRESS, "").toString();

    if (value.toLower().startsWith("0x"))
        spindle.address = hex_to_int(value, &ok);
    else
        spindle.address = value.toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if ((spindle.address < 0) || (spindle.address > 0xFFFF))
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t\t: 0x%04X (%d)\n", KEY_ADDRESS, spindle.address, spindle.address);

    /* Multiplier */
    spindle.multiplier = ini.value(key = KEY_MULTIPLIER, VALUE_MULTIPLIER).toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if (spindle.multiplier <= 0)
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t: %d\n", KEY_MULTIPLIER, spindle.multiplier);

    /* Divider */
    spindle.divider = ini.value(key = KEY_DIVIDER, VALUE_DIVIDER).toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if (spindle.divider <= 0)
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t\t: %d\n", KEY_DIVIDER, spindle.divider);

    ini.endGroup();
    return 0;
fail_invalid_parameter:
    fprintf(stderr, "[%s]:%s - parameter is wrong or missing!\n", GROUP_SPINDLE_OUT, qPrintable(key));
    return -1;
fail_out_of_range:
    fprintf(stderr, "[%s]:%s - parameter is out of range!\n", GROUP_SPINDLE_OUT, qPrintable(key));
    return -1;
}

int load_user_group(QSettings &ini, const QString &group, QVector<user_config_t> &uconfig)
{
    bool ok;
    QString key, value;

    ini.beginGroup(group);
    if (checkFlag)
        printf("\n[%s]\n", qPrintable(group));

    /* Saving group name */
    user_config_t usrcfg;
    usrcfg.groupName = group;

    /* Function code */
    value = ini.value(key = KEY_FUNCTION_CODE, QString("%1").arg(MODBUS_FUNC_READ_MULTIPLE_HOLDING_REGISTERS)).toString();

    if (value.toLower().startsWith("0x"))
        usrcfg.functionCode = hex_to_int(value, &ok);
    else
        usrcfg.functionCode = value.toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if ((usrcfg.functionCode != MODBUS_FUNC_READ_MULTIPLE_HOLDING_REGISTERS)
            && (usrcfg.functionCode != MODBUS_FUNC_READ_MULTIPLE_COILS))
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t: 0x%02X (%d)\n", KEY_FUNCTION_CODE, usrcfg.functionCode, usrcfg.functionCode);

    /* Address */
    value = ini.value(key = KEY_ADDRESS, "").toString();

    if (value.toLower().startsWith("0x"))
        usrcfg.address = hex_to_int(value, &ok);
    else
        usrcfg.address = value.toInt(&ok);

    if (!ok)
        goto fail_invalid_parameter;

    if ((usrcfg.address < 0) || (usrcfg.address > 0xFFFF))
        goto fail_out_of_range;

    if (checkFlag)
        printf("%s\t\t\t: 0x%04X (%d)\n", KEY_ADDRESS, usrcfg.address, usrcfg.address);

    // Function code 0x03
    if (usrcfg.functionCode == MODBUS_FUNC_READ_MULTIPLE_HOLDING_REGISTERS) {

        /* Pin type */
        value = ini.value(key = KEY_PIN_TYPE, "").toString().toLower();

        if (value.isEmpty())
            goto fail_invalid_parameter;

        if (value == "bit")
            usrcfg.pinType = HAL_BIT;
        else
            if (value == "float")
                usrcfg.pinType = HAL_FLOAT;
            else
                if (value == "s32")
                    usrcfg.pinType = HAL_S32;
                else
                    if (value == "u32")
                        usrcfg.pinType = HAL_U32;
                    else
                        goto fail_out_of_range;

        if (checkFlag)
            printf("%s\t\t\t: %s\n", KEY_PIN_TYPE, qPrintable(value));

        if (usrcfg.pinType != HAL_BIT) {

            /* Multiplier */
            usrcfg.multiplier = ini.value(key = KEY_MULTIPLIER, VALUE_MULTIPLIER).toInt(&ok);

            if (!ok)
                goto fail_invalid_parameter;

            if (usrcfg.multiplier <= 0)
                goto fail_out_of_range;

            if (checkFlag)
                printf("%s\t\t: %d\n", KEY_MULTIPLIER, usrcfg.multiplier);

            /* Divider */
            usrcfg.divider = ini.value(key = KEY_DIVIDER, VALUE_DIVIDER).toInt(&ok);

            if (!ok)
                goto fail_invalid_parameter;

            if (usrcfg.divider <= 0)
                goto fail_out_of_range;

            if (checkFlag)
                printf("%s\t\t\t: %d\n", KEY_DIVIDER, usrcfg.divider);

        } else {

            //BitMask
            value = ini.value(key = KEY_BIT_MASK, VALUE_BIT_MASK).toString();

            if (value.toLower().startsWith("0x"))
                usrcfg.bitMask = hex_to_int(value, &ok);
            else
                usrcfg.bitMask = value.toInt(&ok);

            if (!ok)
                goto fail_invalid_parameter;

            if ((usrcfg.bitMask < 0) || (usrcfg.bitMask > 0xFFFF))
                goto fail_out_of_range;

            if (checkFlag)
                printf("%s\t\t\t: 0x%04X (%d)\n", KEY_BIT_MASK, usrcfg.bitMask, usrcfg.bitMask);

        }

    }

    /* Pin name */
    usrcfg.pinName = ini.value(key = KEY_PIN_NAME, "").toString();

    if (usrcfg.pinName.isEmpty())
        goto fail_invalid_parameter;

    if (checkFlag)
        printf("%s\t\t\t: %s\n", KEY_PIN_NAME, qPrintable(usrcfg.pinName));

    /* Append user parameter */
    uconfig.append(usrcfg);

    ini.endGroup();
    return 0;
fail_invalid_parameter:
    fprintf(stderr, "[%s]:%s - parameter is wrong or missing!\n", qPrintable(group), qPrintable(key));
    return -1;
fail_out_of_range:
    fprintf(stderr, "[%s]:%s - parameter is out of range!\n", qPrintable(group), qPrintable(key));
    return -1;
}

/* Case insensitive remove method */
void remove(QStringList &groups, const QString &group)
{
    foreach(QString s, groups) {
        if (s.toLower() == group.toLower())
            groups.removeOne(s);
    }
}

int load_config(const QString &inifile, main_config_t &mconfig, QVector<user_config_t> &uconfig)
{
    if (!QFile::exists(inifile)) {
        fprintf(stderr, "File not found: %s\n", qPrintable(inifile));
        return -1;
    }

    QSettings ini(inifile, QSettings::IniFormat);

    /* Saving all group names to the list */
    QStringList groups = ini.childGroups();

    /* Removing common group from the list */
    if (!groups.contains(GROUP_COMMON, Qt::CaseInsensitive)) {
        fprintf(stderr, "Group not found: %s\n", GROUP_COMMON);
        return -1;
    } else
        remove(groups, GROUP_COMMON);

    /* Loading common settings */
    if (load_common_group(ini, mconfig.common) < 0)
        return -1;

    /* Removing rs485 group from the list */
    if (!groups.contains(GROUP_RS485, Qt::CaseInsensitive)) {
        fprintf(stderr, "Group not found: %s\n", GROUP_RS485);
        return -1;
    } else
        remove(groups, GROUP_RS485);

    /* Loading rs485 settings */
    if (load_rs485_group(ini, mconfig.rs485) < 0)
        return -1;

    /* Removing control group from the list */
    if (!groups.contains(GROUP_CONTROL, Qt::CaseInsensitive)) {
        fprintf(stderr, "Group not found: %s\n", GROUP_CONTROL);
        return -1;
    } else
        remove(groups, GROUP_CONTROL);

    /* Loading control settings */
    if (load_ctrl_group(ini, mconfig.control) < 0)
        return -1;

    /* Removing rpm-in group from the list */
    if (!groups.contains(GROUP_SPINDLE_IN, Qt::CaseInsensitive)) {
        fprintf(stderr, "Group not found: %s\n", GROUP_SPINDLE_IN);
        return -1;
    } else
        remove(groups, GROUP_SPINDLE_IN);

    /* Loading rpm-in settings */
    if (load_rpm_in_group(ini, mconfig.rpmIn) < 0)
        return -1;

    /* Removing rpm-out group from the list */
    if (!groups.contains(GROUP_SPINDLE_OUT, Qt::CaseInsensitive)) {
        fprintf(stderr, "Group not found: %s\n", GROUP_SPINDLE_OUT);
        return -1;
    } else
        remove(groups, GROUP_SPINDLE_OUT);

    /* Loading rpm-out settings */
    if (load_rpm_out_group(ini, mconfig.rpmOut) < 0)
        return -1;

    /* Now groups variable contains only user group names */
    foreach (QString group, groups) {
        /* Loading user settings */
        if (load_user_group(ini, group, uconfig) < 0)
            return -1;
    }

    if (checkFlag)
        printf("\n--- Config file is OK ---\n");

    return 0;
}
