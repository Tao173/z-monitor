#include "AF.h"
#include "OSAL.h"
#include "ZComDef.h"
#include "ZDConfig.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ms.h"
#include "zcl_ha.h"

#include "zcl_app.h"

#include "battery.h"
#include "version.h"
/*********************************************************************
 * CONSTANTS
 */

#define APP_DEVICE_VERSION 2
#define APP_FLAGS 0

#define APP_HWVERSION 1
#define APP_ZCLVERSION 1

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Global attributes
const uint16 zclApp_clusterRevision_all = 0x0001;

int16 zclApp_Temperature_Sensor_MeasuredValue = 0;
int16 zclApp_PressureSensor_MeasuredValue = 0;
int16 zclApp_PressureSensor_ScaledValue = 0;
int8 zclApp_PressureSensor_Scale = -1;

uint16 zclApp_HumiditySensor_MeasuredValue = 0;

uint32 zclApp_GenTime_TimeUTC = 0;

uint16 zclApp_bh1750IlluminanceSensor_MeasuredValue = 0;

//uint8 zclApp_Magnet_OnOff = 0;
//uint8 zclApp_Magnet = 0;
uint8 zclApp_Occupied_OnOff = 0;
uint8 zclApp_Bind_OnOff = 0;

// Occupancy Cluster 
uint8 zclApp_Occupied = 0; 
/* Set default to Not be occupied */ 
//uint8 zclApp_OccType = MS_OCCUPANCY_SENSOR_TYPE_PIR;

#define DEFAULT_PirOccupiedToUnoccupiedDelay 60
#define DEFAULT_PirUnoccupiedToOccupiedDelay 60
#define DEFAULT_MsIlluminanceLevelSensingSensitivity 69
#define DEFAULT_MsTemperatureMinAbsoluteChange 50 // 0.5
#define DEFAULT_MsPressureMinAbsoluteChange 1
#define DEFAULT_MsHumidityMinAbsoluteChange 1000 //10.00%
#define DEFAULT_MsIlluminanceMinAbsoluteChange 10
#define DEFAULT_MsTemperaturePeriod 10
#define DEFAULT_MsPressurePeriod 10
#define DEFAULT_MsHumidityPeriod 10
#define DEFAULT_MsIlluminancePeriod 10
#define DEFAULT_CfgBatteryPeriod 30 // min
#define DEFAULT_HvacUiDisplayMode 3 // 0 - invert, 1 - not invert
application_config_t zclApp_Config = {.PirOccupiedToUnoccupiedDelay = DEFAULT_PirOccupiedToUnoccupiedDelay,
                                      .PirUnoccupiedToOccupiedDelay = DEFAULT_PirUnoccupiedToOccupiedDelay,
                                      .MsIlluminanceLevelSensingSensitivity = DEFAULT_MsIlluminanceLevelSensingSensitivity,
                                      .MsTemperatureMinAbsoluteChange = DEFAULT_MsTemperatureMinAbsoluteChange,
                                      .MsPressureMinAbsoluteChange = DEFAULT_MsPressureMinAbsoluteChange,
                                      .MsHumidityMinAbsoluteChange = DEFAULT_MsHumidityMinAbsoluteChange,
                                      .MsIlluminanceMinAbsoluteChange = DEFAULT_MsIlluminanceMinAbsoluteChange,
                                      .MsTemperaturePeriod = DEFAULT_MsTemperaturePeriod,
                                      .MsPressurePeriod = DEFAULT_MsPressurePeriod,
                                      .MsHumidityPeriod = DEFAULT_MsHumidityPeriod,
                                      .MsIlluminancePeriod = DEFAULT_MsIlluminancePeriod,
                                      .CfgBatteryPeriod = DEFAULT_CfgBatteryPeriod,
                                      .HvacUiDisplayMode = DEFAULT_HvacUiDisplayMode
};

// Basic Cluster
const uint8 zclApp_HWRevision = APP_HWVERSION;
const uint8 zclApp_ZCLVersion = APP_ZCLVERSION;
const uint8 zclApp_ApplicationVersion = 3;
const uint8 zclApp_StackVersion = 4;

//{lenght, 'd', 'a', 't', 'a'}
const uint8 zclApp_ManufacturerName[] = {9, 'm', 'o', 'd', 'k', 'a', 'm', '.', 'r', 'u'};
const uint8 zclApp_ModelId[] = {16, 'D', 'I', 'Y', 'R', 'u', 'Z', '_', 'E', '-','M', 'o', 'n', 'i', 't', 'o', 'r'};
const uint8 zclApp_PowerSource = POWER_SOURCE_BATTERY;

/*********************************************************************
 * ATTRIBUTE DEFINITIONS - Uses REAL cluster IDs
 */

CONST zclAttrRec_t zclApp_AttrsFirstEP[] = {
    {BASIC, {ATTRID_BASIC_ZCL_VERSION, ZCL_UINT8, R, (void *)&zclApp_ZCLVersion}},
    {BASIC, {ATTRID_BASIC_APPL_VERSION, ZCL_UINT8, R, (void *)&zclApp_ApplicationVersion}},
    {BASIC, {ATTRID_BASIC_STACK_VERSION, ZCL_UINT8, R, (void *)&zclApp_StackVersion}},
    {BASIC, {ATTRID_BASIC_HW_VERSION, ZCL_UINT8, R, (void *)&zclApp_HWRevision}},   
    {BASIC, {ATTRID_BASIC_MANUFACTURER_NAME, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_ManufacturerName}},
    {BASIC, {ATTRID_BASIC_MODEL_ID, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_ModelId}},
    {BASIC, {ATTRID_BASIC_DATE_CODE, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_DateCode}},
    {BASIC, {ATTRID_BASIC_POWER_SOURCE, ZCL_DATATYPE_ENUM8, R, (void *)&zclApp_PowerSource}},
    {BASIC, {ATTRID_BASIC_SW_BUILD_ID, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_DateCode}},
    {BASIC, {ATTRID_CLUSTER_REVISION, ZCL_DATATYPE_UINT16, R, (void *)&zclApp_clusterRevision_all}},   
    
    {POWER_CFG, {ATTRID_POWER_CFG_BATTERY_VOLTAGE, ZCL_UINT8, RR, (void *)&zclBattery_Voltage}},
    {POWER_CFG, {ATTRID_POWER_CFG_BATTERY_PERCENTAGE_REMAINING, ZCL_UINT8, RR, (void *)&zclBattery_PercentageRemainig}},
    {POWER_CFG, {ATTRID_POWER_CFG_BATTERY_PERIOD, ZCL_UINT16, RRW, (void *)&zclApp_Config.CfgBatteryPeriod}},
    
    {ONOFF, {ATTRID_ON_OFF, ZCL_BOOLEAN, RR, (void *)&zclApp_Occupied_OnOff}},

    {GEN_TIME, {ATTRID_TIME_TIME, ZCL_UTC, RRW, (void *)&zclApp_GenTime_TimeUTC}},
    {GEN_TIME, {ATTRID_TIME_LOCAL_TIME, ZCL_UINT32, RRW, (void *)&zclApp_GenTime_TimeUTC}},
    
//    {HVAC_UI_CONFIG, {ATTRID_HVAC_THERMOSTAT_UI_CONFIG_DISPLAY_MODE, ZCL_BOOLEAN, RRW, (void *)&zclApp_Config.HvacUiDisplayMode}},
    {HVAC_UI_CONFIG, {ATTRID_HVAC_THERMOSTAT_UI_CONFIG_DISPLAY_MODE, ZCL_UINT8, RRW, (void *)&zclApp_Config.HvacUiDisplayMode}},
    
    {TEMP, {ATTRID_MS_TEMPERATURE_MEASURED_VALUE, ZCL_INT16, RR, (void *)&zclApp_Temperature_Sensor_MeasuredValue}},
    {TEMP, {ATTRID_TEMPERATURE_MIN_ABSOLUTE_CHANGE, ZCL_UINT16, RRW, (void *)&zclApp_Config.MsTemperatureMinAbsoluteChange}},
    {TEMP, {ATTRID_TEMPERATURE_PERIOD, ZCL_UINT16, RRW, (void *)&zclApp_Config.MsTemperaturePeriod}},

    {PRESSURE, {ATTRID_MS_PRESSURE_MEASUREMENT_MEASURED_VALUE, ZCL_INT16, RR, (void *)&zclApp_PressureSensor_MeasuredValue}},
    {PRESSURE, {ATTRID_MS_PRESSURE_MEASUREMENT_SCALED_VALUE, ZCL_INT16, RR, (void *)&zclApp_PressureSensor_ScaledValue}},
    {PRESSURE, {ATTRID_MS_PRESSURE_MEASUREMENT_SCALE, ZCL_INT8, RR, (void *)&zclApp_PressureSensor_Scale}},
    {PRESSURE, {ATTRID_PRESSURE_MIN_ABSOLUTE_CHANGE, ZCL_UINT16, RRW, (void *)&zclApp_Config.MsPressureMinAbsoluteChange}},
    {PRESSURE, {ATTRID_PRESSURE_PERIOD, ZCL_UINT16, RRW, (void *)&zclApp_Config.MsPressurePeriod}},

    {HUMIDITY, {ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE, ZCL_UINT16, RR, (void *)&zclApp_HumiditySensor_MeasuredValue}},
    {HUMIDITY, {ATTRID_HUMIDITY_MIN_ABSOLUTE_CHANGE, ZCL_UINT16, RRW, (void *)&zclApp_Config.MsHumidityMinAbsoluteChange}},
    {HUMIDITY, {ATTRID_HUMIDITY_PERIOD, ZCL_UINT16, RRW, (void *)&zclApp_Config.MsHumidityPeriod}}
};

/*
CONST zclAttrRec_t zclApp_AttrsSecondEP[] = {
//    {ONOFF, {ATTRID_ON_OFF, ZCL_BOOLEAN, R, (void *)&zclApp_Magnet_OnOff}},
    {BINARY_INPUT, {ATTRID_GEN_BINARY_INPUT_PRESENTVALUE, ZCL_BOOLEAN, RR, (void *)&zclApp_Magnet}}
};
*/

CONST zclAttrRec_t zclApp_AttrsThirdEP[] = {
    {ONOFF, {ATTRID_ON_OFF, ZCL_BOOLEAN, R, (void *)&zclApp_Occupied_OnOff}},
    {OCCUPANCY, {ATTRID_MS_OCCUPANCY_SENSING_CONFIG_OCCUPANCY, ZCL_BITMAP8, RR, (void *)&zclApp_Occupied}},
    {OCCUPANCY, {ATTRID_MS_OCCUPANCY_SENSING_CONFIG_PIR_O_TO_U_DELAY, ZCL_UINT16, RRW, (void *)&zclApp_Config.PirOccupiedToUnoccupiedDelay}},
    {OCCUPANCY, {ATTRID_MS_OCCUPANCY_SENSING_CONFIG_PIR_U_TO_O_DELAY, ZCL_UINT16, RRW, (void *)&zclApp_Config.PirUnoccupiedToOccupiedDelay}}
};

CONST zclAttrRec_t zclApp_AttrsFourthEP[] = {
    {ILLUMINANCE, {ATTRID_MS_ILLUMINANCE_MEASURED_VALUE, ZCL_UINT16, RR, (void *)&zclApp_bh1750IlluminanceSensor_MeasuredValue}},
    {ILLUMINANCE, {ATTRID_ILLUMINANCE_MIN_ABSOLUTE_CHANGE, ZCL_UINT16, RRW, (void *)&zclApp_Config.MsIlluminanceMinAbsoluteChange}},
    {ILLUMINANCE, {ATTRID_ILLUMINANCE_LEVEL_SENSING_SENSITIVITY, ZCL_UINT16, RRW, (void *)&zclApp_Config.MsIlluminanceLevelSensingSensitivity}},
    {ILLUMINANCE, {ATTRID_ILLUMINANCE_PERIOD, ZCL_UINT16, RRW, (void *)&zclApp_Config.MsIlluminancePeriod}}
};

//uint8 CONST zclApp_AttrsSecondEPCount = (sizeof(zclApp_AttrsSecondEP) / sizeof(zclApp_AttrsSecondEP[0]));
uint8 CONST zclApp_AttrsFirstEPCount = (sizeof(zclApp_AttrsFirstEP) / sizeof(zclApp_AttrsFirstEP[0]));
uint8 CONST zclApp_AttrsThirdEPCount = (sizeof(zclApp_AttrsThirdEP) / sizeof(zclApp_AttrsThirdEP[0]));
uint8 CONST zclApp_AttrsFourthEPCount = (sizeof(zclApp_AttrsFourthEP) / sizeof(zclApp_AttrsFourthEP[0]));

const cId_t zclApp_InClusterListFirstEP[] = {ZCL_CLUSTER_ID_GEN_BASIC, POWER_CFG, ONOFF, TEMP, PRESSURE, HUMIDITY};
const cId_t zclApp_InClusterListThirdEP[] = {OCCUPANCY};
const cId_t zclApp_InClusterListFourthEP[] = {ILLUMINANCE};

#define APP_MAX_INCLUSTERS_FIRST_EP (sizeof(zclApp_InClusterListFirstEP) / sizeof(zclApp_InClusterListFirstEP[0]))
#define APP_MAX_INCLUSTERS_THIRD_EP (sizeof(zclApp_InClusterListThirdEP) / sizeof(zclApp_InClusterListThirdEP[0]))
#define APP_MAX_INCLUSTERS_FOURTH_EP (sizeof(zclApp_InClusterListFourthEP) / sizeof(zclApp_InClusterListFourthEP[0]))

const cId_t zclApp_OutClusterListFirstEP[] = {GEN_TIME, HVAC_UI_CONFIG};
const cId_t zclApp_OutClusterListThirdEP[] = {ONOFF};

#define APP_MAX_OUTCLUSTERS_FIRST_EP (sizeof(zclApp_OutClusterListFirstEP) / sizeof(zclApp_OutClusterListFirstEP[0]))
#define APP_MAX_OUTCLUSTERS_THIRD_EP (sizeof(zclApp_OutClusterListThirdEP) / sizeof(zclApp_OutClusterListThirdEP[0]))
//#define APP_MAX_OUTCLUSTERS_FOURTH_EP (sizeof(zclApp_OutClusterListFourthEP) / sizeof(zclApp_OutClusterListFourthEP[0]))



SimpleDescriptionFormat_t zclApp_FirstEP = {
    1,                                                  //  int Endpoint;
    ZCL_HA_PROFILE_ID,                                  //  uint16 AppProfId[2];
    ZCL_HA_DEVICEID_SIMPLE_SENSOR,                      //  uint16 AppDeviceId[2];
    APP_DEVICE_VERSION,                          //  int   AppDevVer:4;
    APP_FLAGS,                                   //  int   AppFlags:4;
    APP_MAX_INCLUSTERS_FIRST_EP,                          //  byte  AppNumInClusters;
    (cId_t *)zclApp_InClusterListFirstEP,                //  byte *pAppInClusterList;
    APP_MAX_OUTCLUSTERS_FIRST_EP,                //  byte  AppNumInClusters;
    (cId_t *)zclApp_OutClusterListFirstEP         //  byte *pAppInClusterList;
};

/*
SimpleDescriptionFormat_t zclApp_SecondEP = {
    2,                                                  //  int Endpoint;
    ZCL_HA_PROFILE_ID,                                  //  uint16 AppProfId[2];
    ZCL_HA_DEVICEID_SIMPLE_SENSOR,                      //  uint16 AppDeviceId[2];
    APP_DEVICE_VERSION,                          //  int   AppDevVer:4;
    APP_FLAGS,                                   //  int   AppFlags:4;
    0,                                                  //  byte  AppNumInClusters;
    (cId_t *)NULL,                                      //  byte *pAppInClusterList;
    APP_MAX_OUTCLUSTERS_SECOND_EP,               //  byte  AppNumInClusters;
    (cId_t *)zclApp_OutClusterListSecondEP        //  byte *pAppInClusterList;
};
*/
SimpleDescriptionFormat_t zclApp_ThirdEP = {
    3,                                                  //  int Endpoint;
    ZCL_HA_PROFILE_ID,                                  //  uint16 AppProfId[2];
    ZCL_HA_DEVICEID_OCCUPANCY_SENSOR,                      //  uint16 AppDeviceId[2];
    APP_DEVICE_VERSION,                          //  int   AppDevVer:4;
    APP_FLAGS,                                   //  int   AppFlags:4;
    APP_MAX_INCLUSTERS_THIRD_EP,                                                  //  byte  AppNumInClusters;
    (cId_t *)zclApp_InClusterListThirdEP,                                      //  byte *pAppInClusterList;
    APP_MAX_OUTCLUSTERS_THIRD_EP,               //  byte  AppNumInClusters;
    (cId_t *)zclApp_OutClusterListThirdEP        //  byte *pAppInClusterList;
};

SimpleDescriptionFormat_t zclApp_FourthEP = {
    4,                                                  //  int Endpoint;
    ZCL_HA_PROFILE_ID,                                  //  uint16 AppProfId[2];
    ZCL_HA_DEVICEID_SIMPLE_SENSOR,                      //  uint16 AppDeviceId[2];
    APP_DEVICE_VERSION,                          //  int   AppDevVer:4;
    APP_FLAGS,                                   //  int   AppFlags:4;
    APP_MAX_INCLUSTERS_FOURTH_EP,                                                  //  byte  AppNumInClusters;
    (cId_t *)zclApp_InClusterListFourthEP,                                      //  byte *pAppInClusterList;
    0,               //  byte  AppNumInClusters;
    (cId_t *)NULL        //  byte *pAppInClusterList;
};

void zclApp_ResetAttributesToDefaultValues(void) {
    zclApp_Config.PirOccupiedToUnoccupiedDelay = DEFAULT_PirOccupiedToUnoccupiedDelay;
    zclApp_Config.PirUnoccupiedToOccupiedDelay = DEFAULT_PirUnoccupiedToOccupiedDelay;
    zclApp_Config.MsIlluminanceLevelSensingSensitivity = DEFAULT_MsIlluminanceLevelSensingSensitivity;
    zclApp_Config.MsTemperatureMinAbsoluteChange = DEFAULT_MsTemperatureMinAbsoluteChange;
    zclApp_Config.MsPressureMinAbsoluteChange = DEFAULT_MsPressureMinAbsoluteChange;
    zclApp_Config.MsHumidityMinAbsoluteChange = DEFAULT_MsHumidityMinAbsoluteChange;
    zclApp_Config.MsIlluminanceMinAbsoluteChange = DEFAULT_MsIlluminanceMinAbsoluteChange;
    zclApp_Config.MsTemperaturePeriod = DEFAULT_MsTemperaturePeriod;
    zclApp_Config.MsPressurePeriod = DEFAULT_MsPressurePeriod;
    zclApp_Config.MsHumidityPeriod = DEFAULT_MsHumidityPeriod;
    zclApp_Config.MsIlluminancePeriod = DEFAULT_MsIlluminancePeriod;
    zclApp_Config.CfgBatteryPeriod = DEFAULT_CfgBatteryPeriod;
    zclApp_Config.HvacUiDisplayMode = DEFAULT_HvacUiDisplayMode;    
}