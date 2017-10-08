/* =============================================================================
 *
 * Copyright Backyard Wombat Technoligies, 2017 All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION WHICH IS THE PROPERTY OF 
 * BACKYARD WOMBAT TECHNOLOGIES.
 *
 * SUMMARY: This project controls the RGB LED on the PSoC CY8KIT-042-BLE
 * Pioneer baseboard through BLE with a custom profile.
 * =============================================================================
*/
#include <main.h>

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Start the components */
	InitializeSystem();
    
    for(;;)
    {
        /* Process BLE Events. This API pushes the BLE events from the stack
		* to the generic event callback function in the application. This  function
		* also pushes data from application to BLE stack for transmission */
		CyBle_ProcessEvents();
    }
}

/*******************************************************************************
* Function Name: InitializeSystem
********************************************************************************
* Summary:
*        Start the components in the projects.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void InitializeSystem(void)
{
	/* Enable Global Interrupt Mask */
	CyGlobalIntEnable;		
	
	/* Start BLE stack and register the event callback function. This function
	* will be called by BLE stack whenever an activity for application happens.
	* This function recieves various events from BLE stack and its corresponsing 
	* data */
	CyBle_Start(GeneralEventHandler);
	
	/* Start PrISM modules for LED control */
	PrISM_1_Start();
	PrISM_2_Start();
	
	/* Switch off the RGB LEDs through PrISM modules */
	PrISM_1_WritePulse0(RGB_LED_OFF);
	PrISM_1_WritePulse1(RGB_LED_OFF);
	PrISM_2_WritePulse0(RGB_LED_OFF);
	
	/* Set Drive modes of the output pins to Strong */
	RED_SetDriveMode(RED_DM_STRONG);
	GREEN_SetDriveMode(GREEN_DM_STRONG);
	BLUE_SetDriveMode(BLUE_DM_STRONG);
}

/* [] END OF FILE */
