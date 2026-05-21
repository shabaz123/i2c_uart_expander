/*
 * Copyright (c) 2023, Texas Instruments Incorporated
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

/*
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the MSPM0L110X
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_DEBUG_init();
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_WAKEUP_TIMER_init();
    SYSCFG_DL_TAR_I2C_init();
    SYSCFG_DL_PER_SPI_init();
    SYSCFG_DL_UART0_init();
    SYSCFG_DL_ADC_init();
    SYSCFG_DL_WWDT0_init();
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_TimerG_reset(WAKEUP_TIMER_INST);
    DL_I2C_reset(TAR_I2C_INST);
    DL_SPI_reset(PER_SPI_INST);
    DL_UART_Main_reset(UART0_INST);
    DL_ADC12_reset(ADC_INST);
    DL_WWDT_reset(WWDT0_INST);

    DL_GPIO_enablePower(GPIOA);
    DL_TimerG_enablePower(WAKEUP_TIMER_INST);
    DL_I2C_enablePower(TAR_I2C_INST);
    DL_SPI_enablePower(PER_SPI_INST);
    DL_UART_Main_enablePower(UART0_INST);
    DL_ADC12_enablePower(ADC_INST);
    DL_WWDT_enablePower(WWDT0_INST);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

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

    DL_GPIO_initPeripheralInputFunction(
        GPIO_PER_SPI_IOMUX_SCLK, GPIO_PER_SPI_IOMUX_SCLK_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_PER_SPI_IOMUX_PICO, GPIO_PER_SPI_IOMUX_PICO_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_PER_SPI_IOMUX_POCI, GPIO_PER_SPI_IOMUX_POCI_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_PER_SPI_IOMUX_CS, GPIO_PER_SPI_IOMUX_CS_FUNC);
    
	DL_GPIO_setDigitalInternalResistor(GPIO_PER_SPI_IOMUX_POCI, DL_GPIO_RESISTOR_NONE);

    DL_GPIO_initPeripheralInputFunction(GPIO_UART0_IOMUX_RX,
        GPIO_UART0_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(GPIO_UART0_IOMUX_TX,
        GPIO_UART0_IOMUX_TX_FUNC);
    DL_GPIO_initDigitalOutput(GPIO_UART0_IOMUX_RTS);

    DL_GPIO_initDigitalInput(MCU_GPIO_MCU_GPIO0_1_IOMUX);


}


SYSCONFIG_WEAK void SYSCFG_DL_DEBUG_init(void)
{
    /* Set the DISABLE bit in the SWDCFG register in SYSCTL along with KEY */
    SYSCTL->SOCLOCK.SWDCFG = (SYSCTL_SWDCFG_KEY_VALUE | SYSCTL_SWDCFG_DISABLE_TRUE);
}
SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);

    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIVIDER_DISABLE);

}



/*
 * Timer clock configuration to be sourced by LFCLK /  (32768 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   32768 Hz = 32768 Hz / (1 * (0 + 1))
 */
static const DL_TimerG_ClockConfig gWAKEUP_TIMERClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_LFCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 0U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * WAKEUP_TIMER_INST_LOAD_VALUE = (500 ms * 32768 Hz) - 1
 */
static const DL_TimerG_TimerConfig gWAKEUP_TIMERTimerConfig = {
    .period     = WAKEUP_TIMER_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_WAKEUP_TIMER_init(void) {

    DL_TimerG_setClockConfig(WAKEUP_TIMER_INST,
        (DL_TimerG_ClockConfig *) &gWAKEUP_TIMERClockConfig);

    DL_TimerG_initTimerMode(WAKEUP_TIMER_INST,
        (DL_TimerG_TimerConfig *) &gWAKEUP_TIMERTimerConfig);
    DL_TimerG_enableInterrupt(WAKEUP_TIMER_INST , DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableClock(WAKEUP_TIMER_INST);




}


static const DL_I2C_ClockConfig gTAR_I2CClockConfig = {
    .clockSel = DL_I2C_CLOCK_BUSCLK,
    .divideRatio = DL_I2C_CLOCK_DIVIDE_1,
};

SYSCONFIG_WEAK void SYSCFG_DL_TAR_I2C_init(void) {

    DL_I2C_setClockConfig(TAR_I2C_INST,
        (DL_I2C_ClockConfig *) &gTAR_I2CClockConfig);
    DL_I2C_setAnalogGlitchFilterPulseWidth(TAR_I2C_INST,
        DL_I2C_ANALOG_GLITCH_FILTER_WIDTH_50NS);
    DL_I2C_enableAnalogGlitchFilter(TAR_I2C_INST);

    /* Configure Target Mode */
    DL_I2C_setTargetOwnAddress(TAR_I2C_INST, TAR_I2C_TARGET_OWN_ADDR);
    DL_I2C_setTargetOwnAddressAlternate(TAR_I2C_INST, TAR_I2C_TARGET_SEC_OWN_ADDR);
    DL_I2C_enableTargetOwnAddressAlternate(TAR_I2C_INST);
    DL_I2C_setTargetTXFIFOThreshold(TAR_I2C_INST, DL_I2C_TX_FIFO_LEVEL_EMPTY);
    DL_I2C_setTargetRXFIFOThreshold(TAR_I2C_INST, DL_I2C_RX_FIFO_LEVEL_BYTES_1);
    DL_I2C_enableTargetTXEmptyOnTXRequest(TAR_I2C_INST);

    DL_I2C_enableTargetClockStretching(TAR_I2C_INST);



}

static const DL_SPI_Config gPER_SPI_config = {
    .mode        = DL_SPI_MODE_PERIPHERAL,
    .frameFormat = DL_SPI_FRAME_FORMAT_MOTO4_POL0_PHA0,
    .parity      = DL_SPI_PARITY_NONE,
    .dataSize    = DL_SPI_DATA_SIZE_8,
    .bitOrder    = DL_SPI_BIT_ORDER_MSB_FIRST,
    .chipSelectPin = DL_SPI_CHIP_SELECT_1,
};

static const DL_SPI_ClockConfig gPER_SPI_clockConfig = {
    .clockSel    = DL_SPI_CLOCK_BUSCLK,
    .divideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1
};

SYSCONFIG_WEAK void SYSCFG_DL_PER_SPI_init(void) {
    DL_SPI_setClockConfig(PER_SPI_INST, (DL_SPI_ClockConfig *) &gPER_SPI_clockConfig);

    DL_SPI_init(PER_SPI_INST, (DL_SPI_Config *) &gPER_SPI_config);

    /* Configure Peripheral mode */
    /* Set RX and TX FIFO threshold levels */
    DL_SPI_setFIFOThreshold(PER_SPI_INST, DL_SPI_RX_FIFO_LEVEL_1_2_FULL, DL_SPI_TX_FIFO_LEVEL_1_2_EMPTY);

    /* Enable module */
    DL_SPI_enable(PER_SPI_INST);
}

/* ADC Initialization */
static const DL_ADC12_ClockConfig gADCClockConfig = {
    .clockSel       = DL_ADC12_CLOCK_SYSOSC,
    .divideRatio    = DL_ADC12_CLOCK_DIVIDE_1,
    .freqRange      = DL_ADC12_CLOCK_FREQ_RANGE_24_TO_32,
};
SYSCONFIG_WEAK void SYSCFG_DL_ADC_init(void)
{
    DL_ADC12_setClockConfig(ADC_INST, (DL_ADC12_ClockConfig *) &gADCClockConfig);
    DL_ADC12_configConversionMem(ADC_INST, ADC_ADCMEM_AD7291_EMU_MEM,
        DL_ADC12_INPUT_CHAN_1, DL_ADC12_REFERENCE_VOLTAGE_VDDA, DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
        DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    DL_ADC12_setPowerDownMode(ADC_INST,DL_ADC12_POWER_DOWN_MODE_MANUAL);
    DL_ADC12_setSampleTime0(ADC_INST,143);
    DL_ADC12_enableConversions(ADC_INST);
}

SYSCONFIG_WEAK void SYSCFG_DL_WWDT0_init(void)
{
    /*
     * Initialize WWDT0 in Watchdog mode with following settings
     *   Watchdog Source Clock = (LFCLK Freq) / (WWDT Clock Divider)
     *                         = 32768Hz / 5 = 6.55 kHz
     *   Watchdog Period       = (WWDT Clock Divider) ∗ (WWDT Period Count) / 32768Hz
     *                         = 5 * 2^15 / 32768Hz = 5.00 s
     *   Window0 Closed Period = (WWDT Period) * (Window0 Closed Percent)
     *                         = 5.00 s * 0% = 0.00 s
     *   Window1 Closed Period = (WWDT Period) * (Window1 Closed Percent)
     *                         = 5.00 s * 0% = 0.00 s
     */
    DL_WWDT_initWatchdogMode(WWDT0_INST, DL_WWDT_CLOCK_DIVIDE_5,
        DL_WWDT_TIMER_PERIOD_15_BITS, DL_WWDT_STOP_IN_SLEEP,
        DL_WWDT_WINDOW_PERIOD_0, DL_WWDT_WINDOW_PERIOD_0);

    /* Set Window0 as active window */
    DL_WWDT_setActiveWindow(WWDT0_INST, DL_WWDT_WINDOW0);

}

static const DL_UART_Main_ClockConfig gUART0ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART0Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART0_init(void)
{
    DL_UART_Main_setClockConfig(UART0, (DL_UART_Main_ClockConfig *) &gUART0ClockConfig);
    DL_UART_Main_init(UART0, (DL_UART_Main_Config *) &gUART0Config);
    
    /* Set baud rate to 9600 for 32MHz BUSCLK */
    DL_UART_Main_configBaudRate(UART0, 208, 21);

    DL_UART_Main_enable(UART0);
}


