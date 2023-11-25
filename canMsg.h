#ifndef CANMSG_H
#define CANMSG_H

#include <stdint.h>

#define MAX_CAN_MSG_DATA_COUNT          8

typedef struct CanMsg__Struct
{
    uint32_t  canMsgId;
    uint8_t   ext;
    uint8_t   rtr;
    uint8_t   len;
    uint8_t   dataBuf[MAX_CAN_MSG_DATA_COUNT];
}torkCanMsg;

#endif // CANMSG_H
