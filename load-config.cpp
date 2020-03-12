#include <QFile>
#include <QSettings>
#include <QStringList>
#include "default-values.h"
#include "structures.h"
#include <QDebug>

/*
 * QString::toInt(bool *ok, int base = 10) not available in Qt4,
 * so I will use my own converter from hex string to integer.
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

int load_rs485_group(QSettings &ini, rs485_config_t &rs485)
{
    bool ok;
    QString group, key;

    printf("\n[%s]\n", qPrintable(group = GROUP_RS485));
    ini.beginGroup(group);

    rs485.slaveAddress = ini.value(key = KEY_SLAVE_ADDRESS, VALUE_SLAVE_ADDRESS).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t: %d\n", KEY_SLAVE_ADDRESS, rs485.slaveAddress);

    rs485.serialDevice = ini.value(key = KEY_SERIAL_DEVICE, VALUE_SERIAL_DEVICE).toString();
    if (rs485.serialDevice.isEmpty())
        goto fail;
    else
        printf("%s\t: %s\n", KEY_SERIAL_DEVICE, qPrintable(rs485.serialDevice));

    rs485.baudRate = ini.value(key = KEY_BAUD_RATE, VALUE_BAUD_RATE).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t: %d\n", KEY_BAUD_RATE, rs485.baudRate);

    rs485.dataBits = ini.value(key = KEY_DATA_BITS, VALUE_DATA_BITS).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t: %d\n", KEY_DATA_BITS, rs485.dataBits);

    rs485.parity = ini.value(key = KEY_PARITY, VALUE_PARITY).toString();
    if (rs485.parity.isEmpty())
        goto fail;
    if ((rs485.parity != "N")
            && (rs485.parity != "E")
            && (rs485.parity != "O"))
        goto fail;
    else
        printf("%s\t\t: %s\n", KEY_PARITY, qPrintable(rs485.parity));

    rs485.stopBits = ini.value(key = KEY_STOP_BITS, VALUE_STOP_BITS).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t: %d\n", KEY_STOP_BITS, rs485.stopBits);

    rs485.loopDelayMs = ini.value(key = KEY_LOOP_DELAY_MS, VALUE_LOOP_DELAY_MS).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t: %d\n", KEY_LOOP_DELAY_MS, rs485.loopDelayMs);

    rs485.protocolDelay = ini.value(key = KEY_PROTOCOL_DELAY, VALUE_PROTOCOL_DELAY).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t: %d\n", KEY_PROTOCOL_DELAY, rs485.protocolDelay);

    ini.endGroup();
    return 0;
fail:
    printf("Invalid parameter in '%s/%s'!\n", qPrintable(group), qPrintable(key));
    return -1;
}

int load_ctrl_group(QSettings &ini, ctrl_config_t &ctrl)
{
    bool ok;
    QString group, key, value;

    printf("\n[%s]\n", qPrintable(group = GROUP_CONTROL));
    ini.beginGroup(group);

    value = ini.value(key = KEY_ADDRESS, "").toString();
    if (value.toLower().startsWith("0x"))
        ctrl.address = hex_to_int(value, &ok);
    else
        ctrl.address = value.toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t\t: %s (%d)\n",
               KEY_ADDRESS,
               qPrintable(QString("0x%1").arg(ctrl.address, 4, 16, QChar('0'))),
               ctrl.address);

    value = ini.value(key = KEY_RUN_FWD, "").toString();
    if (value.toLower().startsWith("0x"))
        ctrl.runFwdValue = hex_to_int(value, &ok);
    else
        ctrl.runFwdValue = value.toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t: %s (%d)\n",
               KEY_RUN_FWD,
               qPrintable(QString("0x%1").arg(ctrl.runFwdValue, 4, 16, QChar('0'))),
               ctrl.runFwdValue);

    value = ini.value(key = KEY_RUN_REV, "").toString();
    if (value.toLower().startsWith("0x"))
        ctrl.runRevValue = hex_to_int(value, &ok);
    else
        ctrl.runRevValue = value.toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t: %s (%d)\n",
               KEY_RUN_REV,
               qPrintable(QString("0x%1").arg(ctrl.runRevValue, 4, 16, QChar('0'))),
               ctrl.runRevValue);

    value = ini.value(key = KEY_STOP, "").toString();
    if (value.toLower().startsWith("0x"))
        ctrl.stopValue = hex_to_int(value, &ok);
    else
        ctrl.stopValue = value.toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t: %s (%d)\n",
               KEY_STOP,
               qPrintable(QString("0x%1").arg(ctrl.stopValue, 4, 16, QChar('0'))),
               ctrl.stopValue);

    ini.endGroup();
    return 0;
fail:
    printf("Invalid parameter in '%s/%s'!\n", qPrintable(group), qPrintable(key));
    return -1;
}

int load_rpm_in_group(QSettings &ini, spindle_in_config_t &spindle)
{
    bool ok;
    QString group, key, value;

    printf("\n[%s]\n", qPrintable(group = GROUP_SPINDLE_IN));
    ini.beginGroup(group);

    value = ini.value(key = KEY_ADDRESS, "").toString();
    if (value.toLower().startsWith("0x"))
        spindle.address = hex_to_int(value, &ok);
    else
        spindle.address = value.toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t\t: %s (%d)\n",
               KEY_ADDRESS,
               qPrintable(QString("0x%1").arg(spindle.address, 4, 16, QChar('0'))),
               spindle.address);

    spindle.multiplier = ini.value(key = KEY_MULTIPLIER, VALUE_MULTIPLIER).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t: %d\n", KEY_MULTIPLIER, spindle.multiplier);

    spindle.divider = ini.value(key = KEY_DIVIDER, VALUE_DIVIDER).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t\t: %d\n", KEY_DIVIDER, spindle.divider);

    ini.endGroup();
    return 0;
fail:
    printf("Invalid parameter in '%s/%s'!\n", qPrintable(group), qPrintable(key));
    return -1;
}

int load_rpm_out_group(QSettings &ini, spindle_out_config_t &spindle)
{
    bool ok;
    QString group, key, value;

    printf("\n[%s]\n", qPrintable(group = GROUP_SPINDLE_OUT));
    ini.beginGroup(group);

    value = ini.value(key = KEY_ADDRESS, "").toString();
    if (value.toLower().startsWith("0x"))
        spindle.address = hex_to_int(value, &ok);
    else
        spindle.address = value.toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t\t: %s (%d)\n",
               KEY_ADDRESS,
               qPrintable(QString("0x%1").arg(spindle.address, 4, 16, QChar('0'))),
               spindle.address);

    spindle.multiplier = ini.value(key = KEY_MULTIPLIER, VALUE_MULTIPLIER).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t: %d\n", KEY_MULTIPLIER, spindle.multiplier);

    spindle.divider = ini.value(key = KEY_DIVIDER, VALUE_DIVIDER).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t\t: %d\n", KEY_DIVIDER, spindle.divider);

    spindle.atSpeedAccuracy = ini.value(key = KEY_AT_SPEED_ACCURACY, (int)1).toInt(&ok);
    if (!ok)
        goto fail;
    if ((spindle.atSpeedAccuracy < 0) || (spindle.atSpeedAccuracy > 100))
        goto fail;
    else
        printf("%s\t: %d\n", KEY_AT_SPEED_ACCURACY, spindle.atSpeedAccuracy);

    ini.endGroup();
    return 0;
fail:
    printf("Invalid parameter in '%s/%s'!\n", qPrintable(group), qPrintable(key));
    return -1;
}

int load_user_group(QSettings &ini, const QString &group, QVector<user_config_t> &uconfig)
{
    bool ok;
    QString key, value;
    ini.beginGroup(group);
    printf("\n[%s]\n", qPrintable(group));

    user_config_t usrcfg;

    value = ini.value(key = KEY_ADDRESS, "").toString();
    if (value.toLower().startsWith("0x"))
        usrcfg.address = hex_to_int(value, &ok);
    else
        usrcfg.address = value.toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t\t: %s (%d)\n",
               KEY_ADDRESS,
               qPrintable(QString("0x%1").arg(usrcfg.address, 4, 16, QChar('0'))),
               usrcfg.address);

    usrcfg.multiplier = ini.value(key = KEY_MULTIPLIER, VALUE_MULTIPLIER).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t: %d\n", KEY_MULTIPLIER, usrcfg.multiplier);

    usrcfg.divider = ini.value(key = KEY_DIVIDER, VALUE_DIVIDER).toInt(&ok);
    if (!ok)
        goto fail;
    if (usrcfg.divider == 0)
        goto fail;
    else
        printf("%s\t\t: %d\n", KEY_DIVIDER, usrcfg.divider);

    value = ini.value(key = KEY_PIN_TYPE, "").toString().toLower();
    if (value == "float")
        usrcfg.pinType = PIN_TYPE_FLOAT;
    if (value == "s32")
        usrcfg.pinType = PIN_TYPE_S32;
    if (value == "u32")
        usrcfg.pinType = PIN_TYPE_U32;
    if ((value != "float")
            && (value != "s32")
            && (value != "u32"))
        goto fail;
    else
        printf("%s\t\t: %s\n", KEY_PIN_TYPE, qPrintable(value));

    usrcfg.pinName = ini.value(key = KEY_PIN_NAME, "").toString();
    if (usrcfg.pinName.isEmpty())
        goto fail;
    else
        printf("%s\t\t: %s\n", KEY_PIN_NAME, qPrintable(usrcfg.pinName));

    ini.endGroup();
    uconfig.append(usrcfg);
    return 0;
fail:
    printf("Invalid parameter in '%s/%s'!\n", qPrintable(group), qPrintable(key));
    return -1;
}

int load_config(const QString &fname, main_config_t &mconfig, QVector<user_config_t> &uconfig)
{
    if (!QFile::exists(fname)) {
        printf("File not found: %s\n", qPrintable(fname));
        return -1;
    }

    QSettings ini(fname, QSettings::IniFormat);

    /* Saving all group names to the list */
    QStringList groups = ini.childGroups();

    /* Loading main settings */
    if (groups.contains(GROUP_MAIN)) {
        ini.beginGroup(GROUP_MAIN);
        printf("[%s]\n", qPrintable(GROUP_MAIN));
        mconfig.componentName = ini.value(KEY_COMPONENT_NAME, "vfdmod").toString();
        if (mconfig.componentName.isEmpty())
            mconfig.componentName = "vfdmod";
        printf("%s\t: %s\n", KEY_COMPONENT_NAME, qPrintable(mconfig.componentName));
        ini.endGroup();
        groups.removeOne(GROUP_MAIN);
    }

    /* Loading rs485 settings */
    if (!groups.contains(GROUP_RS485)) {
        printf("Group not found: %s\n", GROUP_RS485);
        return -1;
    } else
        groups.removeOne(GROUP_RS485);

    if (load_rs485_group(ini, mconfig.rs485) < 0)
        return -1;

    /* Loading control settings */
    if (!groups.contains(GROUP_CONTROL)) {
        printf("Group not found: %s\n", GROUP_CONTROL);
        return -1;
    } else
        groups.removeOne(GROUP_CONTROL);

    if (load_ctrl_group(ini, mconfig.control) < 0)
        return -1;

    /* Loading rpm-in settings */
    if (!groups.contains(GROUP_SPINDLE_IN)) {
        printf("Group not found: %s\n", GROUP_SPINDLE_IN);
        return -1;
    } else
        groups.removeOne(GROUP_SPINDLE_IN);

    if (load_rpm_in_group(ini, mconfig.rpmIn) < 0)
        return -1;

    /* Loading rpm-out settings */
    if (!groups.contains(GROUP_SPINDLE_OUT)) {
        printf("Group not found: %s\n", GROUP_SPINDLE_OUT);
        return -1;
    } else
        groups.removeOne(GROUP_SPINDLE_OUT);

    if (load_rpm_out_group(ini, mconfig.rpmOut) < 0)
        return -1;

    /* Loading user settings */
    foreach (QString group, groups) {
        if (load_user_group(ini, group, uconfig) < 0)
            return -1;
    }

    return 0;
}
