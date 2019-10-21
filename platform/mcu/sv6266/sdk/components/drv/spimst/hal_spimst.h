#ifndef HAL_SPIMST
#define HAL_SPIMST

typedef void (*SPI_MST_RX_ISR) (uint8_t *rx_data);

typedef enum {
    SPI_MST_CPHA_0          = 0,
    SPI_MST_CPHA_1          = 1
} ENUM_SPI_MST_CPHA_T;

typedef enum {
    SPI_MST_CPOL_0          = 0,
    SPI_MST_CPOL_1          = 1
} ENUM_SPI_MST_CPOL_T;


typedef enum {
    SPI_MST_TRX_MODE = 0,
    SPI_MST_TX_MODE  = 1,
    SPI_MST_RX_MODE  = 2,
} spimst_trx_mode_t;


typedef enum {
    SPI_MST_TXE_INT         = 0,
    SPI_MST_TXO_INT         = 1,
    SPI_MST_RXU_INT         = 2,
    SPI_MST_RXO_INT         = 3,
    SPI_MST_RXF_INT         = 4
} ENUM_SPI_MST_INT_STATUS_T;

typedef enum {
    SPI_MST_TRX_BUSY        = 0,
    SPI_MST_TX_NOT_FULL     = 1,
    SPI_MST_TX_EMPTY        = 2,
    SPI_MST_RX_NOT_EMPTY    = 3,
    SPI_MST_RX_FULL         = 4,
    SPI_MST_TX_ERROR        = 5
} ENUM_SPI_MST_STATUS_T;


void hal_spimst_set_trx_mode(spimst_trx_mode_t trx_mode);
void hal_spimst_set_cpha_cpol(ENUM_SPI_MST_CPHA_T cpha, ENUM_SPI_MST_CPOL_T cpol);
void hal_spimst_set_clk(uint32_t clk_Hz);
void hal_spimst_enable(void);
void hal_spimst_disable(void);
void hal_spimst_init(uint32_t clk_Hz, ENUM_SPI_MST_CPHA_T cpha, ENUM_SPI_MST_CPOL_T cpol);
void hal_spimst_deinit(void);
int8_t hal_spimst_get_interrupt_status(uint8_t bit);
int8_t hal_spimst_get_status(uint8_t bit) __attribute__((section(".prog_in_sram")));
int8_t hal_spimst_transfer(uint8_t *tx_data, uint8_t *rx_data, uint32_t data_length);
int8_t hal_spimst_dma_trx(uint8_t *tx_data, uint8_t *rx_data, uint32_t data_length, SPI_MST_RX_ISR rx_done_isr);
/*! \brief prepare buffer for spi master driver
 *
 *  prepare buffer for spi master driver when you want to use dma mode.
 *
 * \param length it means the DMA one transfer max length
 * \param tx_buf Tx buffer pointer, Null if Rx Only, the buffer size = 4*length
 * \param rx_buf Rx buffer pointer, Null if Tx Only, the buffer size = 4*length
 * \return 0 success
 *         -1 failed.
 */
int32_t hal_spimst_set_dma_buffer(uint32_t length, uint8_t *tx_buf, uint8_t *rx_buf);
void hal_spimst_set_latch_latency(uint32_t latency);

#endif /* end of include guard */
