#include "platform.h"
#include "murata.h"

uint8_t murata_notification_sent = 0;
struct OCTA_header murataHeader;

session_config_t session_config =
    {
        .interface_type = LORAWAN_OTAA,
        .lorawan_session_config_otaa = {
            .devEUI = { 0x00, 0xF6, 0x28, 0x26, 0x79, 0x89, 0xF7, 0xAF },
            .appEUI = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x02, 0x2F, 0xB6 },
            .appKey = { 0x3B, 0x1A, 0x09, 0x04, 0x4D, 0x65, 0xD4, 0xE3, 0x23, 0x03, 0x3D, 0x12, 0xF0, 0x1E, 0xBE, 0x9C },
            .request_ack = false,
            .application_port = 1}};

/* Init murata lorawan modem */
modem_callbacks_t modem_callbacks = {
    .command_completed_callback = &on_modem_command_completed_callback,
    .return_file_data_callback = &on_modem_return_file_data_callback,
    .write_file_data_callback = &on_modem_write_file_data_callback,
    .modem_rebooted_callback = &on_modem_reboot_callback,
};

void on_modem_command_completed_callback(bool with_error, uint8_t tag_id)
{
    murata_notification_sent = 0;
    printf("LoRaWAN modem command with tag %i completed (success = %i)\r\n", tag_id, !with_error);
}

void on_modem_return_file_data_callback(uint8_t file_id, uint32_t offset, uint32_t size, uint8_t *output_buffer)
{
    murata_notification_sent = 0;
    printf("LoRaWAN modem return file data file %i offset %li size %li buffer %p\r\n", file_id, offset, size, output_buffer);
}

void on_modem_write_file_data_callback(uint8_t file_id, uint32_t offset, uint32_t size, uint8_t *output_buffer)
{
    murata_notification_sent = 0;
    printf("LoRaWAN modem write file data file %i offset %li size %li buffer %p\r\n", file_id, offset, size, output_buffer);
}

void on_modem_reboot_callback(void)
{
    murata_notification_sent = 0;
    printf("LoRaWAN murata modem has rebooted \r\n");
    modem_reinit();
}

uint8_t Murata_Initialize(uint64_t octa_UID)
{    
    printf("***Initializing LoRaWAN modem driver***\r\n");

    #ifndef MURATA_CONNECTOR
        printf("No MURATA_CONNECTOR provided in Makefile\r\n");
        return 1;
    #else
        murataHeader = platform_getHeader((uint8_t)MURATA_CONNECTOR);
        if(!murataHeader.active)
        {
            printf("Invalid MURATA_CONNECTOR provided in Makefile\r\n");
            return 0;  
        }
        else
            printf("Murata on P%d, initializing UART\r\n", (uint8_t)MURATA_CONNECTOR);         
    #endif

    //convert_uint64_array((uint8_t *)&octa_UID);
    //memcpy(session_config.lorawan_session_config_otaa.devEUI, &octa_UID, sizeof(octa_UID));

    // Initialize UART peripheral with driver baudrate
    platform_initialize_UART(murataHeader, MURATA_BAUDRATE);

    murata_wakeup_pin = murataHeader.DIO1;
    murata_reset_pin = murataHeader.DIO6;
    modem_init(murataHeader.uartHandle);
    modem_cb_init(&modem_callbacks);

    printf("Murata module init OK \r\n\r\n");
    return 1;
}

void modem_wakeup(void)
{
    HAL_GPIO_WritePin(murata_wakeup_pin->PORT, murata_wakeup_pin->PIN, GPIO_PIN_SET);
}

void modem_release(void)
{
    HAL_GPIO_WritePin(murata_wakeup_pin->PORT, murata_wakeup_pin->PIN, GPIO_PIN_SET);
}

void Murata_toggleResetPin(void)
{
    HAL_GPIO_WritePin(murata_reset_pin->PORT, murata_reset_pin->PIN, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(murata_reset_pin->PORT, murata_reset_pin->PIN, GPIO_PIN_SET);
    HAL_Delay(1000);
}

uint8_t Murata_SetProcessingThread(osThreadId aThreadId)
{
    threadToNotify = aThreadId;
    return 1;
}

uint8_t Murata_LoRaWAN_Send(uint8_t *buffer, uint8_t length)
{
    
    modem_reinit();
    uint8_t status = 0;
    for(int i = 0;i<length;i++){
      printf("Sending: %x\n\r",buffer[i]);
    }
    status = modem_send_unsolicited_response(0x40, 0, length, (uint8_t *)buffer, &session_config);
    printf("Sending LoRaWAN message with payload size %d\r\n", length);
    return status;
}
void Murata_process_fifo(void)
{
    modem_process_fifo();
}

void Murata_rxCallback(void)
{
    modem_interface_uart_rx_cb();
    if(!murata_notification_sent)
    {
        //start rx processing thread
        RTOS_Send_Notification(threadToNotify);
        murata_notification_sent = 1;
    }
}
