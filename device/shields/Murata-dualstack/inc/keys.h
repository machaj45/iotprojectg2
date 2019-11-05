#ifdef LORAWAN_APPLICATION_iowttn
    #define LORAWAN_APP_KEY { 0x06, 0x24, 0xF2, 0x7F, 0xC4, 0xF5, 0xED, 0xB0, 0x80, 0xC9, 0x98, 0x6D, 0x2B, 0x89, 0x1C, 0x83 }
    #define LORAWAN_APP_EUI { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x01, 0xDF, 0x1B }
    #define LORAWAN_APP_NAME "IoW The Things Network"
#endif

#ifdef LORAWAN_APPLICATION_iowproximus
    //80111111111111111111111111111122
    #define LORAWAN_APP_KEY { 0x80, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x22 }
    //8000000000001111
    #define LORAWAN_APP_EUI { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11 }
    #define LORAWAN_APP_NAME "IoW Proximus"
#endif

#ifdef LORAWAN_APPLICATION_lpiot
    //80111111111111111111111111111122
    #define LORAWAN_APP_KEY { 0xE1, 0x1F, 0x02, 0x0D, 0xA3, 0xCC, 0xFB, 0x4D, 0x23, 0x18, 0xC0, 0xA9, 0xC4, 0x00, 0x86, 0x4A }
    //8000000000001111
    #define LORAWAN_APP_EUI  { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x02, 0x37, 0x4D }
    #define LORAWAN_APP_NAME "LP IoT Test"
#endif