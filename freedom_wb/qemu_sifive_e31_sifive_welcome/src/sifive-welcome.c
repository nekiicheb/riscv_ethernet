#include "./netinet_in.h"
#include "./checksum.h"
#include "./checksum.h"

#include <stdio.h>
#include <metal/cpu.h>
#include <metal/led.h>
#include <metal/button.h>
#include <metal/switch.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define RTC_FREQ    32768

struct metal_cpu *cpu;
struct metal_interrupt *cpu_intr, *tmr_intr;
int tmr_id;
volatile uint32_t timer_isr_flag;

int main(void) {
  #ifdef CHECKSUM_TEST
  test_checksum();
  #endif
  printf("test");
  return 0;
}
