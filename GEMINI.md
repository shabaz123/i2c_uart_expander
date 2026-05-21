# Project: MSPM0L1105 I2C UART Expander

This project aims to emulate multiple I2C devices (ADC and UART) using a TI MSPM0L1105 microcontroller.

## Build Environment

- **ARM GNU Toolchain:** `C:\DEV\arm_gnu_toolchains\15.2.rel1`
  - Contains `arm-none-eabi`, `bin`, `include`, etc.
- **TI MSPM0 SDK:** `C:\ti\mspm0_sdk_2_10_00_04`
  - Local copy of [TexasInstruments/mspm0-sdk](https://github.com/TexasInstruments/mspm0-sdk).
- **Reference Project:** [shabaz123/easyL1105 - MyStarterProject/app_L1105](https://github.com/shabaz123/easyL1105/tree/main/MyStarterProject/app_L1105)
  - The `gcc/makefile` in this project is known to work and should be used as a template for build configuration.

## Project Goals

### Project Status: Completed Phase 1
The project now emulates:
1. **I2C ADC** (AD7291 compatible) at address `0x20`.
2. **I2C UART** (SC16IS740 compatible) at address `0x48`.

**Note:** The EEPROM emulation was removed to accommodate the MSPM0L1105 hardware limitation of two I2C target addresses.

### Implementation Details
- **I2C UART Emulation:**
  - Implemented in `source/modules/emulation/sc16is740_emulation.c`.
  - Supports RHR, THR, LSR, IER, LCR, MCR, and SPR registers.
  - **UART0 Pins:**
    - RX: PA9 (PinCM 10)
    - TX: PA17 (PinCM 18)
    - RTS (RS-485 Direction): PA15 (PinCM 16) - Controlled via MCR register bit 1.
  - **Baud Rate:** Defaults to 9600 (32MHz BUSCLK).
- **Build System:** Fixed `Makefile` for Windows compatibility using `cmd.exe` shell.

## Reference Documentation & Examples

- **UART RX Flow Control:** [mspm0-sdk/.../uart_rx_hw_flow_control](https://github.com/TexasInstruments/mspm0-sdk/tree/main/examples/nortos/LP_MSPM0C1104/driverlib/uart_rx_hw_flow_control)
- **UART TX Flow Control:** [mspm0-sdk/.../uart_tx_hw_flow_control](https://github.com/TexasInstruments/mspm0-sdk/tree/main/examples/nortos/LP_MSPM0C1104/driverlib/uart_tx_hw_flow_control)
