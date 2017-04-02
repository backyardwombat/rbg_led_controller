/******************************************************************************
* Project Name		: animation_controller
* File Name			: BLEProcess.c
* Version 			: 1.0
* Device Used		: CY8C4247LQI-BL483
* Software Used		: PSoC Creator 4.0
* Compiler    		: ARM GCC 4.9.3, ARM MDK Generic
* Related Hardware	: CY8CKIT-042-BLE Bluetooth Low Energy Pioneer Kit 
* Owner             : SEY
*
*******************************************************************************/

/*******************************************************************************
* Contains function definitions for Event callback handler function for BLE 
* events as well as updating RGB characteristic GATT DB
*******************************************************************************/
#include <main.h>

/*******************************************************************************
* Function Name: GeneralEventHandler
********************************************************************************
* Summary:
*        General callback event handler function for BLE component through which
* application recieves various BLE events.
*
* Parameters:
*  event:		event type. See "CYBLE_EVENT_T" structure in BLE_Stack.h
*  eventParam: 	parameter associated with the event
*
* Return:
*  void
*
*******************************************************************************/
void GeneralEventHandler(uint32 event, void * eventParam)
{
	/* Structure to store data written by Client */	
	CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;

    uint8 animationType[1] = {SELECT_RED};
	uint8 animationControl[1] = {CONTROL_OFF};
    uint8 animationStatus[1] = {STATUS_OFF};
    
    static uint8 color = SELECT_RED;
    static uint8 intensity = RGB_LED_MID;
    static uint8 status = STATUS_OFF;
    
	switch(event)
    {
		case CYBLE_EVT_STACK_ON:
			/* BLE stack is on. Start BLE advertisement */
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
		break;
				
		case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
			/* This event is generated at GAP disconnection. */
			animationType[0] = SELECT_RED;
            animationControl[0] = CONTROL_OFF;
            animationStatus[0] = STATUS_OFF;
            
			/* Switch off LEDs */
            UpdateLEDColor(SELECT_RED, STATUS_OFF, RGB_LED_MID); 
			
			/* Register the new color in GATT DB*/			
			UpdateRGBcharacteristic(animationType, 
									sizeof(animationType),
									CYBLE_CUSTOM_SERVICE_ANIMATION_TYPE_CHAR_HANDLE);
            
            UpdateRGBcharacteristic(animationControl, 
									sizeof(animationControl),
									CYBLE_CUSTOM_SERVICE_ANIMATION_CONTROL_CHAR_HANDLE);
            
            UpdateRGBcharacteristic(animationStatus, 
									sizeof(animationStatus),
									CYBLE_CUSTOM_SERVICE_ANIMATION_STATUS_CHAR_HANDLE);
			
			/* Restart advertisement */
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
		break;

        case CYBLE_EVT_GATTS_WRITE_REQ: 
			/* This event is generated when the connected Central device sends a
			* Write request. The parameter contains the data written */
			
			/* Extract the Write data sent by Client */
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
			
			/* If the attribute handle of the characteristic written to 
			* is equal to that of RGB_LED characteristic, then extract 
			* the RGB LED data */
			if(CYBLE_CUSTOM_SERVICE_ANIMATION_TYPE_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
            {
                animationType[0] =  wrReqParam->handleValPair.value.val[0];
                color = animationType[0];
                UpdateLEDColor(color, status, intensity);
                
				/* Update the GATT DB for RGB LED read characteristics*/
				UpdateRGBcharacteristic(animationType, 
										sizeof(animationType), 
										CYBLE_CUSTOM_SERVICE_ANIMATION_TYPE_CHAR_HANDLE);
            }
            
            if(CYBLE_CUSTOM_SERVICE_ANIMATION_CONTROL_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
            {
                animationControl[0] = wrReqParam->handleValPair.value.val[0];
                
                switch(animationControl[0])
                {
                    case CONTROL_ON:
                    status = STATUS_ON;
                    break;
                    
                    case CONTROL_OFF:
                    status = STATUS_OFF;
                    break;
                    
                    case CONTROL_BRIGHT:
                    intensity -= 10;
                    break;
                    
                    case CONTROL_DIM:
                    intensity += 10;
                    break;
                    
                    default:
                    status = STATUS_OFF;
                }
                animationStatus[0] = status;
                
                UpdateLEDColor(color, status, intensity);
                
                /* Update the GATT DB for RGB LED read characteristics*/
				UpdateRGBcharacteristic(animationType, 
										sizeof(animationType), 
										CYBLE_CUSTOM_SERVICE_ANIMATION_CONTROL_CHAR_HANDLE);
                
                UpdateRGBcharacteristic(animationStatus,
                                        sizeof(animationStatus), 
										CYBLE_CUSTOM_SERVICE_ANIMATION_STATUS_CHAR_HANDLE);
                
                /* Send status notification */
                // TODO: Break out status into its own characteristic upate function
                CYBLE_GATTS_HANDLE_VALUE_NTF_T		statusHandle;
                statusHandle.attrHandle = CYBLE_CUSTOM_SERVICE_ANIMATION_STATUS_CHAR_HANDLE;
                statusHandle.value.len = sizeof(animationStatus);
                statusHandle.value.val = animationStatus;
                CyBle_GattsNotification(cyBle_connHandle, &statusHandle); 
            }
			
			/* Send the response to the write request. */
			CyBle_GattsWriteRsp(cyBle_connHandle);
            
			break;

        default:

       	break;
    }   	
}

/*******************************************************************************
* Function Name: UpdateRGBcharacteristic
********************************************************************************
* Summary:
*        Update the GATT DB with new RGB Color received to allow Client to read 
* back updated color values.
*
* Parameters:
*  ledData:		array pointer where color value is stored
*  len: 		length of the array
*  attrHandle:	attribute handle of the characteristic
*
* Return:
*  void
*
*******************************************************************************/
void UpdateRGBcharacteristic(uint8* ledData, uint8 len, uint16 attrHandle)
{
	/* 'rgbHandle' stores RGB control data parameters */
	CYBLE_GATT_HANDLE_VALUE_PAIR_T		rgbHandle;

	/* Update RGB control handle with new values */
	rgbHandle.attrHandle = attrHandle;
	rgbHandle.value.val = ledData;
	rgbHandle.value.len = len;
	
	/* Update the RGB LED attribute value. This will allow 
	* Client device to read the existing color values over 
	* RGB LED characteristic */
	CyBle_GattsWriteAttributeValue(&rgbHandle, 
									FALSE, 
									&cyBle_connHandle, 
									CYBLE_GATT_DB_PEER_INITIATED);
}

/* [] END OF FILE */
