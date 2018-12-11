#ifndef APP_CONFIG_H
#define APP_CONFIG_H



#define SW_VERSION                  1
#define HD_VERSION                  1
#define PRODUCT_ID                  60
#define DEVICE_DEFAULT_NAME         "taker"
#define DEFAULT_POWEROFF_COUNTDOWN_TIME      10*60*60

#define MAX_NOTE_ID                 50

/*
page size =4k   one page has 8 blocks  
APP             0x1f000~0x3f000
download        0x3f000~0x5f000

env-var         0x70000~0x74000
env-constant    0x74000~0x75000
boot_info       0x75000~0x76000
boot            0x78000~0x80000
*/


#define DEVICE_CONSTANT_INFO_START_ADR    0x74000
#define DEVICE_CONSTANT_INFO_END_ADR      0x74fff
#define DEVICE_VAR_INFO_START_ADR         0x70000 
#define DEVICE_VAR_INFO_END_ADR           0x73fff 



enum
{
    ERROR_NONE,
    ERROR_OTA_FLOW_NUM,
    ERROR_OTA_LEN,
    ERROR_OTA_CHECKSUM,
    ERROR_OTA_STATUS,
    ERROR_OTA_VERSION,
    ERROR_NAME_CONTENT,
    ERROR_NO_NOTE,
    ERROR_NAME_OVERLENGTH,
    ERROR_JEDI_DFU,//9
};


enum
{
    OTA_DFU_INIT,
    OTA_DFU_DOWNLOAD,
    OTA_DFU_VERIFY_CHECKSUM,
    OTA_DFU_WAIT_SWTICH,
    
};
enum
{
    
    GATEWAY_FW_NUM      =0x0,
    MCU_FW_NUM          =1,
    BLE_FW_NUM          =2,
    JEDI_A4_FW_NUM      =3,
    JEDI_A5_FW_NUM      =4,

};

enum
{
    START_DFU,
    GET_MCU_FW_INFO,
    DOWNLOAD_MCU_FW,
    GET_MCU_FW_CHECK,
    GET_BLE_FW_INFO,
    DOWNLOAD_BLE_FW,
    GET_BLE_FW_CHECK,
    FINISH_DFU,
    
    
};

enum
{
    DEVICE_POWER_OFF,
    DEVICE_STANDBY,
    DEVICE_INIT_BTN,
    DEVICE_OFFLINE,
    DEVICE_ONLINE,
    DEVICE_LOW_POWER_ACTIVE,
    DEVICE_OTA_MODE,//06
    DEVICE_OTA_WAIT_SWITCH,
    DEVICE_TRYING_POWER_OFF,
    DEVICE_FINISHED_PRODUCT_TEST,
    DEVICE_SYNC_MODE,
    DEVICE_DFU_MODE,
    SENSOR_UPDATE,//12   设备进入模组升级模式
    SENSOR_CALIBRA,//13  进入校准模式
};

enum
{
	
	BUTTON_ONE_CLICK           =0x01,
	BUTTON_DOUBLE_CLICK        =0x02,
	BUTTON_EVENT_CREATE_PAGE   =0x05,
    BUTTON_EVENT_UP_PAGE       =0x03,
	BUTTON_EVENT_DOWN_PAGE     =0x04,
	
	BUTTON_EVENT_UP_SHORT  	   =0x20,
	BUTTON_EVENT_UP_DOUBLE  	 =0x21,
	BUTTON_EVENT_UP_LONG  	   =0x22,
	BUTTON_EVENT_DOWN_SHORT  	 =0x23,
	BUTTON_EVENT_DOWN_DOUBLE   =0x24,
	BUTTON_EVENT_DOWN_LONG  	 =0x25,		
};

//蓝牙接口命令
enum
{
    BLE_CMD_STATUS              =   0x80,
    BLE_CMD_POS_DATA            =   0x81,
    BLE_CMD_NAME_SET            =   0x82,
    BLE_CMD_ERROR_MSG           =   0x83,
    BLE_CMD_GET_VERSION         =   0x84,
    BLE_CMD_AUTH_REQUEST        =   0x85,
    BLE_CMD_AUTH_RESPONSE       =   0x86,
    BLE_CMD_RTC_SET             =   0x87,
    BLE_CMD_BTN_EVENT           =   0x88,
    BLE_CMD_DISPALY_PAGE        =   0x89,
    BLE_CMD_REPORT_PAGE_AUTO    =   0x8a,
    BLE_CMD_REQUST_PAGE         =   0x8b,
    BLE_CMD_CTL_POWER_DOWN_TIME =   0x8c,

    BLE_CMD_SYNC_MODE_ENTER     =   0xA0,
    BLE_CMD_SYNC_MODE_QUIT      =   0xA1,
    BLE_CMD_SYNC_FIRST_HEADER   =   0xA2,
    BLE_CMD_SYNC_ONCE_START     =   0xA3,
    BLE_CMD_SYNC_ONCE_CONTENT   =   0xA4,
    BLE_CMD_SYNC_IS_END         =   0xA5,    //结束
    BLE_CMD_SYNC_ONCE_STOP      =   0xA6,
    BLE_CMD_SYNC_DATA           =   0xa8,
    

	BLE_CMD_ENTER_OTA_MODE      =   0xB0,
    BLE_CMD_OTA_FILE_INFO       =   0xB1,
    BLE_CMD_OTA_RAW_DATA        =   0xB2,
    BLE_CMD_OTA_CHECKSUM        =   0xB3,
    BLE_CMD_OTA_RESULT          =   0xB4,
    BLE_CMD_OTA_SWTICH          =   0xB5,
    BLE_CMD_OTA_QUIT            =   0xB6, 
    
    BLE_CMD_REQUIRE_MAC          =	0xc0,
	BLE_CMD_REQUIRE_NAME         =	0xc1,
    
    BLE_CMD_ENTER_UPDATA_EMR_MODE   =   0xd0,
    BLE_CMD_OTA_EMR_FILE_INFO       =   0xd1,
    BLE_CMD_OTA_EMR_RAW_DATA        =   0xd2,
    BLE_CMD_OTA_EMR_CHECKSUM        =   0xd3,
    BLE_CMD_OTA_EMR_RESULT          =   0xd4,
    BLE_CMD_OTA_EMR_SWTICH          =   0xd5,
    BLE_CMD_OTA_EMR_QUIT            =   0xd6, 
    BLE_CMD_GET_PAD_VERSION         =   0xd7,
    BLE_CMD_ENTER_FRQ_ADJUST        =   0xd8,
    BLE_CMD_QUIT_FRQ_ADJUST         =   0xd9,
    
    
};

typedef struct  
{
    uint16_t hw_version; 
    uint32_t ble_fw_version;

} st_version_info;


#endif

