#include <QFile>
#include <QSettings>
#include "default-values.h"

int write_blank_config(const QString &fname)
{
    if (QFile::exists(fname)) {
        printf("File already exists: %s\n", qPrintable(fname));
        return -1;
    }

    QSettings ini(fname, QSettings::IniFormat);

    ini.beginGroup(GROUP_RS485);
    ini.setValue(KEY_SLAVE_ADDRESS,  VALUE_SLAVE_ADDRESS);
    ini.setValue(KEY_SERIAL_DEVICE,  VALUE_SERIAL_DEVICE);
    ini.setValue(KEY_BAUD_RATE,      VALUE_BAUD_RATE);
    ini.setValue(KEY_DATA_BITS,      VALUE_DATA_BITS);
    ini.setValue(KEY_PARITY,         VALUE_PARITY);
    ini.setValue(KEY_STOP_BITS,      VALUE_STOP_BITS);
    ini.setValue(KEY_LOOP_DELAY_MS,  VALUE_LOOP_DELAY_MS);
    ini.setValue(KEY_PROTOCOL_DELAY, VALUE_PROTOCOL_DELAY);
    ini.endGroup();

    ini.beginGroup(GROUP_SPINDLE_LIMITS);
    ini.setValue(KEY_MAX_SPEED_RPM,  VALUE_MAX_SPEED_RPM);
    ini.setValue(KEY_MIN_SPEED_RPM,  VALUE_MIN_SPEED_RPM);
    ini.endGroup();

    return 0;
}
