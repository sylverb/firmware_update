#include "gw_gui.h"
#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "gw_lcd.h"

logo_image logo_flash = {
    96,
    18,
    {
        // width96, height:18
        0x09, 0x12, 0x00, 0x01, 0xf6, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //____#__#___#__#________________#####_##_#___##__________________________________________________
        0x09, 0x12, 0x00, 0x03, 0xc0, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, //____#__#___#__#_______________####_________#___________#________________________________________
        0x00, 0x40, 0x00, 0xc3, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //_________#______________##____##__________#_____________________________________________________
        0x0f, 0xfe, 0x01, 0x42, 0x00, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, //____###########________#_#____#___________#________#____________________________________________
        0xdf, 0xff, 0x61, 0x82, 0xd8, 0xe3, 0xbe, 0xd8, 0xe0, 0x00, 0x00, 0x00, //##_#############_##____##_____#_##_##___###___###_#####_##_##___###_____________________________
        0x18, 0x03, 0x02, 0x84, 0x69, 0x25, 0x12, 0x69, 0x40, 0x00, 0x00, 0x00, //___##_________##______#_#____#___##_#__#__#__#_#___#__#__##_#__#_#______________________________
        0x18, 0x43, 0x04, 0x84, 0x4a, 0x49, 0x24, 0x52, 0x40, 0x00, 0x00, 0x00, //___##____#____##_____#__#____#___#__#_#__#__#__#__#__#___#_#__#__#______________________________
        0xd8, 0xe3, 0x60, 0x88, 0x92, 0xcb, 0xac, 0x92, 0xc0, 0x00, 0x00, 0x00, //##_##___###___##_##_____#___#___#__#__#_##__#_###_#_##__#__#__#_##______________________________
        0x19, 0xf3, 0x00, 0x70, 0xe3, 0x6d, 0x36, 0x9b, 0x52, 0x40, 0x00, 0x00, //___##__#####__##_________###____###___##_##_##_#__##_##_#__##_##_#_#__#__#______________________
        0x1b, 0xfb, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, //___##_#######_##________________#________________________________#______________________________
        0xd8, 0xe3, 0x60, 0x01, 0x00, 0x00, 0x00, 0xc4, 0x80, 0x00, 0x00, 0x00, //##_##___###___##_##____________#________________________##___#__#_______________________________
        0x18, 0xa3, 0x00, 0x03, 0xc0, 0x00, 0x01, 0x13, 0x00, 0x00, 0x00, 0x00, //___##___#_#___##______________####_____________________#___#__##________________________________
        0x19, 0x13, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, //___##__#___#__##_______________________________________#________________________________________
        0xdc, 0x47, 0x60, 0x00, 0x00, 0x00, 0x03, 0xb6, 0xed, 0xb6, 0xdd, 0xac, //##_###___#___###_##___________________________________###_##_##_###_##_##_##_##_##_###_##_#_##__
        0x0c, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x01, 0x13, 0xb6, 0x92, 0xa4, 0xf2, //____##__#_#__##________________________________________#___#__###_##_##_#__#__#_#_#__#__####__#_
        0x01, 0x50, 0x00, 0x00, 0x00, 0x00, 0x01, 0x12, 0x24, 0x96, 0x9c, 0x9c, //_______#_#_#___________________________________________#___#__#___#__#__#__#_##_#__###__#__###__
        0x0a, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x01, 0x12, 0x24, 0x89, 0x2c, 0x92, //____#_#_#_#_#_#________________________________________#___#__#___#__#__#___#__#__#_##__#__#__#_
        0x25, 0x14, 0x80, 0x00, 0x00, 0x00, 0x03, 0xbf, 0x76, 0xc9, 0x33, 0xcc, //__#__#_#___#_#__#_____________________________________###_######_###_##_##__#__#__##__####__##__
    },
};

const uint8_t font8x8_basic[96][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0020 (space)
    { 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00},   // U+0021 (!)
    { 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0022 (")
    { 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00},   // U+0023 (#)
    { 0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00},   // U+0024 ($)
    { 0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00},   // U+0025 (%)
    { 0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00},   // U+0026 (&)
    { 0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0027 (')
    { 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00},   // U+0028 (()
    { 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00},   // U+0029 ())
    { 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00},   // U+002A (*)
    { 0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00},   // U+002B (+)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06},   // U+002C (,)
    { 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00},   // U+002D (-)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00},   // U+002E (.)
    { 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00},   // U+002F (/)
    { 0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00},   // U+0030 (0)
    { 0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00},   // U+0031 (1)
    { 0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00},   // U+0032 (2)
    { 0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00},   // U+0033 (3)
    { 0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00},   // U+0034 (4)
    { 0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00},   // U+0035 (5)
    { 0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00},   // U+0036 (6)
    { 0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00},   // U+0037 (7)
    { 0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00},   // U+0038 (8)
    { 0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00},   // U+0039 (9)
    { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00},   // U+003A (:)
    { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06},   // U+003B (;)
    { 0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00},   // U+003C (<)
    { 0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00},   // U+003D (=)
    { 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00},   // U+003E (>)
    { 0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00},   // U+003F (?)
    { 0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00},   // U+0040 (@)
    { 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00},   // U+0041 (A)
    { 0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00},   // U+0042 (B)
    { 0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00},   // U+0043 (C)
    { 0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00},   // U+0044 (D)
    { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00},   // U+0045 (E)
    { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00},   // U+0046 (F)
    { 0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00},   // U+0047 (G)
    { 0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00},   // U+0048 (H)
    { 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0049 (I)
    { 0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00},   // U+004A (J)
    { 0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00},   // U+004B (K)
    { 0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00},   // U+004C (L)
    { 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00},   // U+004D (M)
    { 0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00},   // U+004E (N)
    { 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00},   // U+004F (O)
    { 0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00},   // U+0050 (P)
    { 0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00},   // U+0051 (Q)
    { 0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00},   // U+0052 (R)
    { 0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00},   // U+0053 (S)
    { 0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0054 (T)
    { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00},   // U+0055 (U)
    { 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},   // U+0056 (V)
    { 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00},   // U+0057 (W)
    { 0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00},   // U+0058 (X)
    { 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00},   // U+0059 (Y)
    { 0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00},   // U+005A (Z)
    { 0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00},   // U+005B ([)
    { 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00},   // U+005C (\)
    { 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00},   // U+005D (])
    { 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00},   // U+005E (^)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},   // U+005F (_)
    { 0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0060 (`)
    { 0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00},   // U+0061 (a)
    { 0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00},   // U+0062 (b)
    { 0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00},   // U+0063 (c)
    { 0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00},   // U+0064 (d)
    { 0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00},   // U+0065 (e)
    { 0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00},   // U+0066 (f)
    { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F},   // U+0067 (g)
    { 0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00},   // U+0068 (h)
    { 0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0069 (i)
    { 0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E},   // U+006A (j)
    { 0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00},   // U+006B (k)
    { 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+006C (l)
    { 0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00},   // U+006D (m)
    { 0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00},   // U+006E (n)
    { 0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00},   // U+006F (o)
    { 0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F},   // U+0070 (p)
    { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78},   // U+0071 (q)
    { 0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00},   // U+0072 (r)
    { 0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00},   // U+0073 (s)
    { 0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00},   // U+0074 (t)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00},   // U+0075 (u)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},   // U+0076 (v)
    { 0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00},   // U+0077 (w)
    { 0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00},   // U+0078 (x)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F},   // U+0079 (y)
    { 0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00},   // U+007A (z)
    { 0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00},   // U+007B ({)
    { 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00},   // U+007C (|)
    { 0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00},   // U+007D (})
    { 0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+007E (~)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}    // U+007F
};

void gw_gui_fill(pixel_t color){
    pixel_t *dst = framebuffer;
    for (int x = 0; x < GW_LCD_WIDTH; x++) {
        for (int y = 0; y < GW_LCD_HEIGHT; y++) {
            dst[y * GW_LCD_WIDTH + x] = color;
        }
    }
}

void gw_gui_draw_logo(uint16_t x_pos, uint16_t y_pos, uint16_t color)
{
    uint16_t *dst_img = framebuffer;
    logo_image *logo = &logo_flash;

    int w = (logo->width + 7) / 8;
    for (int i = 0; i < w; i++)
        for (int y = 0; y < logo->height; y++)
        {
            const char glyph = logo->logo[y * w + i];
            //for (int x = 0; x < 8; x++)
            if (glyph & 0x80)
                dst_img[(y + y_pos) * 320 + i * 8 + 0 + x_pos] = color;
            if (glyph & 0x40)
                dst_img[(y + y_pos) * 320 + i * 8 + 1 + x_pos] = color;
            if (glyph & 0x20)
                dst_img[(y + y_pos) * 320 + i * 8 + 2 + x_pos] = color;
            if (glyph & 0x10)
                dst_img[(y + y_pos) * 320 + i * 8 + 3 + x_pos] = color;
            if (glyph & 0x08)
                dst_img[(y + y_pos) * 320 + i * 8 + 4 + x_pos] = color;
            if (glyph & 0x04)
                dst_img[(y + y_pos) * 320 + i * 8 + 5 + x_pos] = color;
            if (glyph & 0x02)
                dst_img[(y + y_pos) * 320 + i * 8 + 6 + x_pos] = color;
            if (glyph & 0x01)
                dst_img[(y + y_pos) * 320 + i * 8 + 7 + x_pos] = color;
        }
};

void gw_gui_draw_char(int x, int y, char c, pixel_t color) {
    if (c < 32 || c > 127) return;
    const uint8_t *bitmap = font8x8_basic[c - 32];

    for (int row = 0; row < 8; row++) {
        uint8_t line = bitmap[row];
        for (int col = 0; col < 8; col++) {
            int fb_x = x + col;
            int fb_y = y + row;
            if ((line >> col) & 1) {
                if (fb_x >= 0 && fb_x < GW_LCD_WIDTH && fb_y >= 0 && fb_y < GW_LCD_HEIGHT) {
                    framebuffer[fb_y * GW_LCD_WIDTH + fb_x] = color;
                }
            } else {
                framebuffer[fb_y * GW_LCD_WIDTH + fb_x] = 0x0000;
            }
        }
    }
}

void gw_gui_draw_text(int x, int y, const char *text, pixel_t color) {
    int cursor_x = x;

    for (int clear_x = x; clear_x < GW_LCD_WIDTH; clear_x+=8) {
        gw_gui_draw_char(clear_x, y, ' ', color); // Dessiner un espace pour effacer
    }

    while (*text) {
        if (*text == '\n') {
            // Passe à la ligne suivante
            y += 8;
            cursor_x = x;
            for (int clear_x = x; clear_x < GW_LCD_WIDTH; clear_x+=8) {
                gw_gui_draw_char(clear_x, y, ' ', color); // Dessiner un espace pour effacer
            }
        } else {
            // Dessine un caractère
            gw_gui_draw_char(cursor_x, y, *text, color);
            cursor_x += 8; // Décale le curseur pour le prochain caractère
        }
        text++;
    }
}

void gw_gui_draw_progress_bar(int x, int y, int width, int height, int percent, pixel_t border_color, pixel_t fill_color) {
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    for (int i = 0; i < width; i++) {
        framebuffer[y * GW_LCD_WIDTH + (x + i)] = border_color;
        framebuffer[(y + height - 1) * GW_LCD_WIDTH + (x + i)] = border_color;
    }
    for (int i = 0; i < height; i++) {
        framebuffer[(y + i) * GW_LCD_WIDTH + x] = border_color;
        framebuffer[(y + i) * GW_LCD_WIDTH + (x + width - 1)] = border_color;
    }

    int fill_width = (percent * (width - 2)) / 100;
    for (int i = 0; i < fill_width; i++) {
        for (int j = 1; j < height - 1; j++) {
            framebuffer[(y + j) * GW_LCD_WIDTH + (x + i)] = fill_color;
        }
    }
}

void gw_gui_draw() {
    gw_gui_draw_logo((GW_LCD_WIDTH - logo_flash.width)/2, 20, RGB24_TO_RGB565(255, 255, 255));
}
