#pragma once
#include "nmea_parser.h"

void gps_event_handler(void *, esp_event_base_t, int32_t, void*);

struct navDataGNSS {
    gps_t* GNSS_ptr;
    bool* newData_ptr;
};

class ATGM336{
    private:

    public:
    gps_t GNSS;
    bool newData = false;
    navDataGNSS data;
    ATGM336();
    void initialize();
};
