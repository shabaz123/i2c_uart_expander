# Project Code Summary: I2C UART Expander

This document provides a summary of the core functions within the I2C UART Expander project, detailing their purpose and calling relationships.

## System & Board Level (`source/board/`)

### `main`
The entry point of the application. It calls `System_init` to set up the hardware and emulation modules, then enters an infinite loop. Inside the loop, it manages the window watchdog timer (WWDT) and puts the CPU into a low-power sleep mode until the next interrupt occurs.
**Primary Callers:** Hardware Reset / Bootloader.

### `System_init`
Configures the entire system by calling the SysConfig-generated `SYSCFG_DL_init`. It performs a manual check of the I2C bus state (SDA/SCL high) before enabling the I2C target driver and initializing the SC16IS740 (UART) and AD7291 (ADC) emulation modules. It concludes by enabling the relevant NVIC interrupts.
**Primary Callers:** `main`.

### `System_sleepUntilInterrupt`
Manages the transition to low-power modes. It disables interrupts to safely check for pending system events; if none are found, it executes the `WFI` (Wait For Interrupt) instruction. This ensures the device consumes minimal power while waiting for I2C or UART activity.
**Primary Callers:** `main`.

---

## I2C Target Driver (`source/modules/communication/`)

### `I2CTarDriver_open`
Initializes the I2C peripheral for target (slave) mode. It sets the internal state to IDLE, disables certain hardware features to prevent bus lockup, and enables the START and STOP interrupts which are essential for framing I2C transactions.
**Primary Callers:** `System_init`.

### `I2CTarDriver_ISR`
The central interrupt service routine for I2C activity. It checks the peripheral's interrupt index and dispatches the request to specialized internal handlers for START, STOP, RX FIFO, TX FIFO Trigger, and TX FIFO Empty conditions.
**Primary Callers:** `TAR_I2C_INST_IRQHandler` in `system.c`.

### `I2CTarDriver_read` / `I2CTarDriver_write`
Helper functions for managing the driver's internal software buffers. `read` pops the next byte from the RX queue, while `write` pushes a byte into the TX queue to be sent to the I2C controller.
**Primary Callers:** Emulation module callbacks (`sc16is740_i2c_rx_callback`, `ad7291_i2c_tx_callback`, etc.).

---

## SC16IS740 UART Emulation (`source/modules/emulation/`)

### `sc16is740_open`
Sets up the UART emulation state and the physical UART0 hardware. It resets the local circular RX buffer, enables the UART0 RX interrupt, and registers the emulation module's handles with the I2C driver.
**Primary Callers:** `System_init`.

### `UART0_IRQHandler`
Handles physical UART0 hardware interrupts. When data is received on the physical pins (PA9), this function reads it from the hardware FIFO and pushes it into a software circular buffer, making it available for future I2C read requests from the host.
**Primary Callers:** Hardware NVIC.

### `sc16is740_i2c_rx_callback`
Handles I2C write commands from the host. The first byte received is treated as the SC16IS740 register pointer; subsequent bytes are written to that register. If the THR (Transmit Holding Register) is written to, it immediately passes that data to the UART0 hardware for physical transmission.
**Primary Callers:** `I2CTarDriver_handleStopIRQ` or `I2CTarDriver_handleStartIRQ` (via `i2c_tar_driver.c`).

### `sc16is740_i2c_tx_callback`
Handles I2C read commands from the host. Depending on the current register pointer, it provides data from the UART RX circular buffer (for RHR), the Line Status Register (LSR), or other configuration registers like MCR or IER.
**Primary Callers:** `I2CTarDriver_handleTxEmptyIRQ` in `i2c_tar_driver.c`.

---

## AD7291 ADC Emulation (`source/modules/emulation/`)

### `ad7291_open`
Initializes the AD7291 ADC emulation state. It sets default values for the emulated register file (such as high/low limits) and performs a dummy conversion to ensure the MSPM0 ADC0 peripheral is ready for operation.
**Primary Callers:** `System_init`.

### `AD7291_convertAnalogInput`
A hardware abstraction function that reconfigures the MSPM0 ADC12 peripheral for a specific channel, starts a conversion, and waits for the result. It is used to get real-time data for the emulated ADC registers.
**Primary Callers:** `ad7291_open` and `ad7291_i2c_tx_callback`.

### `ad7291_i2c_tx_callback`
Triggered when the host reads from the ADC I2C address. If the host is reading the voltage conversion results, this function triggers a physical ADC conversion via `AD7291_convertAnalogInput` and writes the result back to the I2C driver.
**Primary Callers:** `I2CTarDriver_handleTxEmptyIRQ` in `i2c_tar_driver.c`.
