#include <QFile>
#include <QSettings>
#include "default-values.h"
#include "structures.h"

int load_config(const QString &fname, connection_t &connection)
{
    if (!QFile::exists(fname)) {
        printf("File not found: %s\n", qPrintable(fname));
        return -1;
    }

    bool ok;
    QString group, key;
    QSettings settings(fname, QSettings::IniFormat);

    printf("[%s]\n", qPrintable(group = GROUP_RS485));
    settings.beginGroup(group);

    connection.slaveAddress = settings.value(key = KEY_SLAVE_ADDRESS, VALUE_SLAVE_ADDRESS).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t= %d\n", KEY_SLAVE_ADDRESS, connection.slaveAddress);

    connection.serialDevice = settings.value(key = KEY_SERIAL_DEVICE, VALUE_SERIAL_DEVICE).toString();
    if (connection.serialDevice.isEmpty())
        goto fail;
    else
        printf("%s\t= %s\n", KEY_SERIAL_DEVICE, qPrintable(connection.serialDevice));

    connection.baudRate = settings.value(key = KEY_BAUD_RATE, VALUE_BAUD_RATE).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t= %d\n", KEY_BAUD_RATE, connection.baudRate);

    connection.dataBits = settings.value(key = KEY_DATA_BITS, VALUE_DATA_BITS).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t= %d\n", KEY_DATA_BITS, connection.dataBits);

    connection.parity = settings.value(key = KEY_PARITY, VALUE_PARITY).toString();
    if (connection.parity.isEmpty())
        goto fail;
    if ((connection.parity != "N")
            && (connection.parity != "E")
            && (connection.parity != "O"))
        goto fail;
    else
        printf("%s\t\t= %s\n", KEY_PARITY, qPrintable(connection.parity));

    connection.stopBits = settings.value(key = KEY_STOP_BITS, VALUE_STOP_BITS).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t= %d\n", KEY_STOP_BITS, connection.stopBits);

    connection.loopDelayMs = settings.value(key = KEY_LOOP_DELAY_MS, VALUE_LOOP_DELAY_MS).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t= %d\n", KEY_LOOP_DELAY_MS, connection.loopDelayMs);

    connection.protocolDelay = settings.value(key = KEY_PROTOCOL_DELAY, VALUE_PROTOCOL_DELAY).toInt(&ok);
    if (!ok)
        goto fail;
    else
        printf("%s\t= %d\n", KEY_PROTOCOL_DELAY, connection.protocolDelay);

    return 0;
fail:
    printf("Invalid parameter in '%s/%s'!\n", qPrintable(group), qPrintable(key));
    return -1;
}
