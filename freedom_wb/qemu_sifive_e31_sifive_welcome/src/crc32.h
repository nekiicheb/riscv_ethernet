#ifndef NET_UTILS_CRC32_H_
#define NET_UTILS_CRC32_H_

#include <stdint.h>
#include <stdlib.h>
uint32_t xcrc32 (const uint8_t *buf, size_t len, uint32_t init);

#endif /* NET_UTILS_CRC32_H_ */
