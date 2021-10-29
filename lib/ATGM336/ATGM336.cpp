#include "config.h"
#include "ATGM336.h"
#include "nmea_parser.h"
#include "serial-logger.h"
#include <iostream>

void gps_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                    int32_t event_id, void *event_data)
{
    gps_t *GNSS = (gps_t *) event_handler_arg;

    switch (event_id) {
    case GPS_UPDATE:
        *GNSS = *((gps_t *) event_data);
#if LOG_ATGM336
        serialLogger::logLongFloat(&(GNSS->latitude), 1, "GLAT");
        serialLogger::logLongFloat(&(GNSS->longitude), 1, "GLON");
        serialLogger::logLongFloat(&(GNSS->altitude), 1, "GALT");
        serialLogger::logLongFloat(&(GNSS->cog), 1, "GCOG");
        serialLogger::logLongFloat(&(GNSS->speed), 1, "GVEL");
#endif
        break;
    case GPS_UNKNOWN:
        break;
    default:
        break;
    }
}

void ATGM336::initialize(){
    nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
    nmea_parser_handle_t nmea_hdl = nmea_parser_init(&config);
    nmea_parser_add_handler(nmea_hdl, gps_event_handler, &GNSS);
}

ATGM336::ATGM336(){
    gps_t GNSS;
}
