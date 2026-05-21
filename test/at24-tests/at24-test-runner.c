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
 *  @file       at24-test-runner.c
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

#include "at24-test-config.h"
#include "at24-test-asserts.h"
#include "at24-tests.h"

int main(int argc, char *argv[])
{
	int devHandle;
	char devFilename[20];
	assert_t status;

	printf("==== MSPM0 AT24 I2C Tester ====\n\n");

	snprintf(devFilename, 19, "/dev/i2c-%d", TEST__I2C_ADAPTER);
	devHandle = open(devFilename, O_RDWR);
	if (devHandle < 0) {
		printf("Error opening I2C adapter!\n");
		exit(1);
	}
	if (ioctl(devHandle, I2C_SLAVE, TEST__I2C_TAR_ADDR) < 0) {
		printf("Error setting I2C target address!\n");
		exit(1);
	}

	printf("Running tests...\n");
	status = runTests(devHandle);
	if (status == ASSERT_PASS) {
		printf("All tests passed!\n!");
	}

	if (close(devHandle) < 0) {
		printf("Error closing I2C device!\n");
		exit(1);
	}

	printf("==== DONE WITH ALL ====\n\n");
}
