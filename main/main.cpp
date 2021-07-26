// #include <iostream>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
#include "main.h"
#include "serial-logger.h"

extern "C" void app_main(void)
{
    log_example();
}
