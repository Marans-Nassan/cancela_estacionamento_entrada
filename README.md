# 🚗 Estacionamento Automatizado com Raspberry Pi Pico W

Este projeto implementa um **sistema de controle de estacionamento** utilizando a **Raspberry Pi Pico W**, integrando **LEDs, buzzer, servo motor e display OLED SSD1306 via I2C**.
O sistema permite controlar a entrada de veículos tanto de forma **automática** quanto **manual**, exibindo informações no display OLED e utilizando botões físicos para interação.

---

## 🔧 Funcionalidades

* **Controle de entrada de veículos**

  * Modo **Automático**: simula o pressionamento do botão de ticket e o sensor de passagem.
  * Modo **Manual**: permite abrir e fechar a cancela por botões físicos.

* **Indicação visual**

  * LED vermelho: entrada bloqueada.
  * LED verde: entrada liberada.

* **Alerta sonoro**

  * Buzzer emite sinal ao trocar entre os modos de operação.

* **Exibição em display OLED (128x64 - SSD1306)**

  * Nome do sistema ("Estacionamento").
  * Estado da entrada (Aberta ou Fechada).
  * Contagem de carros (até 400 veículos).
  * Modo atual (**Automático** ou **Manual**).

* **Servo motor**

  * Controla a abertura e fechamento da cancela.

* **Watchdog**

  * Garante que o sistema continue funcionando sem travamentos.

---

## 🗂 Estrutura do Projeto

```
📂 cancela_estacionamento_entrada
 ┣ 📜 main.c              # Lógica principal do sistema
 ┣ 📂 lib
 ┃ ┣ 📜 ssd1306.h         # Header do driver do display OLED
 ┃ ┣ 📜 ssd1306.c         # Implementação do driver do display OLED
 ┃ ┗ 📜 font.h            # Fonte para exibição de caracteres no OLED
 ┗ 📜 CMakeLists.txt      # Configuração para compilação com Pico SDK
```

---

## 🖥️ Hardware Utilizado

* **Raspberry Pi Pico W**
* **LEDs** (vermelho e verde)
* **Botões (A, B, C)**

  * Botão A → alterna entre **modo automático/manual**
  * Botão B → abre/fecha a cancela em **modo manual**
  * Botão C → simula o botão de ticket no **modo automático**
* **Buzzer piezoelétrico (PWM)**
* **Servo motor (PWM)**
* **Display OLED SSD1306 (I2C, 128x64)**

---

## 📌 Ligações dos Pinos

| Componente   | GPIO Pico |
| ------------ | --------- |
| LED Verde    | GP11      |
| LED Vermelho | GP13      |
| Botão A      | GP5       |
| Botão B      | GP6       |
| Botão C      | GP22      |
| Buzzer       | GP21      |
| Servo Motor  | GP16      |
| OLED SDA     | GP14      |
| OLED SCL     | GP15      |

---

## ⚙️ Como Compilar e Executar

1. **Configurar o ambiente**

   * Instale o [Pico SDK](https://github.com/raspberrypi/pico-sdk).
   * Configure o `CMake`.

2. **Compilar**

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

3. **Gravar na Pico W**

   * Conecte a Pico em modo BOOTSEL.
   * Copie o arquivo `.uf2` gerado para a unidade USB que aparecer.

---

## ▶️ Uso

* Ao iniciar, o sistema entra no **modo automático**.
* Pressione **Botão A** para alternar entre **Automático** e **Manual**.
* Em **modo automático**:

  * Pressione **Botão C** para simular retirada de ticket → cancela abre → carro entra.
* Em **modo manual**:

  * Use **Botão B** para abrir e fechar a cancela manualmente.



