/*
 * usr_gps.c
 *
 *  Created on: Aug 2, 2023
 *      Author: numan
 */
#include "usr_general.h"
#define _buffer_size 1024
#define _max_message_size 90

extern bool GnssRx_Flag;
extern char rxData[1024];
extern UART_HandleTypeDef huart2;

/********************************************/

_io void DataParseProc(void);
_io void GpsGetRmc(void);
_io void formatLocation(float Lat, float Long);
_io void formatDataUTC(float Time);

_io char m_receiveGpsBuf[_buffer_size];
_io char m_gpsTransmitBuf[_buffer_size];

typedef bool _f; // flag type
typedef char _mBuf[100];
typedef char _lgBuf[150];
typedef char _sBuf[6];
typedef char _20_sizedBud[20];
typedef uint8_t _U8;
typedef float _fl;

_io _f rmcDataValidFlag = false;
_io _f northFlag = false;
_io _f eastFlag = false;

_io char *ptr;
_io _mBuf timeUtc;
_io _mBuf gpsPayyload;
_io _mBuf timeData;
_io _mBuf data2;
_io _mBuf dataTime;
_io _lgBuf gpsGSVPayyload;
_io _lgBuf gpsVTGPayyload;
_io _sBuf satInWiev;
_io _sBuf satId;
_io _sBuf elevation;
_io _sBuf azimuth;
_io _sBuf signalNoice;
_io _sBuf courseOG;
_io _sBuf speedOGKnot;
_io _sBuf speedOGKm;
_io _20_sizedBud longToCharLong;
_io _20_sizedBud latToCharLat;

int c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0, c7 = 0, c8 = 0; // temproary counters(needed :p )
_io int Msgindex;

_io _U8 dotIndexLong;
_io _U8 dotIndexLat;
_io _U8 degreeIndexLat;
_io _U8 degreeIndexLong;
_io _U8 comaCounter;

_io _fl time, latitude, longitude;
_io _fl arrLat, arrLong;

/******************************************/
/*******************user*******************/
void UL_GpsL86GetRMCPayyload(void) // *! to be tested
{
    GpsGetRmc();
    if (rmcDataValidFlag)
    {
        HAL_UART_Transmit(&huart2,(uint8_t*)gpsPayyload,strlen(gpsPayyload),HAL_MAX_DELAY);
    }
}
void UL_GpsL86GetLocation(void) //? testi geçti
{
    GpsGetRmc();
    if (rmcDataValidFlag)
    {
        // formatData();
        formatLocation(latitude, longitude);
    }
}
void getUTC(void) // *! to be tested
{
    GpsGetRmc();
    if (rmcDataValidFlag)
    {
        formatDataUTC(time);
    }
}
void getLocalTr(void) // *! to be tested
{
    GpsGetRmc();
    if (rmcDataValidFlag)
    {
        formatDataLocalTr(time);
    }
}
void UL_GpsL86GetGSVData(void)
{
    if(GnssRx_Flag)
    {
        Msgindex = 0;
        strcpy(m_gpsTransmitBuf,(char*)(rxData));
        ptr = strstr(m_gpsTransmitBuf,"GLGSV");
        if(*ptr == 'G')
        {
            while(1)
            {
                gpsGSVPayyload[Msgindex]=*ptr;
                Msgindex++;
                *ptr=*(ptr+Msgindex);
                if(*ptr=='\n')
                {
                    Msgindex=0;
                    for(int i=0;i<1024;i++)
                    {
                        rxData[i]=0;
                        m_gpsTransmitBuf[i]=0;
                    }
                    if(strlen(gpsGSVPayyload)>43)
                    {
                        parseGSV(); // // todo: break

                        for(int i=0;i<5;i++)
                        {
                            satInWiev[i]=0;
                            satId[i]=0;
                            elevation[i]=0;
                            azimuth[i]=0;
                        }
                    }
                    break;
                }
            }
        }
        GnssRx_Flag =false;
    }
}
void UL_GpsL86GetVTGData(void)
{
    Msgindex =0; // TODO: DEVAM ET BREAK
}

/**********************user********************/
/********************STATICS*******************/
_io void GpsGetRmc(void) //? testi geçti
{
    if (GnssRx_Flag)
    {
        Msgindex = 0;
        strcpy(m_gpsTransmitBuf, (char *)(rxData));
        ptr = strstr(m_gpsTransmitBuf, "GNRMC");

        if (*ptr == 'G')
        {
            while (1)
            {
                gpsPayyload[Msgindex] = *ptr;
                Msgindex++;
                *ptr = *(ptr + Msgindex);

                if (*ptr == '\n' || Msgindex > _max_message_size)
                {
                    Msgindex = 0;
                    for (int i = 0; i < _buffer_size; i++)
                    {
                        rxData[i] = 0;
                        m_gpsTransmitBuf[i] = 0;
                    }
                    for (int i = 0; i < 100; i++)
                    {
                        if (gpsPayyload[i] == 'N')
                            northFlag = true;
                        if (gpsPayyload[i] == 'E')
                            eastFlag = true;
                    }
                    if (strlen(gpsPayyload) > 10) // min rmc data
                    {
                        if (northFlag && eastFlag) // is data vaid
                        {
                            northFlag = false;
                            eastFlag = false;

                            sscanf(gpsPayyload, "GNRMC,%f,A,%f,N,%f,", &time, &latitude, &longitude);
                            rmcDataValidFlag = true;
                        }
                        else
                        {
                            HAL_UART_Transmit(&huart2, (uint8_t *)"Con err", 7, HAL_MAX_DELAY);
                        }
                    }
                    else
                    {
                        for (int i = 0; i < 100; i++)
                            gpsPayyload[i] = 0;
                    }
                    break;
                }
            }
        }
        GnssRx_Flag = false;
    }
}

_io void formatDataUTC(float Time) // *! to be tested
{
    Hours = (int)Time / 10000;
    Min = (int)(Time - (Hours * 10000)) / 100;
    Sec = (int)(Time - ((Hours * 10000) + (Min * 100)));
    sprintf(dataTime, "\r\nTime UTC: %d:%d:%d", Hours, Min, Sec);
    HAL_UART_Transmit(&huart2, (uint8_t *)dataTime, strlen(dataTime), HAL_MAX_DELAY);
}

_io void formatDataLocalTr(float Time) // *! to be tested
{
    Hours = (int)Time / 10000;
    Min = (int)(Time - (Hours * 10000)) / 100;
    Sec = (int)(Time - ((Hours * 10000) + (Min * 100)));
    sprintf(dataTime, "\r\nTime UTC: %d:%d:%d", Hours + 3, Min, Sec);
    HAL_UART_Transmit(&huart2, (uint8_t *)dataTime, strlen(dataTime), HAL_MAX_DELAY);
}

_io void formatLocation(float Lat, float Long) //? tested
{
    arrLat = latitude;
    arrLong = longitude;
    sprintf(longToCharLong, "%f", arrLong);
    for (int i = 0; i < 20; i++)
        if (longToCharLong[i] == '.')
            dotIndexLong = i;

    degreeIndexLong = dotIndexLong - 2;

    for (int i = 19; i >= degreeIndexLong; i--)
        longToCharLong[i + 1] = longToCharLong[i];

    longToCharLong[degreeIndexLong] = ' ';
    // EO long

    sprintf(latToCharLat, "%f", arrLat);
    for (int i = 0; i < 20; i++)
        if (latToCharLat[i] == '.')
            dotIndexLat = i;

    degreeIndexLat = dotIndexLat - 2;
    for (int i = 19; i >= degreeIndexLat; i--)
        latToCharLat[i + 1] = latToCharLat[i];

    latToCharLat[degreeIndexLat] = ' ';
    // strcpy(testArr, (char *)(latToCharLat));
    sprintf(data2, "\r\n Lat: %s N, Long: %s E", latToCharLat, longToCharLong);

    /*
     * print location
     */
    HAL_UART_Transmit(&huart2, (uint8_t *)data2, strlen(data2), HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);
    for (int i = 0; i < 100; i++)
    {
        data2[i] = 0;
    }
}
_io void parseGSV(void)             // *! to be tested
{
    comaCounter =0;
    for(int i=0;i<150;i++)
    {
        if(gpsGSVPayyload[i]==',')
            comaCounter ++;
        
        switch(comaCounter)
        {
            case 3:
            {
                if(gpsGSVPayyload[i]==',')
                    break;
                satInWiev[c1]=gpsGSVPayyload[i];
                c1++;
            break;
            }
            case 4:
            {
                if(gpsGSVPayyload[i]==',')
					break;
				satId[c2] =  gpsGSVPayyload[i];
				c2++;
            break;
            }
            case 5:
            {
                if(gpsGSVPayyload[i]==',')
					break;
                elevation[c3]=gpsGSVPayyload[i];
                c3++;
            break;
            }
            case 6:
            {
                if(gpsGSVPayyload[i]==',')
					break;
                azimuth[c4]=gpsGSVPayyload[i];
                c4++;
            break;
            }
            default:
            {
            break;
            }
        }
    }
    c1=0;
	c2=0;
	c3=0;
	c4=0;
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n sat in wiew: ", 16, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)satInWiev, sizeof(satInWiev), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n sat id: ", 11, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)satId, sizeof(satId), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n elevation: ", 13, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)elevation, sizeof(elevation), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n azimuth: ", 11, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)azimuth, sizeof(azimuth), HAL_MAX_DELAY);
	comaCounter=0;

}
void parseVTG(void)
{
    comaCounter  = 0;
    for(int i=0; i<150;i++)
    {
        if(gpsVTGPayyload[i] == ',')
            comaCounter ++;
        switch(comaCounter)
        {
            case 1:
            {
                courseOG[c1]=gpsVTGPayyload[i];
                c1++;
            break;
            }
            case 5:
            {
                speedOGKnot[c2]=gpsVTGPayyload[i];
                c2++;
            break;
            }
            case 7:
            {
                speedOGKm[c3]=gpsVTGPayyload[i];
                c3++;
            break;
            }
        }
    }
    c1=0;c2=0;c3=0;
	HAL_UART_Transmit(&huart2, (uint8_t *)"\n course Over Ground: ", 22, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)courseOG, sizeof(courseOG), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t *)"\n Speed Over Ground(knot): ", 26, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)speedOGKnot, sizeof(speedOGKnot), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t *)"\n Speed Over Ground(KM/H): ", 27, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)speedOGKm, sizeof(speedOGKm), HAL_MAX_DELAY);
	comaCounter = 0;
}
/********************STATICS*******************/