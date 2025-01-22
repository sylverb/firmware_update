#include <stdio.h>
#include <stdbool.h>

#include "gw_sdcard.h"
#include "gw_lcd.h"
#include "gw_gui.h"
#include "gw_buttons.h"
#include "main.h"
#include "gw_intflash.h"
#include "ff.h"
#include "tar.h"
#include "firmware_update.h"

#define UPDATE_ARCHIVE_FILE "/retro-go_update.bin"
#define APP_SIZE (1024 * 1024) /* Archive includes the application */
#define INTFLASH_2_UPDATE_FILE "/update_bank2.bin"

extern sdcard_hw_type_t sdcard_hw_type;

static FATFS FatFs; // Fatfs handle

void sdcard_hw_detect()
{
    FRESULT cause;

    // Check if SD Card is connected to SPI1
    sdcard_init_spi1();
    sdcard_hw_type = SDCARD_HW_SPI1;
    cause = f_mount(&FatFs, (const TCHAR *)"", 1);
    if (cause == FR_OK)
    {
        return;
    }
    else
    {
        sdcard_deinit_spi1();
    }

    // Check if SD Card is connected over OSPI1
    sdcard_init_ospi1();
    sdcard_hw_type = SDCARD_HW_OSPI1;
    cause = f_mount(&FatFs, (const TCHAR *)"", 1);
    if (cause == FR_OK)
    {
        return;
    }
    else
    {
        sdcard_deinit_ospi1();
    }

    // No SD Card detected
    sdcard_hw_type = SDCARD_HW_NO_SD_FOUND;
}

bool file_exists(const char *path) {
    FILINFO fileInfo;
    FRESULT result;

    result = f_stat(path, &fileInfo);
    return (result == FR_OK);
}

static void __attribute__((naked)) start_app(void (*const pc)(void), uint32_t sp)
{
    __asm("           \n\
          msr msp, r1 /* load r1 into MSP */\n\
          bx r0       /* branch to the address at r0 */\n\
    ");
}

void boot_bank2(void)
{
    uint32_t sp = *((uint32_t *)FLASH_BANK2_BASE);
    uint32_t pc = *((uint32_t *)FLASH_BANK2_BASE + 1);

    // Check that Bank 2 content is valid
    if ((pc > FLASH_BANK2_BASE) && (pc < FLASH_END))
    {
        __set_MSP(sp);
        __set_PSP(sp);
        HAL_MPU_Disable();
        start_app((void (*const)(void))pc, (uint32_t)sp);
    }
    else
    {
        while(1) {
            wdog_refresh();
        }
    }
}

void show_untar_progress_cb(unsigned int percentage, const char *file_name) {
    gw_gui_draw_progress_bar(10, 80, 300, 8, percentage, RGB24_TO_RGB565(255, 255, 255), RGB24_TO_RGB565(255, 255, 255));
    gw_gui_draw_text(10, 60, file_name, RGB24_TO_RGB565(255, 255, 255));
    if (percentage == 100) {
        // Delete progress bar and text
        gw_gui_draw_text(10, 60, "", RGB24_TO_RGB565(255, 255, 255));
        gw_gui_draw_text(10, 80, "", RGB24_TO_RGB565(255, 255, 255));
    }
}

void show_flash_progress_cb(unsigned int percentage) {
    gw_gui_draw_progress_bar(10, 60, 300, 8, percentage, RGB24_TO_RGB565(255, 255, 255), RGB24_TO_RGB565(255, 255, 255));
    printf("Flashing progress: %d%%\n", percentage);
    if (percentage == 100) {
        // Delete progress bar
        gw_gui_draw_text(10, 60, "", RGB24_TO_RGB565(255, 255, 255));
    }
}

void enable_screen()
{
    static bool enabled = false;

    if (!enabled)
    {
        gw_gui_fill(0x0000);
        lcd_backlight_set(180);
        gw_gui_draw();
        enabled = true;
    }
}

void firmware_update_main(void)
{
    bool screen_initialized = false;

    printf("firmware_update_main()\n");

    sdcard_hw_detect();

    if (sdcard_hw_type == SDCARD_HW_NO_SD_FOUND)
    {
        printf("No SD Card found\n");
    }
    else
    {
        enable_screen();
        screen_initialized = true;
        // Extract update archive in root folder
        gw_gui_draw_text(10, 50, "Extracting files", RGB24_TO_RGB565(0, 255, 0));
        // Archive starts with the application and 4 bytes for application payload size, skip this
        if (extract_tar(UPDATE_ARCHIVE_FILE, "", APP_SIZE + sizeof(uint32_t), show_untar_progress_cb))
        {
            // Delete update archive
            f_unlink(UPDATE_ARCHIVE_FILE);
        } else {
            gw_gui_draw_text(10, 50, "Firmware update extract failed", RGB24_TO_RGB565(255, 0, 0));
        }
        if (file_exists(INTFLASH_2_UPDATE_FILE))
        {
            enable_screen();
            screen_initialized = true;
            // Flash bank 2
            gw_gui_draw_text(10, 50, "Writing firmware in flash", RGB24_TO_RGB565(0, 255, 0));
            if (update_bank2_flash(INTFLASH_2_UPDATE_FILE, show_flash_progress_cb)) {
                gw_gui_draw_text(10, 50, "Firmware update done", RGB24_TO_RGB565(0, 255, 0));
            } else {
                gw_gui_draw_text(10, 50, "Flash update failed", RGB24_TO_RGB565(255, 0, 0));
            }
        }
    }

    // Unmount Fs and Deinit SD Card if needed
    if (fs_mounted) {
        f_unmount("");
    }
    switch (sdcard_hw_type) {
        case SDCARD_HW_SPI1:
            sdcard_deinit_spi1();
        break;
        case SDCARD_HW_OSPI1:
            sdcard_deinit_ospi1();
        break;
        default:
        break;
    }

    if (screen_initialized) {
        gw_gui_draw_text(10, 60, "Press any button to continue", RGB24_TO_RGB565(0, 255, 0));
        while(1) {
            uint32_t boot_buttons = buttons_get();
            if (boot_buttons) {
                while (1) {
                    boot_bank2();
                }
            }
        }
    }
    while (1)
    {
        boot_bank2();
    }
}
