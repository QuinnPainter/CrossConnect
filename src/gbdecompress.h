#ifndef GBDECOMPRESS_H
#define GBDECOMPRESS_H

#include "stdint.h"

/** gb-decompress data from sour into dest
    @param sour   Pointer to source gb-compressed data
    @param dest   Pointer to destination buffer/address
    @returns      Decompressed data size
 */
uint16_t gb_decompress(const uint8_t* sour, uint8_t* dest);

#endif
