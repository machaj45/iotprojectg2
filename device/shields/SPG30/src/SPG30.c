#include "platform.h"
#include "SPG30.h"

struct OCTA_header SPG30_Header;

uint8_t SPG30_Initialize(void) {
    printf("****_____Initalize SPG30 now!!____****\r\n");
    #ifndef SPG30_CONNECTOR
        printf("No SPG30_CONNECTOR provided in Makefile \r\n");
        return 0;
    #else
        SPG30_Header = platform_getHeader((uint8_t)SPG30_CONNECTOR); //this connector is specified in de makeFile of the application (in this case 1)
        if (!SPG30_Header.active) {
            printf("Invalid SPG30_CONNECTOR provided in Makefile\r\n");
            return 0; 
        }
        else 
            printf("SPG30 sensor on P%d, initializing I2C\r\n", (uint8_t)SPG30_CONNECTOR);
    #endif
    return 1;

}


