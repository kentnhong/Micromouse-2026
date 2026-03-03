# STM32F411 DMA Driver

Xupermouse 2026

## Overview

This module implements a lightweight DMA driver for the STM32F411.

It provides a safe and simple way to configure and run DMA transfers
while keeping DMA logic separate from peripherals like ADC or UART.

Supported transfer types: - Peripheral → Memory - Memory → Peripheral -
Memory → Memory

Current mode/features: - Normal mode (no circular mode) - Direct mode
(FIFO disabled) - Polling-based usage (no DMA interrupts yet)

------------------------------------------------------------------------

## Design Philosophy

- Keep DMA independent from ADC/SPI/UART drivers\
- Follow STM32 reference manual requirements\
- Disable stream before modifying control registers\
- Clear flags before restarting a stream\
- Validate address ranges, alignment, and transfer length

------------------------------------------------------------------------

## StDmaSettings Structure

``` cpp
struct StDmaSettings
{
    DmaChSel channel;
    DmaPriority priority;
    DmaWidth width;
    DmaDataDir data_dir;
};
```

Each field controls specific hardware behavior in the DMA stream.

------------------------------------------------------------------------

### DmaChSel

``` cpp
enum class DmaChSel : uint8_t
{
    CH0 = 0, CH1, CH2, CH3, CH4, CH5, CH6, CH7
};
```

Selects which DMA channel is mapped to the stream.

- The correct channel depends on the peripheral.
- Must match the mapping defined in the STM32 reference manual.
- Configures the `CHSEL` bits in `DMA_SxCR`.

Example: If ADC1 requires Channel 0 on Stream 0, select `CH0`.

------------------------------------------------------------------------

### DmaPriority

``` cpp
enum class DmaPriority : uint8_t
{
    LOW = 0,
    MEDIUM,
    HIGH,
    VERY_HIGH
};
```

Determines arbitration priority when multiple DMA streams are active.

Higher priority streams get bus access first.

Recommended: - ADC sensor sampling → `HIGH` or `VERY_HIGH`

Configures the `PL` bits in `DMA_SxCR`.

------------------------------------------------------------------------

### DmaWidth

``` cpp
enum class DmaWidth : uint8_t
{
    BYTE = 0,
    HALF_WORD,
    WORD
};
```

Defines transfer data size:

- `BYTE` → 8-bit transfer
- `HALF_WORD` → 16-bit transfer
- `WORD` → 32-bit transfer

Configures: - `MSIZE` (memory width) - `PSIZE` (peripheral width)

For 12-bit ADC data, use `HALF_WORD`.

------------------------------------------------------------------------

### DmaDataDir

``` cpp
enum class DmaDataDir : uint8_t
{
    PERIPH_TO_MEM = 0,
    MEM_TO_PERIPH,
    MEM_TO_MEM
};
```

Defines transfer direction:

- `PERIPH_TO_MEM` → Example: ADC → RAM buffer
- `MEM_TO_PERIPH` → Example: RAM buffer → UART
- `MEM_TO_MEM` → Memory copy using DMA

Configures the `DIR` bits in `DMA_SxCR`.

------------------------------------------------------------------------

### Example Configuration (ADC Burst)

``` cpp
StDmaSettings settings {
    DmaChSel::CH0,
    DmaPriority::HIGH,
    DmaWidth::HALF_WORD,
    DmaDataDir::PERIPH_TO_MEM
};
```

This configures DMA for ADC sampling into memory.

------------------------------------------------------------------------

### StDmaSettings Summary

To configure DMA correctly:

1. Choose the correct **stream + channel** for your peripheral (see
    reference manual).
2. Select appropriate **priority** based on system load.
3. Match **width** to peripheral data size.
4. Set correct **direction** of transfer.

------------------------------------------------------------------------

## API Summary

### init()

Configures the stream (channel, priority, width, direction, increment
modes, etc.).\
Does **not** start a transfer.

``` cpp
dma.init();
```

------------------------------------------------------------------------

### start(source, destination, num_items)

Starts a transfer by:

- Disabling the stream and waiting for it to fully stop\
- Clearing DMA flags\
- Validating addresses and `num_items`\
- Writing PAR / M0AR / NDTR\
- Enabling the stream

``` cpp
dma.start(source, destination, num_items);
```

------------------------------------------------------------------------

### abort()

Emergency stop:

- Disables the stream and waits for EN == 0\
- Clears DMA flags

``` cpp
dma.abort();
```

Resume-from-mid-transfer is intentionally not supported for Micromouse
burst use cases.

------------------------------------------------------------------------

## Safety Checks

- Address range checks for SRAM / Flash / Peripheral space\
- NDTR range enforced (1--65535)\
- Alignment checks based on configured transfer width\
- Clears all 5 stream flags before restarting

------------------------------------------------------------------------

## Typical Use Case (ADC Burst)

``` cpp
adc_dma.init();

adc_dma.start(
    reinterpret_cast<uintptr_t>(&ADC1->DR),
    reinterpret_cast<uintptr_t>(buffer),
    sample_count
);
```

Used for:

- IR sensor sampling\
- Batch ADC reads\
- Deterministic burst acquisition
