/* Required by hal.h */
#define ULAPI

#include <hal.h>
#include <getopt.h>
#include <modbus/modbus-rtu.h>
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
} haldata_t;

int load_config(const QString &fname, connection_t &conn);
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

static void delay(const connection_t &conn)
{
    int startBit = 1;
    if (conn.parity != "N")
        startBit++;
    long ns = 1000000 * conn.protocolDelay * (startBit + conn.dataBits + conn.stopBits) / conn.baudRate;
    struct timespec loop_timespec = {0, ns};
    nanosleep(&loop_timespec, NULL);
}

int main(int argc, char *argv[])
{
    int hal_id;
    haldata_t *haldata;
    connection_t connection;
    int checkFlag = 0;
    int debugFlag = 0;
    int newFlag = 0;
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
        return load_config(iniFile, connection);
    }

    /* If --new flag specified */
    if (newFlag) {
        return write_blank_config(iniFile);
    }

    /* Finally, trying to load an existing config */
    int result = load_config(iniFile, connection);
    if (result < 0)
        return result;


    /* HAL init */
    hal_id = hal_init(qPrintable(fname));
    if (hal_id < 0)
        return -1;

    haldata = (haldata_t *)hal_malloc(sizeof(haldata_t));
    if (!haldata)
        return -1;

//    int retval;
//    retval = hal_pin_bit_newf(HAL_OUT, &(haldata->is_connected), hal_id, "%s.modbus-ok", qPrintable(fname));
//    retval = hal_pin_s32_newf(HAL_OUT, &(haldata->error_count), hal_id, "%s.error-count", qPrintable(fname));
//    retval = hal_pin_s32_newf(HAL_OUT, &(haldata->last_error), hal_id, "%s.error-code", qPrintable(fname));

    modbus_t *ctx;
    ctx = modbus_new_rtu(qPrintable(connection.serialDevice),
                         connection.baudRate,
                         connection.parity.at(0).toAscii(),
                         connection.dataBits,
                         connection.stopBits);
    modbus_set_debug(ctx, debugFlag);
    modbus_connect(ctx);
    modbus_set_slave(ctx, connection.slaveAddress);


    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
