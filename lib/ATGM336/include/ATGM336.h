#pragma once
#include "nmea_parser.h"

void gps_event_handler(void *, esp_event_base_t, int32_t, void*);

class ATGM336{
    private:
    gps_t GNSS;

    public:
    ATGM336();
    void initialize();
};
