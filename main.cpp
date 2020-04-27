#include <math.h>
#include <getopt.h>
#include <signal.h>
#include <modbus/modbus-rtu.h>
#include <QVector>
#include <QFileInfo>
#include "structures.h"
#include "default-values.h"

const char *github="Sources: https://github.com/aekhv/vfdmod/\n";
const char *copyright = "2020 (c) Alexander E. (Khabarovsk, Russia)\n";

int checkFlag, debugFlag, exitFlag, okCounter, serialDeviceIsOpened;
QString exeName;
hal_main_data_t *hal_mdata;
hal_user_data_t **hal_udata;

const char *short_options = "cdhnvPV";
const struct option long_options[] = {
    {"check",   no_argument, 0, 'c'},
    {"debug",   no_argument, 0, 'd'},
    {"help",    no_argument, 0, 'h'},
    {"new",     no_argument, 0, 'n'},
    {"version", no_argument, 0, 'v'},
    {"postgui", no_argument, 0, 'P'},
    {"pyvcp",   no_argument, 0, 'V'},
    {0, 0, 0, 0}
};

int load_config(const QString &inifile, main_config_t &mconfig, QVector<user_config_t> &uconfig);
void make_blank_config();
void make_postgui_config(const main_config_t &mconfig, const QVector<user_config_t> &uconfig);
void make_pyvcp_config(const QString &inifile, const main_config_t &mconfig, const QVector<user_config_t> &uconfig);

void print_help()
{
    printf("Usage:\n"
           "\t%s [--debug] [--check|--postgui|--pyvcp] CONFIGFILE\n"
           "\t%s --new\n",
           qPrintable(exeName),
           qPrintable(exeName));
    printf("Keys:\n"
           "\t-c, --check\tCheck config file for errors.\n"
           "\t-d, --debug\tEnable debug mode.\n"
           "\t-h, --help\tPrint this help.\n"
           "\t-n, --new\tMake a blank config file.\n"
           "\t-P, --postgui\tMake postgui HAL file from existing config file.\n"
           "\t-V, --pyvcp\tMake PyVcp XML file from existing config file.\n"
           "\t-v, --version\tPrint program's version.\n"
           "\tCONFIGFILE\tPlain text INI file.\n");
    printf("Description:\n"
           "\tBlah-blah and blah-blah-blah...\n");
    printf("Examples:\n"
           "\t%s --new > config.ini\n"
           "\t%s --check config.ini\n"
           "\t%s --postgui config.ini > postgui.hal\n"
           "\t%s --pyvcp config.ini > my-pyvcp-panel.xml\n"
           "\t%s config.ini\n",
           qPrintable(exeName),
           qPrintable(exeName),
           qPrintable(exeName),
           qPrintable(exeName),
           qPrintable(exeName));
}

void protocol_delay(const rs485_config_t &cfg)
{
    int startBit = 1;
    if (cfg.parity != "N")
        startBit++;
    long ns = (1000000000l  / cfg.baudRate) * cfg.protocolDelay * (startBit + cfg.dataBits + cfg.stopBits);
    struct timespec loop_timespec = {0, ns};
    nanosleep(&loop_timespec, NULL);
}

void delay_ms(const int &t)
{
    long s = t / 1000;
    long ns = (t % 1000) * 1000000l;
    struct timespec loop_timespec = {s, ns};
    nanosleep(&loop_timespec, NULL);
}

void closeRequest(int param) {
    Q_UNUSED(param)
    exitFlag = 1;
    printf("%s: close request received.\n", qPrintable(exeName));
}

int read_registers(modbus_t *ctx, main_config_t &mconfig, QVector<user_config_t> &uconfig)
{
    uint16_t value;

    /* Reading spindle output speed */
    if (debugFlag)
        printf("\n%s: reading '%s.%s' from address %d (0x%04X)...\n",
               qPrintable(exeName),
               HAL_GROUP_SPINDLE,
               HAL_PIN_RPM_OUT,
               mconfig.rpmOut.address,
               mconfig.rpmOut.address);
    protocol_delay(mconfig.rs485);
    if (1 != modbus_read_registers(ctx, mconfig.rpmOut.address, 1, &value))
        goto fail;
    if (debugFlag)
        printf("%s: returned value is %d (0x%04X)\n", qPrintable(exeName), value, value);
    okCounter++;
    *hal_mdata->spindleRpmOut = double(value) * mconfig.rpmOut.multiplier / mconfig.rpmOut.divider;

    /* Reading user parameters */
    for (int i = 0; i < uconfig.count(); ++i) {
        if (debugFlag)
            printf("\n%s: reading '%s.%s' from address %d (0x%04X)...\n",
                   qPrintable(exeName),
                   HAL_GROUP_USER_PARAMETERS,
                   qPrintable(uconfig.at(i).pinName),
                   uconfig.at(i).address,
                   uconfig.at(i).address);
        protocol_delay(mconfig.rs485);
        if (1 != modbus_read_registers(ctx, uconfig.at(i).address, 1, &value))
            goto fail;
        if (debugFlag)
            printf("%s: returned value is %d (0x%04X)\n", qPrintable(exeName), value, value);

        switch (uconfig.at(i).pinType) {
        case HAL_FLOAT:
            *hal_udata[i]->floatPin = double(value) * uconfig.at(i).multiplier / uconfig.at(i).divider;
            break;
        case HAL_S32:
            *hal_udata[i]->s32Pin = value * uconfig.at(i).multiplier / uconfig.at(i).divider;
            break;
        case HAL_U32:
            *hal_udata[i]->u32Pin = value * uconfig.at(i).multiplier / uconfig.at(i).divider;
            break;
        default:
            fprintf(stderr, "%s: incorrect HAL pin type!\n", qPrintable(exeName));
        }

        okCounter++;
    }

    return 0;
fail:
    *hal_mdata->lastError = errno;
    (*hal_mdata->errorCount)++;
    okCounter = 0;
    return -1;
}

int write_registers(modbus_t *ctx, main_config_t &mconfig)
{
    int result = 0;
    uint16_t value;

    /* Setting command speed */
    hal_float_t speed = abs(*hal_mdata->spindleRpmIn);

    if (speed < mconfig.common.minSpeedRpm)
        speed = mconfig.common.minSpeedRpm;

    if (speed > mconfig.common.maxSpeedRpm)
        speed = mconfig.common.maxSpeedRpm;

    speed = speed * mconfig.rpmIn.multiplier / mconfig.rpmIn.divider;

    value = int(speed);
    if (debugFlag)
        printf("\n%s: setting command speed value to %d (0x%04X)...\n", qPrintable(exeName), value, value);

    protocol_delay(mconfig.rs485);

    // Function code 0x06
    if (mconfig.rpmIn.functionCode == MODBUS_FUNC_WRITE_SINGLE_HOLDING_REGISTER)
        result = modbus_write_register(ctx, mconfig.rpmIn.address, value);

    // Function code 0x10
    if (mconfig.rpmIn.functionCode == MODBUS_FUNC_WRITE_MULTIPLE_HOLDING_REGISTERS)
        result = modbus_write_registers(ctx, mconfig.rpmIn.address, 1, &value);

    if (result != 1)
        goto fail;
    else
        okCounter++;


    /* Setting control word value for function codes 0x06 & 0x10 */
    if ((mconfig.control.functionCode == MODBUS_FUNC_WRITE_SINGLE_HOLDING_REGISTER)
            || (mconfig.control.functionCode == MODBUS_FUNC_WRITE_MULTIPLE_HOLDING_REGISTERS)) {

        value = mconfig.control.stopValue;

        if (0 != *hal_mdata->runReverse)
            value = mconfig.control.runRevValue;

        if (0 != *hal_mdata->runForward)
            value = mconfig.control.runFwdValue;

        if (0 != *hal_mdata->faultReset)
            value = mconfig.control.faultResetValue;

        if (debugFlag)
            printf("\n%s: setting control word value to %d (0x%04X)...\n", qPrintable(exeName), value, value);

        protocol_delay(mconfig.rs485);

        if (mconfig.control.functionCode == MODBUS_FUNC_WRITE_SINGLE_HOLDING_REGISTER)
            result = modbus_write_register(ctx, mconfig.control.address, value);

        if (mconfig.control.functionCode == MODBUS_FUNC_WRITE_MULTIPLE_HOLDING_REGISTERS)
            result = modbus_write_registers(ctx, mconfig.control.address, 1, &value);

        if (result != 1)
            goto fail;
        else
            okCounter++;

        // If fault reset has been written successfully then inactive it's input
        if (value == mconfig.control.faultResetValue)
            *hal_mdata->faultReset = 0;

    }

    // Setting coils for function code 0x05
    if (mconfig.control.functionCode == MODBUS_FUNC_WRITE_SINGLE_COIL) {

        // Direction coil
        if (debugFlag)
            printf("\n%s: setting direction coil...\n", qPrintable(exeName));

        protocol_delay(mconfig.rs485);
        result = modbus_write_bit(ctx,
                                  mconfig.control.directionCoil,
                                  *hal_mdata->runReverse != 0 ? 1 : 0);
        if (result != 1)
            goto fail;
        else
            okCounter++;

        // Run coil
        if (debugFlag)
            printf("\n%s: setting run coil...\n", qPrintable(exeName));

        protocol_delay(mconfig.rs485);
        result = modbus_write_bit(ctx,
                                  mconfig.control.runCoil,
                                  ((*hal_mdata->runForward != 0) || (*hal_mdata->runReverse != 0)) ? 1 : 0);

        if (result != 1)
            goto fail;
        else
            okCounter++;

        // Fault reset coil
        if (*hal_mdata->faultReset != 0) {
            if (debugFlag)
                printf("\n%s: setting fault reset coil...\n", qPrintable(exeName));

            protocol_delay(mconfig.rs485);
            result = modbus_write_bit(ctx, mconfig.control.faultResetCoil, 1);

            if (result != 1)
                goto fail;
            else
                okCounter++;

            // If fault reset has been written successfully then inactive it's input
            *hal_mdata->faultReset = 0;
        }

    }

    return 0;
fail:
    *hal_mdata->lastError = errno;
    (*hal_mdata->errorCount)++;
    okCounter = 0;
    return -1;
}

int main(int argc, char *argv[])
{
    main_config_t mconfig;
    QVector<user_config_t> uconfig;
    QString inifile;

    checkFlag = 0;
    debugFlag = 0;
    exitFlag = 0;
    okCounter = 0;
    serialDeviceIsOpened = 0;

    int newFlag = 0;
    int postguiFlag = 0;
    int pyvcpFlag = 0;
    exeName = QFileInfo(argv[0]).fileName();

    int arg;
    int index;
    int count = 0;
    while ((arg = getopt_long(argc, argv, short_options, long_options, &index)) != -1) {
        switch (arg) {
        case 'c':
            checkFlag = 1;
            count++;
            break;
        case 'd':
            debugFlag = 1;
            break;
        case 'h':
            print_help();
            return 0;
        case 'n':
            newFlag = 1;
            count++;
            break;
        case 'v':
            printf("%s %s\n", APP_TARGET, APP_VERSION);
            printf(github);
            printf(copyright);
            return 0;
        case 'P':
            postguiFlag = 1;
            count++;
            break;
        case 'V':
            pyvcpFlag = 1;
            count++;
            break;
        default:
            fprintf(stderr, "Arguments are wrong! Type '%s -h' for help.\n", qPrintable(exeName));
            return 0;
        }
    }

    if (count > 1) {
        fprintf(stderr, "Too many keys! Type '%s -h' for help.\n", qPrintable(exeName));
        return 0;
    }

    /* If --new flag specified */
    if (newFlag) {
        make_blank_config();
        return 0;
    }

    if ((argc - optind) == 0) {
        fprintf(stderr, "File name is missing! Type '%s -h' for help.\n", qPrintable(exeName));
        return 0;
    }

    if ((argc - optind) > 1) {
        fprintf(stderr, "Too many arguments! Type '%s -h' for help.\n", qPrintable(exeName));
        return 0;
    }

    inifile = argv[optind];

    /* Finally, trying to load an existing config */
    if (load_config(inifile, mconfig, uconfig) < 0)
        return -1;

    /* If --check flag specified */
    if (checkFlag) {
        // Nothing to do, just return
        return 0;
    }

    /* If --postgui flag specified */
    if (postguiFlag) {
        make_postgui_config(mconfig, uconfig);
        return 0;
    }

    /* If --pyvcp flag specified */
    if (pyvcpFlag) {
        make_pyvcp_config(QFileInfo(inifile).baseName(), mconfig, uconfig);
        return 0;
    }

    /* HAL init */
    int hal_comp_id = hal_init(qPrintable(mconfig.common.componentName));
    if (hal_comp_id < 0)
        return -1;

    /* HAL memory allocation for main parameters */
    hal_mdata = (hal_main_data_t *)hal_malloc(sizeof(hal_main_data_t));
    if (!hal_mdata)
        goto fail;

    if (0 != hal_pin_bit_newf(HAL_OUT, &(hal_mdata->isConnected), hal_comp_id, "%s.%s.%s",
                              qPrintable(mconfig.common.componentName),
                              HAL_GROUP_RS485,
                              HAL_PIN_IS_CONNECTED))
        goto fail;

    if (0 != hal_pin_s32_newf(HAL_OUT, &(hal_mdata->errorCount), hal_comp_id, "%s.%s.%s",
                              qPrintable(mconfig.common.componentName),
                              HAL_GROUP_RS485,
                              HAL_PIN_ERROR_COUNT))
        goto fail;

    if (0 != hal_pin_s32_newf(HAL_OUT, &(hal_mdata->lastError), hal_comp_id, "%s.%s.%s",
                              qPrintable(mconfig.common.componentName),
                              HAL_GROUP_RS485,
                              HAL_PIN_LAST_ERROR))
        goto fail;

    if (0 != hal_pin_float_newf(HAL_IN, &(hal_mdata->spindleRpmIn), hal_comp_id, "%s.%s.%s",
                                qPrintable(mconfig.common.componentName),
                                HAL_GROUP_SPINDLE,
                                HAL_PIN_RPM_IN))
        goto fail;

    if (0 != hal_pin_float_newf(HAL_OUT, &(hal_mdata->spindleRpmOut), hal_comp_id, "%s.%s.%s",
                                qPrintable(mconfig.common.componentName),
                                HAL_GROUP_SPINDLE,
                                HAL_PIN_RPM_OUT))
        goto fail;

    if (0 != hal_pin_bit_newf(HAL_OUT, &(hal_mdata->atSpeed), hal_comp_id, "%s.%s.%s",
                              qPrintable(mconfig.common.componentName),
                              HAL_GROUP_SPINDLE,
                              HAL_PIN_AT_SPEED))
        goto fail;

    if (0 != hal_pin_bit_newf(HAL_IN, &(hal_mdata->faultReset), hal_comp_id, "%s.%s.%s",
                              qPrintable(mconfig.common.componentName),
                              HAL_GROUP_CONTROL,
                              HAL_PIN_FAULT_RESET))
        goto fail;

    if (0 != hal_pin_bit_newf(HAL_IN, &(hal_mdata->runForward), hal_comp_id, "%s.%s.%s",
                              qPrintable(mconfig.common.componentName),
                              HAL_GROUP_CONTROL,
                              HAL_PIN_RUN_FORWARD))
        goto fail;

    if (0 != hal_pin_bit_newf(HAL_IN, &(hal_mdata->runReverse), hal_comp_id, "%s.%s.%s",
                              qPrintable(mconfig.common.componentName),
                              HAL_GROUP_CONTROL,
                              HAL_PIN_RUN_REVERSE))
        goto fail;

    *hal_mdata->isConnected = 0;
    *hal_mdata->errorCount = 0;
    *hal_mdata->lastError = 0;
    *hal_mdata->spindleRpmIn = 0;
    *hal_mdata->spindleRpmOut = 0;
    *hal_mdata->atSpeed = 0;
    *hal_mdata->runForward = 0;
    *hal_mdata->runReverse = 0;

    /* HAL memory allocation for user parameters */
    hal_udata = new hal_user_data_t*[uconfig.count()];
    for (int i = 0; i < uconfig.count(); ++i) {
        hal_udata[i] = (hal_user_data_t *)hal_malloc(sizeof(hal_user_data_t));
        if (!hal_udata[i])
            goto fail;
    }

    for (int i = 0; i < uconfig.count(); ++i) {
        switch (uconfig.at(i).pinType) {
        case HAL_FLOAT:
            if (0 == hal_pin_float_newf(HAL_OUT, &(hal_udata[i]->floatPin),
                                        hal_comp_id,
                                        "%s.%s.%s",
                                        qPrintable(mconfig.common.componentName),
                                        HAL_GROUP_USER_PARAMETERS,
                                        qPrintable(uconfig.at(i).pinName)))
                *hal_udata[i]->floatPin = 0;
            else
                goto fail;
            break;
        case HAL_S32:
            if (0 == hal_pin_s32_newf(HAL_OUT, &(hal_udata[i]->s32Pin),
                                      hal_comp_id,
                                      "%s.%s.%s",
                                      qPrintable(mconfig.common.componentName),
                                      HAL_GROUP_USER_PARAMETERS,
                                      qPrintable(uconfig.at(i).pinName)))
                *hal_udata[i]->s32Pin = 0;
            else
                goto fail;
            break;
        case HAL_U32:
            if (0 == hal_pin_u32_newf(HAL_OUT, &(hal_udata[i]->u32Pin),
                                      hal_comp_id,
                                      "%s.%s.%s",
                                      qPrintable(mconfig.common.componentName),
                                      HAL_GROUP_USER_PARAMETERS,
                                      qPrintable(uconfig.at(i).pinName)))
                *hal_udata[i]->u32Pin = 0;
            else
                goto fail;
            break;
        default:
            fprintf(stderr, "%s: incorrect HAL pin type!\n", qPrintable(exeName));
        }
    }

    /* Modbus init */
    modbus_t *ctx;
    ctx = modbus_new_rtu(qPrintable(mconfig.rs485.serialDevice),
                         mconfig.rs485.baudRate,
                         mconfig.rs485.parity.at(0).toLatin1(),
                         mconfig.rs485.dataBits,
                         mconfig.rs485.stopBits);
    if (!ctx)
        goto fail;

    modbus_set_debug(ctx, debugFlag);
    modbus_set_slave(ctx, mconfig.rs485.slaveAddress);

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "%s: %s\n", qPrintable(exeName), modbus_strerror(errno));
        modbus_free(ctx);
        goto fail;
    }

    serialDeviceIsOpened = 1;

    signal(SIGINT, closeRequest);
    signal(SIGKILL, closeRequest);
    signal(SIGTERM, closeRequest);

    /* Now HAL component is ready */
    hal_ready(hal_comp_id);

    /* MAIN LOOP */
    while (!exitFlag) {

        /* Reading modbus data */
        if (serialDeviceIsOpened) {
            if (read_registers(ctx, mconfig, uconfig) < 0)
                /* Is last error code means connection lost? */
                foreach (int err, mconfig.rs485.criticalErrors) {
                    if (*hal_mdata->lastError == err) {
                        /* Close connection */
                        modbus_close(ctx);
                        if (debugFlag)
                            printf("\n%s: critical error detected, connection closed.\n", qPrintable(exeName));
                        serialDeviceIsOpened = 0;
                        break;
                    }
                }
        }

        /* Writing modbus data */
        if (serialDeviceIsOpened) {
            if (write_registers(ctx, mconfig) < 0)
                /* Is last error code means connection lost? */
                foreach (int err, mconfig.rs485.criticalErrors) {
                    if (*hal_mdata->lastError == err) {
                        /* Close connection */
                        modbus_close(ctx);
                        if (debugFlag)
                            printf("\n%s: critical error detected, connection closed.\n", qPrintable(exeName));
                        serialDeviceIsOpened = 0;
                        break;
                    }
                }
        }

        /* Should we clear HAL output data if any error has been occured? */
        /* Is this really necessary? I'm doubt... Anyway let's do it. */
        if (okCounter == 0) {
            *hal_mdata->spindleRpmOut = 0;
            *hal_mdata->atSpeed = 0;
            for (int i = 0; i < uconfig.count(); ++i) {
                switch (uconfig.at(i).pinType) {
                case HAL_FLOAT:
                    *hal_udata[i]->floatPin = 0;
                    break;
                case HAL_S32:
                    *hal_udata[i]->s32Pin = 0;
                    break;
                case HAL_U32:
                    *hal_udata[i]->u32Pin = 0;
                    break;
                default:
                    fprintf(stderr, "%s: incorrect HAL pin type!\n", qPrintable(exeName));
                }
            }
        }

        /* Is Modbus connection established? */
        if (okCounter >= mconfig.rs485.isConnectedDelay) {
            *hal_mdata->isConnected = 1;
            okCounter = mconfig.rs485.isConnectedDelay; // to avoid okCounter overflow
        } else
            *hal_mdata->isConnected = 0;

        /* At speed may be valid only when spindle is running */
        if (((0 != *hal_mdata->runReverse) || (0 != *hal_mdata->runForward)) &&
                (0 != *hal_mdata->spindleRpmIn) && (0 != *hal_mdata->isConnected)) {

            hal_float_t rpmIn = abs(*hal_mdata->spindleRpmIn);
            hal_float_t rpmOut = *hal_mdata->spindleRpmOut;
            if ((abs(rpmOut - rpmIn) / rpmIn) <= mconfig.common.atSpeedThreshold)
                *hal_mdata->atSpeed = 1;
            else
                *hal_mdata->atSpeed = 0;

        } else
            *hal_mdata->atSpeed = 0;


        /* Reconnection */
        if (!serialDeviceIsOpened) {
            /* Connection delay */
            delay_ms(mconfig.rs485.connectionDelay);

            if (debugFlag)
                printf("\n%s: connection attempt...\n", qPrintable(exeName));

            /* Open connection */
            if (modbus_connect(ctx) == -1) {
                (*hal_mdata->errorCount)++;
            } else {
                serialDeviceIsOpened = 1;
            }
        } else
            /* Loop delay */
            delay_ms(mconfig.rs485.loopDelay);
    }

    modbus_close(ctx);
    modbus_free(ctx);

    hal_exit(hal_comp_id);
    delete [] hal_udata;
    printf("%s: application closed.\n", qPrintable(exeName));
    return 0;

fail:
    hal_exit(hal_comp_id);
    delete [] hal_udata;
    fprintf(stderr, "%s: fatal error.\n", qPrintable(exeName));
    return -1;
}
