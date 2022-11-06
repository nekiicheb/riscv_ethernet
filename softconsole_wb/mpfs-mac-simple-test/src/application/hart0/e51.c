/*
 * code running on e51
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "mpfs_hal/mss_hal.h"
#include "mpfs_hal/common/nwc/mss_nwc_init.h"

#include "drivers/mss/mss_gpio/mss_gpio.h"
#include "drivers/mss/mss_mmuart/mss_uart.h"

#include "drivers/mss/mss_ethernet_mac/mss_ethernet_registers.h"
#include "drivers/mss/mss_ethernet_mac/mss_ethernet_mac_sw_cfg.h"
#include "drivers/mss/mss_ethernet_mac/mss_ethernet_mac_regs.h"
#include "drivers/mss/mss_ethernet_mac/mss_ethernet_mac.h"
#include "drivers/mss/mss_ethernet_mac/phy.h"
#include "drivers/mss/mss_gpio/mss_gpio.h"
#include "inc/common.h"

#include <stdbool.h>
#define PRINT_STRING(x) MSS_UART_polled_tx_string(&g_mss_uart0_lo, (uint8_t *)x);

volatile bool is_available_rx_pkt = false;
enum BufferState
{
  BUSY = 0,
  FREE,
} BufferState;

volatile enum BufferState buffer_state = FREE;

#define PACKET_MAX   16384U
volatile struct eth_pkt
{
  size_t length;
  uint8_t data[PACKET_MAX];
} eth_pkt;
struct eth_pkt packet_data;

/*
 * Align these on an 8 byte boundary as we might be using IEEE 1588 time
 * stamping and that uses b2 of the buffer pointer to indicate that a timestamp
 * is present in this descriptor.
 */
static uint8_t g_mac_rx_buffer[MSS_MAC_RX_RING_SIZE][MSS_MAC_MAX_RX_BUF_SIZE] __attribute__ ((aligned (16)));

mss_mac_cfg_t g_mac_config;

/*==============================================================================
 * Network configuration globals.
 */
extern void init_memory(void);

typedef struct aligned_tx_buf {
  uint64_t aligner;
  uint8_t packet[MSS_MAC_MAX_PACKET_SIZE];
} ALIGNED_TX_BUF;

ALIGNED_TX_BUF tx_packet;
static volatile uint64_t tx_count = 0;



mss_mac_instance_t *g_test_mac = &g_mac1;

/**============================================================================
 *
 */
static void packet_tx_complete_handler(/* mss_mac_instance_t*/void *this_mac,
    uint32_t queue_no, mss_mac_tx_desc_t *cdesc, void *caller_info)
{
  (void) caller_info;
  (void) cdesc;
  (void) this_mac;
  (void) queue_no;

  tx_count++;
  buffer_state = FREE;
}

static volatile uint64_t rx_count = 0;
/**=============================================================================
 Bottom-half of receive packet handler
 */
static void mac_rx_callback(
/* mss_mac_instance_t */void *this_mac, uint32_t queue_no, uint8_t *p_rx_packet,
    uint32_t pckt_length, mss_mac_rx_desc_t *cdesc, void *caller_info)
{
  (void) caller_info;
  (void) cdesc;
  (void) queue_no;

  if(buffer_state != FREE)
  {
    /*
     * At this point, we have received two packets and not re-sent
     * them. We disable these RX interrupts for now as we know that
     * we cannot handle full line rate bursts of 64 byte frames
     * without starving the rest of the system due to the interrupt
     * rates involved...
     */
    g_test_mac->mac_base->INT_DISABLE = GEM_RECEIVE_COMPLETE
        | GEM_RX_USED_BIT_READ;
    //g_rx_dropped++;
  }
  else
  {
    packet_data.length = pckt_length;
    memcpy(&packet_data.data[0], p_rx_packet, pckt_length);
    buffer_state = BUSY;
    is_available_rx_pkt = true;
  }
  #if defined(MSS_MAC_UNH_TEST)
    MSS_MAC_receive_pkt_isr((mss_mac_instance_t*) this_mac);
  #else
      MSS_MAC_receive_pkt((mss_mac_instance_t *)this_mac, 0, p_rx_packet, 0, 1);
  #endif
  rx_count++;
}

/**=============================================================================
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(void)
{
  uint32_t count;
  /*-------------------------- Initialize the MAC --------------------------*/
  /*
   * Get the default configuration for the Ethernet MAC and change settings
   * to match the system/application. The default values typically changed
   * are:
   *  - interface:
   *      Specifies the interface used to connect the Ethernet MAC to the PHY.
   *      Example choice are MII, GMII, TBI.
   *  - phy_addr:
   *      Specifies the MII management interface address of the external PHY.
   *  - mac_addr:
   *      Specifies the MAC address of the device. This number should be
   *      unique on the network the device is connected to.
   *  - speed_duplex_select:
   *      Specifies the allowed speed and duplex mode for setting up a link.
   *      This can be used to specify the set of allowed speeds and duplex
   *      modes used during auto-negotiation or force the link speed to a
   *      specific speed and duplex mode.
   */
  MSS_MAC_cfg_struct_def_init(&g_mac_config);

  g_mac_config.speed_duplex_select = MSS_MAC_ANEG_ALL_SPEEDS;
  g_mac_config.mac_addr[0] = 0x00;
  g_mac_config.mac_addr[1] = 0xFC;
  g_mac_config.mac_addr[2] = 0x00;
  g_mac_config.mac_addr[3] = 0x12;
  g_mac_config.mac_addr[4] = 0x34;
  g_mac_config.mac_addr[5] = 0x56;

  g_mac_config.tsu_clock_select = 1U;

  /*
   * Icicle board setups with SGMII to VSC8662
   */
  g_test_mac = &g_mac0;

#if MSS_MAC_USE_PHY_DP83867
  g_mac_config.phy_extended_read = NULL_ti_read_extended_regs;
  g_mac_config.phy_extended_write = NULL_ti_write_extended_regs;
#endif

  /*
   * Initialize MAC with specified configuration. The Ethernet MAC is
   * functional after this function returns but still requires transmit and
   * receive buffers to be allocated for communications to take place.
   */
  g_test_mac = &g_mac0;
  g_mac_config.phy_addr = PHY_VSC8662_0_MDIO_ADDR;

  MSS_MAC_init(g_test_mac, &g_mac_config);

  /*
   * Not doing the tx disable/enable sequence here around the queue allocation
   * adjustment results in tx_go being set which causes the new tx code to
   * hang waiting for the last tx to complete...
   */
  g_test_mac->mac_base->NETWORK_CONTROL &= ~GEM_ENABLE_TRANSMIT;

  /* Allocate all 4 segments to queue 0 as this is our only one... */
  g_test_mac->mac_base->TX_Q_SEG_ALLOC_Q0TO3 = 2;

  g_test_mac->mac_base->NETWORK_CONTROL |= GEM_ENABLE_TRANSMIT;

  /*
   * Register MAC interrupt handler listener functions. These functions will
   * be called  by the MAC driver when a packet has been sent or received.
   * These callback functions are intended to help managing transmit and
   * receive buffers by indicating when a transmit buffer can be released or
   * a receive buffer has been filled with an rx packet.
   */

  MSS_MAC_set_tx_callback(g_test_mac, 0, packet_tx_complete_handler);
  MSS_MAC_set_rx_callback(g_test_mac, 0, mac_rx_callback);

  /*
   * Allocate receive buffers.
   *
   * We prime the pump with a full set of packet buffers and then re use them
   * as each packet is handled.
   *
   * This function will need to be called each time a packet is received to
   * hand back the receive buffer to the MAC driver.
   */
  for (count = 0; count < MSS_MAC_RX_RING_SIZE; ++count) {
    /*
     * We allocate the buffers with the Ethernet MAC interrupt disabled
     * until we get to the last one. For the last one we ensure the Ethernet
     * MAC interrupt is enabled on return from MSS_MAC_receive_pkt().
     */
    if (count != (MSS_MAC_RX_RING_SIZE - 1)) {
      MSS_MAC_receive_pkt(g_test_mac, 0, g_mac_rx_buffer[count], 0, 0);
    } else {
      MSS_MAC_receive_pkt(g_test_mac, 0, g_mac_rx_buffer[count], 0, -1);
    }
  }
}


/*==============================================================================
 *
 */
void SysTick_Handler_h0_IRQHandler(void)
{
}

/*==============================================================================
 * SSCG
 */

void e51(void) {
  write_csr(mscratch, 0);
  write_csr(mcause, 0);
  write_csr(mepc, 0);

  PLIC_init();

  mac_task(0);
}

void mac_task(void *pvParameters) {
  //init_memory();
  char info_string[200];

  (void) pvParameters;
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
  low_level_init();
  while (1)
  {
    if(is_available_rx_pkt)
    {
      //isIpPkt();
      //isCrcFromIpCorrect();
      //PRINT_STRING("Incorrect IP Crc Checksum\n\r");
      //transmitEthPkt();
      int32_t tx_status = MSS_MAC_send_pkt(g_test_mac, 0, tx_pak_arp,
                 sizeof(tx_pak_arp), (void*) 0);
      sprintf(info_string, "TX status %d\n\r", (int) tx_status);
    }
  }
}

