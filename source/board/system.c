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
 *  @file       system.c
 *  @brief      System management and ISR's for Processor Companion Utility
 *
 *  This file contains the interrupt handlers and system
 *  management routines used by the application.
 *
 ******************************************************************************/

#include "ti_msp_dl_config.h"
#include "communication/i2c_tar_driver.h"
#include "emulation/sc16is740_emulation.h"
#include "emulation/ad7291_emulation.h"
#include "system.h"

/*!
 * @brief   Length of the I2C receive buffer in bytes
 */
#define TAR_I2C_RXBUF_LEN 128

/*!
 * @brief   Length of the I2C transmit buffer in bytes
 */
#define TAR_I2C_TXBUF_LEN 128

/**
 * @brief   I2C target driver RX buffer
 */
uint8_t tar_i2c_rxbuf[TAR_I2C_RXBUF_LEN];

/**
 * @brief   I2C target driver TX buffer
 */
uint8_t tar_i2c_txbuf[TAR_I2C_TXBUF_LEN];

/**
 * The I2C target driver data structure instance for the I2C I/F we
 * are using for communication with external I2C bus controllers
 */
i2c_tar_driver_t tar_i2c =
{
    .pModule = TAR_I2C_INST,
    .pRxBuf = &tar_i2c_rxbuf[0],
    .rxBufLen = TAR_I2C_RXBUF_LEN,
    .pTxBuf = &tar_i2c_txbuf[0],
    .txBufLen = TAR_I2C_TXBUF_LEN,
    .rxCallback = &sc16is740_i2c_rx_callback,
    .txCallback = &sc16is740_i2c_tx_callback,
    .rxCallback2 = &ad7291_i2c_rx_callback,
    .txCallback2 = &ad7291_i2c_tx_callback
};

/*!
 * @brief Indicates pending system events
 */
volatile uint32_t System_pendingEvents;

void System_init(void)
{
    System_pendingEvents = SYSEVENT__NONE_PENDING;

    SYSCFG_DL_init();
    
    /* Move I2C pins to GPI, check for bus-high condition before
     * continuing to start the I2C target driver. */
    DL_GPIO_initDigitalInputFeatures(GPIO_TAR_I2C_IOMUX_SDA, \
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN, \
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalInputFeatures(GPIO_TAR_I2C_IOMUX_SCL, \
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN, \
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    while (DL_GPIO_readPins(GPIOA, 0x03) != 0x03);

    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_TAR_I2C_IOMUX_SDA,
        GPIO_TAR_I2C_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_TAR_I2C_IOMUX_SCL,
        GPIO_TAR_I2C_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_TAR_I2C_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_TAR_I2C_IOMUX_SCL);

    /* Start the UART emulation module */
    sc16is740_open(&tar_i2c);

    /* Start the ADC emulation module */
    ad7291_open(&tar_i2c);

    /* Start the I2C target driver */
    I2CTarDriver_open(&tar_i2c);

    /* Enable interrupts used by the application */
    NVIC_EnableIRQ(WAKEUP_TIMER_INST_INT_IRQN);
    NVIC_EnableIRQ(TAR_I2C_INST_INT_IRQN);
}

void System_sleepUntilInterrupt(void)
{
    __disable_irq();
    if (System_pendingEvents == SYSEVENT__NONE_PENDING)
    {
        __WFI();
    }
    __enable_irq();
}

void WAKEUP_TIMER_INST_IRQHandler(void)
{
    DL_Timer_clearInterruptStatus(WAKEUP_TIMER_INST, \
        DL_TIMER_INTERRUPT_ZERO_EVENT);
    System_pendingEvents |= SYSEVENT__WAKE_TIMER;
}

void TAR_I2C_INST_IRQHandler(void)
{
    I2CTarDriver_ISR(&tar_i2c);
}
