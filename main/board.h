#pragma once

// MCU masters QSPI_ to FPGA
#define PIN_NUM_QSPI_CS          GPIO_NUM_5 // D10
#define PIN_NUM_QSPI_CLK         GPIO_NUM_18 // D9
#define PIN_NUM_QSPI_MOSI        GPIO_NUM_23 // D8
#define PIN_NUM_QSPI_MISO        GPIO_NUM_19 // D7
#define PIN_NUM_QSPI_WP          GPIO_NUM_22 // D6
#define PIN_NUM_QSPI_HD          GPIO_NUM_21 // D5

// I2S
#define PIN_NUM_I2S_BCLK    GPIO_NUM_33 //D16
#define PIN_NUM_I2S_WS      GPIO_NUM_25 //D15
#define PIN_NUM_I2S_DIN     GPIO_NUM_26 //D14
#define PIN_NUM_I2S_DOUT    GPIO_NUM_27 //D13

// Low latency low throughput async uart
#define PIN_NUM_UART_FROM_FPGA  GPIO_NUM_10 //D11
#define PIN_NUM_UART_TO_FPGA    GPIO_NUM_9 //D12
