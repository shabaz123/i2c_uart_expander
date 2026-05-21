/*
 * Copyright (c) 2024, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!****************************************************************************
 *  @file       at24-tests.c
 *  @brief      Tests for at24 emulation
 *
 ******************************************************************************/
 #include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "at24-tests.h"

assert_t writeToDUT(int devHandle, uint8_t *data, uint32_t len)
{
    assert_t status;
    uint32_t count = 0;

    while (count < TEST__EEPROM_RETRY_CNT) {
        if (write(devHandle, data, len) != len) {
            printf(" <WRITE #%u FAILED, retrying...>\n", count);
            count++;
            usleep(TEST__EEPROM_WRITE_DELAY);
        } else {
            return ASSERT_PASS;
        }
    }
    return ASSERT_FAIL;
}

assert_t readFromDUT(int devHandle, uint8_t *data, uint32_t len)
{
    assert_t status;
    uint32_t count = 0;

    while (count < TEST__EEPROM_RETRY_CNT) {
        if (read(devHandle, data, len) != len) {
            printf(" <READ #%u FAILED, retrying...>\n", count);
            count++;
            usleep(TEST__EEPROM_WRITE_DELAY);
        } else {
            return ASSERT_PASS;
        }
    }
    return ASSERT_FAIL;
}

assert_t zeroEEPROM(int devHandle)
{
	uint8_t txBuf[TEST__EEPROM_PAGE_SIZE+2];
	uint8_t rxBuf[TEST__EEPROM_PAGE_SIZE];
    uint32_t page;
	uint32_t address;
    uint32_t i;
	assert_t status;

    printf(" > zerocmd: Writing %u bytes from 0x00, one page at a time...\n", TEST__EEPROM_TOTAL_SIZE);
    
    for (page=0; page<TEST__EEPROM_PAGE_CNT; page++)
    {
        address = page * TEST__EEPROM_PAGE_SIZE;
        printf(" > zerocmd: Writing %u bytes to page %u at %x...\n", \
            TEST__EEPROM_PAGE_SIZE, page, address);
        txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
        txBuf[1] = (uint8_t)(address & 0xFF);
        for (i=0; i<TEST__EEPROM_PAGE_SIZE; i++)
        {
            txBuf[2+i] = 0;
        }
        if (writeToDUT(devHandle, &txBuf[0], (TEST__EEPROM_PAGE_SIZE+2)) != ASSERT_PASS)
        {
            printf(" > zerocmd: I2C write failed!\n");
            return ASSERT_FAIL;
        }
        usleep(TEST__EEPROM_WRITE_DELAY);
    }

    printf(" > zerocmd: Reading %u bytes from from 0x00, one page at a time...\n", TEST__EEPROM_TOTAL_SIZE);
    for (page=0; page<TEST__EEPROM_PAGE_CNT; page++)
    {
        address = page * TEST__EEPROM_PAGE_SIZE;
        printf(" > zerocmd: Reading %u bytes from page %u at %x...\n", \
            TEST__EEPROM_PAGE_SIZE, page, address);
        txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
        txBuf[1] = (uint8_t)(address & 0xFF);
        if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
        {
            printf(" > zerocmd: I2C address write failed!\n");
            return ASSERT_FAIL;
        }
        if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_PAGE_SIZE) != ASSERT_PASS)
        {
            printf(" > zerocmd: I2C read failed!\n");
            return ASSERT_FAIL;
        }
        status = ASSERT_arrayEqual(&txBuf[2], &rxBuf[0], TEST__EEPROM_PAGE_SIZE);
        if (status == ASSERT_FAIL) {
            printf(" > zerocmd: Readback did not match written values!\n");
            return ASSERT_FAIL;
        }
    }

    return ASSERT_PASS;
}

assert_t test1(int devHandle)
{
	uint8_t txBuf[TEST__EEPROM_PAGE_SIZE+2];
	uint8_t rxBuf[TEST__EEPROM_PAGE_SIZE];
    uint32_t page;
	uint32_t address;
    uint32_t i;
	assert_t status;

	printf(" > test1: Pass #1  - Full memory write and readback with pattern A");
    printf(" > test1: Writing %u bytes from 0x00, one page at a time...\n", TEST__EEPROM_TOTAL_SIZE);
    
    for (page=0; page<TEST__EEPROM_PAGE_CNT; page++)
    {
        address = page * TEST__EEPROM_PAGE_SIZE;
        printf(" > test1: Writing %u bytes to page %u at %x...\n", \
            TEST__EEPROM_PAGE_SIZE, page, address);
        txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
        txBuf[1] = (uint8_t)(address & 0xFF);
        for (i=0; i<TEST__EEPROM_PAGE_SIZE; i++)
        {
            txBuf[2+i] = i;
        }
        if (writeToDUT(devHandle, &txBuf[0], (TEST__EEPROM_PAGE_SIZE+2)) != ASSERT_PASS)
        {
            printf(" > test1: I2C write failed!\n");
            return ASSERT_FAIL;
        }
        usleep(TEST__EEPROM_WRITE_DELAY);
    }

    printf(" > test1: Reading %u bytes from from 0x00, one page at a time...\n", TEST__EEPROM_TOTAL_SIZE);
    for (page=0; page<TEST__EEPROM_PAGE_CNT; page++)
    {
        address = page * TEST__EEPROM_PAGE_SIZE;
        printf(" > test1: Reading %u bytes from page %u at %x...\n", \
            TEST__EEPROM_PAGE_SIZE, page, address);
        txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
        txBuf[1] = (uint8_t)(address & 0xFF);
        if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
        {
            printf(" > test1: I2C address write failed!\n");
            return ASSERT_FAIL;
        }
        if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_PAGE_SIZE) != ASSERT_PASS)
        {
            printf(" > test1: I2C read failed!\n");
            return ASSERT_FAIL;
        }
        status = ASSERT_arrayEqual(&txBuf[2], &rxBuf[0], TEST__EEPROM_PAGE_SIZE);
        if (status == ASSERT_FAIL) {
            printf(" > test1: Readback did not match written values!\n");
            return ASSERT_FAIL;
        }
    }

    printf(" > test1: Pass #2  - Full memory write and readback with pattern B");
    printf(" > test1: Writing %u bytes from 0x00, one page at a time...\n", TEST__EEPROM_TOTAL_SIZE);
    
    for (page=0; page<TEST__EEPROM_PAGE_CNT; page++)
    {
        address = page * TEST__EEPROM_PAGE_SIZE;
        printf(" > test1: Writing %u bytes to page %u at %x...\n", \
            TEST__EEPROM_PAGE_SIZE, page, address);
        txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
        txBuf[1] = (uint8_t)(address & 0xFF);
        for (i=0; i<TEST__EEPROM_PAGE_SIZE; i++)
        {
            txBuf[2+i] = i | 0x80;
        }
        if (writeToDUT(devHandle, &txBuf[0], (TEST__EEPROM_PAGE_SIZE+2)) != ASSERT_PASS)
        {
            printf(" > test1: I2C write failed!\n");
            return ASSERT_FAIL;
        }
        usleep(TEST__EEPROM_WRITE_DELAY);
    }

    printf(" > test1: Reading %u bytes from from 0x00, one page at a time...\n", TEST__EEPROM_TOTAL_SIZE);
    for (page=0; page<TEST__EEPROM_PAGE_CNT; page++)
    {
        address = page * TEST__EEPROM_PAGE_SIZE;
        printf(" > test1: Reading %u bytes from page %u at %x...\n", \
            TEST__EEPROM_PAGE_SIZE, page, address);
        txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
        txBuf[1] = (uint8_t)(address & 0xFF);
        if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
        {
            printf(" > test1: I2C address write failed!\n");
            return ASSERT_FAIL;
        }
        if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_PAGE_SIZE) != ASSERT_PASS)
        {
            printf(" > test1: I2C read failed!\n");
            return ASSERT_FAIL;
        }
        status = ASSERT_arrayEqual(&txBuf[2], &rxBuf[0], TEST__EEPROM_PAGE_SIZE);
        if (status == ASSERT_FAIL) {
            printf(" > test1: Readback did not match written values!\n");
            return ASSERT_FAIL;
        }
    }

    return ASSERT_PASS;
}

assert_t test2(int devHandle)
{
	uint8_t txBuf[TEST__EEPROM_PAGE_SIZE+2];
	uint8_t rxBuf[TEST__EEPROM_TOTAL_SIZE];
    uint8_t compareBuf[TEST__EEPROM_TOTAL_SIZE];
    uint32_t page;
	uint32_t address;
    uint32_t i;
    uint32_t compareBufIdx;
	assert_t status;

	printf(" > test2: Pass #1  - Full memory write and readback with pattern A");
    printf(" > test2: Writing %u bytes from 0x00, one page at a time...\n", TEST__EEPROM_TOTAL_SIZE);
    
    compareBufIdx = 0;
    for (page=0; page<TEST__EEPROM_PAGE_CNT; page++)
    {
        address = page * TEST__EEPROM_PAGE_SIZE;
        printf(" > test2: Writing %u bytes to page %u at %x...\n", \
            TEST__EEPROM_PAGE_SIZE, page, address);
        txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
        txBuf[1] = (uint8_t)(address & 0xFF);
        for (i=0; i<TEST__EEPROM_PAGE_SIZE; i++)
        {
            txBuf[2+i] = i | 0x40;
            compareBuf[compareBufIdx++] = i | 0x40;
        }
        if (writeToDUT(devHandle, &txBuf[0], (TEST__EEPROM_PAGE_SIZE+2)) != ASSERT_PASS)
        {
            printf(" > test2: I2C write failed!\n");
            return ASSERT_FAIL;
        }
        usleep(TEST__EEPROM_WRITE_DELAY);
    }

    printf(" > test2: Reading %u bytes from from 0x00, whole device memory...\n", TEST__EEPROM_TOTAL_SIZE);
    address = 0x0000;
    printf(" > test2: Reading %u bytes at %x...\n", TEST__EEPROM_TOTAL_SIZE, address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
    {
        printf(" > test2: I2C address write failed!\n");
        return ASSERT_FAIL;
    }
    if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_TOTAL_SIZE) != ASSERT_PASS)
    {
        printf(" > test2: I2C read failed!\n");
        return ASSERT_FAIL;
    }
    status = ASSERT_arrayEqual(&compareBuf[0], &rxBuf[0], TEST__EEPROM_TOTAL_SIZE);
    if (status == ASSERT_FAIL) {
        printf(" > test2: Readback did not match written values!\n");
        return ASSERT_FAIL;
    }

    return ASSERT_PASS;
}

assert_t test3(int devHandle)
{
	uint8_t txBuf[TEST__EEPROM_PAGE_SIZE+2];
	uint8_t rxBuf[TEST__EEPROM_TOTAL_SIZE];
    uint8_t compareBuf[TEST__EEPROM_TOTAL_SIZE];
    uint32_t page;
	uint32_t address;
    uint32_t i;
    uint32_t compareBufIdx;
	assert_t status;

    if (zeroEEPROM(devHandle) != ASSERT_PASS)
    {
        printf(" > test3: zero cmd failed!\n");
        return ASSERT_FAIL;
    }
    else {
        printf(" > test3: zero cmd passed!\n");
    }
    for (i=0; i<TEST__EEPROM_TOTAL_SIZE; i++)
    {
        compareBuf[i] = 0x00;
    }

	printf(" > test3: Offset page write");
    printf(" > test3: Writing %u bytes from offset...\n", TEST__EEPROM_PAGE_SIZE);
    
    compareBufIdx = 0x0005;
    address = 0x0005;
    printf(" > test3: Writing %u bytes at 0x%x...\n", TEST__EEPROM_PAGE_SIZE, address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    for (i=0; i<TEST__EEPROM_PAGE_SIZE; i++)
    {
        txBuf[2+i] = i;
        compareBuf[compareBufIdx++] = i;
    }
    if (writeToDUT(devHandle, &txBuf[0], (TEST__EEPROM_PAGE_SIZE+2)) != ASSERT_PASS)
    {
        printf(" > test3: I2C write failed!\n");
        return ASSERT_FAIL;
    }
    usleep(TEST__EEPROM_WRITE_DELAY);

    compareBuf[0] = compareBuf[TEST__EEPROM_PAGE_SIZE];
    compareBuf[1] = compareBuf[TEST__EEPROM_PAGE_SIZE + 1];
    compareBuf[2] = compareBuf[TEST__EEPROM_PAGE_SIZE + 2];
    compareBuf[3] = compareBuf[TEST__EEPROM_PAGE_SIZE + 3];
    compareBuf[4] = compareBuf[TEST__EEPROM_PAGE_SIZE + 4];
    compareBuf[TEST__EEPROM_PAGE_SIZE] = 0x00;
    compareBuf[TEST__EEPROM_PAGE_SIZE + 1] = 0x00;
    compareBuf[TEST__EEPROM_PAGE_SIZE + 2] = 0x00;
    compareBuf[TEST__EEPROM_PAGE_SIZE + 3] = 0x00;
    compareBuf[TEST__EEPROM_PAGE_SIZE + 4] = 0x00;

    printf(" > test3: Reading %u bytes from from 0x00, whole device memory...\n", TEST__EEPROM_TOTAL_SIZE);
    address = 0x0000;
    printf(" > test3: Reading %u bytes at %x...\n", TEST__EEPROM_TOTAL_SIZE, address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
    {
        printf(" > test3: I2C address write failed!\n");
        return ASSERT_FAIL;
    }
    if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_TOTAL_SIZE) != ASSERT_PASS)
    {
        printf(" > test3: I2C read failed!\n");
        return ASSERT_FAIL;
    }
    status = ASSERT_arrayEqual(&compareBuf[0], &rxBuf[0], TEST__EEPROM_TOTAL_SIZE);
    if (status == ASSERT_FAIL) {
        printf(" > test3: Readback did not match written values!\n");
        return ASSERT_FAIL;
    }

    return ASSERT_PASS;
}

assert_t test4(int devHandle)
{
	uint8_t txBuf[TEST__EEPROM_PAGE_SIZE+2];
	uint8_t rxBuf[TEST__EEPROM_TOTAL_SIZE];
    uint8_t compareBuf[TEST__EEPROM_TOTAL_SIZE];
    uint32_t page;
	uint32_t address;
    uint32_t i;
    uint32_t compareBufIdx;
	assert_t status;

    if (zeroEEPROM(devHandle) != ASSERT_PASS)
    {
        printf(" > test4: zero cmd failed!\n");
        return ASSERT_FAIL;
    }
    else {
        printf(" > test4: zero cmd passed!\n");
    }
    for (i=0; i<TEST__EEPROM_TOTAL_SIZE; i++)
    {
        compareBuf[i] = 0x00;
    }

	printf(" > test4: Offset page write");
    printf(" > test4: Writing %u bytes from offset...\n", TEST__EEPROM_PAGE_SIZE);
    
    compareBufIdx = 41;
    address = 41;
    printf(" > test4: Writing %u bytes at 0x%x...\n", TEST__EEPROM_PAGE_SIZE, address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    for (i=0; i<TEST__EEPROM_PAGE_SIZE; i++)
    {
        txBuf[2+i] = i;
        compareBuf[compareBufIdx++] = i;
    }
    if (writeToDUT(devHandle, &txBuf[0], (TEST__EEPROM_PAGE_SIZE+2)) != ASSERT_PASS)
    {
        printf(" > test4: I2C write failed!\n");
        return ASSERT_FAIL;
    }
    usleep(TEST__EEPROM_WRITE_DELAY);

    compareBuf[32] = compareBuf[32 + TEST__EEPROM_PAGE_SIZE];
    compareBuf[33] = compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 1];
    compareBuf[34] = compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 2];
    compareBuf[35] = compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 3];
    compareBuf[36] = compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 4];
    compareBuf[37] = compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 5];
    compareBuf[38] = compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 6];
    compareBuf[39] = compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 7];
    compareBuf[40] = compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 8];
    compareBuf[41] = compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 9];
    compareBuf[32 + TEST__EEPROM_PAGE_SIZE] = 0x00;
    compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 1] = 0x00;
    compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 2] = 0x00;
    compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 3] = 0x00;
    compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 4] = 0x00;
    compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 5] = 0x00;
    compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 6] = 0x00;
    compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 7] = 0x00;
    compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 8] = 0x00;
    compareBuf[32 + TEST__EEPROM_PAGE_SIZE + 9] = 0x00;

    printf(" > test4: Reading %u bytes from from 0x00, whole device memory...\n", TEST__EEPROM_TOTAL_SIZE);
    address = 0x0000;
    printf(" > test4: Reading %u bytes at %x...\n", TEST__EEPROM_TOTAL_SIZE, address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
    {
        printf(" > test4: I2C address write failed!\n");
        return ASSERT_FAIL;
    }
    if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_TOTAL_SIZE) != ASSERT_PASS)
    {
        printf(" > test4: I2C read failed!\n");
        return ASSERT_FAIL;
    }
    status = ASSERT_arrayEqual(&compareBuf[0], &rxBuf[0], TEST__EEPROM_TOTAL_SIZE);
    if (status == ASSERT_FAIL) {
        printf(" > test4: Readback did not match written values!\n");
        return ASSERT_FAIL;
    }

    return ASSERT_PASS;
}

assert_t test5(int devHandle)
{
	uint8_t txBuf[TEST__EEPROM_PAGE_SIZE+2];
	uint8_t rxBuf[TEST__EEPROM_TOTAL_SIZE];
    uint8_t compareBuf[TEST__EEPROM_TOTAL_SIZE];
    uint32_t page;
	uint32_t address;
    uint32_t i;
    uint32_t compareBufIdx;
	assert_t status;

    if (zeroEEPROM(devHandle) != ASSERT_PASS)
    {
        printf(" > test5: zero cmd failed!\n");
        return ASSERT_FAIL;
    }
    else {
        printf(" > test5: zero cmd passed!\n");
    }
    for (i=0; i<TEST__EEPROM_TOTAL_SIZE; i++)
    {
        compareBuf[i] = 0x00;
    }

	printf(" > test5: Random byte read/write");
    
    // Check 0xA5 at 0x00
    address = 0x0000;
    printf(" > test5a: Writing 1 byte at 0x%x...\n", address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    txBuf[2] = 0xA5;
    compareBuf[address] = 0xA5;
    if (writeToDUT(devHandle, &txBuf[0], 3) != ASSERT_PASS)
    {
        printf(" > test5a: I2C write failed!\n");
        return ASSERT_FAIL;
    }
    usleep(TEST__EEPROM_WRITE_DELAY);

    printf(" > test5a: Reading %u bytes from from 0x00, whole device memory...\n", TEST__EEPROM_TOTAL_SIZE);
    address = 0x0000;
    printf(" > test5a: Reading %u bytes at %x...\n", TEST__EEPROM_TOTAL_SIZE, address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
    {
        printf(" > test5a: I2C address write failed!\n");
        return ASSERT_FAIL;
    }
    if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_TOTAL_SIZE) != ASSERT_PASS)
    {
        printf(" > test5a: I2C read failed!\n");
        return ASSERT_FAIL;
    }
    status = ASSERT_arrayEqual(&compareBuf[0], &rxBuf[0], TEST__EEPROM_TOTAL_SIZE);
    if (status == ASSERT_FAIL) {
        printf(" > test5a: Readback did not match written values!\n");
        return ASSERT_FAIL;
    }

    // Check 0xBC at 0x03
    address = 0x0003;
    printf(" > test5b: Writing 1 byte at 0x%x...\n", address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    txBuf[2] = 0xBC;
    compareBuf[address] = 0xBC;
    if (writeToDUT(devHandle, &txBuf[0], 3) != ASSERT_PASS)
    {
        printf(" > test5b: I2C write failed!\n");
        return ASSERT_FAIL;
    }
    usleep(TEST__EEPROM_WRITE_DELAY);

    printf(" > test5b: Reading %u bytes from from 0x00, whole device memory...\n", TEST__EEPROM_TOTAL_SIZE);
    address = 0x0000;
    printf(" > test5b: Reading %u bytes at %x...\n", TEST__EEPROM_TOTAL_SIZE, address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
    {
        printf(" > test5b: I2C address write failed!\n");
        return ASSERT_FAIL;
    }
    if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_TOTAL_SIZE) != ASSERT_PASS)
    {
        printf(" > test5b: I2C read failed!\n");
        return ASSERT_FAIL;
    }
    status = ASSERT_arrayEqual(&compareBuf[0], &rxBuf[0], TEST__EEPROM_TOTAL_SIZE);
    if (status == ASSERT_FAIL) {
        printf(" > test5b: Readback did not match written values!\n");
        return ASSERT_FAIL;
    }

    // Check 0xDE at 0x05
    address = 0x0005;
    printf(" > test5c: Writing 1 byte at 0x%x...\n", address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    txBuf[2] = 0xDE;
    compareBuf[address] = 0xDE;
    if (writeToDUT(devHandle, &txBuf[0], 3) != ASSERT_PASS)
    {
        printf(" > test5c: I2C write failed!\n");
        return ASSERT_FAIL;
    }
    usleep(TEST__EEPROM_WRITE_DELAY);

    printf(" > test5c: Reading %u bytes from from 0x00, whole device memory...\n", TEST__EEPROM_TOTAL_SIZE);
    address = 0x0000;
    printf(" > test5c: Reading %u bytes at %x...\n", TEST__EEPROM_TOTAL_SIZE, address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
    {
        printf(" > test5c: I2C address write failed!\n");
        return ASSERT_FAIL;
    }
    if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_TOTAL_SIZE) != ASSERT_PASS)
    {
        printf(" > test5c: I2C read failed!\n");
        return ASSERT_FAIL;
    }
    status = ASSERT_arrayEqual(&compareBuf[0], &rxBuf[0], TEST__EEPROM_TOTAL_SIZE);
    if (status == ASSERT_FAIL) {
        printf(" > test5c: Readback did not match written values!\n");
        return ASSERT_FAIL;
    }

    // Check 0x43 at 31
    address = 31;
    printf(" > test5d: Writing 1 byte at 0x%x...\n", address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    txBuf[2] = 0x43;
    compareBuf[address] = 0x43;
    if (writeToDUT(devHandle, &txBuf[0], 3) != ASSERT_PASS)
    {
        printf(" > test5d: I2C write failed!\n");
        return ASSERT_FAIL;
    }
    usleep(TEST__EEPROM_WRITE_DELAY);

    printf(" > test5d: Reading %u bytes from from 0x00, whole device memory...\n", TEST__EEPROM_TOTAL_SIZE);
    address = 0x0000;
    printf(" > test5d: Reading %u bytes at %x...\n", TEST__EEPROM_TOTAL_SIZE, address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
    {
        printf(" > test5d: I2C address write failed!\n");
        return ASSERT_FAIL;
    }
    if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_TOTAL_SIZE) != ASSERT_PASS)
    {
        printf(" > test5d: I2C read failed!\n");
        return ASSERT_FAIL;
    }
    status = ASSERT_arrayEqual(&compareBuf[0], &rxBuf[0], TEST__EEPROM_TOTAL_SIZE);
    if (status == ASSERT_FAIL) {
        printf(" > test5d: Readback did not match written values!\n");
        return ASSERT_FAIL;
    }

    // Check 0x54 at 32
    address = 32;
    printf(" > test5e: Writing 1 byte at 0x%x...\n", address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    txBuf[2] = 0x54;
    compareBuf[address] = 0x54;
    if (writeToDUT(devHandle, &txBuf[0], 3) != ASSERT_PASS)
    {
        printf(" > test5e: I2C write failed!\n");
        return ASSERT_FAIL;
    }
    usleep(TEST__EEPROM_WRITE_DELAY);

    printf(" > test5e: Reading %u bytes from from 0x00, whole device memory...\n", TEST__EEPROM_TOTAL_SIZE);
    address = 0x0000;
    printf(" > test5e: Reading %u bytes at %x...\n", TEST__EEPROM_TOTAL_SIZE, address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
    {
        printf(" > test5e: I2C address write failed!\n");
        return ASSERT_FAIL;
    }
    if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_TOTAL_SIZE) != ASSERT_PASS)
    {
        printf(" > test5e: I2C read failed!\n");
        return ASSERT_FAIL;
    }
    status = ASSERT_arrayEqual(&compareBuf[0], &rxBuf[0], TEST__EEPROM_TOTAL_SIZE);
    if (status == ASSERT_FAIL) {
        printf(" > test5e: Readback did not match written values!\n");
        return ASSERT_FAIL;
    }

    // Check 0x85 at 712
    address = 712;
    printf(" > test5f: Writing 1 byte at 0x%x...\n", address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    txBuf[2] = 0x85;
    compareBuf[address] = 0x85;
    if (writeToDUT(devHandle, &txBuf[0], 3) != ASSERT_PASS)
    {
        printf(" > test5f: I2C write failed!\n");
        return ASSERT_FAIL;
    }
    usleep(TEST__EEPROM_WRITE_DELAY);

    printf(" > test5f: Reading %u bytes from from 0x00, whole device memory...\n", TEST__EEPROM_TOTAL_SIZE);
    address = 0x0000;
    printf(" > test5f: Reading %u bytes at %x...\n", TEST__EEPROM_TOTAL_SIZE, address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
    {
        printf(" > test5f: I2C address write failed!\n");
        return ASSERT_FAIL;
    }
    if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_TOTAL_SIZE) != ASSERT_PASS)
    {
        printf(" > test5f: I2C read failed!\n");
        return ASSERT_FAIL;
    }
    status = ASSERT_arrayEqual(&compareBuf[0], &rxBuf[0], TEST__EEPROM_TOTAL_SIZE);
    if (status == ASSERT_FAIL) {
        printf(" > test5f: Readback did not match written values!\n");
        return ASSERT_FAIL;
    }

    // Check 0x92 at 1023
    address = 1023;
    printf(" > test5g: Writing 1 byte at 0x%x...\n", address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    txBuf[2] = 0x92;
    compareBuf[address] = 0x92;
    if (writeToDUT(devHandle, &txBuf[0], 3) != ASSERT_PASS)
    {
        printf(" > test5g: I2C write failed!\n");
        return ASSERT_FAIL;
    }
    usleep(TEST__EEPROM_WRITE_DELAY);

    printf(" > test5g: Reading %u bytes from from 0x00, whole device memory...\n", TEST__EEPROM_TOTAL_SIZE);
    address = 0x0000;
    printf(" > test5g: Reading %u bytes at %x...\n", TEST__EEPROM_TOTAL_SIZE, address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
    {
        printf(" > test5g: I2C address write failed!\n");
        return ASSERT_FAIL;
    }
    if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_TOTAL_SIZE) != ASSERT_PASS)
    {
        printf(" > test5g: I2C read failed!\n");
        return ASSERT_FAIL;
    }
    status = ASSERT_arrayEqual(&compareBuf[0], &rxBuf[0], TEST__EEPROM_TOTAL_SIZE);
    if (status == ASSERT_FAIL) {
        printf(" > test5g: Readback did not match written values!\n");
        return ASSERT_FAIL;
    }

    // Check 0x6B at 1002
    address = 1002;
    printf(" > test5i: Writing 1 byte at 0x%x...\n", address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    txBuf[2] = 0x6B;
    compareBuf[address] = 0x6B;
    if (writeToDUT(devHandle, &txBuf[0], 3) != ASSERT_PASS)
    {
        printf(" > test5i: I2C write failed!\n");
        return ASSERT_FAIL;
    }
    usleep(TEST__EEPROM_WRITE_DELAY);

    printf(" > test5i: Reading %u bytes from from 0x00, whole device memory...\n", TEST__EEPROM_TOTAL_SIZE);
    address = 0x0000;
    printf(" > test5i: Reading %u bytes at %x...\n", TEST__EEPROM_TOTAL_SIZE, address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
    {
        printf(" > test5i: I2C address write failed!\n");
        return ASSERT_FAIL;
    }
    if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_TOTAL_SIZE) != ASSERT_PASS)
    {
        printf(" > test5i: I2C read failed!\n");
        return ASSERT_FAIL;
    }
    status = ASSERT_arrayEqual(&compareBuf[0], &rxBuf[0], TEST__EEPROM_TOTAL_SIZE);
    if (status == ASSERT_FAIL) {
        printf(" > test5i: Readback did not match written values!\n");
        return ASSERT_FAIL;
    }

    // Check 0x78 at 59
    address = 59;
    printf(" > test5j: Writing 1 byte at 0x%x...\n", address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    txBuf[2] = 0x78;
    compareBuf[address] = 0x78;
    if (writeToDUT(devHandle, &txBuf[0], 3) != ASSERT_PASS)
    {
        printf(" > test5j: I2C write failed!\n");
        return ASSERT_FAIL;
    }
    usleep(TEST__EEPROM_WRITE_DELAY);

    printf(" > test5j: Reading %u bytes from from 0x00, whole device memory...\n", TEST__EEPROM_TOTAL_SIZE);
    address = 0x0000;
    printf(" > test5j: Reading %u bytes at %x...\n", TEST__EEPROM_TOTAL_SIZE, address);
    txBuf[0] = (uint8_t)((address >> 8) & 0xFF);
    txBuf[1] = (uint8_t)(address & 0xFF);
    if (writeToDUT(devHandle, &txBuf[0], 2) != ASSERT_PASS)
    {
        printf(" > test5j: I2C address write failed!\n");
        return ASSERT_FAIL;
    }
    if (readFromDUT(devHandle, &rxBuf[0], TEST__EEPROM_TOTAL_SIZE) != ASSERT_PASS)
    {
        printf(" > test5j: I2C read failed!\n");
        return ASSERT_FAIL;
    }
    status = ASSERT_arrayEqual(&compareBuf[0], &rxBuf[0], TEST__EEPROM_TOTAL_SIZE);
    if (status == ASSERT_FAIL) {
        printf(" > test5j: Readback did not match written values!\n");
        return ASSERT_FAIL;
    }

    return ASSERT_PASS;
}

assert_t runTests(int devHandle)
{
	assert_t status;
    
    status = test1(devHandle);
    if(status == ASSERT_FAIL) {
		printf("FAIL: test1\n");
        return ASSERT_FAIL;
	}

    status = test2(devHandle);
    if(status == ASSERT_FAIL) {
		printf("FAIL: test2\n");
        return ASSERT_FAIL;
	}

    status = test3(devHandle);
    if(status == ASSERT_FAIL) {
		printf("FAIL: test3\n");
        return ASSERT_FAIL;
	}

    status = test4(devHandle);
    if(status == ASSERT_FAIL) {
		printf("FAIL: test4\n");
        return ASSERT_FAIL;
	}

    status = test5(devHandle);
    if(status == ASSERT_FAIL) {
		printf("FAIL: test5\n");
        return ASSERT_FAIL;
	}
	
    return ASSERT_PASS;
}