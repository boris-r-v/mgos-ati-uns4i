#ifndef MGOS_ATI_UNS4i_HANDLER
#define MGOS_ATI_UNS4i_HANDLER

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stdint.h>

#include "mgos.h"
#include "mjs.h"


struct uns4i_trc{
    float VDC1, VAC1, VAC1f420, VAC1f480, VAC1f580, VAC1f720, VAC1f780;
    uint16_t F1cur, F1mod;
    float VDC2, VAC2, VAC2f420, VAC2f480, VAC2f580, VAC2f720, VAC2f780;
    uint16_t F2cur, F2mod;
    float VDC3, VAC3, VAC3f420, VAC3f480, VAC3f580, VAC3f720, VAC3f780;
    uint16_t F3cur, F3mod;
    float VDC4, VAC4, VAC4f420, VAC4f480, VAC4f580, VAC4f720, VAC4f780;
    uint16_t F4cur, F4mod;
    uint16_t uns_mode, uns_diag;
};

//struct mjs_c_struct_member const* get_uns4i_trc_descr(void);

char const* get_request_string( uint8_t _addr, uint8_t _code_isol, uint8_t _amplif );
size_t get_request_size( void );

bool is_crc_good( char const* _answer, size_t _len );

struct uns4i_trc const* get_uns4i_data( char const*  _answer, size_t _len );







#ifdef __cplusplus
}   /*extern "C"*/
#endif  /*__cplusplus*/
#endif  /*MGOS_ATI_UNS4i_HANDLER*/
