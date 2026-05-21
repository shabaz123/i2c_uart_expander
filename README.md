# I2C UART Expander (MSPM0L1105)

This project implements a multi-device I2C target (slave) using the TI MSPM0L1105 microcontroller. It allows a host controller (such as a Linux single-board computer or another MCU) to interface with a UART and an ADC over a single I2C bus.

## Features

- **I2C UART Emulation**: Compatible with the **SC16IS740** I2C-to-UART bridge.
  - Specifically designed for **RS-485** communication with hardware direction control.
  - **I2C Address**: `0x48`.
  - **Baud Rate**: Pre-configured to 9600.
- **I2C ADC Emulation**: Compatible with the **AD7291** 8-channel, 12-bit ADC.
  - **I2C Address**: `0x20`.

## Hardware Mapping

- **UART0_RXD**: PA9 (PinCM 10)
- **UART0_TXD**: PA17 (PinCM 18)
- **RS-485 RTS (Direction)**: PA15 (PinCM 16) - Automatically controlled via the emulated MCR register.
- **I2C SDA**: PA0 (PinCM 1)
- **I2C SCL**: PA1 (PinCM 2)

## Documentation

For technical details on implementation and usage, see the following summaries:

- [**Code Summary**](./CODE_SUMMARY.md): A detailed overview of the internal functions, module structure, and calling relationships.
- [**SC16IS740 Emulation Guide**](./SC16IS740_EMULATION_SUMMARY.md): A guide to the implemented I2C register map and pseudocode examples for transmitting and receiving data.

## Building the Project

The project is built using the ARM GNU Toolchain and the TI MSPM0 SDK.

```powershell
# Clean previous build artifacts
make clean

# Build the project
make
```

The build process produces `i2c_uart_expander.hex`, `.out`, and `.map` files in the root directory.

## Credits

This project is based on the [beagleboard/mspm0-adc-eeprom](https://github.com/beagleboard/mspm0-adc-eeprom) project and has been extended to include UART emulation while optimizing for the MSPM0L1105 hardware.
