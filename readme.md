# Sistema de Monitoramento e Segurança com Watchdog (RP2040)

Este projeto demonstra a implementação de um sistema de controle de estados utilizando o microcontrolador **RP2040** na placa de desenvolvimento **BitDogLab**. O foco principal é a utilização do periférico **Watchdog Timer (WDT)** para garantir a resiliência do sistema contra travamentos de software.

## 🛠️ Funcionalidades

* **Controle de Estados:** Sistema On/Off controlado via interrupção (Botão A).
* **Níveis de Operação:** Três níveis indicados visualmente por cores no LED RGB (Botão B).
* **Mecanismo de Segurança (Watchdog):** Reinicialização automática em caso de falha crítica com timeout de 4 segundos.
* **Diagnóstico Pós-Reset:** Uso de *Scratch Registers* para identificar se o reset foi causado por um erro específico (`0xDEAD`).
* **Feedback Visual:** Sistema de cores para indicar operação normal, travamento e recuperação.

## 📱 Mapeamento de Hardware (BitDogLab)

| Componente | Pino (GPIO) | Função |
| :--- | :--- | :--- |
| **Botão A** | 5 | Alterna entre Sistema Ligado/Desligado |
| **Botão B** | 6 | Troca de Nível (Clique) / Simular Falha (Segurar) |
| **LED RGB (Verde)** | 11 | Operação Nível 1 |
| **LED RGB (Azul)** | 12 | Operação Nível 2 |
| **LED RGB (Vermelho)** | 13 | Operação Nível 3 / Alerta de Erro |



## 🚀 Como Funciona

### 1. Operação Normal
Ao ligar, o sistema inicia em modo standby. Pressionando o **Botão A**, o sistema é ativado. O **Botão B** alterna as cores do LED. O loop principal "alimenta" o Watchdog constantemente através da função `watchdog_update()`.

### 2. Simulação de Falha
Para testar a segurança, se o **Botão B** for mantido pressionado por mais de 1 segundo:
1.  O sistema grava o código de erro `0xDEAD` no registrador `scratch[4]`.
2.  O LED fica **Branco** (indicando congelamento).
3.  O código entra em um `while(true)` infinito, parando de alimentar o Watchdog.

### 3. Recuperação Automática
Após 4 segundos de inatividade do software, o hardware força o reset. Ao reiniciar, o código executa a função `watchdog_caused_reboot()`. Se detectar o código `0xDEAD`, o sistema:
* Exibe uma mensagem de diagnóstico no terminal serial.
* Pisca o LED vermelho 3 vezes para alertar o usuário.
* Limpa o registro e volta ao estado de prontidão.




## 📋 Requisitos e Compilação

Para compilar este projeto, você precisará do **Pico SDK** configurado em seu ambiente.

```bash
# Clone o repositório
git clone [https://github.com/seu-usuario/nome-do-repositorio.git](https://github.com/seu-usuario/nome-do-repositorio.git)

# Entre na pasta e crie o diretório de build
cd nome-do-repositorio
mkdir build
cd build

# Compile o projeto
cmake ..
make




✒️ Autores
Este projeto foi desenvolvido como parte das atividades do Polo Ilhéus/Itabuna por:

Leonardo Bonifácio Vieira Santos

Matheus Santos Silva

Yuri Coutinho Costa

Juan Pablo Azevedo Souza

Arthur de Oliveira Moreira

Data: 13 de Janeiro de 2026