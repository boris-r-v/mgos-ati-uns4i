let UNS4i_TRC = {
    _uns4i_answer_descr: ffi("void *get_uns4i_trc_descr()"),
    _get_uns4i_data: ffi( "void* get_uns4i_data( char const*  _answer, size_t _len )" ),
    
    is_crc_good: ffi( "bool is_crc_good( char const* _answer, size_t _len )" ),
    
    get_uns4i_data: function( str, len ) {
        return s2o( UNS4i_TRC._get_uns4i_data( str, len ),  UNS4i_TRC._uns4i_answer_descr );
    }
}