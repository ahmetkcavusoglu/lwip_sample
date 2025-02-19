#ifndef LWIP_UART_COMM_H
#define LWIP_UART_COMM_H

#include "lwip/opt.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "lwip/ip_addr.h"
#include "usart.h"

#define TCP_SERVER_PORT 5000
#define RX_BUFFER_SIZE  64  // UART RX buffer boyutu

extern struct netif gnetif; // <---- gnetif'yi dışarıdan al

extern struct tcp_pcb *tcp_server_pcb; 
extern uint8_t rx_buffer[RX_BUFFER_SIZE];  
extern uint8_t rx_data;  
extern uint8_t rx_index;  
extern UART_HandleTypeDef huart3;

void tcp_server_init(void);
err_t tcp_server_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err);
err_t tcp_server_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void uart_receive_callback(void);

#endif /* LWIP_UART_COMM_H */
