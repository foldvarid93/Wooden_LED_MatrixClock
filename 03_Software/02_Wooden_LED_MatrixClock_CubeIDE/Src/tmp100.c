/**
* @file tmp100.c
* David
* 2020. okt. 23.
*/

/**** INCLUDES ***********************************************************************************/
#include "tmp100.h"
/**** END OF INCLUDES ****************************************************************************/

/**** MACROS *************************************************************************************/

/**** END OF MACROS ******************************************************************************/

/**** TYPE DEFINITIONS ***************************************************************************/

/**** END OF TYPE DEFINITIONS ********************************************************************/

/**** VARIABLES **********************************************************************************/

/**** END OF VARIABLES ***************************************************************************/

/**** LOCAL FUNCTION DECLARATIONS ****************************************************************/

/**** END OF LOCAL FUNCTION DECLARATIONS *********************************************************/

/**** LOCAL FUNCTION DEFINITIONS *****************************************************************/
HAL_StatusTypeDef TMP100_GetTemp(float* Temperature)
{
	/**/
    uint8_t buf[2];
    uint16_t result;

    if ( HAL_I2C_Master_Transmit(&hi2c3, (uint16_t)TMP100_ADDR, (uint8_t*)REG_TEMP, 1, 100) != HAL_OK )
    {
  	  return HAL_ERROR;
    }
    else
    {
      // Read 2 bytes from the temperature register
      if ( HAL_I2C_Master_Receive(&hi2c3, TMP100_ADDR, buf, 2, HAL_MAX_DELAY) != HAL_OK )
      {
    	  return HAL_ERROR;
      }
      else
      {
        //Combine the bytes
    	  result = ((int16_t)buf[0] << 4) | (buf[1] >> 4);

        // Convert to 2's complement, since temperature can be negative
        if ( result > 0x7FF )
        {
        	result |= 0xF000;
        }

        // Convert to float temperature value (Celsius)
        *Temperature = result * (float)0.0625;
        //temp_c *= 100;
        //sprintf((char*)buf, "%u.%u C\r\n", ((unsigned int)temp_c / 100),((unsigned int)temp_c % 100));
      }
    }
    return HAL_OK;
}
/**** END OF LOCAL FUNCTION DEFINITIONS **********************************************************/

/**** GLOBAL FUNCTION DEFINITIONS ****************************************************************/

/**** END OF GLOBAL FUNCTION DEFINITIONS *********************************************************/

/* End of file tmp100.c */
