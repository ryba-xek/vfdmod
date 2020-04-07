#include <stdio.h>
#include <QString>
#include <QVector>
#include "default-values.h"
#include "structures.h"

void make_blank_config()
{
    printf("[%s]\n", GROUP_COMMON);
    printf("%s=%s\n", KEY_MAX_SPEED_RPM, "????");
    printf("%s=%s\n", KEY_MIN_SPEED_RPM, "????");
    printf("%s=%s\n", KEY_AT_SPEED_THRESHOLD, VALUE_AT_SPEED_THRESHOLD);
    printf("\n");

    printf("[%s]\n", GROUP_RS485);
    printf("%s=%s\n", KEY_SLAVE_ADDRESS, VALUE_SLAVE_ADDRESS);
    printf("%s=%s\n", KEY_SERIAL_DEVICE, VALUE_SERIAL_DEVICE);
    printf("%s=%s\n", KEY_BAUD_RATE, VALUE_BAUD_RATE);
    printf("%s=%s\n", KEY_DATA_BITS, VALUE_DATA_BITS);
    printf("%s=%s\n", KEY_PARITY, VALUE_PARITY);
    printf("%s=%s\n", KEY_STOP_BITS, VALUE_STOP_BITS);
    printf("\n");

    printf("[%s]\n", GROUP_CONTROL);
    printf("%s=%s\n", KEY_ADDRESS, "0x????");
    printf("%s=%s\n", KEY_RUN_FWD, "0x????");
    printf("%s=%s\n", KEY_RUN_REV, "0x????");
    printf("%s=%s\n", KEY_STOP, "0x????");
    printf("\n");

    printf("[%s]\n", GROUP_SPINDLE_IN);
    printf("%s=%s\n", KEY_ADDRESS, "0x????");
    printf("%s=%s\n", KEY_MULTIPLIER, VALUE_MULTIPLIER);
    printf("%s=%s\n", KEY_DIVIDER, VALUE_DIVIDER);
    printf("\n");

    printf("[%s]\n", GROUP_SPINDLE_OUT);
    printf("%s=%s\n", KEY_ADDRESS, "0x????");
    printf("%s=%s\n", KEY_MULTIPLIER, VALUE_MULTIPLIER);
    printf("%s=%s\n", KEY_DIVIDER, VALUE_DIVIDER);
    printf("\n");

    printf("[%s]\n", "User parameter 1");
    printf("%s=%s\n", KEY_ADDRESS, "0x????");
    printf("%s=%s\n", KEY_MULTIPLIER, VALUE_MULTIPLIER);
    printf("%s=%s\n", KEY_DIVIDER, VALUE_DIVIDER);
    printf("%s=%s\n", KEY_PIN_TYPE, "float");
    printf("%s=%s\n", KEY_PIN_NAME, "user-float-parameter");
    printf("\n");

    printf("[%s]\n", "User parameter 2");
    printf("%s=%s\n", KEY_ADDRESS, "0x????");
    printf("%s=%s\n", KEY_MULTIPLIER, VALUE_MULTIPLIER);
    printf("%s=%s\n", KEY_DIVIDER, VALUE_DIVIDER);
    printf("%s=%s\n", KEY_PIN_TYPE, "s32");
    printf("%s=%s\n", KEY_PIN_NAME, "user-s32-parameter");
    printf("\n");

    printf("[%s]\n", "User parameter 3");
    printf("%s=%s\n", KEY_ADDRESS, "0x????");
    printf("%s=%s\n", KEY_MULTIPLIER, VALUE_MULTIPLIER);
    printf("%s=%s\n", KEY_DIVIDER, VALUE_DIVIDER);
    printf("%s=%s\n", KEY_PIN_TYPE, "u32");
    printf("%s=%s\n", KEY_PIN_NAME, "user-u32-parameter");
    printf("\n");
}

void make_postgui_config(const main_config_t &mconfig, const QVector<user_config_t> &uconfig)
{
    printf("# Spindle output speed\n"
           "net spindle-rpm-out %s.%s.%s => %s.%s\n",
           qPrintable(mconfig.common.componentName),
           HAL_GROUP_SPINDLE,
           HAL_PIN_RPM_OUT,
           PYVCP,
           HAL_PIN_RPM_OUT);

    printf("net spindle-at-speed <= %s.%s\n",
           PYVCP,
           HAL_PIN_AT_SPEED);

    printf("\n"
           "# Communication\n"
           "net %s-%s %s.%s.%s => %s.%s\n",
           PYVCP,
           HAL_PIN_IS_CONNECTED,
           qPrintable(mconfig.common.componentName),
           HAL_GROUP_RS485,
           HAL_PIN_IS_CONNECTED,
           PYVCP,
           HAL_PIN_IS_CONNECTED);

    printf("net %s-%s %s.%s.%s => %s.%s\n",
           PYVCP,
           HAL_PIN_ERROR_COUNT,
           qPrintable(mconfig.common.componentName),
           HAL_GROUP_RS485,
           HAL_PIN_ERROR_COUNT,
           PYVCP,
           HAL_PIN_ERROR_COUNT);

    printf("net %s-%s %s.%s.%s => %s.%s\n"
           "\n"
           "# User parameters\n",
           PYVCP,
           HAL_PIN_LAST_ERROR,
           qPrintable(mconfig.common.componentName),
           HAL_GROUP_RS485,
           HAL_PIN_LAST_ERROR,
           PYVCP,
           HAL_PIN_LAST_ERROR);

    for (int i = 0; i < uconfig.size(); i++) {
        printf("net %s-%s %s.%s.%s => %s.%s\n",
               PYVCP,
               qPrintable(uconfig.at(i).pinName),
               qPrintable(mconfig.common.componentName),
               HAL_GROUP_USER_PARAMETERS,
               qPrintable(uconfig.at(i).pinName),
               PYVCP,
               qPrintable(uconfig.at(i).pinName));
    }
}

void make_pyvcp_config(const QString &inifile, const main_config_t &mconfig, const QVector<user_config_t> &uconfig)
{
    printf("<pyvcp>\n"
           "<labelframe text=\"%s\">\n"
           "\n", qPrintable(inifile));

    printf("    <label text=\"Output RPM:\"/>\n"
           "    <bar halpin=\"%s\" max_=\"%d\"/>\n"
           "\n", HAL_PIN_RPM_OUT, mconfig.common.maxSpeedRpm);

    printf("    <table flexible_rows=\"[1]\" flexible_columns=\"[2]\">\n"
           "    <tablerow/>\n"
           "        <label text=\"Spindle at speed\"/>\n"
           "        <led halpin=\"%s\" size=\"12\" on_color=\"green\" off_color=\"red\"/>\n"
           "    <tablerow/>\n", HAL_PIN_AT_SPEED);

    printf("        <label text=\"'\\nParameters:'\"/>\n"
           "\n"
           "    <!-- User parameters start here -->\n");

    for (int i = 0; i < uconfig.size(); i++) {
        printf("    <tablerow/>\n"
               "        <label text=\"%s\"/>\n", qPrintable(uconfig.at(i).groupName));
        if (uconfig.at(i).pinType == HAL_FLOAT)
            printf("        <number halpin=\"%s\" format=\".1f\"/>\n", qPrintable(uconfig.at(i).pinName));
        else
            printf("        <number halpin=\"%s\"/>\n", qPrintable(uconfig.at(i).pinName));
    }

    printf("    <!-- User parameters end -->\n"
           "\n"
           "    <tablerow/>\n"
           "        <label text=\"'\\nRS485:'\"/>\n");

    printf("    <tablerow/>\n"
           "        <label text=\"Is connected\"/>\n"
           "        <led halpin=\"%s\" size=\"12\" on_color=\"green\" off_color=\"red\"/>\n",
           HAL_PIN_IS_CONNECTED);

    printf("    <tablerow/>\n"
           "        <label text=\"Error count\"/>\n"
           "        <s32 halpin=\"%s\"/>\n",
           HAL_PIN_ERROR_COUNT);

    printf("    <tablerow/>\n"
           "        <label text=\"Last error\"/>\n"
           "        <s32 halpin=\"%s\"/>\n",
           HAL_PIN_LAST_ERROR);

    printf("    </table>\n"
           "\n"
           "</labelframe>\n"
           "</pyvcp>\n");
}
