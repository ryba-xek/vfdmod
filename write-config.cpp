#include <QFile>
#include <QSettings>
#include "default-values.h"

int write_blank_config(const QString &fname)
{
    if (QFile::exists(fname)) {
        printf("File already exists: %s\n", qPrintable(fname));
        return -1;
    }

    QSettings settings(fname, QSettings::IniFormat);

    settings.beginGroup(GROUP_RS485);
    settings.setValue(KEY_SLAVE_ADDRESS,  VALUE_SLAVE_ADDRESS);
    settings.setValue(KEY_SERIAL_DEVICE,  VALUE_SERIAL_DEVICE);
    settings.setValue(KEY_BAUD_RATE,      VALUE_BAUD_RATE);
    settings.setValue(KEY_DATA_BITS,      VALUE_DATA_BITS);
    settings.setValue(KEY_PARITY,         VALUE_PARITY);
    settings.setValue(KEY_STOP_BITS,      VALUE_STOP_BITS);
    settings.setValue(KEY_LOOP_DELAY_MS,  VALUE_LOOP_DELAY_MS);
    settings.setValue(KEY_PROTOCOL_DELAY, VALUE_PROTOCOL_DELAY);
    settings.endGroup();

    settings.beginGroup(GROUP_SPINDLE_LIMITS);
    settings.setValue(KEY_MAX_SPEED_RPM,  VALUE_MAX_SPEED_RPM);
    settings.setValue(KEY_MIN_SPEED_RPM,  VALUE_MIN_SPEED_RPM);
    settings.endGroup();

    return 0;
}
