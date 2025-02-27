#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

// Defina o SSID e a senha da sua rede Wi-Fi
#define WIFI_SSID "Iphone de Silas"
#define WIFI_PASS "kenjigostoso"

// Lista de bloqueio de IPs
const char *blocked_ips[] = {
    "192.168.0.100",  // Exemplo de IP bloqueado
    // Adicione mais IPs conforme necessário
};

// Função para verificar se o IP está na lista de bloqueio
bool is_ip_blocked(const char *ip) {
    for (int i = 0; i < sizeof(blocked_ips) / sizeof(blocked_ips[0]); i++) {
        if (strcmp(ip, blocked_ips[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Função para obter o IP do cliente
void get_client_ip(struct tcp_pcb *tpcb, char *ip_str) {
    ip_addr_t client_ip = tpcb->remote_ip;
    sprintf(ip_str, "%d.%d.%d.%d", ip4_addr1(&client_ip), ip4_addr2(&client_ip), ip4_addr3(&client_ip), ip4_addr4(&client_ip));
}

// Função para monitorar a rede
void monitor_network() {
    // Verifica o status da conexão Wi-Fi
    if (netif_is_link_up(&cyw43_state.netif[0])) {
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Conectado ao Wi-Fi. Endereço IP: %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    } else {
        printf("Desconectado do Wi-Fi\n");
    }
}

// Função de callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        // Cliente fechou a conexão
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Obtém o IP do cliente
    char client_ip[16];
    get_client_ip(tpcb, client_ip);

    // Verifica se o IP do cliente está bloqueado
    if (is_ip_blocked(client_ip)) {
        printf("Conexão bloqueada do IP: %s\n", client_ip);
        tcp_close(tpcb);
        return ERR_OK;
    }

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

    // Aumenta o tempo de espera para a conexão Wi-Fi
    int connect_status = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 20000);
    if (connect_status) {
        printf("Falha ao conectar ao Wi-Fi, código de erro: %d\n", connect_status);
        return 1;
    } else {
        printf("Conectado ao Wi-Fi.\n");
        // Read the IP address in a human readable way
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Endereço IP: %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    printf("Wi-Fi conectado!\n");

    // Inicia o servidor HTTP
    start_http_server();
    
    // Loop principal
    while (true) {
        cyw43_arch_poll();  // Necessário para manter o Wi-Fi ativo
        monitor_network();  // Chama a função de monitoramento da rede
        sleep_ms(10000);    // Intervalo de 10 segundos entre verificações
    }

    cyw43_arch_deinit();  // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
    return 0;
}
