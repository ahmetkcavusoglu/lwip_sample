#include "lwip_uart_comm.h"
#include <string.h>
#include "lwip.h"

struct tcp_pcb *tcp_server_pcb; // TCP kontrol bloğu
uint8_t rx_buffer[RX_BUFFER_SIZE];  
uint8_t rx_data;  
uint8_t rx_index = 0;  

/**
 * @brief  TCP veri alındığında çağrılan callback fonksiyon
 */
err_t tcp_server_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    if (strncmp((char*)p->payload, "hello", p->len) == 0) {
        tcp_write(tpcb, " world ", 7, TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
    }

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

    tcp_output(newpcb);
    tcp_recv(newpcb, tcp_server_recv_callback);
    return ERR_OK;
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
    if (rx_data == 'a') 
    {
        rx_buffer[rx_index++] = '\n';  
        HAL_UART_Transmit(&huart3, rx_buffer, rx_index, HAL_MAX_DELAY);
        rx_index = 0;  
    }
    else
    {
        rx_buffer[rx_index++] = rx_data;  
        if (rx_index >= RX_BUFFER_SIZE)  
        {
            rx_index = 0;
        }
    }

    HAL_UART_Receive_IT(&huart3, &rx_data, 1);  
}
