/*
 * ubx_const.h
 *
 *  Created on: 25.08.2018
 *		Author: Rem Norton
 *
 */


#ifndef UBX_CONST_H
#define UBX_CONST_H

#include <stdint.h>

//
//Definitions
//

#define     UBX_SIGNATURE   (uint16_t)0x62b5


//Classes
#define     UBX_CLSS_NAV   (uint8_t)0x01    //!Navigation Results: Position, Speed, Time, Acc, Heading, DOP, SVs used
#define     UBX_CLSS_RXM   (uint8_t)0x02    //!Receiver Manager Messages: Satellite Status, RTC Status
#define     UBX_CLSS_INF   (uint8_t)0x04    //!Information Messages: Printf-Style Messages, with IDs such as Error, Warning, Notice
#define     UBX_CLSS_ACK   (uint8_t)0x05    //!Ack/Nack Messages: as replies to CFG Input Messages
#define     UBX_CLSS_CFG   (uint8_t)0x06    //!Configuration Input Messages: Set Dynamic Model, Set DOP Mask, Set Baud Rate, etc.
#define     UBX_CLSS_MON   (uint8_t)0x0A    //!Monitoring Messages: Comunication Status, CPU Load, Stack Usage, Task Status
#define     UBX_CLSS_AID   (uint8_t)0x0B    //!AssistNow Aiding Messages: Ephemeris, Almanac, other A-GPS data input
#define     UBX_CLSS_TIM   (uint8_t)0x0D    //!Timing Messages: Timepulse Output, Timemark Results
#define     UBX_CLSS_ESF   (uint8_t)0x10    //!External Sensor Fusion Messages: External sensor measurements and status information
#define     UBX_CLSS_RTCM3 (uint8_t)0xF5    //!< This is undocumented (?)

//Message ID's
#define     UBX_ID_NAV_POSLLH       (uint8_t)0x02
#define     UBX_ID_NAV_DOP          (uint8_t)0x04
#define     UBX_ID_NAV_SOL          (uint8_t)0x06
#define     UBX_ID_NAV_PVT          (uint8_t)0x07
#define     UBX_ID_NAV_VELNED       (uint8_t)0x12
#define     UBX_ID_NAV_TIMEUTC      (uint8_t)0x21
#define     UBX_ID_NAV_SVINFO       (uint8_t)0x30
#define     UBX_ID_NAV_SAT          (uint8_t)0x35
#define     UBX_ID_NAV_SVIN         (uint8_t)0x3B
#define     UBX_ID_NAV_RELPOSNED  	(uint8_t)0x3C
#define     UBX_ID_INF_DEBUG        (uint8_t)0x04
#define     UBX_ID_INF_ERROR        (uint8_t)0x00
#define     UBX_ID_INF_NOTICE       (uint8_t)0x02
#define     UBX_ID_INF_WARNING      (uint8_t)0x01
#define     UBX_ID_ACK_NAK          (uint8_t)0x00
#define     UBX_ID_ACK_ACK          (uint8_t)0x01
#define     UBX_ID_CFG_PRT          (uint8_t)0x00
#define     UBX_ID_CFG_MSG          (uint8_t)0x01
#define     UBX_ID_CFG_RATE         (uint8_t)0x08
#define     UBX_ID_CFG_CFG          (uint8_t)0x09
#define     UBX_ID_CFG_NAV5         (uint8_t)0x24
#define     UBX_ID_CFG_SBAS         (uint8_t)0x16
#define     UBX_ID_CFG_TMODE3       (uint8_t)0x71
#define     UBX_ID_MON_VER          (uint8_t)0x04
#define     UBX_ID_MON_HW           (uint8_t)0x09

#define     UBX_ID_RTCM3_1005       (uint8_t)0x05
#define     UBX_ID_RTCM3_1077       (uint8_t)0x4D
#define     UBX_ID_RTCM3_1087       (uint8_t)0x57


//Composite id
#define     UBX_CLSS_ID_NAV_POSLLH      ((((uint16_t)UBX_ID_NAV_POSLLH) << 8) | UBX_CLSS_NAV)
#define     UBX_CLSS_ID_NAV_DOP         ((((uint16_t)UBX_ID_NAV_DOP) << 8) | UBX_CLSS_NAV)
#define     UBX_CLSS_ID_NAV_SOL         ((((uint16_t)UBX_ID_NAV_SOL) << 8) | UBX_CLSS_NAV)
#define     UBX_CLSS_ID_NAV_PVT         ((((uint16_t)UBX_ID_NAV_PVT) << 8) | UBX_CLSS_NAV)
#define     UBX_CLSS_ID_NAV_VELNED      ((((uint16_t)UBX_ID_NAV_VELNED) << 8) | UBX_CLSS_NAV)
#define     UBX_CLSS_ID_NAV_TIMEUTC     ((((uint16_t)UBX_ID_NAV_TIMEUTC) << 8) | UBX_CLSS_NAV)
#define     UBX_CLSS_ID_NAV_SVINFO      ((((uint16_t)UBX_ID_NAV_SVINFO) << 8) | UBX_CLSS_NAV)
#define     UBX_CLSS_ID_NAV_SAT         ((((uint16_t)UBX_ID_NAV_SAT) << 8) | UBX_CLSS_NAV)
#define     UBX_CLSS_ID_NAV_SVIN        ((((uint16_t)UBX_ID_NAV_SVIN) << 8) | UBX_CLSS_NAV)
#define     UBX_CLSS_ID_NAV_RELPOSNED   ((((uint16_t)UBX_ID_NAV_RELPOSNED) << 8) | UBX_CLSS_NAV)
#define     UBX_CLSS_ID_INF_DEBUG       ((((uint16_t)UBX_ID_INF_DEBUG) << 8) | UBX_CLSS_INF)
#define     UBX_CLSS_ID_INF_ERROR       ((((uint16_t)UBX_ID_INF_ERROR) << 8) | UBX_CLSS_INF)
#define     UBX_CLSS_ID_INF_NOTICE      ((((uint16_t)UBX_ID_INF_NOTICE) << 8) | UBX_CLSS_INF)
#define     UBX_CLSS_ID_INF_WARNING     ((((uint16_t)UBX_ID_INF_WARNING) << 8) | UBX_CLSS_INF)
#define     UBX_CLSS_ID_ACK_NAK         ((((uint16_t)UBX_ID_ACK_NAK) << 8) | UBX_CLSS_ACK)
#define     UBX_CLSS_ID_ACK_ACK         ((((uint16_t)UBX_ID_ACK_ACK) << 8) | UBX_CLSS_ACK)

#define     UBX_CLSS_ID_CFG_PRT         ((((uint16_t)UBX_ID_CFG_PRT) << 8) | UBX_CLSS_CFG)
#define     UBX_CLSS_ID_CFG_MSG         ((((uint16_t)UBX_ID_CFG_MSG) << 8) | UBX_CLSS_CFG)
#define     UBX_CLSS_ID_CFG_RATE        ((((uint16_t)UBX_ID_CFG_RATE) << 8) | UBX_CLSS_CFG)
#define     UBX_CLSS_ID_CFG_CFG         ((((uint16_t)UBX_ID_CFG_CFG) << 8) | UBX_CLSS_CFG)
#define     UBX_CLSS_ID_CFG_NAV5        ((((uint16_t)UBX_ID_CFG_NAV5) << 8) | UBX_CLSS_CFG)
#define     UBX_CLSS_ID_CFG_SBAS        ((((uint16_t)UBX_ID_CFG_SBAS) << 8) | UBX_CLSS_CFG)
#define     UBX_CLSS_ID_CFG_TMODE3      ((((uint16_t)UBX_ID_CFG_TMODE3) << 8) | UBX_CLSS_CFG)

#define     UBX_CLSS_ID_MON_VER         ((((uint16_t)UBX_ID_MON_VER) << 8) | UBX_CLSS_MON)
#define     UBX_CLSS_ID_MON_HW          ((((uint16_t)UBX_ID_MON_HW) << 8) | UBX_CLSS_MON)

//#define     xxx ((((uint16_t)mmm) << 8) | ccc)

//SBAS Services available
#define     SBAS_SER_RANGING        (uint8_t)0x01
#define     SBAS_SER_CORRECTIONS    (uint8_t)0x02
#define     SBAS_SER_INTEGRITY      (uint8_t)0x04
#define     SBAS_SER_TESTMODE       (uint8_t)0x08


//Fix Status Flags
#define     FIX_STATUS_GPSfixOK     (uint8_t)0x01
#define     FIX_STATUS_DiffSoln     (uint8_t)0x02
#define     FIX_STATUS_WKNSET       (uint8_t)0x04
#define     FIX_STATUS_TOWSET       (uint8_t)0x08

//Antenna Flag Mask
#define     ANT_FLAG_SVCS           (uint16_t)0x01  //Enable Antenna Supply Voltage Control Signal
#define     ANT_FLAG_SCD            (uint16_t)0x02  //Enable Short Circuit Detection
#define     ANT_FLAG_OCD            (uint16_t)0x04  //Enable Open Circuit Detection
#define     ANT_FLAG_PD_ON_SCD      (uint16_t)0x08  //Power Down Antenna supply if Short Circuit is detected. (only in combination with Bit 1)GPS.
#define     ANT_FLAG_RECOVERY       (uint16_t)0x10  //Enable automatic recovery from short state

//Antenna Pin Configuration
#define     ANT_PIN_CFG_SWITCH      (uint16_t)0x0010 //PIO-Pin used for switching antenna supply (internal to TIM-LP/TIM-LF)
#define     ANT_PIN_CFG_SCD         (uint16_t)0x0200 //PIO-Pin used for detecting a short in the antenna supply
#define     ANT_PIN_CFG_OCD         (uint16_t)0x4000 //PIO-Pin used for detecting open/not connected antenna
#define     ANT_PIN_CFG_RECONFIG    (uint16_t)0x8000 //if set to 1, and this command is sent to the receiver, the receiver will reconfigure the pins as specified.

//Sub-sections of configuration mask
#define     CFG_SUB_SEC_IO_PORT     (uint32_t) 0x00000001   //Port Settings
#define     CFG_SUB_SEC_MSG         (uint32_t) 0x00000002   //Message Configuration
#define     CFG_SUB_SEC_INF         (uint32_t) 0x00000004   //INF Message Configuration
#define     CFG_SUB_SEC_NAV         (uint32_t) 0x00000008   //Navigation Configuration
#define     CFG_SUB_SEC_RMX         (uint32_t) 0x00000010   //Receiver Manager Configuration
#define     CFG_SUB_SEC_RINV        (uint32_t) 0x00000200   //Remote Inventory Configuration
#define     CFG_SUB_SEC_ANT         (uint32_t) 0x00000400   //Antenna Configuration

//Mask which selects the devices for configuration operation
#define     CFG_DEV_BBR             (uint8_t)0x01   //device battery backed RAM
#define     CFG_DEV_FLASH           (uint8_t)0x02   //device Flash
#define     CFG_DEV_EEPROM          (uint8_t)0x04   //device EEPROM
#define     CFG_DEV_SPI_FLASH       (uint8_t)0x10   //device SPI Flash


//EKF solution action flags
#define     EKF_SOL_ACT_CL_TAB      (uint8_t)0x02   //Clear temperature compensation table
#define     EKF_SOL_ACT_CL_CALIB    (uint8_t)0x04   //Clear stored calibration
#define     EKF_SOL_ACT_NOM_TACHO   (uint8_t)0x10   //Set nominal tacho pulses
#define     EKF_SOL_ACT_NOM_GYRO    (uint8_t)0x20   //Set nominal gyro values
#define     EKF_SOL_ACT_SET_TEMP    (uint8_t)0x40   //Set temp table config (Fields tempUpdate, rmsTemp)
#define     EKF_SOL_ACT_DIR         (uint8_t)0x80   //Set direction pin and gyro sense meaning (inverse_flags)


//EKF solution configuration flags
#define     EKF_SOL_CFG_PPM         (uint8_t)0x01   //pulses per distance (nomPPDist) is given in pulses per meter
#define     EKF_SOL_CFG_USW         (uint8_t)0x02   //use serial wheel tick instead of analog pulse


//EKF solution inversion flags
#define     EKF_SOL_INV_HIGHT_FWD   (uint8_t)0x00   //invert meaning of direction pin: High=Forwards
#define     EKF_SOL_INV_HIGHT_BKW   (uint8_t)0x01   //invert meaning of direction pin: High=Backwards
#define     EKF_SOL_INV_GYRO_CLW    (uint8_t)0x00   //invert meaning of gyro rotation sense: clockwise positive (axis downwards for right handed system)
#define     EKF_SOL_INV_GYRO_ACLW   (uint8_t)0x02   //invert meaning of gyro rotation sense: anti-clockwise positive (axis upwards for right handed system)


//Settings of gyro+wheel tick sol (GWT) flags
#define     ESFGWT_FLG_VEHICLE      (uint16_t)0x1000 //apply the vehicle settings
#define     ESFGWT_FLG_TIME         (uint16_t)0x2000 //apply the timing settings
#define     ESFGWT_FLG_WT           (uint16_t)0x4000 //apply the wheel tick settings


//FXN configuration flags
#define     FXN_FLAG_SLEEP          (uint32_t)0x02  //If this bit is set, the unit will enter Sleep Mode. Otherwise, it will enter CPU only mode.
#define     FXN_FLAG_ABS_ALIGN      (uint32_t)0x08  //Absolute Alignment (only with on/off time)
#define     FXN_FLAG_ON_OFF         (uint32_t)0x10  //Use on/off time Remaining bits shall never be set.


//CFG Information message configuration (protocol)
#define     CFG_INF_PROTO_UBX       (uint8_t)0x00
#define     CFG_INF_PROTO_NMEA      (uint8_t)0x01


//CFG Information message configuration INF-MSG mask
#define     CFG_INF_MSG_ERROR       (uint16_t)0x0001
#define     CFG_INF_MSG_WARNING     (uint16_t)0x0002
#define     CFG_INF_MSG_NOTICE      (uint16_t)0x0004
#define     CFG_INF_MSG_TEST        (uint16_t)0x0008


//CFG-ITFM antenna setting
#define     CFG_ITFM_ANT_UNKNOWN    (uint8_t)0x00
#define     CFG_ITFM_ANT_PASSIVE    (uint8_t)0x01
#define     CFG_ITFM_ANT_ACTIVE     (uint8_t)0x02

//Chip hardware generation
#define     CHIP_GEN_ANTARIS        (uint8_t)0x00
#define     CHIP_GEN_UBLOX5         (uint8_t)0x01
#define     CHIP_GEN_UBLOX6         (uint8_t)0x02

//NAV-SVINFO DataBlock flags
#define     NAV_SVINFO_DBL_SV_USED      (uint8_t)0x01   //SV is used for navigation
#define     NAV_SVINFO_DBL_DIFF_COR     (uint8_t)0x02   //Differential correction data is available for this SV
#define     NAV_SVINFO_DBL_ORBIT_AVAIIL (uint8_t)0x04   //Orbit information is available for this SV (Ephemeris or Almanach)
#define     NAV_SVINFO_DBL_ORBIT_EPH    (uint8_t)0x08   //Orbit information is Ephemeris
#define     NAV_SVINFO_DBL_UNHEALTY     (uint8_t)0x10   //SV is unhealthy / shall not be used
#define     NAV_SVINFO_DBL_ORBIT_ALM    (uint8_t)0x20   //Orbit information is Almanac Plus
#define     NAV_SVINFO_DBL_ORBIT_AOP    (uint8_t)0x40   //Orbit information is AssistNow Autonomous
#define     NAV_SVINFO_DBL_SMOOTHED     (uint8_t)0x80   //Carrier smoothed pseudorange used (see PPP for details)

//CFG-PRT in/out proto mask
#define     CFG_PRT_PROTO_UBX           (uint16_t)0x0001
#define     CFG_PRT_PROTO_NMEA          (uint16_t)0x0002
#define     CFG_PRT_PROTO_RTCMP2        (uint16_t)0x0004


//CFG-PRT spi mode
#define     CFG_PRT_SPI_CPHA            (uint8_t)0x01
#define     CFG_PRT_SPI_CPOL            (uint8_t)0x02

//CFG-PRT uart parity
#define     CFG_PRT_UART_PARITY_EVEN    (uint8_t)0x00
#define     CFG_PRT_UART_PARITY_ODD     (uint8_t)0x01
#define     CFG_PRT_UART_PARITY_NONE    (uint8_t)0x04

//Timesource reference
#define     CFG_TIME_REF_UTC            (uint16_t)0x00
#define     CFG_TIME_REF_GPS            (uint16_t)0x01

//NAV-TIMEUTC validity flags
#define		NAV_TIMEUTC_VALID_TOW		(uint8_t)0x01
#define		NAV_TIMEUTC_VALID_WKN		(uint8_t)0x02
#define		NAV_TIMEUTC_VALID_UTC		(uint8_t)0x04


/*

*/

//
//Enumerations
//

//CFG-PRT uart stop bits
enum
{
    CFG_PRT_UART_SB_ONE = 0,
    CFG_PRT_UART_SB_ONE_HALF,
    CFG_PRT_UART_SB_TWO,
    CFG_PRT_UART_SB_HALF
};

//CFG-PRT uart data lenght
enum
{
    CFG_PRT_UART_DATA_5 = 0,
    CFG_PRT_UART_DATA_6,
    CFG_PRT_UART_DATA_7,
    CFG_PRT_UART_DATA_8,
};


//NAV-SVINFO DataBlock quality
enum
{
    NAV_SVINFO_DBL_QUAL_IDLE = 0,       //This channel is idle
    NAV_SVINFO_DBL_QUAL_SEARCHING,      //Channel is searching
    NAV_SVINFO_DBL_QUAL_AQUIRED,        //Signal aquired
    NAV_SVINFO_DBL_QUAL_DET_UNUSABLE,   //Signal detected but unusable
    NAV_SVINFO_DBL_QUAL_CODE_LOCK,      //Code Lock on Signal
    NAV_SVINFO_DBL_QUAL_CAC_LOCKED      //Code and Carrier locked (5, 6, 7)
};

//DGPS Correction Type Status
enum
{
    DGPS_CS_NONE = 0,
    DGPS_CS_PR_RPR
};

//SBAS Mode
enum
{
    SBAS_MD_Disabled = 0,
    SBAS_MD_Enabled_Integrity,
    SBAS_MD_Enabled_Testmode = 3
};

//SBAS System
enum
{
    SBAS_SYS_Unknown = -1,
    SBAS_SYS_WAAS,
    SBAS_SYS_EGNOS,
    SBAS_SYS_MSAS,
    SBAS_SYS_GPS = 16
};

//GPSfix Type
enum
{
    GPS_FIX_No_Fix = 0,
    GPS_FIX_Dead_Reckoning_only,
    GPS_FIX_2D,
    GPS_FIX_3D,
    GPS_FIX_GPS_dead_reckoning_combined,
    GPS_FIX_Time_only
};

//EKF solution enable
enum
{
    EKF_SOL_DIASBLE = 0,
    EKF_SOL_ENABLE
};

//IO Ports
enum
{
    CFG_IO_PORT_I2C = 0,
    CFG_IO_PORT_UART_1,
    CFG_IO_PORT_UART_2,
    CFG_IO_PORT_USB,
    CFG_IO_PORT_SPI
};

//UART Tx PIN polarity
enum
{
    CFG_UART_TX_POL_LOW = 0,
    CFG_UART_TX_POL_HI
};

#endif // UBX_CONST_H
