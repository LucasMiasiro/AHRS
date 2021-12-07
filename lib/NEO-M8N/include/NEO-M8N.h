#pragma once
#include "nmea_parser.h"

void gps_event_handler(void *, esp_event_base_t, int32_t, void*);

struct navDataGNSS {
    gps_t* GNSS_ptr;
    bool* newData_ptr;
};

class NEOM8N{
    private:
    const float f = EARTH_FLATTENING;
    const float R = EARTH_EQUATORIAL_RADIUS;
    void calcFlatEarthParameters();
    float K_N, K_E;

    public:
    double Home_LatLon[2] = {0, 0};
    gps_t GNSS;
    bool newData = false;
    navDataGNSS data;
    NEOM8N();
    void initialize();
    void setHome();
    void getPos(float*, float*);
};
