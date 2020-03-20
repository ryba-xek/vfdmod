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

    ini.beginGroup(GROUP_COMMON);
    ini.setValue(KEY_MAX_SPEED_RPM,     QString("?"));
    ini.setValue(KEY_MIN_SPEED_RPM,     QString("?"));
    ini.setValue(KEY_AT_SPEED_THRESHOLD,VALUE_AT_SPEED_THRESHOLD);
    ini.endGroup();

    ini.beginGroup(GROUP_RS485);
    ini.setValue(KEY_SLAVE_ADDRESS,     VALUE_SLAVE_ADDRESS);
    ini.setValue(KEY_SERIAL_DEVICE,     VALUE_SERIAL_DEVICE);
    ini.setValue(KEY_BAUD_RATE,         VALUE_BAUD_RATE);
    ini.setValue(KEY_DATA_BITS,         VALUE_DATA_BITS);
    ini.setValue(KEY_PARITY,            VALUE_PARITY);
    ini.setValue(KEY_STOP_BITS,         VALUE_STOP_BITS);
    ini.setValue(KEY_LOOP_DELAY,        VALUE_LOOP_DELAY);
    ini.setValue(KEY_PROTOCOL_DELAY,    VALUE_PROTOCOL_DELAY);
    ini.setValue(KEY_IS_CONNECTED_DELAY,VALUE_IS_CONNECTED_DELAY);
    ini.endGroup();

    ini.beginGroup(GROUP_CONTROL);
    ini.setValue(KEY_ADDRESS,           QString("0x????"));
    ini.setValue(KEY_RUN_FWD,           QString("0x????"));
    ini.setValue(KEY_RUN_REV,           QString("0x????"));
    ini.setValue(KEY_STOP,              QString("0x????"));
    ini.endGroup();

    ini.beginGroup(GROUP_SPINDLE_IN);
    ini.setValue(KEY_ADDRESS,           QString("0x????"));
    ini.setValue(KEY_MULTIPLIER,        VALUE_MULTIPLIER);
    ini.setValue(KEY_DIVIDER,           VALUE_DIVIDER);
    ini.endGroup();

    ini.beginGroup(GROUP_SPINDLE_OUT);
    ini.setValue(KEY_ADDRESS,           QString("0x????"));
    ini.setValue(KEY_MULTIPLIER,        VALUE_MULTIPLIER);
    ini.setValue(KEY_DIVIDER,           VALUE_DIVIDER);
    ini.endGroup();

    ini.beginGroup("1");
    ini.setValue(KEY_ADDRESS,           QString("0x????"));
    ini.setValue(KEY_MULTIPLIER,        VALUE_MULTIPLIER);
    ini.setValue(KEY_DIVIDER,           VALUE_DIVIDER);
    ini.setValue(KEY_PIN_TYPE,          QString("float"));
    ini.setValue(KEY_PIN_NAME,          QString("user-float-parameter"));
    ini.endGroup();

    ini.beginGroup("2");
    ini.setValue(KEY_ADDRESS,           QString("0x????"));
    ini.setValue(KEY_MULTIPLIER,        VALUE_MULTIPLIER);
    ini.setValue(KEY_DIVIDER,           VALUE_DIVIDER);
    ini.setValue(KEY_PIN_TYPE,          QString("s32"));
    ini.setValue(KEY_PIN_NAME,          QString("user-s32-parameter"));
    ini.endGroup();

    ini.beginGroup("3");
    ini.setValue(KEY_ADDRESS,           QString("0x????"));
    ini.setValue(KEY_MULTIPLIER,        VALUE_MULTIPLIER);
    ini.setValue(KEY_DIVIDER,           VALUE_DIVIDER);
    ini.setValue(KEY_PIN_TYPE,          QString("u32"));
    ini.setValue(KEY_PIN_NAME,          QString("user-u32-parameter"));
    ini.endGroup();

    return 0;
}
