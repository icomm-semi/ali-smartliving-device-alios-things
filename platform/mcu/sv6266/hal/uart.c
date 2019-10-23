#include "hal/soc/soc.h"
#include "sys/dbgcon.h"
#include "drv_uart.h"
#include "osal.h"
#include "aos/errno.h"
#define ISR_MODE

uart_dev_t uart_0 = {
    .port = 0,
    .config = {
        .baud_rate = 921600,
        .data_width = DATA_WIDTH_8BIT,
        .parity = NO_PARITY,
    },
    .priv = NULL,
};

#define M_UART_BUFFER_SIZE          (256)

#define RINGBUF_SIZE        (M_UART_BUFFER_SIZE)
#define RINGBUF_IDX_WRAP    (RINGBUF_SIZE   -1)
#define RINGBUF_VIDX_WRAP   (RINGBUF_SIZE*2 -1)

typedef struct ringbuf {
    uint32_t vidx_w;
    uint32_t vidx_r;
    uint8_t  data[RINGBUF_SIZE];
} ringbuf_st;

ringbuf_st g_uart_rx_ringbuf = {
    0,
    0,
    {0},
};

#if defined(ISR_MODE)
OsEvent cli_rx_evt = NULL;
#endif

void uart_rx_isr(void)
{
    uint32_t vidx_w_full;
    uint32_t vidx_w;
    uint32_t idx_w;

    OS_DeclareCritical();
    OS_EnterCritical();
    vidx_w_full = (g_uart_rx_ringbuf.vidx_r + RINGBUF_SIZE) & RINGBUF_VIDX_WRAP;
    vidx_w      =  g_uart_rx_ringbuf.vidx_w;

    // # when data ready ,move all data in fifo into ringbuf.
    // ## if full, drop all and alert.
    // TODO: rx error
    while(hal_uart_get_line_status () & 1) {
        if(vidx_w != vidx_w_full) {
            // ### write data
            idx_w = vidx_w & RINGBUF_IDX_WRAP;
            g_uart_rx_ringbuf.data[idx_w] =  (uint8_t)hal_uart_direct_read_byte();
            //hal_uart_read_fifo (&g_uart_rx_ringbuf.data[idx_w], 1, UART_NON_BLOCKING);
            if(g_uart_rx_ringbuf.data[idx_w] == 0x00) {
                printf("\n!RX Error %d!\n", drv_uart_get_line_status());
            }
            // ### update pointer
            vidx_w = (vidx_w+1) & RINGBUF_VIDX_WRAP;
        }
        else {
            // force output to screen,
            printf("Queue Full!\n");
            putc(hal_uart_direct_read_byte());
            drv_uart_register_isr(UART_DATA_RDY_IE, NULL);
            break;
        }
    }
    // # update idx back to structure
    g_uart_rx_ringbuf.vidx_w = vidx_w;
    OS_ExitCritical();
    OS_EventSet(cli_rx_evt);
}

static uint32_t uart_read(uint32_t char_2read, uint8_t* dst) {

    uint32_t vidx_r;
    uint32_t vidx_r_empty;
    uint32_t idx_r ;
    uint32_t cnt = 0;

    OS_DeclareCritical();
    OS_EnterCritical();
    vidx_r       = g_uart_rx_ringbuf.vidx_r;
    vidx_r_empty = g_uart_rx_ringbuf.vidx_w;

    while((vidx_r != vidx_r_empty) && (cnt != char_2read)) {
        idx_r = vidx_r & RINGBUF_IDX_WRAP;
        //
        if(dst) {
            dst[cnt] = g_uart_rx_ringbuf.data[idx_r];
        }
        cnt++;
        //
        vidx_r = (vidx_r+1) & RINGBUF_VIDX_WRAP;
    }
    g_uart_rx_ringbuf.vidx_r = vidx_r;
    drv_uart_register_isr(UART_DATA_RDY_IE, uart_rx_isr);
    OS_ExitCritical();


    return cnt;
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    //drv_uart_init();
#if defined(ISR_MODE)
    if (OS_SUCCESS != OS_EventCreate(&cli_rx_evt)) {
        return EIO;
    }
    drv_uart_register_isr(UART_DATA_RDY_IE, uart_rx_isr);
#endif
	return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
#if defined(ISR_MODE)
    OS_DeclareCritical();
    OS_EnterCritical();
    drv_uart_register_isr(UART_DATA_RDY_IE, NULL);
    if (NULL != cli_rx_evt) {
        OS_EventDelete(cli_rx_evt);
        cli_rx_evt = NULL;
    }
    OS_ExitCritical();
#endif
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint8_t* pTmp = (uint8_t*)data;
    for (int i = 0; i < size; i++) {
        dbgcon_tx(*pTmp++);
    }
    return size;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t* pTmp = (uint8_t*)data;
    uint32_t size = 0;

    while (expect_size > 0)
    {
        int one_time_size = 0;
        size += (one_time_size = uart_read(expect_size, pTmp));
        expect_size -= one_time_size;
        pTmp += one_time_size;
#if defined(ISR_MODE)
        if (expect_size > 0) {
            if (OS_FAILED == OS_EventWait(cli_rx_evt, timeout)) {
                break;
            }
        }
#else
        if (!timeout)
        {
            break;
        }
        if (expect_size > 0)
        {
            if (timeout > 200) {
                OS_MsDelay(200);
                timeout-=200;
            } else {
                OS_MsDelay(timeout);
            }
        }
#endif
    }
    if (recv_size != NULL) {
        *recv_size = size;
    }

    //if (size == 0) return EIO;
    return 0;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout) {
    // aMH: duplicate function to reduce stack size.
    uint8_t* pTmp = (uint8_t*)data;
    uint32_t size = 0;

    while (expect_size > 0)
    {
        int one_time_size = 0;
        size += (one_time_size = uart_read(expect_size, pTmp));
        expect_size -= one_time_size;
        pTmp += one_time_size;
#if defined(ISR_MODE)
        if (expect_size > 0) {
            if (OS_FAILED == OS_EventWait(cli_rx_evt, timeout)) {
                return 0;
            }
        }
#else
        if (!timeout)
        {
            break;
        }
        if (expect_size > 0)
        {
            OS_MsDelay(10);
            timeout--;
        }
#endif
    }

    //if (size == 0) return EIO;
    return 0;
}

