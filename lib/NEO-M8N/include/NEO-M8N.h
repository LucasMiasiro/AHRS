#pragma once
#include "nmea_parser.h"

void gps_event_handler(void *, esp_event_base_t, int32_t, void*);

struct navDataGNSS {
    gps_t* GNSS_ptr;
    bool* newData_ptr;
};

class NEOM8N{
    private:

    public:
    float Home_LatLon[2] = {0, 0};
    gps_t GNSS;
    bool newData = false;
    navDataGNSS data;
    NEOM8N();
    void initialize();
    void waitFix();
};
