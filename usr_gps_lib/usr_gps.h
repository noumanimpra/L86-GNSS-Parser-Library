/*
 * usr_gps.h
 *
 *  Created on: Aug 2, 2023
 *      Author:  numan
 *      Caution:
 */

#ifndef USR_GPS_H_
#define USR_GPS_H_
#include "usr_general.h"
#define _io static

/****************USR functions******************/

/*
 * get formatted location value
 */
void UL_GpsL86GetLocation(void);
/**
 * *get utc time formatted to hh:mm:ss 
 */
void getUTC(void);
/**
 * *get local time formatted to hh:mm:ss 
 * ! defined as turkey
 */
void getLocalTr(void);

/****************USR functions******************/

#endif /* USR_GPS_H_ */
