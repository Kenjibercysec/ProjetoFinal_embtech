# Monitoramento de Redes e Firewall com Raspberry Pi Pico W

Este projeto demonstra como configurar um servidor HTTP na Raspberry Pi Pico W para monitorar redes e firewall via Wi-Fi. A aplicação permite visualizar e gerenciar o status da rede através de um navegador web.

## Introdução

A Raspberry Pi Pico W oferece suporte à conectividade Wi-Fi, permitindo a implementação de funcionalidades avançadas, como servidores HTTP, utilizando a linguagem C e o SDK oficial da Raspberry Pi. Com essa capacidade, é possível criar aplicações que interajam diretamente com dispositivos, como smartphones, via redes Wi-Fi.

## Utilidade do Projeto

Este projeto pode ser utilizado em diversas situações, como:

- **Monitoramento de Redes Domésticas:** Permite que usuários monitorem o status da sua rede doméstica e firewall, garantindo que todos os dispositivos estejam conectados e protegidos.
- **Ambientes Corporativos:** Empresas podem usar este projeto para monitorar a rede interna e garantir que não haja acessos não autorizados.
- **Educação:** Ótimo para estudantes e entusiastas de eletrônica e programação que desejam aprender sobre redes, Wi-Fi e servidores HTTP utilizando a Raspberry Pi Pico W.

## Aplicações

- **Monitoramento de Dispositivos:** Verifique quais dispositivos estão conectados à rede e seu status.
- **Gerenciamento de Firewall:** Controle e visualize o status do firewall da rede.
- **Bloqueio de IPs:** Implemente uma lista de bloqueio de IPs para evitar acessos indesejados.

## Pré-requisitos

1. **Ambiente Configurado:**
    * Instale o Raspberry Pi Pico SDK.
    * Baixe e configure o ***lwIP*** (protocolo usado para suporte à rede no Pico W).
        1. Iniciar um projeto C/C++ na extensão da Raspberry Pi Pico.
        2. Mudar o tipo da placa para Pico W.
        3. Ir em “Pico wireless options” e clicar em background lwIP.
    * Certifique-se de que as bibliotecas ***pico_cyw43_arch_lwip_threadsafe_background*** estão configuradas corretamente.

2. **Credenciais de Wi-Fi:** Substitua o nome e a senha do Wi-Fi no código.

## Estrutura do Projeto

- **Servidor HTTP:** Configura um servidor HTTP na Raspberry Pi Pico W para responder a requisições HTTP.
- **Monitoramento de Rede:** Verifica o status da conexão Wi-Fi e exibe o endereço IP.
- **Bloqueio de IPs:** Implementa uma lista de IPs bloqueados para evitar acessos indesejados.

## Como Usar

1. **Configuração Inicial:**
    - Clone este repositório.
    - Configure o ambiente de desenvolvimento conforme os pré-requisitos.

2. **Compilação e Upload:**
    - Compile o código utilizando o CMake.
    - Faça o upload do binário para a Raspberry Pi Pico W.

3. **Execução:**
    - Conecte a Raspberry Pi Pico W à sua rede Wi-Fi.
    - Acesse o endereço IP da Pico W no navegador para visualizar o status da rede e firewall.

## Exemplo de Código

Aqui está um trecho do código que configura o servidor HTTP e processa requisições:

```c
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
```

## Vídeo do projeto

https://youtu.be/LSzBRtT2_fo

    // Libera o buffer recebido
    pbuf_free(p);

    return ERR_OK;
}
