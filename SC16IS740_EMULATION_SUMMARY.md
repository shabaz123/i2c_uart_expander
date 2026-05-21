# SC16IS740 Emulation Summary

This document describes the implemented registers for the SC16IS740 I2C-to-UART bridge emulation and provides examples for controlling the device over I2C to perform RS-485 communication.

## I2C Register Map (Shifted Format)

The SC16IS740 uses a 4-bit register address shifted left by 3 bits. The emulated I2C Target Address is **0x48**.

| Register | I2C Byte | Access | Description |
| :--- | :--- | :--- | :--- |
| **RHR** | 0x00 | Read | **Receive Holding Register**: Returns the oldest byte from the RX FIFO. |
| **THR** | 0x00 | Write | **Transmit Holding Register**: Data written here is immediately sent to the UART hardware. |
| **IER** | 0x08 | R/W | **Interrupt Enable Register**: (Currently stores value only; basic emulation). |
| **LCR** | 0x18 | R/W | **Line Control Register**: (Currently stores value only; baud rate is fixed at 9600). |
| **MCR** | 0x20 | R/W | **Modem Control Register**: Bit 1 controls the **RTS** line (PA15) for RS-485 direction. |
| **LSR** | 0x28 | Read | **Line Status Register**: Reports if data is available (DR) and if THR is empty (THRE). |
| **SPR** | 0x38 | R/W | **Scratchpad Register**: General purpose 8-bit storage. |

### RS-485 Direction Control (MCR)
- **MCR Bit 1 = 1**: RTS (PA15) is High. Set this before transmitting to enable the RS-485 driver.
- **MCR Bit 1 = 0**: RTS (PA15) is Low. Set this after transmission finishes to return to receive mode.

---

## Communication Example (Pseudocode)

This example demonstrates how an I2C Controller (e.g., a Linux Host or another MCU) would interact with the emulated SC16IS740 to send 4 bytes and receive 8 bytes over RS-485.

### UART Settings
- **Address:** 0x48
- **Baud:** 9600 (Fixed in firmware)

### 1. Transmit 4 bytes over RS-485

```python
# 1. Enable RS-485 Transmit (Set MCR Bit 1)
i2c_write(0x48, [0x20, 0x02]) # Register 0x20 (MCR), Data 0x02 (RTS High)

# 2. Write 4 bytes to THR
# The SC16IS740 supports burst writes to the same register
i2c_write(0x48, [0x00, 0xDE, 0xAD, 0xBE, 0xEF]) # Reg 0x00 (THR), then 4 bytes

# 3. Wait for Transmission to complete (Poll LSR Bit 6 - TEMT)
# In a real scenario, you might wait based on baud rate or poll LSR
loop:
    status = i2c_read_reg(0x48, 0x28) # Reg 0x28 (LSR)
    if (status & 0x40): break         # Wait for Transmit Empty (Bit 6)

# 4. Disable RS-485 Transmit (Return to Receive Mode)
i2c_write(0x48, [0x20, 0x00]) # Register 0x20 (MCR), Data 0x00 (RTS Low)
```

### 2. Receive 8 bytes from RS-485

```python
received_count = 0
data_buffer = []

while received_count < 8:
    # 1. Check if data is available (Poll LSR Bit 0 - DR)
    status = i2c_read_reg(0x48, 0x28) # Reg 0x28 (LSR)
    
    if (status & 0x01):
        # 2. Read byte from RHR
        # To read, we write the reg pointer, then perform a combined read
        val = i2c_read_reg(0x48, 0x00) # Reg 0x00 (RHR)
        data_buffer.append(val)
        received_count += 1
```

---

## Notes on Implementation
- **Baud Rate:** The firmware currently ignores LCR/DLL/DLH writes and defaults to **9600 baud**.
- **FIFO:** The emulation provides a **64-byte software RX buffer**.
- **Interrupts:** The physical IRQ pin of the SC16IS740 is not emulated; the controller must poll the **LSR** register via I2C to detect data.
