#include "config.h"
#include "NEO-M8N.h"
#include "nmea_parser.h"
#include "serial-logger.h"
#include <iostream>
#include <math.h> 
#include "gpio-handler.h"

void gps_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                    int32_t event_id, void *event_data)
{
    navDataGNSS *navDataGNSS_ptr = (navDataGNSS *) event_handler_arg;

    switch (event_id) {
    case GPS_UPDATE:
        if (((gps_t *) event_data)->sats_in_use > GNSS_MIN_SATS
            && ((gps_t *) event_data)->fix > 0) {

            *(navDataGNSS_ptr->GNSS_ptr) = *((gps_t *) event_data);
            *(navDataGNSS_ptr->newData_ptr) = true;
#if LOG_NEOM8N
            serialLogger::logLongFloat(&(navDataGNSS_ptr->GNSS_ptr->latitude), 1, "GLAT");
            serialLogger::logLongFloat(&(navDataGNSS_ptr->GNSS_ptr->longitude), 1, "GLON");
            serialLogger::logLongFloat(&(navDataGNSS_ptr->GNSS_ptr->altitude), 1, "GALT");
            serialLogger::logLongFloat(&(navDataGNSS_ptr->GNSS_ptr->cog), 1, "GCOG");
            serialLogger::logLongFloat(&(navDataGNSS_ptr->GNSS_ptr->speed), 1, "GVEL");
            serialLogger::logUInt8(&(navDataGNSS_ptr->GNSS_ptr->sats_in_use), "STUSE");
            serialLogger::logUInt8(&(navDataGNSS_ptr->GNSS_ptr->sats_in_view), "STVIEW");
            std::cout << "FIX " << navDataGNSS_ptr->GNSS_ptr->fix << std::endl;
            serialLogger::blank_lines(1);
#endif

            }
        break;
    case GPS_UNKNOWN:
        break;
    default:
        break;
    }
}

void NEOM8N::initialize(){
    data = {.GNSS_ptr = &GNSS, .newData_ptr = &newData};
    nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
    nmea_parser_handle_t nmea_hdl = nmea_parser_init(&config);
    nmea_parser_add_handler(nmea_hdl, gps_event_handler, &data);
}

void NEOM8N::setHome(){
    builtin_led led;
    uint8_t count = 0;
    bool is3DFixed = false, reachedConvergence = false;

    led.set_level(1);

    while(count < GNSS_CONV_TIME_S){

#if GNSS_HOME_REQ_3D==1
        is3DFixed = (GNSS.fix > GPS_FIX_INVALID) && 
                    (GNSS.fix_mode == GPS_MODE_3D) &&
                    (GNSS.sats_in_use > GNSS_MIN_SATS) &&
                    (GNSS.latitude != 0) &&
                    (GNSS.longitude != 0);
#elif GNSS_HOME_REQ_3D==2
        is3DFixed = (GNSS.fix > GPS_FIX_INVALID) && 
                    (GNSS.fix_mode >= GPS_MODE_2D) &&
                    (GNSS.sats_in_use > GNSS_MIN_SATS) &&
                    (GNSS.latitude != 0) &&
                    (GNSS.longitude != 0);
#else
        is3DFixed = true;
#endif

        reachedConvergence = (GNSS.speed < GNSS_CONV_VEL);

        if (is3DFixed && reachedConvergence){
            count++;
            led.blink(3, true);
#if LOG_NEOM8N
            serialLogger::logUInt8(&count, "GCONV");
#endif
        } else {
            count = 0;
        }

        vTaskDelay(1000.0f/SYSTEM_SAMPLE_PERIOD_MS);
    }

    calcFlatEarthParameters();
}

void NEOM8N::calcFlatEarthParameters(){
    Home_LatLon[0] = GNSS.latitude;
    Home_LatLon[1] = GNSS.longitude;
    float R_N, R_M;

    R_N = R / sqrt(1 - (2*f - f*f)*
    sin(Home_LatLon[0]*DEG2RAD)*sin(Home_LatLon[0]*DEG2RAD));

    R_M = R_N * (1 - (2*f - f*f))/(1 - (2*f - f*f)*
    sin(Home_LatLon[0]*DEG2RAD)*sin(Home_LatLon[0]*DEG2RAD));

    K_N = DEG2RAD/atan2f(1, R_M);
    K_E = DEG2RAD/atan2f(1, R_N*cos(Home_LatLon[0]*DEG2RAD));

#if LOG_NEOM8N
    serialLogger::logFloat(&K_N, 1, "K_N");
    serialLogger::logFloat(&K_E, 1, "K_E");
#endif
}

void NEOM8N::getPos(float *posX, float *posY){
    *posX = (GNSS.latitude - Home_LatLon[0])*K_N;
    *posY = (GNSS.longitude - Home_LatLon[1])*K_E;
}

NEOM8N::NEOM8N(){
}
