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

int load_config(const QString &fname, main_config_t &mconfig, QVector<user_config_t> &uconfig);
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

    main_config_t mconfig;
    QVector<user_config_t> uconfig;
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
        return load_config(iniFile, mconfig, uconfig);
    }

    /* If --new flag specified */
    if (newFlag) {
        return write_blank_config(iniFile);
    }

    /* Finally, trying to load an existing config */
    int result = load_config(iniFile, mconfig, uconfig);
    if (result < 0)
        return result;

    /* HAL init */
    int hal_comp_id = hal_init(qPrintable(mconfig.componentName));
    if (hal_comp_id < 0)
        return -1;

    /* HAL memory allocation for main parameters */
    hal_main_data_t *hal_mdata = NULL;
    hal_mdata = (hal_main_data_t *)hal_malloc(sizeof(hal_main_data_t));
    if (!hal_mdata) {
        hal_exit(hal_comp_id);
        return -1;
    }

    /* INCORRECT RETURN HERE!!! */
    if (0 != hal_pin_bit_newf(HAL_OUT, &(hal_mdata->isConnected), hal_comp_id, "%s.rs485.is-connected", qPrintable(mconfig.componentName)))
        return -1;
    if (0 != hal_pin_s32_newf(HAL_OUT, &(hal_mdata->errorCount), hal_comp_id, "%s.rs485.error-count", qPrintable(mconfig.componentName)))
        return -1;
    if (0 != hal_pin_s32_newf(HAL_OUT, &(hal_mdata->lastError), hal_comp_id, "%s.rs485.last-error", qPrintable(mconfig.componentName)))
        return -1;

    if (0 != hal_pin_float_newf(HAL_IN, &(hal_mdata->spindleRpmIn), hal_comp_id, "%s.spindle.speed-rpm-in", qPrintable(mconfig.componentName)))
        return -1;
    if (0 != hal_pin_float_newf(HAL_OUT, &(hal_mdata->spindleRpmOut), hal_comp_id, "%s.spindle.speed-rpm-out", qPrintable(mconfig.componentName)))
        return -1;
    if (0 != hal_pin_bit_newf(HAL_OUT, &(hal_mdata->atSpeed), hal_comp_id, "%s.spindle.at-speed", qPrintable(mconfig.componentName)))
        return -1;

    if (0 != hal_pin_bit_newf(HAL_OUT, &(hal_mdata->runForward), hal_comp_id, "%s.spindle.run-forward", qPrintable(mconfig.componentName)))
        return -1;
    if (0 != hal_pin_bit_newf(HAL_OUT, &(hal_mdata->runReverse), hal_comp_id, "%s.spindle.run-reverse", qPrintable(mconfig.componentName)))
        return -1;

    /* HAL memory allocation for user parameters */
    hal_user_data_t **hal_udata = NULL;
    hal_udata = new hal_user_data_t*[uconfig.count()];
    for (int i = 0; i < uconfig.count(); ++i) {
        hal_udata[i] = (hal_user_data_t *)hal_malloc(sizeof(hal_user_data_t));
        if (!hal_udata[i])
            goto fail;
    }

    for (int i = 0; i < uconfig.count(); ++i) {
        //int result = 0;
        switch (uconfig.at(i).pinType) {
        case PIN_TYPE_FLOAT:
            result = hal_pin_float_newf(HAL_OUT, &(hal_udata[i]->floatPin),
                                        hal_comp_id,
                                        "%s.parameters.%s",
                                        qPrintable(mconfig.componentName),
                                        qPrintable(uconfig.at(i).pinName));
            *hal_udata[i]->floatPin = 0;
            break;
        case PIN_TYPE_S32:
            result = hal_pin_s32_newf(HAL_OUT, &(hal_udata[i]->s32Pin),
                                      hal_comp_id,
                                      "%s.parameters.%s",
                                      qPrintable(mconfig.componentName),
                                      qPrintable(uconfig.at(i).pinName));
            *hal_udata[i]->s32Pin = 0;
            break;
        case PIN_TYPE_U32:
            result = hal_pin_u32_newf(HAL_OUT, &(hal_udata[i]->u32Pin),
                                      hal_comp_id,
                                      "%s.parameters.%s",
                                      qPrintable(mconfig.componentName),
                                      qPrintable(uconfig.at(i).pinName));
            *hal_udata[i]->u32Pin = 0;
            break;
        }
        if (result != 0)
            goto fail;
    }

    hal_ready(hal_comp_id);

    modbus_t *ctx;
    ctx = modbus_new_rtu(qPrintable(mconfig.rs485.serialDevice),
                         mconfig.rs485.baudRate,
                         mconfig.rs485.parity.at(0).toAscii(),
                         mconfig.rs485.dataBits,
                         mconfig.rs485.stopBits);
    modbus_set_debug(ctx, debugFlag);
    modbus_connect(ctx);
    modbus_set_slave(ctx, mconfig.rs485.slaveAddress);
    uint16_t dd;
    modbus_read_registers(ctx, 0x1001, 1, &dd);

    modbus_close(ctx);
    modbus_free(ctx);

//    hal_exit(hal_comp_id);
//    delete [] hal_udata;
    return 0;
fail:
    hal_exit(hal_comp_id);
    delete [] hal_udata;
    return -1;
}
