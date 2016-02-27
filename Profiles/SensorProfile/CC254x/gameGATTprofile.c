/**************************************************************************************************
  Filename:       gameGATTprofile.c
  Revised:        $Date: 2015-03-24 09:19:15 -0700 (Tue, 24 Mar 2015) $
  Revision:       $Revision: 43274 $

  Description:    This file contains the Game GATT profile sample GATT service
                  profile for use with the BLE sample application.

  Copyright 2010 - 2015 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include "bcomdef.h"
#include "linkdb.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"

#include "gameGATTprofile.h"
#include "st_util.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/* Service configuration values */
#define SENSOR_SERVICE_UUID     GAMEPROFILE_SERV_UUID
#define SENSOR_DATA_UUID        GAMEPROFILE_DATA_UUID
#define SENSOR_CONFIG_UUID      GAMEPROFILE_ACTION_UUID
#define SENSOR_PERIOD_UUID      GAMEPROFILE_PERIOD_UUID

#define SENSOR_SERVICE          GAMEPROFILE_SERVICE
#define SENSOR_DATA_LEN         GAMEPROFILE_DATA_LEN

#ifdef USER_DESCRIPTION
#define SENSOR_DATA_DESCR       "Meas. Data."
#define SENSOR_CONFIG_DESCR     "Action. Call."
#define SENSOR_PERIOD_DESCR     "Period. Data."
#endif

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Service UUID
static CONST uint8 sensorServiceUUID[TI_UUID_SIZE] =
{
  TI_UUID(SENSOR_SERVICE_UUID),
};

// Characteristic UUID: data
static CONST uint8 sensorDataUUID[TI_UUID_SIZE] =
{
  TI_UUID(SENSOR_DATA_UUID),
};

// Characteristic UUID: config
static CONST uint8 sensorCfgUUID[TI_UUID_SIZE] =
{
  TI_UUID(SENSOR_CONFIG_UUID),
};

// Characteristic UUID: period
static CONST uint8 sensorPeriodUUID[TI_UUID_SIZE] =
{
  TI_UUID(SENSOR_PERIOD_UUID),
};


/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static gameProfileCBs_t *gameProfile_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Profile Service attribute
static CONST gattAttrType_t gameProfileService = { TI_UUID_SIZE, sensorServiceUUID };

// Characteristic Value: data
static uint8 gameProfileData[SENSOR_DATA_LEN] = { 0, 0, 0, 0, 0};

// Characteristic Properties: data
static uint8 gameProfileDataProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Characteristic Configuration: data
static gattCharCfg_t *gameProfileDataConfig;

#ifdef USER_DESCRIPTION
// Characteristic User Description: data
static uint8 gameProfileDataUserDesp[] = SENSOR_DATA_DESCR;
#endif
// Characteristic Properties: configuration
static uint8 gameProfileActionProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic Value: configuration
static uint8 gameProfileAction = 0;

#ifdef USER_DESCRIPTION
// Characteristic User Description: configuration
static uint8 gameProfileActionUserDesp[] = SENSOR_CONFIG_DESCR;
#endif
// Characteristic Properties: period
static uint8 gameProfileNotifProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic Value: period
static uint8 sensorPeriod = 0;
#ifdef USER_DESCRIPTION
// Characteristic User Description: period
static uint8 gameProfileNotifUserDesp[] = SENSOR_PERIOD_DESCR;
#endif
/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t gameProfileAttrTbl[] =
{
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&gameProfileService                   /* pValue */
  },

    // Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &gameProfileDataProps
    },

      // Characteristic Value "Data"
      {
        { TI_UUID_SIZE, sensorDataUUID },
        GATT_PERMIT_READ,
        0,
        gameProfileData
      },

      // Characteristic configuration
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&gameProfileDataConfig
      },
#ifdef USER_DESCRIPTION
      // Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        gameProfileDataUserDesp
      },
#endif
    // Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &gameProfileActionProps
    },

      // Characteristic Value "Configuration"
      {
        { TI_UUID_SIZE, sensorCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &gameProfileAction
      },
#ifdef USER_DESCRIPTION
      // Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        gameProfileActionUserDesp
      },
#endif
     // Characteristic Declaration "Period"
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &gameProfileNotifProps
    },

      // Characteristic Value "Period"
      {
        { TI_UUID_SIZE, sensorPeriodUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &sensorPeriod
      },
#ifdef USER_DESCRIPTION
      // Characteristic User Description "Period"
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        gameProfileNotifUserDesp
      },
#endif
};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t gameProfile_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                           uint8 *pValue, uint8 *pLen, uint16 offset,
                                           uint8 maxLen, uint8 method );
static bStatus_t gameProfile_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                            uint8 *pValue, uint8 len, uint16 offset,
                                            uint8 method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Game Profile Service Callbacks
CONST gattServiceCBs_t gameProfileCBs =
{
  gameProfile_ReadAttrCB,  // Read callback function pointer
  gameProfile_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      GameProfile_AddService
 *
 * @brief   Initializes the Game Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t GameProfile_AddService( void )
{
  bStatus_t ret;


  ret = util_initCharacteristicConfig(&gameProfileDataConfig);
  if (ret != SUCCESS)
  {
    return ret;
  }

  return GATTServApp_RegisterService( gameProfileAttrTbl,
                                          GATT_NUM_ATTRS( gameProfileAttrTbl ),
                                          GATT_MAX_ENCRYPT_KEY_SIZE,
                                          &gameProfileCBs );


}

/*********************************************************************
 * @fn      GameProfile_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t GameProfile_RegisterAppCBs( gameProfileCBs_t *appCallbacks )
{
  if ( appCallbacks != NULL)
  {
    gameProfile_AppCBs = appCallbacks;
    return ( SUCCESS );
  }

    return ( bleAlreadyInRequestedMode );
}

/*********************************************************************
 * @fn      GameProfile_SetParameter
 *
 * @brief   Set a Game Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to write
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t GameProfile_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case GAMEPROFILE_DATA:
      if ( len == GAMEPROFILE_DATA_LEN )
      {
        VOID osal_memcpy( gameProfileData, value, GAMEPROFILE_DATA_LEN );
        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg( gameProfileDataConfig, gameProfileData, FALSE,
                                    gameProfileAttrTbl, GATT_NUM_ATTRS( gameProfileAttrTbl ),
                                    INVALID_TASK_ID, gameProfile_ReadAttrCB );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      case GAMEPROFILE_PERIOD:
        if ( len == sizeof ( uint8 ) )
        {
          sensorPeriod = *((uint8*)value);
        }
        else
        {
          ret = bleInvalidRange;
        }
        break;

      default:
        ret = INVALIDPARAMETER;
        break;
  }

  return ( ret );
}

/*********************************************************************
 * @fn      GameProfile_GetParameter
 *
 * @brief   Get a Game Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t GameProfile_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case GAMEPROFILE_ACTION:
      *((uint8*)value) = gameProfileAction;
      break;

      case GAMEPROFILE_DATA:
        VOID osal_memcpy( value, gameProfileData, GAMEPROFILE_DATA_LEN );
        break;

    case GAMEPROFILE_PERIOD:
    *((uint8*)value) = sensorPeriod;
          break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return ( ret );
}

/*********************************************************************
 * @fn          gameProfile_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t gameProfile_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                           uint8 *pValue, uint8 *pLen, uint16 offset,
                                           uint8 maxLen, uint8 method )
{
  uint16 uuid;
  bStatus_t status = SUCCESS;

  // If attribute permissions require authorization to read, return error
  if ( gattPermitAuthorRead( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }

  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE) {
    // Invalid handle
    *pLen = 0;
    return ATT_ERR_INVALID_HANDLE;
  }

    switch ( uuid )
    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those reads

      // characteristics 1 and 2 have read permissions
      // characteritisc 3 does not have read permissions; therefore it is not
      //   included here
      // characteristic 4 does not have read permissions, but because it
      //   can be sent as a notification, it is included here
      case SENSOR_DATA_UUID:
          *pLen = GAMEPROFILE_DATA_LEN;
          VOID osal_memcpy( pValue, pAttr->pValue, GAMEPROFILE_DATA_LEN );
      break;


          case SENSOR_CONFIG_UUID:
          case SENSOR_PERIOD_UUID:
            *pLen = 1;
            pValue[0] = *pAttr->pValue;
            break;

      default:
        // Should never get here! (characteristics 3 and 4 do not have read permissions)
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }


  return ( status );
}

/*********************************************************************
 * @fn      gameProfile_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t gameProfile_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                            uint8 *pValue, uint8 len, uint16 offset,
                                            uint8 method )
{
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;
  uint16 uuid;

  // If attribute permissions require authorization to write, return error
  if ( gattPermitAuthorWrite( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE) {
    // Invalid handle
    return ATT_ERR_INVALID_HANDLE;
  }

    switch ( uuid )
    {
      case SENSOR_CONFIG_UUID:

        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len != 1 )
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }

        //Write the value
        if ( status == SUCCESS )
        {
          uint8 *pCurValue = (uint8 *)pAttr->pValue;
          *pCurValue = pValue[0];

          if( pAttr->pValue == &gameProfileAction)
          {
            notifyApp = GAMEPROFILE_ACTION;
          }
        }
        case SENSOR_PERIOD_UUID:
          // Validate the value
          // Make sure it's not a blob oper
          if ( offset == 0 )
          {
            if ( len != 1 )
            {
              status = ATT_ERR_INVALID_VALUE_SIZE;
            }
          }
          else
          {
            status = ATT_ERR_ATTR_NOT_LONG;
          }
          // Write the value
          if ( status == SUCCESS )
          {

              uint8 *pCurValue = (uint8 *)pAttr->pValue;

              *pCurValue = pValue[0];

              if( pAttr->pValue == &sensorPeriod )
              {
                notifyApp = GAMEPROFILE_PERIOD;
              }
            else
            {
               status = ATT_ERR_INVALID_VALUE;
            }
          }
          break;
        break;

      case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                                 offset, GATT_CLIENT_CFG_NOTIFY );
        break;

      default:
        // Should never get here! (characteristics 2 and 4 do not have write permissions)
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }

  // If a charactersitic value changed then callback function to notify application of change
  if ( (notifyApp != 0xFF ) && gameProfile_AppCBs && gameProfile_AppCBs->pfnGameProfileChange )
  {
    gameProfile_AppCBs->pfnGameProfileChange( notifyApp );
  }

  return ( status );
}

/*********************************************************************
*********************************************************************/
