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
#define bot_c 22 // Esse botão será utilizado para simular o pressionar de botão do ticket do estacionamento.
bool manual = 0;
bool abrir = 0;

/*PWM
Som*/
#define buzzer_a 21
#define div_buzz 100.0f
#define wrap_buzz 2840
uint32_t slice_buzz = 0;
//Servo
#define servo 16
#define div_servo 100.0f
#define wrap_servo 24999 // div + wrap = 50HZ
uint32_t slice_servo = 0;

//i2c
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

//Oled
ssd1306_t ssd;
char str_permissao[10];
uint16_t carros = 0;
char str_estado[10];
char str_limite[10] = {"400 = MAX"};


//Interrupção
#define interrupcoes(bots) gpio_irq_set_enabled_with_callback(bots, GPIO_IRQ_EDGE_FALL, true, &irq_gpio_handler);

void core1(void);
void led_init(uint8_t led);
void bot_init(void);
void irq_gpio_handler(uint32_t evets, uint gpio);
void pwm_setup(uint32_t slice, float div, uint32_t wrap, uint8_t pin);
void sirene(uint8_t pin, uint32_t wrap, uint8_t i);
void i2c_setup(void);
void oled_init(void);
void oled_run(void); //display oled apresentando os valores
void passagem(void); //lógica do loop principal


int main(){
    multicore_launch_core1(core1);
    led_init(green_led);
    led_init(red_led);
    bot_init();
    pwm_setup(slice_buzz, div_buzz, wrap_buzz, buzzer_a);
    pwm_setup(slice_servo, div_servo, wrap_servo, servo);

    watchdog_enable(12000, 1);
    while (true) {
        if(!manual){
            passagem();
        }
    }        
        sleep_ms(500);
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
    uint8_t botoes[2] = {bot_a, bot_b, bot_c};
    for(uint8_t i = 0; i < 3; i++){
        gpio_init(botoes[i]);
        gpio_set_dir(botoes[i], GPIO_IN);
        gpio_pull_up(botoes[i]);
    }
}

void irq_gpio_handler(uint32_t evets, uint gpio){
    uint64_t current_time = to_ms_since_boot(get_absolute_time());
    static uint64_t last_time_a = 0, last_time_b = 0;
    if(gpio == bot_a && (current_time - last_time_a > 300)){
        if(!manual){
            sirene(buzzer_a, wrap_buzz, 1);
            manual = 1;
            manual ? sprintf(str_estado, "Manual") : sprintf(str_estado, "Automatico");
        } else {
            sirene(buzzer_a, wrap_buzz, 2);
            manual = 0;
            manual ? sprintf(str_estado, "Manual") : sprintf(str_estado, "Automatico");
        }

    } else if ((gpio == bot_b && manual) && (current_time - last_time_b > 300)){
        if(abrir){
            //função pra fechar
            abrir = 0;
            abrir ? sprintf(str_permissao, "PERMITIDO") : sprintf(str_permissao, "NEGADO");

        } else {
            //função pra abrir
            abrir = 1;
            abrir ? sprintf(str_permissao, "PERMITIDO") : sprintf(str_permissao, "NEGADO");
        }
    } 
}

void pwm_setup(uint32_t slice, float div, uint32_t wrap, uint8_t pin){
    gpio_set_function(pin, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(pin);
    pwm_set_clkdiv(slice, div);
    pwm_set_wrap(slice, wrap);
    pwm_set_enabled(slice, true);
}

void sirene(uint8_t pin, uint32_t wrap, uint8_t quantidade){
    for(uint8_t i; i < quantidade; i++){
        pwm_set_gpio_level(pin, wrap);
        sleep_ms(100);
        pwm_set_gpio_level(pin, 0);
        sleep_ms(100);
    }
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
    ssd1306_draw_string(&ssd, str_permissao, 1, 0);
    if(carros < 400) {
        ssd1306_draw_String(&ssd, carros, 2, 0);
    } else{
        ssd1306_draw_String(&ssd, "400 = MAX", 2, 0);    
    }
    ssd1306_draw_String(&ssd, str_estado, 3, 0);
    sleep_ms(50);
}

void passagem(void){
    uint64_t current_time = to_ms_since_boot(get_absolute_time());
    static uint64_t last_time = 0;
    if(bot_c && (current_time - last_time >300)){
        if(carros < 400){
            sleep_ms(2000);
            carros++;
            abrir = 1;
            sprintf(str_permissao, "PERMITIDO");
            gpio_put(red_led, 0);
            gpio_put(green_led, 1);
            sleep_ms(5000); // Este sleep é pra simular o sensor que detecta a passagem do carro.
            abrir = 0;
            gpio_put(green_led, 0);
            gpio_put(red_led, 1);
            sprintf(str_permissao, "NEGADO");
        } 
    } 
}