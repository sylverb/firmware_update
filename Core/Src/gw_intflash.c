#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include "stm32h7xx_hal.h"
#include "ff.h"
#include "gw_intflash.h"

#define INTFLASH_BANK_SIZE (256 << 10) // 256KB
#define INTFLASH_SECTOR_SIZE 8192      // 8KB, must match erase_intflash
#define BUFFER_SIZE 256

static uint8_t buffer[BUFFER_SIZE];

/**
 * @param bank - Must be 1 or 2.
 * @param offset - Must be a multiple of 8192
 * @param size - Must be a multiple of 8192
 */
static uint32_t erase_intflash(uint8_t bank, uint32_t offset, uint32_t size)
{
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PAGEError;

    assert(bank == 1 || bank == 2);
    assert((offset & 0x1fff) == 0);
    assert((size & 0x1fff) == 0);

    HAL_FLASH_Unlock();

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Banks = bank; // Must be 2
    EraseInitStruct.Sector = offset >> 13;
    EraseInitStruct.NbSectors = size >> 13;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_FLASH_Lock();

    return 0;
}

static HAL_StatusTypeDef flash_write(uint32_t flash_address, uint8_t *data, uint32_t length)
{
    // A flash word is 128bits (16 bytes)
    HAL_StatusTypeDef status;
    HAL_FLASH_Unlock();
    for (uint32_t i = 0; i < length; i += 16)
    {
        uint32_t data_address = (uint32_t)(data) + i;
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, flash_address + i, data_address);
        if (status != HAL_OK)
        {
            printf("status %x\n", status);
            HAL_FLASH_Lock();
            return status;
        }
    }
    HAL_FLASH_Lock();
    return HAL_OK;
}

bool update_intflash(uint8_t bank, uint32_t offset, const char *path, flash_progress_callback_t progress_callback)
{
    FIL file;
    UINT bytesRead;
    bool update_status = true;
    FRESULT res;

    assert(bank == 1 || bank == 2);
    assert((offset & 0xf) == 0); /* offset must be 16-byte aligned for FLASHWORD programming */

    res = f_open(&file, path, FA_READ);
    if (res != FR_OK)
    {
        printf("No update file found\n");
        return false;
    }

    uint32_t file_size = (uint32_t)f_size(&file);
    uint32_t bank_base = bank == 1 ? FLASH_BANK1_BASE : FLASH_BANK2_BASE;

    /* Check we don't write past the end of the bank */
    if (offset > INTFLASH_BANK_SIZE || file_size > INTFLASH_BANK_SIZE - offset)
    {
        printf("File too large for bank: offset=%lu size=%lu bank_size=%u\n",
               (unsigned long)offset, (unsigned long)file_size, INTFLASH_BANK_SIZE);
        f_close(&file);
        return false;
    }

    /* Erase only the sectors that will be written (sector size 8KB) */
    uint32_t sector_start = (offset / INTFLASH_SECTOR_SIZE) * INTFLASH_SECTOR_SIZE;
    uint32_t end_offset = offset + file_size;
    uint32_t sector_end = ((end_offset + INTFLASH_SECTOR_SIZE - 1) / INTFLASH_SECTOR_SIZE) * INTFLASH_SECTOR_SIZE;
    uint32_t erase_size = sector_end - sector_start;

    erase_intflash(bank, sector_start, erase_size);

    uint32_t flash_address = bank_base + offset;
    do
    {
        res = f_read(&file, buffer, BUFFER_SIZE, &bytesRead);
        if (res != FR_OK)
        {
            update_status = false;
            break;
        }

        if (bytesRead == 0)
            break;

        /* Write block in internal flash bank */
        if (flash_write(flash_address, buffer, bytesRead) != HAL_OK)
        {
            update_status = false;
            printf("Flash writing error @0x%lx - %u\n", (unsigned long)flash_address, (unsigned)bytesRead);
            break;
        }

        /* Verify written data */
        if (memcmp((void *)flash_address, buffer, bytesRead) != 0)
        {
            update_status = false;
            printf("Verification error @0x%lx - %u\n", (unsigned long)flash_address, (unsigned)bytesRead);
            break;
        }

        flash_address += bytesRead;

        if (progress_callback && file_size > 0)
        {
            uint32_t written = flash_address - bank_base - offset;
            unsigned int percentage = (unsigned int)((written * 100) / file_size);
            progress_callback(percentage);
        }
    } while (bytesRead == BUFFER_SIZE);

    f_close(&file);
    if (update_status)
    {
        printf("Flashing done, delete update file %s\n", path);
        f_unlink(path);
    }
    else
    {
        printf("Flashing failed\n");
    }
    return update_status;
}
