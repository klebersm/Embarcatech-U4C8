#include <stdio.h>
#include "pico/stdlib.h"
#include "U4C8.h"

// define os slices que serão utilizados
static uint sliceR;
static uint sliceB;
// Variável auxiliar, para habilitar ou desabilitar os slices
bool enableSlices;

// Alarmes para controle de debounce dos botões
static alarm_id_t btn_a_alm;
static alarm_id_t btn_b_alm;
static alarm_id_t joy_btn_alm;

// Variável para controle do display
ssd1306_t ssd;

void init_btns() {
    // Cria máscara com os valores dos 3 botões para inicialização
    uint btnMask = (0x01 << BUTTON_A) | (0x01 << BUTTON_B) | (0x01 << JOYSTICK_BTN);
    gpio_init_mask(btnMask);
    gpio_set_dir_in_masked(btnMask);
    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_B);
    gpio_pull_up(JOYSTICK_BTN);

    // Inicializa as interrupções para os botão A e botão do Joystick
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &debounce);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BTN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &debounce);
}

void init_leds() {
    // Configura o led verde
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);

    // Configura os leds para PWM
    gpio_set_function(LED_R, GPIO_FUNC_PWM);
    gpio_set_function(LED_B, GPIO_FUNC_PWM);

    // Seta o valor dos slices para os dois leds PWM
    sliceR = pwm_gpio_to_slice_num(LED_R);
    sliceB = pwm_gpio_to_slice_num(LED_B);

    // Configura os slices com os valores calculados
    // WRAP foi setado para 2048, que é a posição central do joystick
    // para facilitar o cálculo dos levels
    pwm_set_clkdiv(sliceR, DIV);
    pwm_set_wrap(sliceR, WRAP);
    pwm_set_clkdiv(sliceB, DIV);
    pwm_set_wrap(sliceB, WRAP);

    // Inicia o programa com slices habilitados
    pwm_set_enabled(sliceR, true);
    pwm_set_enabled(sliceB, true);
    enableSlices = true;
}

void init_display() {
    i2c_init(I2C_PORT, SSD1306_FREQ);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
    
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, SSD1306_ADDR, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

void init_joystick_adc() {
    // Inicia o ADC
    adc_init();

    // Configura os pinos do Joystick como ADC
    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y);
}

uint64_t handle_btn_a(alarm_id_t id, void *user_data) {
    // Se os slices estão habilitados, zera os levels antes de desabilitar
    // garantindo que o led sempre fique desligado
    if(enableSlices) {
        pwm_set_gpio_level(LED_R, 0);
        pwm_set_gpio_level(LED_B, 0);
    }
    // Inverte o valor da variável para habilitar/desabilitar os slices
    enableSlices = !enableSlices;
}

uint64_t handle_joy_btn(alarm_id_t id, void *user_data) {
    gpio_put(LED_G, !gpio_get(LED_G));
}

static void debounce(uint gpio, uint32_t events) {
    if(gpio == BUTTON_A) {
        cancel_alarm(btn_a_alm);
        if(events == 0x04) btn_a_alm = add_alarm_in_ms(DEBOUNCE_MS, handle_btn_a, NULL, false);
    }
    else if(gpio == JOYSTICK_BTN){
        cancel_alarm(joy_btn_alm);
        if(events == 0x04) joy_btn_alm = add_alarm_in_ms(DEBOUNCE_MS, handle_joy_btn, NULL, false);
    }
}

void update_display(uint16_t joy_x, uint16_t joy_y) {
    // Limpa o display e renderiza o retangulo mais externo (borda fina)
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 3, 3, 122, 58, true, false);

    // Se o led verde está acionado, renderiza outro retangulo por dentro, duplicando a borda
    if(gpio_get(LED_G))
        ssd1306_rect(&ssd, 4, 4, 120, 56, true, false);

    // Calcula a posição do quadrado relativa à posição do joystick
    // Sempre subtrai 4 para centralizar o qudrado
    int left = (joy_y * WIDTH / 4096) - 4;
    int top = ((4096 - joy_x) * HEIGHT / 4096) - 4;

    // Evita travamento da placa por coordenada menor que zero
    if(left < 0) left = 0;
    if (top < 0) top = 0;

    // Evita que o quadrado passe dos limites do display
    if(left > WIDTH - 8) left = WIDTH - 8;
    if(top > HEIGHT - 8) top = HEIGHT - 8; 

    ssd1306_rect(&ssd, top, left, 8, 8, true, true);
    ssd1306_send_data(&ssd); // Atualiza o display
}

void update_pwm(uint16_t joy_x, uint16_t joy_y) {
    // Habilita ou desabilita os slices, de acordo com a variável auxiliar de controle
    pwm_set_enabled(sliceR, enableSlices);
    pwm_set_enabled(sliceB, enableSlices);
    
    // Como o PWM foi inicilizado com WRAP de 2048,
    // basta calcular a "distância" da posição do joystick até 2048
    // e, usando a função abs para obter o módulo, carregar o valor direto no level
    pwm_set_gpio_level(LED_R, abs(2048 - joy_x));
    pwm_set_gpio_level(LED_B, abs(2048 - joy_y));
}

int main()
{
    stdio_init_all();

    init_btns();            // Inicializa as configurações dos botóes
    init_leds();            // Inicializa as configurações dos leds
    init_display();         // Inicializa o display
    init_joystick_adc();    // Inicializa o joystick
    
    while (true) {
        // Lê os valores do joystick
        adc_select_input(0);
        uint16_t joy_x = adc_read();
        adc_select_input(1);
        uint16_t joy_y = adc_read();
        
        // Atualiza o pwm de acordo com a posição do joystick
        update_pwm(joy_x, joy_y);              
        // Atualiza o display de acordo com a posição do joystick
        update_display(joy_x, joy_y);

        sleep_ms(100);
    }
}
