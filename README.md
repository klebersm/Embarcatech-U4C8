# Embarcatech-U4C8

Este projeto utiliza o Joystick, o display, o led RGB e os botões da BitDogLab com o intuito de fixar conceitos acerca de conversão analógico-digital, PWM, interrupções, entre outros.

O Joystick controla, através dos eixos X e Y, a intensidade dos leds azul e vermelho, além de um quadro 8x8 no display, que seque a posição do joystick.

Ao pressionar o botão A da placa, os slices PWM são desabilitados, fazendo com que os leds vermelho e azul apaguem, independende da posição do Joystick. Neste caso, o quadro 8x8 no display continua se movimentando normalmente. Ao pressionar novamente o botão A, os slices são reativados, fazendo com que os leds vermelho e azul voltem a brilhar de seguindo o joystick.

Já o push button do Joystick alterna o led verde, do led RGB, para 100% ou desligado. Além disso, quando o led verde está ligado, a linha de contorno do display fica mais espessa.

## Funcionalidades das Teclas 🕹️

| Tecla   | Funcionalidade                                                                 |
| ------- | ------------------------------------------------------------------------------ |
| A       | Liga/Desliga os slices PWM                                                     |
| JOY BTN | Liga/Desliga led verde e linha do display                                      |
| JOY X   | Controla a posição eixo x do quadro no display e a intensidade do lev vermelho |
| JOY Y   | Controla a posição eixo y do quadro no display e a intensidade do lev azul     |

## Hardware 🛠️

- Microcontrolador RP2040 (Raspberry Pi Pico).
- Botões tipo Switch.
- Display i2c 128 x 64 pixels
- Joystick analógico 2 eixos

## Software 💻

- **SDK do Raspberry Pi Pico:** O SDK (Software Development Kit) do Pico, que inclui as bibliotecas e ferramentas necessárias para desenvolver e compilar o código. [Instruções de instalação](https://www.raspberrypi.com/documentation/pico/getting-started/)
- **CMake:** Um sistema de construção multiplataforma usado para gerar os arquivos de construção do projeto.
- **Compilador C/C++:** Um compilador C/C++ como o GCC (GNU Compiler Collection).
- **Git:** (Opcional) Para clonar o repositório do projeto.

### O código está dividido em vários arquivos para melhor organização:

- **`U4T8.C`**: Código com a função de loop principal: lê a porta serial e os botões e aciona os leds e o display.
- **`display/ssd1306.c/.h`**: Comunica com o display via i2c.
- **`CMakeLists.txt`:** Define a estrutura do projeto para o CMake.
  |

## Como Compilar e Executar ⚙️

1. **Instale o SDK do Raspberry Pi Pico:** Siga as instruções no site oficial do Raspberry Pi.
2. **Clone este repositório:** `git clone git@github.com:klebersm/embarcatech-U4C8.git`
3. **Navegue até o diretório do projeto:** `cd Embarcatech-U4C8`
4. **Compile o projeto:** `cmake -B build && cmake --build build`
5. **Copie para o Pico:** Copie o arquivo `U4C8.uf2` da pasta `build` (gerada após a compilação) para o Raspberry Pi Pico. O programa iniciará automaticamente.

## Tratamento dos botões - Interrupções e Alarmes ⏱️

O botão A e o Push Button do Joystick foram inicializados como entradas em PULL UP e são monitorados por interrupções com uma função para o tratamento dessas interrupções. À medida que ocorre uma interrupção, a função de tratamento verifica qual o botão gerou o evento e qual o tipo de evento. Se o evento foi de FALL EDGE (borda de descida), significa que o botão foi pressionado e um alarme é gerado para ser disparado em 100ms (Tempo configurado para debounce). Se qualquer outro evento acontecer no mesmo botão, esse alarme é cancelado. Sendo assim, o alarme só é realmente disparado se o botão for pressionado e ficar 100ms sem sofrer qualquer alteração.

## 🔗 Link do Vídeo de Funcionamento:

https://drive.google.com/file/d/1MjeI71_rBEzL1JrJcC_6zLeaOcR3C827/view?usp=sharing

## 📞 Contato

- 👤 **Autor**: Kleber Marçal

- 📧 **E-mail**:kleber.sm@gmail.com

---
