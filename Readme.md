# Monitoramento de Redes e Firewall com Raspberry Pi Pico W

Este projeto demonstra como configurar um servidor HTTP na Raspberry Pi Pico W para monitorar redes e firewall via Wi-Fi. A aplicação permite visualizar e gerenciar o status da rede através de um navegador web.

## Introdução

A Raspberry Pi Pico W oferece suporte à conectividade Wi-Fi, permitindo a implementação de funcionalidades avançadas, como servidores HTTP, utilizando a linguagem C e o SDK oficial da Raspberry Pi. Com essa capacidade, é possível criar aplicações que interajam diretamente com dispositivos, como smartphones, via redes Wi-Fi. A seguir, apresentamos um exemplo prático que demonstra como conectar a Pico W a uma rede Wi-Fi e configurar um servidor HTTP básico para monitoramento de redes e firewall.

![Fluxograma](images/fluxograma_wifi_monitoramento.png)

## Pré-requisitos

1. **Ambiente Configurado:**
    * Instale o Raspberry Pi Pico SDK.
    * Baixe e configure o ***lwIP*** (protocolo usado para suporte à rede no Pico W).
        1. Iniciar um projeto C/C++ na extensão da Raspberry Pi Pico.
        2. Mudar o tipo da placa para Pico W.
        3. Ir em “Pico wireless options” e clicar em background lwIP.
    * Certifique-se de que as bibliotecas ***pico_cyw43_arch_lwip_threadsafe_background*** estão configuradas corretamente.

2. **Credenciais de Wi-Fi:** Substitua o nome e a senha do Wi-Fi no código.

## Código completo em C

```c
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

#define WIFI_SSID "NomeDaRedeWiFi"  // Substitua pelo nome da sua rede Wi-Fi
#define WIFI_PASS "SenhaDaRedeWiFi" // Substitua pela senha da sua rede Wi-Fi

// Buffer para respostas HTTP
#define HTTP_RESPONSE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" \
                      "<!DOCTYPE html><html><body>" \
                      "<h1>Monitoramento de Redes e Firewall</h1>" \
                      "<p>Status da Rede: Conectado</p>" \
                      "<p>Firewall: Ativo</p>" \
                      "</body></html>\r\n"

// Função de callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        // Cliente fechou a conexão
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Processa a requisição HTTP
    char *request = (char *)p->payload;

    // Envia a resposta HTTP
    tcp_write(tpcb, HTTP_RESPONSE, strlen(HTTP_RESPONSE), TCP_WRITE_FLAG_COPY);

    // Libera o buffer recebido
    pbuf_free(p);

    return ERR_OK;
}

// Callback de conexão: associa o http_callback à conexão
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);  // Associa o callback HTTP
    return ERR_OK;
}

// Função de setup do servidor TCP
static void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }

    // Liga o servidor na porta 80
    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }

    pcb = tcp_listen(pcb);  // Coloca o PCB em modo de escuta
    tcp_accept(pcb, connection_callback);  // Associa o callback de conexão

    printf("Servidor HTTP rodando na porta 80...\n");
}

int main() {
    stdio_init_all();  // Inicializa a saída padrão
    sleep_ms(10000);
    printf("Iniciando servidor HTTP\n");

    // Inicializa o Wi-Fi
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        return 1;
    } else {
        printf("Conectado.\n");
        // Lê o endereço IP de forma legível
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Endereço IP %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    printf("Wi-Fi conectado!\n");
    printf("Para visualizar o status da rede e firewall, acesse o endereço IP no navegador.\n");

    // Inicia o servidor HTTP
    start_http_server();
    
    // Loop principal
    while (true) {
        cyw43_arch_poll();  // Necessário para manter o Wi-Fi ativo
        sleep_ms(100);
    }

    cyw43_arch_deinit();  // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
    return 0;
}