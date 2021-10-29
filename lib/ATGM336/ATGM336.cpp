#include "config.h"
#include "ATGM336.h"
#include "nmea_parser.h"
#include "serial-logger.h"
#include <iostream>

void gps_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                    int32_t event_id, void *event_data)
{
    navDataGNSS *navDataGNSS_ptr = (navDataGNSS *) event_handler_arg;

    switch (event_id) {
    case GPS_UPDATE:
        *(navDataGNSS_ptr->GNSS_ptr) = *((gps_t *) event_data);
        *(navDataGNSS_ptr->newData_ptr) = true;
#if LOG_ATGM336
        serialLogger::logLongFloat(&(navDataGNSS_ptr->GNSS_ptr->latitude), 1, "GLAT");
        serialLogger::logLongFloat(&(navDataGNSS_ptr->GNSS_ptr->longitude), 1, "GLON");
        serialLogger::logLongFloat(&(navDataGNSS_ptr->GNSS_ptr->altitude), 1, "GALT");
        serialLogger::logLongFloat(&(navDataGNSS_ptr->GNSS_ptr->cog), 1, "GCOG");
        serialLogger::logLongFloat(&(navDataGNSS_ptr->GNSS_ptr->speed), 1, "GVEL");
#endif
        break;
    case GPS_UNKNOWN:
        break;
    default:
        break;
    }
}

void ATGM336::initialize(){
    data = {.GNSS_ptr = &GNSS, .newData_ptr = &newData};
    nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
    nmea_parser_handle_t nmea_hdl = nmea_parser_init(&config);
    nmea_parser_add_handler(nmea_hdl, gps_event_handler, &data);
}

ATGM336::ATGM336(){
}
