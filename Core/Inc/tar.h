#ifndef TAR_H
#define TAR_H
#include <stdbool.h>
#include <stddef.h>

typedef void (*progress_callback_t)(unsigned int percentage, const char *file_name);

bool extract_tar(const char *tar_path, const char *dest_path, size_t data_offset, progress_callback_t progress_callback);

#endif // TAR_H