#ifndef ATI_UNS4I_INNER
#define ATI_UNS4I_INNER

#include <common/mbuf.h>

struct uns4i_uart{
    uint8_t uart;
    uint8_t addr;
    uint8_t uart_status;
    struct mbuf *answer, ans;
};

struct Uns4i_Request{
    uint8_t start;          /* allways 0x00 */
    uint8_t addr;           /* device address */
    uint8_t command_len;    /* lenght of command field in bytes: 0x03 (command, code_isol, amplif) */
    uint8_t command;        /* default - 0x00 get_data */
    uint8_t code_isol;
                            /* Resitanse, FRC_mode (low half byte) and code sizing mode (hight half byte)
                                ISOL_MASK:         0x08
                                FRC25_MASK:        0x01
                                FRC50_MASK:        0x00
                                    update 21.03.2018, implement 21.06.2018
                                Code mode on 1-st channel mask:   0x10
                                Code mode on 2-nd channel mask:   0x20
                                Code mode on 3-rd channel mask:   0x40
                                Code mode on 4-th channel mask:   0x80
                            */
    uint8_t amplif; 
                           /* Amplify
                                COEF_MASK       0x03
                                COEF1_MASK      (COEF_MASK)<<0
                                COEF2_MASK      (COEF_MASK)<<2
                                COEF3_MASK      (COEF_MASK)<<4
                                COEF4_MASK      (COEF_MASK)<<6
                                channel coefficient
                                    0x0: coefficient OFF
                                    0x1: amplification ON
                                    0x2: amplification AUTO
                                    0x3: not used
                        
                            */
    uint8_t crc1;           /* crc16 poly: 0x8005, high byte */
    uint8_t crc2;           /* crc16 poly: 0x8005, low byte */
}__attribute((packed));

struct channelTRC{
    float VDC;
    float VAC;
    float VACf[5];
    uint16_t Fcur;
    uint16_t Fmod;
} __attribute((packed));
            
struct UnsTrc_Answer{
    uint8_t start;
    uint8_t addr;
    uint8_t data_len;
    struct channelTRC TRC[4];
    uint16_t mode;
    uint16_t diag;
    uint16_t crc16;
}__attribute((packed));

uint16_t crc_tab16[] =  {
  0x0,0x8005,0x800f,0xa,0x801b,0x1e,0x14,0x8011,0x8033,0x36,0x3c,
  0x8039,0x28,0x802d,0x8027,0x22,0x8063,0x66,0x6c,0x8069,0x78,
  0x807d,0x8077,0x72,0x50,0x8055,0x805f,0x5a,0x804b,0x4e,0x44,
  0x8041,0x80c3,0xc6,0xcc,0x80c9,0xd8,0x80dd,0x80d7,0xd2,0xf0,
  0x80f5,0x80ff,0xfa,0x80eb,0xee,0xe4,0x80e1,0xa0,0x80a5,0x80af,
  0xaa,0x80bb,0xbe,0xb4,0x80b1,0x8093,0x96,0x9c,0x8099,0x88,0x808d,
  0x8087,0x82,0x8183,0x186,0x18c,0x8189,0x198,0x819d,0x8197,0x192,
  0x1b0,0x81b5,0x81bf,0x1ba,0x81ab,0x1ae,0x1a4,0x81a1,0x1e0,0x81e5,
  0x81ef,0x1ea,0x81fb,0x1fe,0x1f4,0x81f1,0x81d3,0x1d6,0x1dc,0x81d9,
  0x1c8,0x81cd,0x81c7,0x1c2,0x140,0x8145,0x814f,0x14a,0x815b,0x15e,
  0x154,0x8151,0x8173,0x176,0x17c,0x8179,0x168,0x816d,0x8167,0x162,
  0x8123,0x126,0x12c,0x8129,0x138,0x813d,0x8137,0x132,0x110,0x8115,
  0x811f,0x11a,0x810b,0x10e,0x104,0x8101,0x8303,0x306,0x30c,0x8309,
  0x318,0x831d,0x8317,0x312,0x330,0x8335,0x833f,0x33a,0x832b,0x32e,
  0x324,0x8321,0x360,0x8365,0x836f,0x36a,0x837b,0x37e,0x374,0x8371,
  0x8353,0x356,0x35c,0x8359,0x348,0x834d,0x8347,0x342,0x3c0,0x83c5,
  0x83cf,0x3ca,0x83db,0x3de,0x3d4,0x83d1,0x83f3,0x3f6,0x3fc,0x83f9,
  0x3e8,0x83ed,0x83e7,0x3e2,0x83a3,0x3a6,0x3ac,0x83a9,0x3b8,0x83bd,
  0x83b7,0x3b2,0x390,0x8395,0x839f,0x39a,0x838b,0x38e,0x384,0x8381,
  0x280,0x8285,0x828f,0x28a,0x829b,0x29e,0x294,0x8291,0x82b3,0x2b6,
  0x2bc,0x82b9,0x2a8,0x82ad,0x82a7,0x2a2,0x82e3,0x2e6,0x2ec,0x82e9,
  0x2f8,0x82fd,0x82f7,0x2f2,0x2d0,0x82d5,0x82df,0x2da,0x82cb,0x2ce,
  0x2c4,0x82c1,0x8243,0x246,0x24c,0x8249,0x258,0x825d,0x8257,0x252,
  0x270,0x8275,0x827f,0x27a,0x826b,0x26e,0x264,0x8261,0x220,0x8225,
  0x822f,0x22a,0x823b,0x23e,0x234,0x8231,0x8213,0x216,0x21c,0x8219,
  0x208,0x820d,0x8207,0x202
};

#endif //ATI_UNS4I_INNER