/*
 * code running on e51
*/
#include "project_cfg.h"
#include "./net/net.h"
#include "mpfs_hal/mss_hal.h"
#include "mpfs_hal/common/nwc/mss_nwc_init.h"
#include "drivers/mss/mss_gpio/mss_gpio.h"
#include "drivers/mss/mss_mmuart/mss_uart.h"
#include "inc/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define PRINT_STRING(x) MSS_UART_polled_tx_string(&g_mss_uart0_lo, (uint8_t *)x);

extern void init_memory(void);

//typedef struct aligned_tx_buf {
//  uint64_t aligner;
//  uint8_t packet[MSS_MAC_MAX_PACKET_SIZE];
//} ALIGNED_TX_BUF;
//
//ALIGNED_TX_BUF tx_packet;

void e51(void) {
  write_csr(mscratch, 0);
  write_csr(mcause, 0);
  write_csr(mepc, 0);

  PLIC_init();

  SYSREG->SOFT_RESET_CR = 0U;
  SYSREG->SUBBLK_CLOCK_CR = 0xFFFFFFFFUL;

  MSS_GPIO_init(GPIO2_LO);

  MSS_GPIO_config(GPIO2_LO, MSS_GPIO_16, MSS_GPIO_OUTPUT_MODE); /* LED 0 */
  MSS_GPIO_config(GPIO2_LO, MSS_GPIO_17, MSS_GPIO_OUTPUT_MODE); /* LED 1 */
  MSS_GPIO_config(GPIO2_LO, MSS_GPIO_18, MSS_GPIO_OUTPUT_MODE); /* LED 2 */
  MSS_GPIO_config(GPIO2_LO, MSS_GPIO_19, MSS_GPIO_OUTPUT_MODE); /* LED 3 */
  MSS_GPIO_config(GPIO2_LO, MSS_GPIO_26, MSS_GPIO_OUTPUT_MODE); /* PB 0 Force */
  MSS_GPIO_config(GPIO2_LO, MSS_GPIO_27, MSS_GPIO_OUTPUT_MODE); /* PB 0 Force */
  MSS_GPIO_config(GPIO2_LO, MSS_GPIO_28, MSS_GPIO_OUTPUT_MODE); /* PB 0 Force */
  MSS_GPIO_config(GPIO2_LO, MSS_GPIO_30, MSS_GPIO_INPUT_MODE); /* PB 1 */
  MSS_GPIO_config(GPIO2_LO, MSS_GPIO_31, MSS_GPIO_INPUT_MODE); /* PB 2 */

  MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_16, 0);
  MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_17, 0);
  MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_18, 0);
  MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_19, 0);
  MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_26, 0);
  MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_27, 0);
  MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_28, 0);
//    PLIC_init();
  __disable_local_irq((int8_t) MMUART0_E51_INT);
  SysTick_Config();
  MSS_UART_init(&g_mss_uart0_lo,
  MSS_UART_115200_BAUD,
  MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);
  PRINT_STRING("UART works\n\r");
  __enable_irq();
  eth_low_level_init();
  while (1)
  {
    if(is_available_rx_pkt)
    {
      // received rx ethernet packet
      //isIpPkt();
      //isCrcFromIpCorrect();
      //PRINT_STRING("Incorrect IP Crc Checksum\n\r");
      //transmitEthPkt();
      //int32_t tx_status = MSS_MAC_send_pkt(g_test_mac, 0, tx_pak_arp,
      //           sizeof(tx_pak_arp), (void*) 0);
      //sprintf(info_string, "TX status %d\n\r", (int) tx_status);
    }
  }
}

void SysTick_Handler_h0_IRQHandler(void)
{
}
