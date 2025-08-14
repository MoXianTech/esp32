#ifndef __PRESSURE_THREAD_H__
#define __PRESSURE_THREAD_H__

typedef enum {
    CRC16_MODBUS,
    CHECK_SUM
} CHECK_TYPE_T;

void pressure_thread(void *pvparams);

#endif
