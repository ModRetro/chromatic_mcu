#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

size_t crc8_sae_j1850_encode(const uint8_t *pMsg, const size_t Length, uint8_t *const pOutput);
bool crc8_sae_j1850_decode(const uint8_t *const pMsg, const size_t Length);
