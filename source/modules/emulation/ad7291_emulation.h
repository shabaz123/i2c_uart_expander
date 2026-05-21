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
 *  @file       ad7291_emulation.h
 *  @brief      Emulation library for AD7291 12-bit SAR ADC
 *  @defgroup   AD7291 I2C ADC Emulation Library
 *
 *  The AD7291 emulation library enables MSPM0 devices to emulate a discrete
 *  SAR ADC via an I2C target interface, compatible with I2C bus controllers
 *  which are expecting to use the AD7291 Linux IIO driver.
 *
 *  This emulation library uses the i2c_tar_driver API for implementing the
 *  I2C target I/F on the MSPM0, such that external I2C bus controllers
 *  connected via I2C to the MSPM0 may read and write emulated registers
 *  using AD7291 compatible I2C commands.  This library layer
 *  is the glue logic between the I2C target driver API and the MSPM0 ADC
 *  driver library.  As such it is responsible for handling the I2C
 *  commands in the same way as a discrete AD7291 IC would handle them, to the
 *  extent that this is possible with embedded 12-bit SAR ADC in the MSPM0.
 *
 *  Known limitations:
 *      (LIMITATION-1): Temperature sensor (TSENSE) is not implemented.
 *      (LIMITATION-2): Autocycle mode is not implemented.
 *      (LIMITATION-3): Alert features are not implemented.
 *
 ******************************************************************************/

#ifndef EMULATION_AD7291_EMULATION_H_
#define EMULATION_AD7291_EMULATION_H_

#include "communication/i2c_tar_driver.h"

/**
 *  @brief      Open the AD7291 ADC emulation library
 *
 *  This function opens the ADC emulation library.  It must be called
 *  at start-up before I2C requests from external I2C bus controllers may be
 *  handled by the device.
 *
 *  This function expects that the I2C target driver instance which is passed
 *  has been initialized by the application with the expected configuration
 *  settings and target address.
 *
 *  This function expects that the application above this layer has configured
 *  the IOMUX to enable I2C communication on the desired pins to the module
 *  whose base address was passed to this function.
 *
 *  @param[in]  Pointer to the I2C target driver instance to use.
 *
 *  @return     none
 *
 */
extern void ad7291_open(i2c_tar_driver_t *pI2CTarDriverInst);

/**
 *  @brief      I2C target receive callback handler
 *
 *  This callback function must be connected to the I2C target driver.
 *  It is responsible for handling incoming I2C data to this module from an
 *  external I2C bus controller (this is the scenario of an I2C bus write).
 *
 *  @param      bytes is the number of bytes received for processing
 *
 *  @param      trig indicates if we got this data after a RESTART
 *              or a STOP condition on the I2C bus
 *
 *  @return    none
 *
 */
extern void ad7291_i2c_rx_callback(uint32_t bytes, \
    i2c_tar_driver_call_trig_t trig);

/**
 *  @brief      I2C target transmit callback handler
 *
 *  This callback function must be connected to the I2C target driver.
 *  It is responsible for handling outbound I2C data from this module to an
 *  external I2C bus controller (this is the scenario of an I2C bus read).
 *
 *  @param      none
 *
 *  @return    none
 *
 */
extern void ad7291_i2c_tx_callback(void);

#endif /* EMULATION_AD7291_EMULATION_H_ */

