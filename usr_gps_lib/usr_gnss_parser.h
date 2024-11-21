/*
 * usr_gnss_parser.h
 *
 *  Created on: Mar 16, 2024
 *      Author: noumanimpra
 */

// pmtk commands
// HAL_UART_Transmit(&huart2, "$PMTK251,57600*2C\r\n", 19, 100);				// 57600 bps
// HAL_UART_Transmit(&huart2, "$PMTK251,38400*27\r\n", 19, 100);				// 38400 bps
// HAL_UART_Transmit(&huart2, "$PMTK251,9600*17\r\n", 18, 100);				// 9600 bps

#ifndef USR_GNSS_PARSER_H_
#define USR_GNSS_PARSER_H_

#define VIEW_GPS()                                               \
    while (1)                                                    \
    {                                                            \
        if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) == SET) \
        {                                                        \
            uint8_t dat[1];                                      \
            HAL_UART_Receive(&huart2, (uint8_t *)dat, 1, 100);   \
            HAL_UART_Transmit(&huart1, (uint8_t *)dat, 1, 100);  \
        }                                                        \
    }

#include "main.h"
#include "usr_general.h"

extern UART_HandleTypeDef huart2;

typedef bool _f; // flag type

typedef struct S_GPS_L86_DATA_TAG
{
    float lat;
    float lon;
    float speedKN;
    float timeDateBuf;
    float fixedTime;
    float fixedLatBaseFormat;
    float fixedLonBaseFormat;
    int fixQualityID;
    int satInUse;
    float hdop;
    float altitudeInMeter;
    float WGS84;

} S_GPS_L86_DATA;

void UsrGpsL86Init(UART_HandleTypeDef *huart);
/*
 * L86 initial
 * Starts the dma
 *
 * */
void Usr_GpsL86GetValues(S_GPS_L86_DATA *gpsData_);
/*
 * data set for L86
 *
    gpsData.lat;
   gpsData.lon;
   gpsData.hdop;
   gpsData.speedKN;
   gpsData.satInUse;
   gpsData.timeDateBuf;
   gpsData.fixQualityID;
   gpsData.altitudeInMeter;

 * 2nd structure for the usr
 * */

#endif /* USR_GNSS_PARSER_H_ */
