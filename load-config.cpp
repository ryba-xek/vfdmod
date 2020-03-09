#include <QFile>
#include <QSettings>
#include <QStringList>
#include "default-values.h"
#include "structures.h"
#include <QDebug>

int hex_to_int(QString &s, bool *ok)
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

int load_rs485_group(QSettings &ini, rs485_config_t &rs485_config)
{
    bool ok;
    QString group, key;

    ini.beginGroup(group);
    printf("[%s]\n", qPrintable(group = GROUP_RS485));

    rs485_config.slaveAddress = ini.value(key = KEY_SLAVE_ADDRESS, VALUE_SLAVE_ADDRESS).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t= %d\n", KEY_SLAVE_ADDRESS, rs485_config.slaveAddress);

    rs485_config.serialDevice = ini.value(key = KEY_SERIAL_DEVICE, VALUE_SERIAL_DEVICE).toString();
    if (rs485_config.serialDevice.isEmpty())
        goto fail;
    else
        printf("%s\t= %s\n", KEY_SERIAL_DEVICE, qPrintable(rs485_config.serialDevice));

    rs485_config.baudRate = ini.value(key = KEY_BAUD_RATE, VALUE_BAUD_RATE).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t= %d\n", KEY_BAUD_RATE, rs485_config.baudRate);

    rs485_config.dataBits = ini.value(key = KEY_DATA_BITS, VALUE_DATA_BITS).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t= %d\n", KEY_DATA_BITS, rs485_config.dataBits);

    rs485_config.parity = ini.value(key = KEY_PARITY, VALUE_PARITY).toString();
    if (rs485_config.parity.isEmpty())
        goto fail;
    if ((rs485_config.parity != "N")
            && (rs485_config.parity != "E")
            && (rs485_config.parity != "O"))
        goto fail;
    else
        printf("%s\t\t= %s\n", KEY_PARITY, qPrintable(rs485_config.parity));

    rs485_config.stopBits = ini.value(key = KEY_STOP_BITS, VALUE_STOP_BITS).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t= %d\n", KEY_STOP_BITS, rs485_config.stopBits);

    rs485_config.loopDelayMs = ini.value(key = KEY_LOOP_DELAY_MS, VALUE_LOOP_DELAY_MS).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t= %d\n", KEY_LOOP_DELAY_MS, rs485_config.loopDelayMs);

    rs485_config.protocolDelay = ini.value(key = KEY_PROTOCOL_DELAY, VALUE_PROTOCOL_DELAY).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t= %d\n", KEY_PROTOCOL_DELAY, rs485_config.protocolDelay);

    ini.endGroup();
    return 0;
fail:
    printf("Invalid parameter in '%s/%s'!\n", qPrintable(group), qPrintable(key));
    return -1;
}

int load_user_group(QSettings &ini, const QString &group, QVector<user_parameter_t> &parameters)
{
    bool ok;
    QString key, value;
    ini.beginGroup(group);
    printf("[%s]\n", qPrintable(group));

    user_parameter_t uparam;

    value = ini.value(key = KEY_ADDRESS, "").toString();
    if (value.toLower().startsWith("0x"))
        uparam.address = hex_to_int(value, &ok);
    else
        uparam.address = value.toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t\t= %d\n", KEY_ADDRESS, uparam.address);

    uparam.multiplier = ini.value(key = KEY_MULTIPLIER, "1").toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t= %d\n", KEY_MULTIPLIER, uparam.multiplier);

    uparam.divider = ini.value(key = KEY_DIVIDER, "1").toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t\t= %d\n", KEY_DIVIDER, uparam.divider);

    value = ini.value(key = KEY_PIN_TYPE, "").toString().toLower();
    if (value == "float")
        uparam.pinType = PIN_TYPE_FLOAT;
    if (value == "s32")
        uparam.pinType = PIN_TYPE_S32;
    if (value == "u32")
        uparam.pinType = PIN_TYPE_U32;
    if ((value != "float")
            && (value != "s32")
            && (value != "u32"))
        goto fail;
    else
        printf("%s\t\t= %s\n", KEY_PIN_TYPE, qPrintable(value));

    uparam.pinName = ini.value(key = KEY_PIN_NAME, "").toString();
    if (uparam.pinName.isEmpty())
        goto fail;
    else
        printf("%s\t\t= %s\n", KEY_PIN_NAME, qPrintable(uparam.pinName));

    ini.endGroup();
    parameters.append(uparam);
    return 0;
fail:
    printf("Invalid parameter in '%s/%s'!\n", qPrintable(group), qPrintable(key));
    return -1;
}

int load_config(const QString &fname, rs485_config_t &rs485_config, QVector<user_parameter_t> &parameters)
{
    if (!QFile::exists(fname)) {
        printf("File not found: %s\n", qPrintable(fname));
        return -1;
    }

    QSettings ini(fname, QSettings::IniFormat);

    QStringList groups = ini.childGroups();

    /* Loading connection settings */
    if (!groups.contains(GROUP_RS485)) {
        printf("Group not found: %s\n", GROUP_RS485);
        return -1;
    } else
        groups.removeOne(GROUP_RS485);

    if (load_rs485_group(ini, rs485_config) < 0)
        return -1;

    foreach (QString group, groups) {
        if (load_user_group(ini, group, parameters) < 0)
            return -1;
    }

    return 0;
}
