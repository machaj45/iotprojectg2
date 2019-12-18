#include "platform.h"
#include "murata.h"
#include <stdio.h>

uint8_t            use_scheduler = 0;
volatile uint8_t   murata_successful;
struct OCTA_header murataHeader;

extern volatile activeSending;

session_config_t session_config_lora = {.interface_type              = LORAWAN_OTAA,
                                        .lorawan_session_config_otaa = {.devEUI           = LORAWAN_DEV_EUI,
                                                                        .appEUI           = LORAWAN_APP_EUI,
                                                                        .appKey           = LORAWAN_APP_KEY,
                                                                        .request_ack      = false,
                                                                        .application_port = 1,
                                                                        .adr_enabled      = true,
                                                                        .data_rate        = 0}};

// change qos_resp_mode to  SESSION_RESP_MODE_PREFERRE Dif you want all gateways to receive your message
// SESSION_RESP_MODE_PREFERRED to only communicate with one (the preferred) gateway
session_config_t session_config_d7 = {.interface_type      = DASH7,
                                      .d7ap_session_config = {
                                          .qos             = {.qos_resp_mode  = SESSION_RESP_MODE_ALL,  // SESSION_RESP_MODE_ALL,
                                                  .qos_retry_mode = SESSION_RETRY_MODE_NO},
                                          .dormant_timeout = 0,
                                          .addressee =
                                              {
                                                  .ctrl         = {.nls_method = AES_NONE, .id_type = ID_TYPE_NOID},
                                                  .access_class = 0x01,
                                                  .id           = {0},
                                              },
                                      }};

/* Init murata lorawan modem */
modem_callbacks_t modem_callbacks = {
    .command_completed_callback      = &on_modem_command_completed_callback,
    .return_file_data_callback       = &on_modem_return_file_data_callback,
    .write_file_data_callback        = &on_modem_write_file_data_callback,
    .modem_interface_status_callback = &on_modem_interface_status_callback,
    .modem_rebooted_callback         = &on_modem_reboot_callback,
};

void on_modem_command_completed_callback(bool with_error, uint8_t tag_id) {
  printf("Murata modem command with tag %i completed (success = %i)\r\n", tag_id, !with_error);
  if (!with_error)
  {
    murata_successful = 0;
    activeSending = 0;
  // tag_id=0;
  // Murata_toggleResetPin();
  // HAL_Delay(500);
  // Murata_LoRaWAN_Join();
  } 
}

void on_modem_return_file_data_callback(uint8_t file_id, uint32_t offset, uint32_t size, uint8_t *output_buffer) {
  printf("Murata modem return file data file %i offset %li size %li buffer %p\r\n", file_id, offset, size, output_buffer);
}

void on_modem_write_file_data_callback(uint8_t file_id, uint32_t offset, uint32_t size, uint8_t *output_buffer) {
  printf("Murata modem write file data file %i offset %li size %li buffer %p\r\n", file_id, offset, size, output_buffer);
}

void on_modem_interface_status_callback(alp_itf_id_t interface_type, uint8_t *data) {
  if ((interface_type == ALP_ITF_ID_LORAWAN_ABP) || (interface_type == ALP_ITF_ID_LORAWAN_OTAA)) {
    lorawan_session_result_t interface_status = *((lorawan_session_result_t *)data);
    
    printf("LoRaWAN interface status: attemps = %d, error state = %d, duty cycle wait time = %d \r\n", interface_status.attempts, interface_status.error_state,
           interface_status.duty_cycle_wait_time);
    switch (interface_status.error_state) {
      case 0:
        printf("OK\n\r");
        murata_successful = 0;
        break;
      case 1:
        printf("NOT_JOINED\n\r");
        break;
      case 2:
        printf("TX_NOT_POSSIBLE\n\r");
        break;
      case 3:
        printf("UNKNOWN\n\r");
        break;
      case 4:
        printf("NACK\n\r");
        break;
      case 5:
        printf("JOIN_FAILED\n\r");
        break;
      case 6:
        printf("DUTY_CYCLE_DELAY\n\r");
        break;
      case 7:
        printf("RETRY_TRANSMISSION\n\r");
        break;
      case 8:
        printf("JOINED\n\r");
        murata_successful = 3;
        break;
    }
  } else if (interface_type == ALP_ITF_ID_D7ASP) {
    d7ap_session_result_t interface_status = *((d7ap_session_result_t *)data);
    printf(
        "Dash7 interface status: channel.header =  %d, channel.center_freq_index = %d, rx_level = %d, link_budget = %d, link_quality = %d, target_rx_level = "
        "%d, fifo_token = %d, seqnr = %d, response_to = %d, response_expected = %d \r\n",
        interface_status.channel.channel_header, interface_status.channel.center_freq_index, interface_status.rx_level, interface_status.link_budget,
        interface_status.link_quality, interface_status.target_rx_level, interface_status.fifo_token, interface_status.seqnr, interface_status.response_to,
        interface_status.response_expected);
  }
}

void on_modem_reboot_callback(void) {
  printf("Murata murata-dual modem has rebooted \r\n");
  modem_reinit();
  murata_successful = 0;
}

// TODO: use_scheduler in makefiles
uint8_t Murata_Initialize(uint64_t octa_UID, uint8_t use_RTOS) {

#ifndef MURATA_CONNECTOR
  printf("No MURATA_CONNECTOR provided in Makefile\r\n");
  return 1;
#else
  murataHeader = platform_getHeader((uint8_t)MURATA_CONNECTOR);
  if (!murataHeader.active) {
    printf("Invalid MURATA_CONNECTOR provided in Makefile\r\n");
    return 0;
  } else
#endif
#ifdef LORAWAN_APP_NAME
#endif

  // copy OCTA UID into lorawan otaa DEV EUI
  memcpy(session_config_lora.lorawan_session_config_otaa.devEUI, &octa_UID, sizeof(octa_UID));

  // Initialize UART peripheral with driver baudrate
  platform_initialize_UART(murataHeader, MURATA_BAUDRATE);

  murata_wakeup_pin = murataHeader.DIO1;
  murata_reset_pin  = murataHeader.DIO6;

  modem_interface_init(murataHeader.uartHandle);
  modem_cb_init(&modem_callbacks);

  use_scheduler = use_RTOS;

  // printf("Murata module init OK \r\n\r\n");
  return 1;
}

void murata_wakeup(void) {
  HAL_GPIO_WritePin(murata_wakeup_pin->PORT, murata_wakeup_pin->PIN, GPIO_PIN_SET);
}

void murata_release(void) {
  HAL_GPIO_WritePin(murata_wakeup_pin->PORT, murata_wakeup_pin->PIN, GPIO_PIN_SET);
}

void Murata_toggleResetPin(void) {
  HAL_GPIO_WritePin(murata_reset_pin->PORT, murata_reset_pin->PIN, GPIO_PIN_RESET);
  HAL_Delay(50);
  HAL_GPIO_WritePin(murata_reset_pin->PORT, murata_reset_pin->PIN, GPIO_PIN_SET);
  HAL_Delay(1000);
}

uint8_t Murata_SetProcessingThread(osThreadId aThreadId) {
  threadToNotify = aThreadId;
  return 1;
}

uint8_t Murata_LoRaWAN_Join(void) {
  uint8_t status = 0;
  status         = modem_send_unsolicited_response(0x40, 0, 0, 0, &session_config_lora);
  // printf("Joining the LoRaWAN Network\r\n");
  return status;
}

uint8_t Murata_LoRaWAN_Send(uint8_t *buffer, uint8_t length) {
  uint8_t status = 0;
  status         = modem_send_unsolicited_response(0x40, 0, length, (uint8_t *)buffer, &session_config_lora);
 printf("Sending LoRaWAN message with payload size %d\r\n", length);
  return status;
}


uint8_t Murata_Dash7_Send(uint8_t *buffer, uint8_t length) {
  uint8_t status = 0;
  status         = modem_send_unsolicited_response(0x40, 0, length, (uint8_t *)buffer, &session_config_d7);
  printf("Dash7 message of size: %d B and data: \n [0x%x, 0x%x, 0x%x 0x%x, 0x%x, 0x%x 0x%x, 0x%x, 0x%x 0x%x, 0x%x, 0x%x 0x%x 0x%x] and status is %d\r\n", length,buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],buffer[8],buffer[9],buffer[10],buffer[11],buffer[12],buffer[13],status);
  return status;
}

uint8_t Murata_process_fifo(void) {
  return modem_process_fifo();
}

void Murata_rxCallback(void)
{
    uart_rx_cb();
    if(use_scheduler)
    {
        //start rx processing thread
    //    RTOS_Send_Notification(threadToNotify);
    }
}
