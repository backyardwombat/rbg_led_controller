/*******************************************************************************
File Name: led.h

Description:
 This file contains the macros and declaration of LED control related functions.

********************************************************************************
Copyright 2014-15, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/
#if !defined (LED_H)
#define LED_H
#include <project.h>

/********************************** Macors ******************************************/
/* LED Color and status related Macros */
#define RGB_LED_MAX_VAL					0xFF
#define RGB_LED_OFF						0xFF
#define RGB_LED_ON						0x00
#define RGB_LED_MID                     0x80

/* Index values in array where respective color coordiantes
* are saved */
#define RED_INDEX						0x00
#define GREEN_INDEX						0x01
#define BLUE_INDEX						0x02
#define INTENSITY_INDEX					0x03
    
#define SELECT_RED                      0x01
#define SELECT_GREEN                    0x02
#define SELECT_BLUE                     0x03
    
#define CONTROL_ON                      0x01
#define CONTROL_OFF                     0x02
#define CONTROL_BRIGHT                  0x03
#define CONTROL_DIM                     0x04
#define CONTROL_STATUS_MSK              0x03
    
#define STATUS_ON                       0x01
#define STATUS_OFF                      0x00    
/***************************************************************************************/

/******************************* Function Declaration **********************************/
void UpdateRGBLED(uint8* , uint8);
void UpdateLEDColor(uint8 color, uint8 control, uint8 intensity);
/***************************************************************************************/

#endif
/* [] END OF FILE */
