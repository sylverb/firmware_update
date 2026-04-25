#include "qr_code.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define QR_VERSION          3
#define QR_DATA_CODEWORDS   44
#define QR_ECC_CODEWORDS    26
#define QR_TOTAL_CODEWORDS  (QR_DATA_CODEWORDS + QR_ECC_CODEWORDS)
#define QR_ECL_FORMAT_BITS  0 /* Error correction M */

static uint8_t gf_mul(uint8_t x, uint8_t y)
{
    uint16_t z = 0;
    uint16_t xx = x;

    while (y) {
        if (y & 1)
            z ^= xx;
        y >>= 1;
        xx <<= 1;
        if (xx & 0x100)
            xx ^= 0x11D;
    }
    return (uint8_t)z;
}

static void rs_compute_divisor(uint8_t divisor[QR_ECC_CODEWORDS])
{
    uint8_t root = 1;

    memset(divisor, 0, QR_ECC_CODEWORDS);
    divisor[QR_ECC_CODEWORDS - 1] = 1;

    for (int i = 0; i < QR_ECC_CODEWORDS; i++) {
        for (int j = 0; j < QR_ECC_CODEWORDS; j++) {
            divisor[j] = gf_mul(divisor[j], root);
            if (j + 1 < QR_ECC_CODEWORDS)
                divisor[j] ^= divisor[j + 1];
        }
        root = gf_mul(root, 0x02);
    }
}

static void rs_compute_remainder(const uint8_t data[QR_DATA_CODEWORDS],
                                 uint8_t ecc[QR_ECC_CODEWORDS])
{
    uint8_t divisor[QR_ECC_CODEWORDS];

    rs_compute_divisor(divisor);
    memset(ecc, 0, QR_ECC_CODEWORDS);

    for (int i = 0; i < QR_DATA_CODEWORDS; i++) {
        uint8_t factor = data[i] ^ ecc[0];
        memmove(&ecc[0], &ecc[1], QR_ECC_CODEWORDS - 1);
        ecc[QR_ECC_CODEWORDS - 1] = 0;
        for (int j = 0; j < QR_ECC_CODEWORDS; j++)
            ecc[j] ^= gf_mul(divisor[j], factor);
    }
}

static bool append_bits(uint8_t data[QR_DATA_CODEWORDS], int *bit_len,
                        uint32_t val, int len)
{
    if (*bit_len + len > QR_DATA_CODEWORDS * 8)
        return false;

    for (int i = len - 1; i >= 0; i--) {
        if ((val >> i) & 1)
            data[*bit_len >> 3] |= (uint8_t)(0x80 >> (*bit_len & 7));
        (*bit_len)++;
    }
    return true;
}

static bool encode_data(const char *text, uint8_t codewords[QR_TOTAL_CODEWORDS])
{
    uint8_t data[QR_DATA_CODEWORDS];
    size_t len = strlen(text);
    int bit_len = 0;

    if (len > QR_CODE_MAX_TEXT_LEN)
        return false;

    memset(data, 0, sizeof(data));
    if (!append_bits(data, &bit_len, 0x4, 4)) /* byte mode */
        return false;
    if (!append_bits(data, &bit_len, (uint32_t)len, 8))
        return false;
    for (size_t i = 0; i < len; i++) {
        if (!append_bits(data, &bit_len, (uint8_t)text[i], 8))
            return false;
    }

    int terminator = QR_DATA_CODEWORDS * 8 - bit_len;
    if (terminator > 4)
        terminator = 4;
    if (!append_bits(data, &bit_len, 0, terminator))
        return false;
    while ((bit_len & 7) != 0) {
        if (!append_bits(data, &bit_len, 0, 1))
            return false;
    }

    for (int i = bit_len >> 3, pad = 0; i < QR_DATA_CODEWORDS; i++, pad ^= 1)
        data[i] = pad ? 0x11 : 0xEC;

    memcpy(codewords, data, QR_DATA_CODEWORDS);
    rs_compute_remainder(data, &codewords[QR_DATA_CODEWORDS]);
    return true;
}

static void set_function_module(uint8_t modules[QR_CODE_SIZE][QR_CODE_SIZE],
                                uint8_t function[QR_CODE_SIZE][QR_CODE_SIZE],
                                int x, int y, bool dark)
{
    if (x < 0 || x >= QR_CODE_SIZE || y < 0 || y >= QR_CODE_SIZE)
        return;

    modules[y][x] = dark ? 1 : 0;
    function[y][x] = 1;
}

static void draw_finder(uint8_t modules[QR_CODE_SIZE][QR_CODE_SIZE],
                        uint8_t function[QR_CODE_SIZE][QR_CODE_SIZE],
                        int x, int y)
{
    for (int dy = -1; dy <= 7; dy++) {
        for (int dx = -1; dx <= 7; dx++) {
            bool in_finder = dx >= 0 && dx <= 6 && dy >= 0 && dy <= 6;
            bool border = dx == 0 || dx == 6 || dy == 0 || dy == 6;
            bool center = dx >= 2 && dx <= 4 && dy >= 2 && dy <= 4;
            set_function_module(modules, function, x + dx, y + dy,
                                in_finder && (border || center));
        }
    }
}

static void draw_alignment(uint8_t modules[QR_CODE_SIZE][QR_CODE_SIZE],
                           uint8_t function[QR_CODE_SIZE][QR_CODE_SIZE],
                           int cx, int cy)
{
    for (int dy = -2; dy <= 2; dy++) {
        for (int dx = -2; dx <= 2; dx++) {
            int dist = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
            set_function_module(modules, function, cx + dx, cy + dy, dist != 1);
        }
    }
}

static uint16_t format_bits_for_mask(int mask)
{
    uint16_t data = (QR_ECL_FORMAT_BITS << 3) | (uint16_t)mask;
    uint16_t rem = data;

    for (int i = 0; i < 10; i++)
        rem = (uint16_t)((rem << 1) ^ (((rem >> 9) & 1) ? 0x537 : 0));

    return (uint16_t)(((data << 10) | (rem & 0x03FF)) ^ 0x5412);
}

static void draw_format(uint8_t modules[QR_CODE_SIZE][QR_CODE_SIZE],
                        uint8_t function[QR_CODE_SIZE][QR_CODE_SIZE],
                        int mask)
{
    uint16_t bits = format_bits_for_mask(mask);

    for (int i = 0; i < 15; i++) {
        int y = i < 6 ? i : i < 8 ? i + 1 : QR_CODE_SIZE - 15 + i;
        set_function_module(modules, function, 8, y, (bits >> i) & 1);
    }

    for (int i = 0; i < 15; i++) {
        int x = i < 8 ? QR_CODE_SIZE - 1 - i : i == 8 ? 7 : 14 - i;
        set_function_module(modules, function, x, 8, (bits >> i) & 1);
    }

    set_function_module(modules, function, 8, QR_CODE_SIZE - 8, true);
}

static void draw_function_patterns(uint8_t modules[QR_CODE_SIZE][QR_CODE_SIZE],
                                   uint8_t function[QR_CODE_SIZE][QR_CODE_SIZE])
{
    draw_finder(modules, function, 0, 0);
    draw_finder(modules, function, QR_CODE_SIZE - 7, 0);
    draw_finder(modules, function, 0, QR_CODE_SIZE - 7);

    for (int i = 8; i < QR_CODE_SIZE - 8; i++) {
        bool dark = (i & 1) == 0;
        set_function_module(modules, function, i, 6, dark);
        set_function_module(modules, function, 6, i, dark);
    }

    draw_alignment(modules, function, 22, 22);
    draw_format(modules, function, 0);
}

static bool mask_bit(int mask, int x, int y)
{
    switch (mask) {
    case 0: return ((x + y) & 1) == 0;
    case 1: return (y & 1) == 0;
    case 2: return x % 3 == 0;
    case 3: return (x + y) % 3 == 0;
    case 4: return (((y / 2) + (x / 3)) & 1) == 0;
    case 5: return ((x * y) % 2 + (x * y) % 3) == 0;
    case 6: return (((x * y) % 2 + (x * y) % 3) & 1) == 0;
    default: return (((x + y) % 2 + (x * y) % 3) & 1) == 0;
    }
}

static void draw_codewords(uint8_t modules[QR_CODE_SIZE][QR_CODE_SIZE],
                           const uint8_t function[QR_CODE_SIZE][QR_CODE_SIZE],
                           const uint8_t codewords[QR_TOTAL_CODEWORDS],
                           int mask)
{
    int bit_index = 0;

    for (int right = QR_CODE_SIZE - 1; right >= 1; right -= 2) {
        if (right == 6)
            right = 5;

        for (int vert = 0; vert < QR_CODE_SIZE; vert++) {
            int y = (((right + 1) & 2) == 0) ? QR_CODE_SIZE - 1 - vert : vert;
            for (int j = 0; j < 2; j++) {
                int x = right - j;
                bool dark = false;

                if (function[y][x])
                    continue;

                if (bit_index < QR_TOTAL_CODEWORDS * 8)
                    dark = (codewords[bit_index >> 3] >> (7 - (bit_index & 7))) & 1;
                bit_index++;

                if (mask_bit(mask, x, y))
                    dark = !dark;
                modules[y][x] = dark ? 1 : 0;
            }
        }
    }
}

static int score_adjacent(const uint8_t modules[QR_CODE_SIZE][QR_CODE_SIZE], bool vertical)
{
    int penalty = 0;

    for (int i = 0; i < QR_CODE_SIZE; i++) {
        int run_color = -1;
        int run_len = 0;
        for (int j = 0; j < QR_CODE_SIZE; j++) {
            int color = vertical ? modules[j][i] : modules[i][j];
            if (color == run_color) {
                run_len++;
                if (run_len == 5)
                    penalty += 3;
                else if (run_len > 5)
                    penalty++;
            } else {
                run_color = color;
                run_len = 1;
            }
        }
    }
    return penalty;
}

static int score_modules(const uint8_t modules[QR_CODE_SIZE][QR_CODE_SIZE])
{
    int penalty = score_adjacent(modules, false) + score_adjacent(modules, true);
    int dark = 0;

    for (int y = 0; y < QR_CODE_SIZE; y++) {
        for (int x = 0; x < QR_CODE_SIZE; x++) {
            if (modules[y][x])
                dark++;
            if (x + 1 < QR_CODE_SIZE && y + 1 < QR_CODE_SIZE) {
                int color = modules[y][x];
                if (modules[y][x + 1] == color && modules[y + 1][x] == color &&
                    modules[y + 1][x + 1] == color)
                    penalty += 3;
            }
        }
    }

    for (int y = 0; y < QR_CODE_SIZE; y++) {
        for (int x = 0; x + 10 < QR_CODE_SIZE; x++) {
            if (modules[y][x] && !modules[y][x + 1] && modules[y][x + 2] &&
                modules[y][x + 3] && modules[y][x + 4] && !modules[y][x + 5] &&
                modules[y][x + 6] && !modules[y][x + 7] && !modules[y][x + 8] &&
                !modules[y][x + 9] && !modules[y][x + 10])
                penalty += 40;
            if (!modules[y][x] && !modules[y][x + 1] && !modules[y][x + 2] &&
                !modules[y][x + 3] && modules[y][x + 4] && !modules[y][x + 5] &&
                modules[y][x + 6] && modules[y][x + 7] && modules[y][x + 8] &&
                !modules[y][x + 9] && modules[y][x + 10])
                penalty += 40;
        }
    }
    for (int x = 0; x < QR_CODE_SIZE; x++) {
        for (int y = 0; y + 10 < QR_CODE_SIZE; y++) {
            if (modules[y][x] && !modules[y + 1][x] && modules[y + 2][x] &&
                modules[y + 3][x] && modules[y + 4][x] && !modules[y + 5][x] &&
                modules[y + 6][x] && !modules[y + 7][x] && !modules[y + 8][x] &&
                !modules[y + 9][x] && !modules[y + 10][x])
                penalty += 40;
            if (!modules[y][x] && !modules[y + 1][x] && !modules[y + 2][x] &&
                !modules[y + 3][x] && modules[y + 4][x] && !modules[y + 5][x] &&
                modules[y + 6][x] && modules[y + 7][x] && modules[y + 8][x] &&
                !modules[y + 9][x] && modules[y + 10][x])
                penalty += 40;
        }
    }

    int total = QR_CODE_SIZE * QR_CODE_SIZE;
    int k = (dark * 20 - total * 10);
    if (k < 0)
        k = -k;
    penalty += (k / total) * 10;

    return penalty;
}

bool qr_code_generate(const char *text, uint8_t modules[QR_CODE_SIZE][QR_CODE_SIZE])
{
    uint8_t codewords[QR_TOTAL_CODEWORDS];
    uint8_t base[QR_CODE_SIZE][QR_CODE_SIZE];
    uint8_t function[QR_CODE_SIZE][QR_CODE_SIZE];
    int best_score = 0x7FFFFFFF;
    int best_mask = 0;

    if (!text || !modules)
        return false;
    if (!encode_data(text, codewords))
        return false;

    memset(base, 0, sizeof(base));
    memset(function, 0, sizeof(function));
    draw_function_patterns(base, function);

    for (int mask = 0; mask < 8; mask++) {
        uint8_t candidate[QR_CODE_SIZE][QR_CODE_SIZE];
        memcpy(candidate, base, sizeof(candidate));
        draw_codewords(candidate, function, codewords, mask);
        draw_format(candidate, function, mask);

        int score = score_modules(candidate);
        if (score < best_score) {
            best_score = score;
            best_mask = mask;
            memcpy(modules, candidate, sizeof(candidate));
        }
    }

    (void)best_mask;
    return true;
}
