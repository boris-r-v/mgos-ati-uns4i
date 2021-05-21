let UNS4i_TRC = {
    _uns4i_answer_descr: ffi("void *get_uns4i_trc_descr()"),
    _get_uns4i_data: ffi( "void* get_uns4i_data( char*, int )" ),
    
    is_crc_good: ffi( "bool is_crc_good( char*, int )" ),
    
    get_uns4i_request: ffi("char* get_request_string( int, int, int )"),

    get_uns4i_data: function( str, len ) {
        return s2o( UNS4i_TRC._get_uns4i_data( str, len ),  UNS4i_TRC._uns4i_answer_descr );
    },

    get_request: ffi( "char* get_request( int ) " ) 
}
