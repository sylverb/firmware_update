#pragma once

#include <stdbool.h>
#include <stdint.h>

#define QR_CODE_SIZE         29
#define QR_CODE_QUIET_ZONE   4
#define QR_CODE_TOTAL_SIZE   (QR_CODE_SIZE + QR_CODE_QUIET_ZONE * 2)
#define QR_CODE_MAX_TEXT_LEN 42

bool qr_code_generate(const char *text, uint8_t modules[QR_CODE_SIZE][QR_CODE_SIZE]);
