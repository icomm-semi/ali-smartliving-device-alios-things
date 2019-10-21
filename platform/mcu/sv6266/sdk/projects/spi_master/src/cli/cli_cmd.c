#include "soc_types.h"
#include "cli.h"
#include "cli_cmd.h"
#include "osal.h"
#include "spimst/drv_spimst.h"
#include "gpio/hal_gpio.h"
#include "sys_clock.h"

/*---------------------------------- CMDs -----------------------------------*/

uint8_t *write_buf;
uint8_t *read_buf;

static void test_spi_master_dma_transfer_common_usage(const char *cmd) {
    printf("Usage           : %s <clk_Hz> <length> <cpha> <cpol> <csn>\n", cmd);
    printf("<clk_Hz>        : SPI Clock (BUS 40MHz : 611 - 10MHz, BUS 80MHz : 1221 - 20MHz)/in Auto CSN\n");
    printf("<clk_Hz>        : SPI Clock (BUS 40MHz : 611 - 20MHz, BUS 80MHz : 1221 - 40MHz)/in manual CSN\n");
    printf("<length>        : data length(1-8190), if over 4096 must be an even number\n");
    printf("<cpha>          : CPHA(0, 1)\n");
    printf("<cpol>          : CPOL(0, 1)\n");
    printf("<csn>           : GPIO_13 is auto CSN, otherwise manual CSN\n");
}
static void test_spi_master_transfer_common_usage(const char *cmd) {
    printf("Usage           : %s <clk_Hz> <length> <cpha> <cpol> <csn>\n", cmd);
    printf("<clk_Hz>        : SPI Clock (BUS 40MHz : 611 - 10MHz, BUS 80MHz : 1221 - 20MHz)/in Auto CSN\n");
    printf("<clk_Hz>        : SPI Clock (BUS 40MHz : 611 - 20MHz, BUS 80MHz : 1221 - 40MHz)/in manual CSN\n");
    printf("<length>        : data length(1-32768)\n");
    printf("<cpha>          : CPHA(0, 1)\n");
    printf("<cpol>          : CPOL(0, 1)\n");
    printf("<csn>           : GPIO_13 is auto CSN, otherwise manual CSN\n");
}
static void test_spi_master_common_flash_transfer_usage(const char *cmd) {
    printf("Usage           : %s <clk_Hz> <length> <cpha> <cpol> <csn>\n", cmd);
    printf("<clk_Hz>        : SPI Clock (BUS 40MHz : 611 - 10MHz, BUS 80MHz : 1221 - 20MHz)/in Auto CSN\n");
    printf("<clk_Hz>        : SPI Clock (BUS 40MHz : 611 - 20MHz, BUS 80MHz : 1221 - 40MHz)/in manual CSN\n");
    printf("<length>        : data length(1-Flash size)\n");
    printf("<cpha>          : CPHA(0, 1)\n");
    printf("<cpol>          : CPOL(0, 1)\n");
    printf("<csn>           : GPIO_13 is auto CSN, otherwise manual CSN\n");
}
int test_spi_master_dma_data_loopback_transfer(s32 argc, s8 *argv[]) { 
    uint32_t data_length = 0;
    uint32_t clk_Hz = 0;
    uint8_t cpha = 0;
    uint8_t cpol = 0;
    int8_t *rx_buf;
    int8_t *tx_buf;
    uint32_t csn = 0;

    if (argc != 6) {
        test_spi_master_dma_transfer_common_usage(argv[0]);
        return 0;
    }

    clk_Hz = strtoul(argv[1], NULL, 10);
    data_length = strtoul(argv[2], NULL, 10);
    cpha = strtoul(argv[3], NULL, 10);
    cpol = strtoul(argv[4], NULL, 10);
    csn = strtoul(argv[5], NULL, 10);

    if ((clk_Hz <= sys_bus_clock()/65534) || (clk_Hz > sys_bus_clock()/2) || (data_length < 1)|| (cpha > 1) || (cpol > 1)) {
        test_spi_master_dma_transfer_common_usage(argv[0]);
        return 0;
    }

    drv_spimst_init(clk_Hz, cpha, cpol);
    rx_buf = drv_spimst_alloc_dma_buffer(data_length);
    tx_buf = drv_spimst_alloc_dma_buffer(data_length);

    int i;
    for (i = 0; i < data_length; i++) {
        tx_buf[i] = i % 256;
        rx_buf[i] = 0;
    }
    drv_spimst_set_dma_buffer(data_length, tx_buf, rx_buf);

    drv_spimst_dma_trx(tx_buf, rx_buf, data_length, csn);
    int error = 0;
    for (i = 0; i < data_length; i++) {
        if (rx_buf[i] != tx_buf[i]) {
            error = 1;
        }
    }
    drv_spimst_free_dma_buffer(tx_buf);
    drv_spimst_free_dma_buffer(rx_buf);
    drv_spimst_deinit();

    if (error) {
        printf("data compare failed!\n");
    } else {
        printf("data compare success!\n");
    }
}

int test_spi_master_dma_data_transfer(s32 argc, s8 *argv[]) { 
    uint32_t data_length = 0;
    uint32_t clk_Hz = 0;
    uint8_t cpha = 0;
    uint8_t cpol = 0;
    int8_t *rx_buf;
    int8_t *tx_buf;
    uint32_t csn = 0;

    if (argc != 6) {
        test_spi_master_dma_transfer_common_usage(argv[0]);
        return 0;
    }

    clk_Hz = strtoul(argv[1], NULL, 10);
    data_length = strtoul(argv[2], NULL, 10);
    cpha = strtoul(argv[3], NULL, 10);
    cpol = strtoul(argv[4], NULL, 10);
    csn = strtoul(argv[5], NULL, 10);

    if ((clk_Hz <= sys_bus_clock()/65534) || (clk_Hz > sys_bus_clock()/2) || (data_length < 1)|| (cpha > 1) || (cpol > 1)) {
        test_spi_master_dma_transfer_common_usage(argv[0]);
        return 0;
    }

    drv_spimst_init(clk_Hz, cpha, cpol);
    rx_buf = drv_spimst_alloc_dma_buffer(data_length);
    tx_buf = drv_spimst_alloc_dma_buffer(data_length);
    drv_spimst_set_dma_buffer(data_length, tx_buf, rx_buf);


    int i;
    for (i = 0; i < data_length; i++) {
        tx_buf[i] = i % 256;
        rx_buf[i] = 0;
    }
    drv_spimst_dma_trx(tx_buf, rx_buf, data_length, csn);
    drv_spimst_free_dma_buffer(tx_buf);
    drv_spimst_free_dma_buffer(rx_buf);
    drv_spimst_deinit();

    printf("\n[SPI Master Test : PASS!]\n");
    return 0;
}

static uint8_t dma_flash_read_busy(uint8_t *tx_buf, uint8_t *rx_buf, uint32_t csn) {
    /* Set flash command : Write Enable. */
    tx_buf[0] = 0x05;
    /* Enable SPI HW. */
    drv_spimst_dma_trx(tx_buf, rx_buf, 2, csn);
    return rx_buf[1];
}

static uint8_t dma_wait_flash_busy(uint8_t *tx_buf, uint8_t *rx_buf, uint32_t csn) {
    while((dma_flash_read_busy(tx_buf, rx_buf, csn) & 0x1) != 0) {
        OS_MsDelay(500);
    }
}

static void dma_flash_write_enable(uint8_t *tx_buf, uint8_t *rx_buf, uint32_t csn) {
    /* Set flash command : Write Enable. */
    tx_buf[0] = 0x06;
    /* Enable SPI HW. */
    drv_spimst_dma_trx(tx_buf, rx_buf, 1, csn);
    dma_wait_flash_busy(tx_buf, rx_buf, csn);
}

int test_spi_master_dma_flash_transfer(s32 argc, s8 *argv[]) {
    uint8_t ret = 0x0; 
    uint32_t i = 0;
    uint32_t data_length = 0;
    uint32_t clk_Hz = 0;
    uint8_t cpha = 0;
    uint8_t cpol = 0;
    uint32_t csn;
    int8_t *rx_buf;
    int8_t *tx_buf;

    if (argc != 6) {
        test_spi_master_common_flash_transfer_usage(argv[0]);
        return 0;
    }

    clk_Hz = strtoul(argv[1], NULL, 10);
    data_length = strtoul(argv[2], NULL, 10);
    cpha = strtoul(argv[3], NULL, 10);
    cpol = strtoul(argv[4], NULL, 10);
    csn = strtoul(argv[5], NULL, 10);

    if ((clk_Hz <= sys_bus_clock()/65534) || (clk_Hz > sys_bus_clock()/2) || (data_length < 1)|| (cpha > 1) || (cpol > 1)) {
        test_spi_master_common_flash_transfer_usage(argv[0]);
        return 0;
    }

    drv_spimst_init(clk_Hz, cpha, cpol);
    rx_buf = drv_spimst_alloc_dma_buffer(256+4); // max page write 256.
    tx_buf = drv_spimst_alloc_dma_buffer(256+4);
    drv_spimst_set_dma_buffer(256+4, tx_buf, rx_buf);

    /* Set flash command : Read Manufacturer/Device ID. */
    tx_buf[0] = 0x90;
    tx_buf[1] = 0x00;
    tx_buf[2] = 0x00;
    tx_buf[3] = 0x00;
    /* Enable SPI HW. */
    // get Manufacturer ID, Device ID.
    drv_spimst_dma_trx(tx_buf, rx_buf, 6, csn); /* The size "+2" is for manufacturer ID and device ID. */

    printf("======================================\n");
    printf("Manufacturer ID:  0x%02x\n", rx_buf[4]);
    printf("Device ID:  0x%02x\n", rx_buf[5]);

    printf("before write enable status: %xh\n", dma_flash_read_busy(tx_buf, rx_buf, csn));
    dma_flash_write_enable(tx_buf, rx_buf, csn);
    printf("after write enable status: %xh\n", dma_flash_read_busy(tx_buf, rx_buf, csn));

    /* Set flash command : chip Erase. */
    tx_buf[0] = 0x60;
    tx_buf[1] = 0x00;
    tx_buf[2] = 0x00;
    tx_buf[3] = 0x00;
    /* Enable SPI HW. */
    drv_spimst_dma_trx(tx_buf, rx_buf, 1, csn);
    printf("after chip erase status: %xh\n", dma_flash_read_busy(tx_buf, rx_buf, csn));

    dma_wait_flash_busy(tx_buf, rx_buf, csn);

    int j;
    printf("Write to Flash:\n");
    printf("============================================================\n");
    for (j = 0; j < data_length; j+=256) {
        int len = ((data_length-j)>256)?256:(data_length-j);
        dma_flash_write_enable(tx_buf, rx_buf, csn);

        int i;
        for (i = 0; i < len; i++) {
            tx_buf[4+i] = OS_Random();
            printf("%08xh", tx_buf[4+i]);
            if (((i+1)%16)==0) {
                printf("\n");
            } else {
                printf(" ");
            }
        }
        /* Set flash command : Page Program. */
        tx_buf[0] = 0x02;
        tx_buf[1] = ((j>>16) & 0xFF);
        tx_buf[2] = ((j>>8) & 0xFF);
        tx_buf[3] = (j&0xFF);
        /* Enable SPI HW. */
        ret = drv_spimst_dma_trx(tx_buf, rx_buf, len+4, csn);
        dma_wait_flash_busy(tx_buf, rx_buf, csn);
    }
    printf("\n============================================================\n");


    printf("Read from Flash:\n");
    printf("============================================================\n");
    for (j = 0; j < data_length; j+=256) {
        int len = ((data_length-j)>256)?256:(data_length-j);
        int i;
        for (i = 0; i < len; i++) {
            rx_buf[4+i] = OS_Random();
        }

        /* Set flash command : Read. */
        tx_buf[0] = 0x03;
        tx_buf[1] = ((j>>16) & 0xFF);
        tx_buf[2] = ((j>>8) & 0xFF);
        tx_buf[3] = (j&0xFF);
        /* Enable SPI HW. */
        drv_spimst_dma_trx(tx_buf, rx_buf, len+4, csn);
        for (i = 0; i < len; i++) {
            printf("%08xh", rx_buf[4+i]);
            if (((i+1)%16)==0) {
                printf("\n");
            } else {
                printf(" ");
            }
        }
    }
    printf("\n============================================================\n");
    drv_spimst_free_dma_buffer(tx_buf);
    drv_spimst_free_dma_buffer(rx_buf);
    drv_spimst_deinit();

    return 0;
}

static uint8_t flash_read_busy(uint8_t *tx_buf, uint8_t *rx_buf, uint32_t csn) {
    /* Set flash command : Write Enable. */
    tx_buf[0] = 0x05;
    /* Enable SPI HW. */
    drv_spimst_trx(tx_buf, rx_buf, 2, csn);
    return rx_buf[1];
}

static void wait_flash_busy(uint8_t *tx_buf, uint8_t *rx_buf, uint32_t csn) {
    while((flash_read_busy(tx_buf, rx_buf, csn) & 0x1) != 0) {
        OS_MsDelay(500);
    }
}

static void flash_write_enable(uint8_t *tx_buf, uint8_t *rx_buf, uint32_t csn) {
    /* Set flash command : Write Enable. */
    tx_buf[0] = 0x06;
    /* Enable SPI HW. */
    drv_spimst_trx(tx_buf, rx_buf, 1, csn);
    wait_flash_busy(tx_buf, rx_buf, csn);
}

int test_spi_master_flash_transfer(s32 argc, s8 *argv[]) {
    uint8_t ret = 0x0; 
    uint32_t i = 0;
    uint32_t data_length = 0;
    uint32_t clk_Hz = 0;
    uint8_t cpha = 0;
    uint8_t cpol = 0;
    uint32_t csn;
    int8_t *rx_buf;
    int8_t *tx_buf;

    if (argc != 6) {
        test_spi_master_common_flash_transfer_usage(argv[0]);
        return 0;
    }

    clk_Hz = strtoul(argv[1], NULL, 10);
    data_length = strtoul(argv[2], NULL, 10);
    cpha = strtoul(argv[3], NULL, 10);
    cpol = strtoul(argv[4], NULL, 10);
    csn = strtoul(argv[5], NULL, 10);

    if ((clk_Hz <= sys_bus_clock()/65534) || (clk_Hz > sys_bus_clock()/2) || (data_length < 1)|| (cpha > 1) || (cpol > 1)) {
        test_spi_master_common_flash_transfer_usage(argv[0]);
        return 0;
    }

    drv_spimst_init(clk_Hz, cpha, cpol);
    rx_buf = OS_MemAlloc(256+4);
    tx_buf = OS_MemAlloc(256+4);

    /* Set flash command : Read Manufacturer/Device ID. */
    tx_buf[0] = 0x90;
    tx_buf[1] = 0x00;
    tx_buf[2] = 0x00;
    tx_buf[3] = 0x00;
    /* Enable SPI HW. */
    // get Manufacturer ID, Device ID.
    drv_spimst_trx(tx_buf, rx_buf, 6, csn); /* The size "+2" is for manufacturer ID and device ID. */

    printf("======================================\n");
    printf("Manufacturer ID:  0x%02x\n", rx_buf[4]);
    printf("Device ID:  0x%02x\n", rx_buf[5]);

    printf("before write enable status: %xh\n", flash_read_busy(tx_buf, rx_buf, csn));
    flash_write_enable(tx_buf, rx_buf, csn);
    printf("after write enable status: %xh\n", flash_read_busy(tx_buf, rx_buf, csn));

    /* Set flash command : Chip Erase. */
    tx_buf[0] = 0x60;
    tx_buf[1] = 0x00;
    tx_buf[2] = 0x00;
    tx_buf[3] = 0x00;
    /* Enable SPI HW. */
    drv_spimst_trx(tx_buf, rx_buf, 1, csn);
    printf("after chip erase status: %xh\n", flash_read_busy(tx_buf, rx_buf, csn));

    wait_flash_busy(tx_buf, rx_buf, csn);

    int j;
    printf("Write to Flash:\n");
    printf("============================================================\n");
    for (j = 0; j < data_length; j+=256) {
        int len = ((data_length-j)>256)?256:(data_length-j);
        flash_write_enable(tx_buf, rx_buf, csn);

        int i;
        for (i = 0; i < len; i++) {
            tx_buf[4+i] = OS_Random();
            printf("%08xh", tx_buf[4+i]);
            if (((i+1)%16)==0) {
                printf("\n");
            } else {
                printf(" ");
            }
        }
        /* Set flash command : Page Program. */
        tx_buf[0] = 0x02;
        tx_buf[1] = ((j>>16) & 0xFF);
        tx_buf[2] = ((j>>8) & 0xFF);
        tx_buf[3] = (j&0xFF);
        /* Enable SPI HW. */
        ret = drv_spimst_trx(tx_buf, rx_buf, len+4, csn);
        wait_flash_busy(tx_buf, rx_buf, csn);
    }
    printf("\n============================================================\n");


    printf("Read from Flash:\n");
    printf("============================================================\n");
    for (j = 0; j < data_length; j+=256) {
        int len = ((data_length-j)>256)?256:(data_length-j);
        int i;
        for (i = 0; i < len; i++) {
            rx_buf[4+i] = OS_Random();
        }

        /* Set flash command : Read. */
        tx_buf[0] = 0x03;
        tx_buf[1] = ((j>>16) & 0xFF);
        tx_buf[2] = ((j>>8) & 0xFF);
        tx_buf[3] = (j&0xFF);
        /* Enable SPI HW. */
        drv_spimst_trx(tx_buf, rx_buf, len+4, csn);
        for (i = 0; i < len; i++) {
            printf("%08xh", rx_buf[4+i]);
            if (((i+1)%16)==0) {
                printf("\n");
            } else {
                printf(" ");
            }
        }
    }
    printf("\n============================================================\n");
    OS_MemFree(tx_buf);
    OS_MemFree(rx_buf);
    drv_spimst_deinit();
    return 0;
}

//////////////////////



int test_spi_master_data_transfer(s32 argc, s8 *argv[]) {
    uint8_t ret = 0x0; 
    uint32_t i = 0;
    uint32_t data_length = 0;
    uint32_t clk_Hz = 0;
    uint8_t cpha = 0;
    uint8_t cpol = 0;
    uint8_t csn = 0;

    if (argc != 6) {
        test_spi_master_transfer_common_usage(argv[0]);
        return 0;
    }

    clk_Hz = strtoul(argv[1], NULL, 10);
    data_length = strtoul(argv[2], NULL, 10);
    cpha = strtoul(argv[3], NULL, 10);
    cpol = strtoul(argv[4], NULL, 10);
    csn = strtoul(argv[5], NULL, 10);

    if ((clk_Hz <= sys_bus_clock()/65534) || (clk_Hz > sys_bus_clock()/2) || (data_length < 1)|| (cpha > 1) || (cpol > 1)) {
        test_spi_master_transfer_common_usage(argv[0]);
        return 0;
    }

    /* Initialize tx/rx buffer. */
    write_buf = OS_MemAlloc(data_length);
    read_buf = OS_MemAlloc(data_length);
    for(i = 0; i < data_length; i ++) {
        write_buf[i] = i%256;
        read_buf[i] = 0;
    }

    /* Initialize SPI HW. */
    drv_spimst_init(clk_Hz, cpha, cpol);

    /* Start SPI operation. */
    ret = drv_spimst_trx(write_buf, read_buf, data_length, csn);
    OS_MemFree(write_buf);
    OS_MemFree(read_buf);

    printf("\n[SPI Master Test : PASS!]\n");

    return 0;
}
int test_spi_master_data_loopback_transfer(s32 argc, s8 *argv[]) { 
    uint32_t data_length = 0;
    uint32_t clk_Hz = 0;
    uint8_t cpha = 0;
    uint8_t cpol = 0;
    int8_t *rx_buf;
    int8_t *tx_buf;
    uint32_t csn = 0;

    if (argc != 6) {
        test_spi_master_transfer_common_usage(argv[0]);
        return 0;
    }

    clk_Hz = strtoul(argv[1], NULL, 10);
    data_length = strtoul(argv[2], NULL, 10);
    cpha = strtoul(argv[3], NULL, 10);
    cpol = strtoul(argv[4], NULL, 10);
    csn = strtoul(argv[5], NULL, 10);

    if ((clk_Hz <= sys_bus_clock()/65534) || (clk_Hz > sys_bus_clock()/2) || (data_length < 1)|| (cpha > 1) || (cpol > 1)) {
        test_spi_master_transfer_common_usage(argv[0]);
        return 0;
    }

    drv_spimst_init(clk_Hz, cpha, cpol);
    tx_buf = OS_MemAlloc(data_length);
    rx_buf = OS_MemAlloc(data_length);

    int i;
    for (i = 0; i < data_length; i++) {
        tx_buf[i] = i % 256;
        rx_buf[i] = 0;
    }

    drv_spimst_trx(tx_buf, rx_buf, data_length, csn);
    int error = 0;
    for (i = 0; i < data_length; i++) {
        if (rx_buf[i] != tx_buf[i]) {
            error = 1;
        }
    }
    OS_MemFree(tx_buf);
    OS_MemFree(rx_buf);
    drv_spimst_deinit();

    if (error) {
        printf("data compare failed!\n");
    } else {
        printf("data compare success!\n");
    }
}


static void Cmd_help (int32_t argc, char *argv[])
{
    printf ("\n*************************************************************************\n");
    printf ("Test SPI Target : \n");
    printf ("                   GPIO_08 : SPI_M_CLK\n");
    printf ("                   GPIO_10 : SPI_M_MISO\n");
    printf ("                   GPIO_12 : SPI_M_MOSI\n");
    printf ("                   GPIO_13 : SPI_M_CS\n");
    printf ("\n*************************************************************************\n");
}


/* ---------------------- Registered CMDs to CMD Table ---------------------- */
extern void _notify_host_reset();
const CLICmds gCliCmdTable[] =
{	
    { "help"                      , Cmd_help                                   , "SPI Master test help"                                            } ,
    { "spi_dma_transfer"          , test_spi_master_dma_data_transfer          , "spi_dma_transfer <clk_Hz> <length> <cpha> <cpol> <csn>"          } ,
    { "spi_dma_flash_transfer"    , test_spi_master_dma_flash_transfer         , "spi_dma_flash_transfer <clk_Hz> <length> <cpha> <cpol> <csn>"    } ,
    { "spi_dma_loopback_transfer" , test_spi_master_dma_data_loopback_transfer , "spi_dma_loopback_transfer <clk_Hz> <length> <cpha> <cpol> <csn>" } ,
    { "spi_transfer"              , test_spi_master_data_transfer              , "spi_transfer <clk_Hz> <length> <cpha> <cpol> <csn>"              } ,
    { "spi_flash_transfer"        , test_spi_master_flash_transfer             , "spi_flash_transfer <clk_Hz> <length> <cpha> <cpol> <csn>"        } ,
    { "spi_loopback_transfer"     , test_spi_master_data_loopback_transfer     , "spi_loopback_transfer <clk_Hz> <length> <cpha> <cpol> <csn>"     } ,
    { (const char *)NULL          , (CliCmdFunc)NULL                           , (const char *)NULL                                                } ,
};


