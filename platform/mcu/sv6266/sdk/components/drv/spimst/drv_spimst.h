#ifndef SPIMST
#define SPIMST
#include "hal_spimst.h"



/**
 * Initilize SPI bus to enable SPI operations.
 * 
 * @param   clk_Hz     SPI Clock (BUS 40MHz : 610 - 20M, BUS 80MHz : 1221 - 40MHzM)
 *              
 * @param   CPHA       Serial Clock Phase. 
 *              0x00 - Serial clock toggles in middle of first data bit
 *              0x01 - Serial clock toggles at start of first data bit
 * @param   CPOL       Serial Clock Polarity. 
 *              0x00 - Inactive state of serial clock is low
 *              0x01 - Inactive state of serial clock is high, need pull up in the clock pin.
 */
int32_t drv_spimst_init(uint32_t clk_Hz, ENUM_SPI_MST_CPHA_T CPHA, ENUM_SPI_MST_CPOL_T CPOL);



/**
 * Deinitialize SPI bus to disable SPI operations
 * 
 */
void drv_spimst_deinit(void);


/**
 * Transmit and receive the spi data by dma.
 * 
 * @param   write_data      Pointer to the data buffer to be written by the master.
 * @param   read_data       Pointer to the data buffer to be read by the master.
 * @param   length          Size of the data buffer exposed to the SPI master - max dma length : 8190 bytes for even length, 4095 for odd length.
 * @param   csn             Chip select pin.
 *
 * @return  -1              The operation error.
 * @return   0              The operation completed successfully.
 */
int8_t drv_spimst_dma_trx(uint8_t *write_data, uint8_t *read_data, uint32_t length, uint32_t csn);


/**
 * Transmit and receive the spi data.
 * 
 * @param   write_data      Pointer to the data buffer to be written by the master.
 * @param   read_data       Pointer to the data buffer to be read by the master.
 * @param   length          Size of the data buffer exposed to the SPI master.
 * @param   csn             Chip select pin.
 *
 * @return  -1              The operation error.
 * @return   0              The operation completed successfully.
 */
int8_t drv_spimst_trx(uint8_t *write_data, uint8_t *read_data, uint32_t length, uint32_t csn);

/*! \brief set up DMA SPI transfer buffer
 *
 *  set up DMA SPI transfer buffer
 *
 * \param max_length set the MAX length
 * \param tx_buf set tx_buffer ptr
 * \param rx_buf set rx buffer ptr
 */
void drv_spimst_set_dma_buffer(uint32_t max_length, uint8_t *tx_buf, uint8_t *rx_buf);

/*! \brief Alloc the DMA SPI transfer buffer
 *
 *  Alloc the DMA buffer
 *
 * \param max_length the DMA should transfer max size
 * \return NULL         System no resource.
 *         otherwise    The buffer.
 */
uint8_t *drv_spimst_alloc_dma_buffer(uint32_t max_length);

/*! \brief Alloc the DMA SPI transfer buffer, it must used by even data length.
 *
 *  Alloc the DMA buffer, little buffer size.
 *
 * \param max_length the DMA should transfer max size
 * \return NULL         System no resource.
 *         otherwise    The buffer.
 */
uint8_t *drv_spimst_alloc_dma_limited_buffer(uint32_t max_length);

/*! \brief Release the DMA SPI transfer buffer
 *
 *  Release the DMA SPI transfer buffer
 *
 * \param buffer the buffer alloc from drv_spimst_alloc_buffer
 */
void drv_spimst_free_dma_buffer(uint8_t *buffer);

#endif /* end of include guard */
