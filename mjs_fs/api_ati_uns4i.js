let UNS4i = {

    // ## **`UNS4i.create(uart_no, address)`**
    // Create a USN4i handler.
    // A handler receives 2 parameters: UNS4i handle uart number [0,1,2,etc] and uns2i address [from 0 to 255]
    // Return value: none.
    //
    // Example:
    // ```javascript
    // load('api_ati_uns4i.js');
    //      ...
    // let handler = UNS4i.create( 1, 32 );
    // ```
    init: ffi( 'void init_handler(int, int)' ),


    _get: ffi( 'void* get_data(void)' ),
    _descr: ffi('void* get_uns4i_data_descr(void)'),
    // ## **`UNS4i.get()`**
    // Get a USN4i sizing data.
    // Has no paramtrs
    // Return value: none.
    //
    // Example:
    // ```javascript
    // load('api_ati_uns4i.js');
    //      ...
    // let data = UNS4i.get( );
    // print ( data.VDC1 )
    // ```
    get: function() {
        return s2o( UNS4i._get(), UNS4i._descr() )
    },

};

