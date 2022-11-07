#include "./net_utils/checksum.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <string.h>

int main(void) {
  #ifdef CHECKSUM_TEST
  test_checksum();
  #endif
	return EXIT_SUCCESS;
}
