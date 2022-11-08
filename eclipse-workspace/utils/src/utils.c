#include "./net_utils/proprietary_pkt_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <string.h>
#include "net_utils/netinet_in.h"

int main(void) {
  #ifdef CHECKSUM_TEST
  test_checksum();
  #endif
  printf("test");
	return EXIT_SUCCESS;
}
