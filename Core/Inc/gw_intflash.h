#pragma once

#include "stdbool.h"

typedef void (*flash_progress_callback_t)(unsigned int percentage);

bool update_intflash(uint8_t bank, const char *path, flash_progress_callback_t progress_callback);
