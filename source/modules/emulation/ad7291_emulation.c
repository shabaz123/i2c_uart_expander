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
 *  @file       ad7291_emulation.c
 *
 *  AD7291 I2C ADC Emulation Library
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <ti/driverlib/dl_adc12.h>
#include "ad7291_emulation.h"

/*!
 * @brief Total # analog input channels
 */
#define AD7291_CH_CNT 8

/*!
 * @brief Total # of 16-bit user registers in the AD7291
 */
#define AD7291_REGFILE_LEN 33

/*!
 * @brief AD7291_REG_CMD indicates no channels are enabled
 */
#define AD7291_NO_CHANNELS_ENABLED 0xFF

/**
 * @brief   AD7291 emulated register file names
 */
enum ad7291_regs
{
    AD7291_REG_CMD = 0x00,
    AD7291_REG_VCONV_RES = 0x01,
    AD7291_REG_TCONV_RES = 0x02,
    AD7291_REG_TAVG_RES = 0x03,
    AD7291_REG_CH0_DHIGH = 0x04,
    AD7291_REG_CH0_DLOW = 0x05,
    AD7291_REG_CH0_HYST = 0x06,
    AD7291_REG_CH1_DHIGH = 0x07,
    AD7291_REG_CH1_DLOW = 0x08,
    AD7291_REG_CH1_HYST = 0x09,
    AD7291_REG_CH2_DHIGH = 0x0A,
    AD7291_REG_CH2_DLOW = 0x0B,
    AD7291_REG_CH2_HYST = 0x0C,
    AD7291_REG_CH3_DHIGH = 0x0D,
    AD7291_REG_CH3_DLOW = 0x0E,
    AD7291_REG_CH3_HYST = 0x0F,
    AD7291_REG_CH4_DHIGH = 0x10,
    AD7291_REG_CH4_DLOW = 0x11,
    AD7291_REG_CH4_HYST = 0x12,
    AD7291_REG_CH5_DHIGH = 0x13,
    AD7291_REG_CH5_DLOW = 0x14,
    AD7291_REG_CH5_HYST = 0x15,
    AD7291_REG_CH6_DHIGH = 0x16,
    AD7291_REG_CH6_DLOW = 0x17,
    AD7291_REG_CH6_HYST = 0x18,
    AD7291_REG_CH7_DHIGH = 0x19,
    AD7291_REG_CH7_DLOW = 0x1A,
    AD7291_REG_CH7_HYST = 0x1B,
    AD7291_REG_TS_DHIGH = 0x1C,
    AD7291_REG_TS_DLOW = 0x1D,
    AD7291_REG_TS_HYST = 0x1E,
    AD7291_REG_ALERT_A = 0x1F,
    AD7291_REG_ALERT_B = 0x20,
    AD7291_REG_INVALID = 0x21
};

typedef union
{
    uint16_t r16;
    uint8_t r8[2];
} ad7291_reg_t;

/**
 * @brief   AD7291 emulated register file storage
 */
ad7291_reg_t ad7291_regFile[AD7291_REGFILE_LEN];

/**
 * @brief   AD7291 emulated register file storage
 */
const uint8_t ad7291_chLUT[AD7291_CH_CNT] =
{
    DL_ADC12_INPUT_CHAN_1,
    DL_ADC12_INPUT_CHAN_2,
    DL_ADC12_INPUT_CHAN_3,
    DL_ADC12_INPUT_CHAN_4,
    DL_ADC12_INPUT_CHAN_5,
    DL_ADC12_INPUT_CHAN_6,
    DL_ADC12_INPUT_CHAN_8,
    DL_ADC12_INPUT_CHAN_9
};

/**
 * @brief   AD7291 emulated register file pointer register
 */
uint8_t ad7291_regFilePtr;

/**
 * @brief   AD7291 next channel select
 */
uint8_t ad7291_nextChannel;

/**
 * @brief   I2C target driver to use for read/write operations
 *          when handling callbacks
 */
i2c_tar_driver_t *ad7291_i2c_tar_driver_handle;

/**
 *  @brief      Set the global next channel pointer to default value
 *
 *  @param      none
 *
 *  @return     none
 *
 */
static void ad7291_resetNextChannel(void);

/**
 *  @brief      Get the next channel index to convert (index in channel LUT)
 *
 *  @param      none
 *
 *  @return     The next channel index to convert
 *
 */
static uint8_t ad7291_getNextChannel(void);

/**
 *  @brief      Convert the selected analog input
 *
 *  @param      MSPM0 ADC channel index to convert
 *
 *  @return     The 12-bit conversion result
 *
 */
static uint16_t AD7291_convertAnalogInput(uint8_t channel);

/**
 * Standard function implementations
 */

void ad7291_open(i2c_tar_driver_t *pI2CTarDriverInst)
{
    uint8_t i;
    uint8_t base;

    ad7291_i2c_tar_driver_handle = pI2CTarDriverInst;
    ad7291_regFilePtr = 0;

    for (i=0; i<AD7291_REGFILE_LEN; i++)
    {
        ad7291_regFile[i].r16 = 0x0000;
    }

    ad7291_regFile[AD7291_REG_CH0_DHIGH].r16 = 0x0FFF;
    ad7291_regFile[AD7291_REG_CH1_DHIGH].r16 = 0x0FFF;
    ad7291_regFile[AD7291_REG_CH2_DHIGH].r16 = 0x0FFF;
    ad7291_regFile[AD7291_REG_CH3_DHIGH].r16 = 0x0FFF;
    ad7291_regFile[AD7291_REG_CH4_DHIGH].r16 = 0x0FFF;
    ad7291_regFile[AD7291_REG_CH5_DHIGH].r16 = 0x0FFF;
    ad7291_regFile[AD7291_REG_CH6_DHIGH].r16 = 0x0FFF;
    ad7291_regFile[AD7291_REG_CH7_DHIGH].r16 = 0x0FFF;
    ad7291_regFile[AD7291_REG_TS_DHIGH].r16 = 0x0800;
    ad7291_regFile[AD7291_REG_TS_DLOW].r16 = 0x07FF;

    /*
     * Take dummy conversion of the internal supply monitor to fire up the ADC
     */
    AD7291_convertAnalogInput(15);

    return;
}

void ad7291_resetNextChannel(void)
{
    ad7291_nextChannel = 0;
}

uint8_t ad7291_getNextChannel(void)
{
    uint8_t enabledChannelsMask;
    uint8_t channelSelMask;
    uint8_t channelSel;

    enabledChannelsMask = ad7291_regFile[AD7291_REG_CMD].r8[1];

    if (enabledChannelsMask == 0)
    {
        return AD7291_NO_CHANNELS_ENABLED;
    }

    do {
        channelSel = ad7291_nextChannel;
        channelSelMask = 0x80 >> ad7291_nextChannel;
        if (++ad7291_nextChannel >= 8)
        {
            ad7291_nextChannel = 0;
        }
    } while ((enabledChannelsMask & channelSelMask) == 0);

    return channelSel;
}

uint16_t AD7291_convertAnalogInput(uint8_t channel)
{
    uint16_t result;

    DL_ADC12_configConversionMem(ADC0, DL_ADC12_MEM_IDX_0, channel,
        DL_ADC12_REFERENCE_VOLTAGE_VDDA,
        DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0,
        DL_ADC12_AVERAGING_MODE_DISABLED,
        DL_ADC12_BURN_OUT_SOURCE_DISABLED,
        DL_ADC12_TRIGGER_MODE_AUTO_NEXT,
        DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    DL_ADC12_enableConversions(ADC0);
    DL_ADC12_startConversion(ADC0);
    while(!DL_ADC12_getRawInterruptStatus(ADC0,
        DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED));
    DL_ADC12_clearInterruptStatus(ADC0,
        DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED);
    result = DL_ADC12_getMemResult(ADC0, DL_ADC12_MEM_IDX_0);

    return result;
}

void ad7291_i2c_rx_callback(uint32_t bytes, i2c_tar_driver_call_trig_t trig)
{
    ad7291_reg_t updatedReg;
    uint8_t newRegFilePtr;

    while (bytes>0)
    {
        if (bytes >=3)
        {
            newRegFilePtr = I2CTarDriver_read(ad7291_i2c_tar_driver_handle);
            updatedReg.r8[1] = I2CTarDriver_read(ad7291_i2c_tar_driver_handle);
            updatedReg.r8[0] = I2CTarDriver_read(ad7291_i2c_tar_driver_handle);
            if (newRegFilePtr < AD7291_REG_INVALID)
            {
                ad7291_regFilePtr = newRegFilePtr;
                ad7291_regFile[ad7291_regFilePtr] = updatedReg;

            }
            bytes -= 3;
        }
        else if (bytes == 1)
        {
            newRegFilePtr = I2CTarDriver_read(ad7291_i2c_tar_driver_handle);
            if (newRegFilePtr < AD7291_REG_INVALID)
            {
                ad7291_regFilePtr = newRegFilePtr;
            }
            bytes -= 1;
        }
        if (ad7291_regFilePtr == AD7291_REG_VCONV_RES)
        {
            ad7291_resetNextChannel();
        }
    }
}

void ad7291_i2c_tx_callback(void)
{
    uint8_t channel;

    if (ad7291_regFilePtr == AD7291_REG_VCONV_RES)
    {
        channel = ad7291_getNextChannel();
        if (channel == AD7291_NO_CHANNELS_ENABLED)
        {
            ad7291_regFile[AD7291_REG_VCONV_RES].r16 = 0;
        }
        else
        {
            ad7291_regFile[AD7291_REG_VCONV_RES].r16 \
                = AD7291_convertAnalogInput(ad7291_chLUT[channel]);
            ad7291_regFile[AD7291_REG_VCONV_RES].r8[1] \
                |= (channel << 4) & 0xF0;
        }
    }

    // Write back 16 bit data for host to read out
    I2CTarDriver_write(ad7291_i2c_tar_driver_handle, \
        ad7291_regFile[ad7291_regFilePtr].r8[1]);
    I2CTarDriver_write(ad7291_i2c_tar_driver_handle, \
        ad7291_regFile[ad7291_regFilePtr].r8[0]);
}
