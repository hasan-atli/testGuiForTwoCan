#ifndef CA_SERIAL_H
#define CA_SERIAL_H

#include <stdint.h>

#define TORK_CHR_SP			      ' '
#define TORK_CHR_DOT			  '.'
#define TORK_CHR_RA			      '>'
#define TORK_CHR_CC			      ':'
#define TORK_CHR_DQ			      0x22		//'"'			// double quotes
#define TORK_CHR_LF			      0x0A		//'\n'
#define TORK_CHR_VT			      0x0B  	//'\v'
#define TORK_CHR_CR			      0x0D  	//'\r'
#define TORK_CHR_HT			      0x09  	//'\t'
#define TORK_CHR_FF			      0x0C		//'\f'
#define TORK_CHR_BS			      0x08		//'\b'
#define TORK_CHR_BL			      0x07		//'\a'
#define TORK_CHR_ESC			  0x1B	    //'\a'
#define	TORK_CHR_SOH			  0x01
#define	TORK_CHR_STX			  0x02
#define	TORK_CHR_ETX			  0x03
#define	TORK_CHR_ACK			  0x06
#define	TORK_CHR_NAK			  0x15

#define TORK_PARSER_WAITING_STX                0x1
#define TORK_PARSER_WAITING_HDR                0x2
#define TORK_PARSER_WAITING_ID                 0x3
#define TORK_PARSER_WAITING_RTR                0x4
#define TORK_PARSER_WAITING_EXT                0x5
#define TORK_PARSER_WAITING_LEN                0x6
#define TORK_PARSER_WAITING_DATA               0x7
#define TORK_PARSER_WAITING_CRC                0x8
#define TORK_PARSER_WAITING_ETX                0x9

#define TORK_MAX_PAYLOAD_DATALEN          8
#define TORK_MAX_HDR_DATALEN                 1
#define TORK_MAX_CANID_DATALEN               4
#define TORK_MAX_RTR_DATALEN                 1
#define TORK_MAX_EXT_DATALEN                 1
#define TORK_MAX_LENTGH_DATALEN              1
#define TORK_MAX_CRC_DATALEN                 2
#define TORK_MAX_SERIAL_MSG_BODY_DATALEN     (TORK_MAX_PAYLOAD_DATALEN + TORK_MAX_HDR_DATALEN + TORK_MAX_RTR_DATALEN  + TORK_MAX_EXT_DATALEN + TORK_MAX_LENTGH_DATALEN + TORK_MAX_CANID_DATALEN + TORK_MAX_CRC_DATALEN)

#define TORK_EXTERNAL_MSG_HDR                0xaa
#define TORK_MAX_SERIAL_MSG_IN_RNG_BUF       2
#define TORK_MAX_TRY_COUNT                   10
#define TORK_BYPASS_MESSAGE_HDR              0xb
#define TORK_CONFIG_MESSAGE_HDR              0xa
#define TORK_AD_MESSAGE_HDR                  0xc

#define TORK_TRANSMIT_MESSAGE_HDR            0xe
#define TORK_RECEIVE_MESSAGE_HDR             0xc

/******************************************************************************
* Module Typedefs
*******************************************************************************/
typedef struct Msg__Struct
{
    uint8_t    hdr;
    uint32_t   canMsgId;
    uint8_t    isRtr;
    uint8_t    isExt;
    uint8_t    len;
    uint8_t    dataBuf[TORK_MAX_PAYLOAD_DATALEN];
    uint16_t   crcCalc;
    uint8_t    serializeData[TORK_MAX_SERIAL_MSG_BODY_DATALEN];
}torkSerialMsg;


typedef union
{
    uint32_t val;
    uint8_t  byte[4];
}torkUint32;
/************************************************************************************************************************************************************/

#endif // CA_SERIAL_H
