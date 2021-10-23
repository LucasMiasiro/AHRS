#include "serial-logger.h"
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace serialLogger{

void log_example()
{
    std::cout << "Hey!" << std::endl;
    for (int i = 0; i < 100000; i++)
    {
        std::cout << i << ","<< i+2 << std::endl;
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void header()
{
    blank_lines(3);
    ruler();
    std::cout << "Starting..." << std::endl;
    ruler();
}

void blank_lines(int line_number = 1)
{
    for (int i = 0; i < line_number; i++)
    {
        std::cout << "\n";
    }
}

void ruler()
{
    std::cout << "--------------------" << std::endl;
}


};
