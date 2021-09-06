#include "gpio-handler.h"
#include "driver/gpio.h"

builtin_led::builtin_led(){
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << BUILTIN_LED);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
}

void builtin_led::set_level(bool state){
    gpio_set_level(BUILTIN_LED, state);
}