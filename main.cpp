#include <getopt.h>
#include <modbus/modbus-rtu.h>
#include <QVector>
#include <QFileInfo>
#include "structures.h"

const char *copyright = "2020 (c) Alexander E. (Khabarovsk, Russia)\n";

const char *short_options = "cdhn";
const struct option long_options[] = {
    {"check",   no_argument, 0, 'c'},
    {"debug",   no_argument, 0, 'd'},
    {"help",    no_argument, 0, 'h'},
    {"new",     no_argument, 0, 'n'},
    {"version", no_argument, 0, 'v'},
    {0, 0, 0, 0}
};

typedef struct {
    hal_bit_t *is_connected;
    hal_s32_t *error_count;
    hal_s32_t *last_error;
} hal_data_t;

int load_config(const QString &fname, rs485_config_t &rs485_config, QVector<user_parameter_t> &parameters);
int write_blank_config(const QString &fname);

static void print_help(const QString &fname)
{
    printf("Usage:\n"
           "\t%s [keys] CONFIGFILE\n",
           qPrintable(fname));
    printf("Keys:\n"
           "\t-c, --check\tCheck config file for errors.\n"
           "\t-d, --debug\tEnable debug mode.\n"
           "\t-h, --help\tPrint this help.\n"
           "\t-n, --new\tWrite blank config file.\n"
           "\t--version\tPrint program's version.\n");
    printf("Description:\n"
           "\tBlah-blah-blah and blah-blah-blah...\n");
    printf("Examples:\n"
           "\t%s config.ini\n"
           "\t%s --new config.ini\n"
           "\t%s --check config.ini\n",
           qPrintable(fname),
           qPrintable(fname),
           qPrintable(fname));
}

static void delay(rs485_config_t &cfg)
{
    int startBit = 1;
    if (cfg.parity != "N")
        startBit++;
    long ns = 1000000 * cfg.protocolDelay * (startBit + cfg.dataBits + cfg.stopBits) / cfg.baudRate;
    struct timespec loop_timespec = {0, ns};
    nanosleep(&loop_timespec, NULL);
}

int main(int argc, char *argv[])
{
    int checkFlag = 0;
    int debugFlag = 0;
    int newFlag = 0;

    int hal_comp_id;
    hal_data_t *hdata;
    rs485_config_t rs485_config;
    QVector<user_parameter_t> parameters;
    QString iniFile;
    const QString fname = QFileInfo(argv[0]).fileName();

    int arg;
    int index;
    while ((arg = getopt_long(argc, argv, short_options, long_options, &index)) != -1) {
        switch (arg) {
        case 'c':
            checkFlag = 1;
            break;
        case 'd':
            debugFlag = 1;
            break;
        case 'h':
            print_help(fname);
            return 0;
        case 'n':
            newFlag = 1;
            break;
        case 'v':
            printf("%s %s\n", qPrintable(fname), APP_VERSION);
            printf(copyright);
            return 0;
        default:
            printf("Arguments are wrong! Type '%s -h' for help.\n", qPrintable(fname));
            return 0;
        }
    }

    if ((argc - optind) == 0) {
        printf("Critical argument is missing! Type '%s -h' for help.\n", qPrintable(fname));
        return 0;
    }

    if ((argc - optind) > 1) {
        printf("Too many arguments! Type '%s -h' for help.\n", qPrintable(fname));
        return 0;
    }

    iniFile = argv[optind];

    /* If --check flag specified */
    if (checkFlag) {
        return load_config(iniFile, rs485_config, parameters);
    }

    /* If --new flag specified */
    if (newFlag) {
        return write_blank_config(iniFile);
    }

    /* Finally, trying to load an existing config */
    int result = load_config(iniFile, rs485_config, parameters);
    if (result < 0)
        return result;

    /* HAL init */
    hal_comp_id = hal_init(qPrintable(fname));
    if (hal_comp_id < 0)
        return -1;

    /* HAL memory allocation for main parameters */
    hdata = (hal_data_t *)hal_malloc(sizeof(hal_data_t));
    if (!hdata)
        return -1;

    if (0 != hal_pin_bit_newf(HAL_OUT, &(hdata->is_connected), hal_comp_id, "%s.is-connected", qPrintable(fname)))
        return -1;
    if (0 != hal_pin_s32_newf(HAL_OUT, &(hdata->error_count), hal_comp_id, "%s.error-count", qPrintable(fname)))
        return -1;
    if (0 != hal_pin_s32_newf(HAL_OUT, &(hdata->last_error), hal_comp_id, "%s.last-error", qPrintable(fname)))
        return -1;

    hal_parameter_t **hparam;
    hparam = new hal_parameter_t*[parameters.count()];

    for (int i = 0; i < parameters.count(); ++i) {
        hparam[i] = (hal_parameter_t *)hal_malloc(sizeof(hal_parameter_t));
    }
    printf("Memory allocated...\n");

    for (int i = 0; i < parameters.count(); ++i) {
        int result = 0;
        switch (parameters.at(i).pinType) {
        case PIN_TYPE_FLOAT:
            result = hal_pin_float_newf(HAL_OUT, &(hparam[i]->floatPin), hal_comp_id,
                                        "%s.%s", qPrintable(fname), qPrintable(parameters.at(i).pinName));
            break;
        case PIN_TYPE_S32:
            result = hal_pin_s32_newf(HAL_OUT, &(hparam[i]->s32Pin), hal_comp_id,
                                        "%s.%s", qPrintable(fname), qPrintable(parameters.at(i).pinName));
            break;
        case PIN_TYPE_U32:
            result = hal_pin_u32_newf(HAL_OUT, &(hparam[i]->u32Pin), hal_comp_id,
                                        "%s.%s", qPrintable(fname), qPrintable(parameters.at(i).pinName));
            break;
        }
        if (result != 0)
            return -1;
    }
    printf("Pins created...\n");

    modbus_t *ctx;
    ctx = modbus_new_rtu(qPrintable(rs485_config.serialDevice),
                         rs485_config.baudRate,
                         rs485_config.parity.at(0).toAscii(),
                         rs485_config.dataBits,
                         rs485_config.stopBits);
    modbus_set_debug(ctx, debugFlag);
    modbus_connect(ctx);
    modbus_set_slave(ctx, rs485_config.slaveAddress);
    uint16_t dd;
    modbus_read_registers(ctx, 0x1001, 1, &dd);

    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
