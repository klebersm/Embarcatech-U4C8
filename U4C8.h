#ifndef __U4C8_H_

#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "display/ssd1306.h"

#define LED_R         13
#define LED_G         11
#define LED_B         12

#define JOYSTICK_X    26
#define JOYSTICK_Y    27
#define JOYSTICK_BTN  22

#define BUTTON_A      5
#define BUTTON_B      6

#define DIV           10
#define WRAP          2048

#define X_CENTER      WIDTH / 2 - 4
#define Y_CENTER      HEIGHT / 2 - 4 

#define DEBOUNCE_MS   100

void init_btns();
void init_leds();
void init_display();
void init_joystick_adc();

uint64_t handle_btn_a(alarm_id_t id, void *user_data);
uint64_t handle_btn_b(alarm_id_t id, void *user_data);
uint64_t handle_joy_btn(alarm_id_t id, void *user_data);

static void debounce(uint gpio, uint32_t events);

#define __U4C8_H_
#endif