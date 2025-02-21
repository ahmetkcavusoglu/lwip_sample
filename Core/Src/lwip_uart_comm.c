#include "lwip_uart_comm.h"
#include <string.h>
#include "lwip.h"
#include "stdbool.h"

struct tcp_pcb *tcp_server_pcb;
static struct tcp_pcb *global_pcb = NULL;
uint8_t rx_buffer[RX_BUFFER_SIZE];  
uint8_t rx_data;  
uint8_t rx_index = 0;  
uint8_t receiving = 0;
static bool uart_to_send_tcp = false;

/**
 * @brief  TCP veri alındığında çağrılan callback fonksiyon
 */
err_t tcp_server_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }
    HAL_UART_Transmit(&huart3, (uint8_t *)p->payload, p->len, HAL_MAX_DELAY);
    pbuf_free(p);
    return ERR_OK;
}

/**
 * @brief  TCP bağlantısı kabul edildiğinde çağrılan callback fonksiyon
 */
err_t tcp_server_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    const char *msg = "Connected.\r\n";
    if (netif_is_link_up(&gnetif))
    {
        tcp_write(newpcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);

    }
    global_pcb = newpcb;

    //tcp_output(newpcb);
    tcp_recv(newpcb, tcp_server_recv_callback);
    return ERR_OK;
}

void send_uart_data_to_tcp(void)
{
    if (uart_to_send_tcp && global_pcb) // Eğer UART verisi alındıysa ve TCP bağlantısı varsa
    {
        tcp_write(global_pcb, rx_buffer, strlen(rx_buffer), TCP_WRITE_FLAG_COPY);
        tcp_output(global_pcb); 
        //memset(rx_buffer, 0, sizeof(rx_buffer)); 
        rx_index = 0;
        uart_to_send_tcp = false;
    }
}
/**
 * @brief  TCP server başlatma fonksiyonu
 */
void tcp_server_init(void)
{
    tcp_server_pcb = tcp_new();
    if (tcp_server_pcb == NULL) {
        return;
    }

    tcp_bind(tcp_server_pcb, IP_ADDR_ANY, TCP_SERVER_PORT);
    tcp_server_pcb = tcp_listen(tcp_server_pcb);
    tcp_accept(tcp_server_pcb, tcp_server_accept_callback);
}

/**
 * @brief  UART RX Tamamlama Callback Fonksiyonu
 */
void uart_receive_callback(void)
{
    if (rx_index >= RX_BUFFER_SIZE - 1)
    {
        rx_index = 0;
        receiving = 0;
    }

    rx_buffer[rx_index++] = rx_data;
    rx_buffer[rx_index] = '\0';

    if (!receiving && strstr((char *)rx_buffer, "Start"))
    {
        receiving = 1;
        memset(rx_buffer, 0, sizeof(rx_buffer));
        rx_index = 0;
    }
    if (receiving && rx_index > 0)
    {
        uart_to_send_tcp = true;
        rx_buffer[rx_index] = '\0';
    }

    if (receiving && strstr((char *)rx_buffer, "Hello"))
    {
        uart_to_send_tcp = true;
        rx_index = 0;
    }
    HAL_UART_Receive_IT(&huart3, &rx_data, 1);
}
