#include "ati_uns4i.h"
#include "ati_uns4i_mjs.h"
#include "ati_uns4i_ep.h"

#include "mgos.h"
#include "mgos_timers.h"
#include "mgos_uart.h"


static struct uns4i_data uns4i_data_inner__;
static struct uns4i_uart uns4i_uart_inner__;

static struct Uns4i_Request uns4i_request_inner__;

enum UART_STATUS {UART_NOT_READY, UART_READY };

static void config_uart_inner( );

static uint8_t const* get_request_buffer_inner( uint8_t _addr, uint8_t _code_isol, uint8_t _amplif );
static size_t get_request_size_inner( void );
static void uart_send_request( void );

static uint16_t CRC16tabl( uint8_t *_buf, size_t _len );
static bool check_crc_inner( char const* _answer, size_t _len );
void fill_answer_inner( char const* _answer, size_t _len );


void init_handler( int _uart_no, int _addr )
{
    uns4i_uart_inner__.answer = &uns4i_uart_inner__.ans;
    uns4i_uart_inner__.uart = _uart_no & 0xff;    
    uns4i_uart_inner__.addr = _addr & 0xff;    
    LOG(LL_INFO, ("Create UNS4i_Handler UART%d send %d bytes.", uns4i_uart_inner__.uart, uns4i_uart_inner__.addr ) );

    uns4i_uart_inner__.uart_status = UART_NOT_READY;    
    mbuf_init( uns4i_uart_inner__.answer, sizeof(struct UnsTrc_Answer)+2 );
    config_uart_inner( );
}

struct uns4i_data const* get_data()
{
    return &uns4i_data_inner__;
}

/*-----------private section----------------*/

static void uart_send_request( void )
{
    size_t b = mgos_uart_write( uns4i_uart_inner__.uart,
                                get_request_buffer_inner( uns4i_uart_inner__.addr, 0x00, 0x00 ),
                                get_request_size_inner( )
    );

    LOG(LL_INFO, ("UART%d send %d bytes.", uns4i_uart_inner__.uart, b ) );
    LOG(LL_INFO, ("UNS Answer id: %d.", uns4i_data_inner__.cntr ) );

    char* res = "\x80\x20\x84\x10\xeb\x2c\xbc\x20\x08\x82\x3d\x00\x00\x00\x00\x00\x00\x00\x00\x36\xb8\x58\x3d\x00\x00\x00\x00\x00\x00\x00\x00\xfc\x01\x07\x00\xcd\x01\x02\xbd\x6c\x60\x82\x3d\x2b\x60\x2d\x3c\x2b\x60\x2d\x3c\x2b\x60\x2d\x3c\xe6\xd5\x2d\x3c\xe6\xd5\x2d\x3c\xa5\x00\x00\x00\x00\x00\x00\x00\x2b\x60\xad\x3c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x8e\x00\x00\x00\x2b\x60\x2d\x3c\xe6\xd5\xad\x3c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xe3\x00\x00\x00\x00\x00\x00\x03\x03\x5c";
    mbuf_append(uns4i_uart_inner__.answer, res, sizeof(struct UnsTrc_Answer) );
    LOG(LL_INFO, ("LEN %d %d", (int)uns4i_uart_inner__.answer->len, (int)sizeof(struct UnsTrc_Answer) ) );
    if ( uns4i_uart_inner__.answer->len == sizeof(struct UnsTrc_Answer) )
    {
    LOG(LL_INFO, ("CRC_OK" ) );
        if (check_crc_inner( uns4i_uart_inner__.answer->buf, uns4i_uart_inner__.answer->len) )
        {
    LOG(LL_INFO, ("HANDLE_DATA" ) );
            fill_answer_inner( uns4i_uart_inner__.answer->buf, uns4i_uart_inner__.answer->len );
            mbuf_clear( uns4i_uart_inner__.answer );
        }
    }

}


static void uart_receive_dispatcher(int _uart_no, void *_arg) 
{
    static struct mbuf lb = {0};
    assert(uns4i_uart_inner__.uart == _uart_no );
    size_t rx_av = mgos_uart_read_avail( _uart_no );
    if (rx_av == 0) return;

    LOG(LL_INFO, ("UART%d ready %d bytes.", uns4i_uart_inner__.uart, rx_av ) );

    mgos_uart_read_mbuf( _uart_no, &lb, rx_av );
  
    mbuf_append( uns4i_uart_inner__.answer, lb.buf, lb.len);
    //FIX ME - данные могут за один раз не прийти - нужно дождаться всего пакета
    LOG(LL_INFO, ("UART%d read %d bytes, in answer buffer %d bytes.", uns4i_uart_inner__.uart, lb.len, uns4i_uart_inner__.answer->len ) );
    
    if ( uns4i_uart_inner__.answer->len == sizeof(struct UnsTrc_Answer) )
    {
        if (check_crc_inner( uns4i_uart_inner__.answer->buf, uns4i_uart_inner__.answer->len) )
        {
            fill_answer_inner( uns4i_uart_inner__.answer->buf, uns4i_uart_inner__.answer->len );
            mbuf_clear( uns4i_uart_inner__.answer );
        }
    }
    

  /* Finally, remove the line data from the buffer. */
  mbuf_remove( &lb, lb.len );
  (void) _arg;
}


static void config_uart_inner( )
{
    struct mgos_uart_config ucfg;
    mgos_uart_config_set_defaults( uns4i_uart_inner__.uart, &ucfg);
    ucfg.baud_rate = 115200;
    ucfg.num_data_bits = 8;
    ucfg.parity = MGOS_UART_PARITY_NONE;
    ucfg.stop_bits = MGOS_UART_STOP_BITS_1;
    if ( mgos_uart_configure( uns4i_uart_inner__.uart, &ucfg ) )  
    {
        uns4i_uart_inner__.uart_status = UART_READY;    
        mgos_uart_set_dispatcher( uns4i_uart_inner__.uart, uart_receive_dispatcher, NULL /* arg */);
        mgos_uart_set_rx_enabled( uns4i_uart_inner__.uart, true );

        LOG(LL_INFO, ("UART%d configure", uns4i_uart_inner__.uart ) );
    }
    else
    {
        LOG(LL_ERROR, ("Failed to configure UART%d", uns4i_uart_inner__.uart) );
    }
}


static uint8_t const* get_request_buffer_inner( uint8_t _addr, uint8_t _code_isol, uint8_t _amplif )
{
    bzero( &uns4i_request_inner__, sizeof(struct Uns4i_Request) );
    uns4i_request_inner__.addr = _addr;
    uns4i_request_inner__.command_len = 0x03;
    uns4i_request_inner__.code_isol = _code_isol;
    uns4i_request_inner__.amplif = _amplif;
    
    uint8_t crc_buff[sizeof(struct Uns4i_Request)-1];
    memcpy( crc_buff, &uns4i_request_inner__.addr, sizeof(struct Uns4i_Request)-1 );

    uint16_t crc = CRC16tabl( crc_buff, sizeof(crc_buff) );
    uns4i_request_inner__.crc1=crc >> 8;
    uns4i_request_inner__.crc2=crc & 0xff;

    int i;
    uint8_t const* ptr = (const uint8_t*)&uns4i_request_inner__;    
    for (i = 0; i < sizeof(struct Uns4i_Request); i++)
    {
        LOG(LL_INFO, ("REQUEST: '%02X'", ptr[i] ) );    
    }


    return (uint8_t*)&uns4i_request_inner__;    
}
static size_t get_request_size_inner( void )
{
    return sizeof(struct Uns4i_Request);
}


void fill_answer_inner( char const* _answer, size_t _len )
{
    struct UnsTrc_Answer tmp;
    memcpy( &tmp, _answer, _len );

    memcpy( &uns4i_data_inner__, tmp.TRC, 4*sizeof(struct channelTRC) );

    uns4i_data_inner__.uns_mode = tmp.mode;
    uns4i_data_inner__.uns_diag = tmp.diag;
    ++uns4i_data_inner__.cntr;
}

static bool check_crc_inner( char const* _answer, size_t _len )
{
    uint8_t data[_len];
    memcpy( data, _answer, _len );
    uint16_t crc_dev = data[_len-2] << 8 | data[_len-1];
    data[_len-1] = 0;
    data[_len-2] = 0;
    return crc_dev == CRC16tabl(data+1, _len-1);
}
static uint16_t CRC16tabl( uint8_t *_buf, size_t _len )
{
    if ( _len <= 2)
        return 0x8005;
        
    size_t data_index = 0;                
    uint16_t crc = 0;
    
    while ( _len-- )
    {
        crc = crc_tab16[crc >> 8] ^ ((crc << 8) | _buf[data_index++]);
    }
    return crc;
}

static void timer_cb(void *arg) 
{
    LOG(LL_INFO, ("LIB in timer: send request to UART%d", uns4i_uart_inner__.uart ) );
    uart_send_request();
    (void) arg;
}

bool mgos_mgos_ati_uns4i_init(void)
{ 
    mgos_set_timer(1000 /* ms */, true /* repeat */, timer_cb, NULL /* arg */);
    return true; 
}
