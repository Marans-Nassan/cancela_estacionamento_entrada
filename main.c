#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"
#include "hardware/watchdog.h"

//Leds básicos
#define green_led 11
#define red_led 13

//Botões
#define bot_a 5
#define bot_b 6

//PWM
#define buzzer_a 21
#define div 100.0f
#define wrap 24999 // div + wrap = 50HZ
uint32_t slice = 0;

//i2c
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

//Controle Servomotor
#define velocidade_servo 17
#define direcao_servo 16

//Oled
ssd1306_t ssd;

void core1(void);
void led_init(uint8_t led);
void bot_init(void);
void pwm_setup(void);
void i2c_setup(void);
void oled_init();


int main(){
    stdio_init_all();
    multicore_launch_core1(core1);
    led_init(green_led);
    led_init(red_led);
    bot_init();

    watchdog_enable(5000, 1);
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}

//Função do core1 do MCU
void core1(void){
    i2c_setup();
    oled_init();
    watchdog_enable(5000, 1);
    while(true){
        oled_run();
        watchdog_update();
    }
}

void led_init(uint8_t led){
    gpio_init(led);
    gpio_set_dir(led, GPIO_OUT);
    gpio_put(led, 0);
}

void bot_init(void){
    for(uint8_t i = 5; i < 7; i++){
        gpio_init(i);
        gpio_set_dir(i, GPIO_IN);
        gpio_pull_up(i);
    }
}

void pwm_setup(void){
    gpio_set_function(buzzer_a, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(buzzer_a);
    pwm_set_clkdiv(slice, div);
    pwm_set_wrap(slice, wrap);
    pwm_set_enabled(slice, true);
}

void i2c_setup(void){
    i2c_init(I2C_PORT, 400*1000);
    for(uint8_t init_i2c1 = 14 ; init_i2c1 < 16; init_i2c1 ++){
        gpio_set_function(init_i2c1, GPIO_FUNC_I2C);
        gpio_pull_up(init_i2c1);
    }
}

void oled_init(void){
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

void oled_run(void){
    ssd1306_draw_string(&ssd, "Estacionamento", 0, 0);
    /*ssd1306_draw_string(&ssd, , 1, 0);
    ssd1306_draw_String(&ssd, , 2, 0);
    ssd1306_draw_String(&ssd, , 3, 0);*/
    sleep_ms(50);
}