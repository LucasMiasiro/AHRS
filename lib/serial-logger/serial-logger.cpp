#include "serial-logger.h"
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void log_example()
{
    std::cout << "Hey!" << std::endl;
    for (int i = 0; i < 100000; i++)
    {
        std::cout << i << ","<< i+2 << std::endl;
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}
