#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct {
    int slaveAddress;
    QString serialDevice;
    int baudRate;
    int dataBits;
    QString parity;
    int stopBits;
    int loopDelayMs;
    int protocolDelay;
} connection_t;

#endif // STRUCTURES_H
