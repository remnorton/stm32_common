/*
 * ubx.h
 *
 *  Created on: 25.08.2018
 *		Author:  Rem Norton
 *
 *	UBX exchange protocol handling
 */


#ifndef UBX_H
#define UBX_H

#include "ubx_const.h"



//
//public types
//

typedef uint16_t	ubx_crc_t;

#pragma pack(1)

typedef struct
{
    uint16_t    signature;
    union
    {
        struct
        {
            uint8_t     class_id;
            uint8_t     mess_id;
        }detail;
        uint16_t    simple;
    }id;
    uint16_t    size;
}UBX_HEADER_t;

//
//ACK messages payload
//
typedef struct
{
    uint8_t     clssId;
    uint8_t     messId;
}UBX_ACK_PAYLOAD_t;

//
//AID messages payload
//
typedef struct
{
    uint8_t     svId;
}UBX_AID_ALM_SV_PAYLOAD_t;

typedef struct
{
    uint32_t    svId;
    uint32_t    week;
    uint32_t    dwrd[4];
}UBX_AID_ALM_GPS_PAYLOAD_t;

typedef struct
{
    uint8_t     id_size;
    uint8_t     type;
    uint16_t    offset;
    uint16_t    size;
    uint16_t    field_id;
    uint16_t    data_size;
    uint8_t     id1;
    uint8_t     id2;
    uint32_t    id3;
    /*16 + 1*N U1 - data - Data for the ALP client*/
}UBX_AID_ALPSRV_PAYLOAD_HEADER_t;

//
//NAV messages payload
//
typedef struct
{
    uint32_t    i_tow;          //ms GPS millisecond time of week
    uint8_t     config;         //AssistNow Autonomous is disabled (0) or enabled (not 0)
    uint8_t     status;         //AssistNow Autonomous subsystem is idle (0) or running (not 0)
    uint8_t     reserved1[2];   //Always set to zero
    uint32_t    avail;          //data availability mask for GPS SVs (bits 0-31 correspond to GPS PRN 1-32)
    uint32_t    reserved2[2];   //Always set to zero
}UBX_NAV_AOPSTATUS_PAYLOAD_t;

typedef struct
{
    uint32_t    i_tow;          //ms GPS millisecond time of week
    int32_t     clk_bias;       //ns Clock bias in nanoseconds
    int32_t     clk_drift;      //ns/s Clock drift in nanoseconds per second
    uint32_t    time_acc;       //ns Time Accuracy Estimate
    uint32_t    freq_acc;       //ps/s Frequency Accuracy Estimate
}UBX_NAV_CLOCK_PAYLOAD_t;

typedef struct
{
    uint32_t    i_tow;          //ms GPS millisecond time of week
    int32_t     age;            //ms Age of newest correction data
    int16_t     base_id;        //DGPS Base Station ID
    int16_t     base_health;    //DGPS Base Station Health Status
    uint8_t     num_chan;       //Number of channels for which correction data is following
    uint8_t     status;         //DGPS Correction Type Status (DGPS_CS_NONE, DGPS_CS_PR_RPR)
    uint16_t    reserved1;      //
}UBX_NAV_DGPS_PAYLOAD_t;

typedef struct
{
    uint8_t     svid;           //Satellite ID
    uint8_t     flags;          //Bitmask / Channel Number Bits 0x01 .. 0x08: = GPS Channel this SV is on Bit 0x10: is DGPS Used for this SV/Channel? Bit 0x20 .. 0x80: reserved
    uint16_t    age_c;          //ms Age of latest correction data
    float       prc;            //m Pseudo Range Correction
    float       prrc;           //m/s Pseudo Range Rate Correction
}UBX_NAV_DGPS_DATA_BLOCK_t;

typedef struct
{
    uint32_t    i_tow;          //ms GPS millisecond time of week
    uint16_t    g_dop;          //x0.01 Geometric DOP
    uint16_t    p_dop;          //x0.01 Position DOP
    uint16_t    t_dop;          //x0.01 Time DOP
    uint16_t    v_dop;          //x0.01 Vertical DOP
    uint16_t    h_dop;          //x0.01 Horizontal DOP
    uint16_t    n_dop;          //x0.01 Northing DOP
    uint16_t    e_dop;          //x0.01 Easting DOP
}UBX_NAV_DOP_PAYLOAD_t;

typedef struct
{
    int32_t     pulses;         //number of pulsed in last update period
    int32_t     period;         //ms Duration of last period
    uint32_t    gyro_mean;      //Uncorrected average Gyro value in last period
    int16_t     temperature;    //x2^-8 temperature celsius degrees Temperature
    int8_t      direstion;      //Direction flag
}UBX_NAV_EKFSTATUS_PAYLOAD_t;

typedef struct
{
    uint32_t    i_tow;          //ms GPS millisecond time of week
    int32_t     ecef_x;         //cm ECEF X coordinate
    int32_t     ecef_y;         //cm ECEF Y coordinate
    int32_t     ecef_z;         //cm ECEF Z coordinate
    uint32_t    p_acc;          //cm Position Accuracy Estimate
}UBX_NAV_POSECEF_PAYLOAD_t;

typedef struct
{
    uint32_t    i_tow;          //ms GPS millisecond time of week
    int32_t     lon;            //x1e-7 deg Longitude
    int32_t     lat;            //x1e-7 deg Latitude
    int32_t     heihgt;         //mm Height above Ellipsoid
    int32_t     h_msl;          //mm Height above mean sea level
    uint32_t    h_acc;          //mm Horizontal Accuracy Estimate
    uint32_t    v_acc;          //mm Vertical Accuracy Estimate
}UBX_NAV_POSLLH_PAYLOAD_t;

typedef struct
{
    uint32_t    i_tow;          //ms GPS millisecond time of week
    uint8_t     geo;            //PRN Number of the GEO where correction and integrity data is used from
    uint8_t     mode;           //SBAS Mode (SBAS_MD_Disabled, SBAS_MD_Enabled_Integrity, SBAS_MD_Enabled_Testmode)
    int8_t      sys;            //SBAS System (WAAS/EGNOS/...)
    uint8_t     service;        //SBAS Services available (bit flags SBAS_SER_x)
    uint8_t     cnt;            //Number of SV data following
    uint8_t     reserved[3];    //
}UBX_NAV_SBAS_PAYLOAD_t;

typedef struct
{
    uint8_t     sv_id;          //SV Id
    uint8_t     flags;          //Flags for this SV
    uint8_t     udre;           // Monitoring status
    uint8_t     sys;            //SBAS System (WAAS/EGNOS/...)
    uint8_t     service;        //SBAS Services available (bit flags SBAS_SER_x)
    uint8_t     reserved1;      //
    int16_t     prc;            //Pseudo Range correction in [cm]
    uint16_t    reserved2;      //
    int16_t     ic;             //Ionosphere correction in [cm]
}UBX_NAV_SBAS_DATA_BLOCK_t;

typedef struct
{
    uint32_t    i_tow;          //ms GPS millisecond time of week
    int32_t     f_tow;          //ns Fractional Nanoseconds remainder of rounded ms above, range -500000 .. 500000
    int16_t     week;           //GPS week (GPS time)
    uint8_t     gps_fix;        //GPSfix Type, range 0..5 (GPS_FIX_x)
    uint8_t     flags;          //Fix Status Flags (FIX_STATUS_x)
    int32_t     ecef_x;         //cm ECEF X coordinate
    int32_t     ecef_y;         //cm ECEF Y coordinate
    int32_t     ecef_z;         //cm ECEF Z coordinate
    uint32_t    p_acc;          //cm 3D Position Accuracy Estimate
    int32_t     ece_vx;         //cm/s ECEF X velocity
    int32_t     ece_vy;         //cm/s ECEF Y velocity
    int32_t     ece_vz;         //cm/s ECEF Z velocity
    uint32_t    s_acc;          //cm/s Speed Accuracy Estimate
    uint16_t    p_dop;          //x0.01 Position DOP
    uint8_t     reserved1;      //
    uint8_t     num_sv;         //Number of SVs used in Nav Solution
    uint32_t    reserved2;      //
}UBX_NAV_SOL_PAYLOAD_t;

//!NAV-SVINFO (0x01 0x30)
typedef struct
{
    uint32_t     i_tow;          //ms GPS Millisecond time of week
    uint8_t     num_ch;         //Number of channels
    struct
    {
        uint8_t reserved : 5;
        uint8_t chip_gen : 3;   //CHIP_GEN_x
    }flags;                     //
    uint16_t    reserved;       //
}UBX_NAV_SVINFO_PAYLOAD_t;

typedef struct
{
    uint8_t     channel_num;    //Channel number, 255 for SVs not assigned to a channel
    uint8_t     svid;           //Satellite ID
    uint8_t     flags;          //Bitmask (NAV_SVINFO_DBL_x)
    uint8_t     quality;        //Bitfield (NAV_SVINFO_DBL_QUAL_x)
    uint8_t     cno;            //dbHz Carrier to Noise Ratio (Signal Strength)
    int8_t      elev;           //deg Elevation in integer degrees
    int16_t     azim;           //deg Azimuth in integer degrees
    int32_t     pr_res;         //cm Pseudo range residual in centimetres
}UBX_NAV_SVINFO_DATA_BLOCK_t;

//!NAV-TIMEUTC (0x01 0x21)
typedef struct
{
    uint32_t    i_tow;          //ms GPS Millisecond Time of Week
    uint32_t    t_acc;          //ns Time Accuracy Estimate
    int32_t     nano;           //ns Nanoseconds of second, range -1e9 .. 1e9
    uint16_t    year;           //Year, range 1999..2099 (UTC)
    uint8_t     month;          //Month, range 1..12 (UTC)
    uint8_t     day;            //Day of Month, range 1..31 (UTC)
    uint8_t     hour;           //Hour of Day, range 0..23 (UTC)
    uint8_t     min;            //Minute of Hour, range 0..59 (UTC)
    uint8_t     sec;            //Seconds of Minute, range 0..59 (UTC)
    uint8_t     valid;          //Validity Flags
}UBX_NAV_TIMEUTC_PAYLOAD_t;

/*

*/

//
//CFG messages payload
//

typedef struct
{
    uint16_t    flags;          //Antenna Flag Mask (ANT_FLAG_x)
    uint16_t    pins;           //Antenna Pin Configuration (ANT_PIN_CFG_x)
}UBX_CFG_ANT_PAYLOAD_t;


typedef struct
{
    uint32_t    clear_mask;     //Mask with configuration sub-sections to Clear (CFG_SUB_SEC_x)
    uint32_t    save_mask;      //Mask with configuration sub-section to Save (CFG_SUB_SEC_x)
    uint32_t    load_mask;      //Mask with configuration sub-sections to Load (CFG_SUB_SEC_x)
    uint8_t     device_mask;    //Mask which selects the devices for this command
}UBX_CFG_CFG_PAYLOAD_t;

//!0x06 0x06
typedef struct
{
    double      maj_a;          //m Semi-major Axis ( accepted range = 6,300,000.0 to 6,500,000.0 metres ). 8 R8 - flat - 1.0 / Flattening ( accepted range is 0.0 to 500.0 )
    float       dx;             //m X Axis shift at the origin ( accepted range is +/- 5000.0 metres )
    float       dy;             //m Y Axis shift at the origin ( accepted range is +/- 5000.0 metres )
    float       dz;             //m Z Axis shift at the origin ( accepted range is +/- 5000.0 metres )
    float       rot_x;          //s Rotation about the X Axis ( accepted range is +/- 20.0 milli-arc seconds )
    float       rot_y;          //s Rotation about the Y Axis ( accepted range is +/- 20.0 milli-arc seconds )
    float       rot_z;          //s Rotation about the Z Axis ( accepted range is +/- 20.0 milli-arc seconds )
    float       scale;          //ppm Scale change ( accepted range is 0.0 to 50.0 parts per million )
}UBX_CFG_DAT_PAYLOAD_t;

//!CFG-EKF (0x06 0x12)
typedef struct
{
    uint8_t     disable_ekf;    //EKF_SOL_x
    uint8_t     action_flags;   //Meaning in input struct: Config action flags (EKF_SOL_ACT_x)
    uint8_t     config_flags;   //configuration flags (EKF_SOL_CFG_x)
    uint8_t     inverse_flags;  //The following flags can be used to 'invert' the meaning of the sensor signals (EKF_SOL_INV_x)
    uint32_t    reserved2;      //Always set to zero
    uint16_t    nom_pp_dist;    //Nominal tacho pulses per distance, permitted range and distance unit (m or km) depend on pulsesPerM flag
    uint16_t    nom_zero;       //mV Nominal gyro zero point output, permitted range: 2000--3000
    uint8_t     nom_sens;       //mV/(deg/s) Nominal gyro sensitivity, permitted range: 20--40
    uint8_t     rms_temp;       //x0.1 mV Maximum allowable RMS threshold for zero velocity temperature compensation, permitted range: 1--10
    uint16_t    temp_update;    //s Temperature table save-to-flash-interval, minimum: 9
}UBX_CFG_EKF_PAYLOAD_t;


//!CFG-ESFGWT (0x06 0x29)
typedef struct
{
    uint16_t    flags;          //Flags (ESFGWT_FLG_x)
    uint16_t    id;             //identification number of the sensor data provider, set to zero if not advised differently by u-blox.
    uint32_t    wt_factor;      //x1e-6 wheel tick factor to obtain distance [m] from WT (0= not set)
    uint32_t    reserved1;      //m wheel tick quantization error, calculated as 2*PI*wheelRadius divided by ticksPerRotation
    uint32_t    wt_quant_error; //
    uint32_t    time_tag_factor;//x1e-6 factor of sensor time tag to obtain seconds
    uint32_t    wt_count_max;   //maximum value of tick counter (rollover - 1) (0 if no rollover but relative values)
    uint32_t    time_tag_max;   //maximum value of sensor time tag (rollover - 1) (0 if no rollover but relative dt values)
    uint16_t    wt_latency;     //ms latency of wheel tick data due to e. g. CAN-Bus
    uint16_t    reserved2;      //
    uint8_t     wt_frequaency;  //Hz Nominal wheel tick data frequency
    uint8_t     reserved3;      //
    uint16_t    speed_dead_band;//cm/s dead band of speed sensor (0 = not set)
    uint32_t    reserved45[2];  //
}UBX_CFG_ESFGWT_PAYLOAD_t;



//!CFG-FXN (0x06 0x0E)
typedef struct
{
    uint32_t    flags;          //FXN configuration flags. Bitmask, Combination of the following flags. (FXN_FLAG_x)
    uint32_t    t_reacq;        //ms Time the receiver tries to re-acquire satellites, before going to off state
    uint32_t    t_acq;          //ms Time the receiver tries to acquire satellites, before going to off state
    uint32_t    t_reacq_off;    //ms Time the receiver stays in Off-State, if re-acquisition failed
    uint32_t    t_acq_off;      //ms Time the receiver stays in Off-State, if acquisition failed
    uint32_t    t_on;           //ms On time (starts with first fix)
    uint32_t    t_off;          //ms Sleep time after normal ontime (actual off time may vary due to data download)
    uint32_t    reserved;       //
    uint32_t    base_tow;       //ms Base TOW to which t_on/t_sleep are aligned if ABSOLUTE_ALIGN is set
}UBX_CFG_FXN_PAYLOAD_t;


//!CFG-INF (0x06 0x02)
typedef struct
{
    uint8_t     potocol_id;     //Protocol Identifier, identifying the output protocol for this Poll Request (CFG_INF_PROTO_x)
}UBX_CFG_INF_PAYLOAD_t;


typedef struct
{
    uint8_t     potocol_id;     //Protocol Identifier, identifying the output protocol for this Poll Request (CFG_INF_PROTO_x)
    uint8_t     reserved0;      //
    uint16_t    reserved1;      //
    uint32_t    skip_mask;      //mask[0..3]
    uint16_t    inf_msg_mask;  //A bit mask, saying which information messages are enabled on each I/O target (CFG_INF_MSG_x)
}UBX_CFG_INF_DATA_BLOCK_t;


//!CFG-ITFM (0x06 0x39)
typedef struct
{
    struct
    {
        uint32_t    enable      : 1;    //enable interference detection
        uint32_t    reserved    : 22;   //reserved algorithm settings - should be set to 0x16B156 in hex for correct settings
        uint32_t    cw_treshold : 5;    //CW jamming detection threshold (dB)
        uint32_t    bb_treshold : 4;    //Broadband jamming detection threshold (dB)
    }config;         //interference config word (valid setting: CFG_ITFM_RESERVED_BITS | CFG_ITFM_x)
    struct
    {
        uint32_t    reserved1   : 18;   //should be set to 0x31E in hex for correct setting
        uint32_t    ant_setting : 2;    //antenna Setting (CFG_ITFM_ANT_x)
        uint32_t    reserved2   : 12;   //reserved, set to 0
    }config_ex;      //extra settings for jamming/interference monitor (CFG_ITFM_EX_x)
}UBX_CFG_ITFM_PAYLOAD_t;


//!CFG-MSG (0x06 0x01) POOL
typedef struct
{
    uint8_t     msg_class;
    uint8_t     msg_id;
}UBX_CFG_MSG_POOL_PAYLOAD_t;

//!CFG-MSG (0x06 0x01)
typedef struct
{
    uint8_t     msg_class;
    uint8_t     msg_id;
    uint8_t     rate[6];
}UBX_CFG_MSG_PAYLOAD_t;


//!CFG-PRT (0x06 0x00)
typedef struct
{
    uint8_t port_id;        //IO port ID (CFG_IO_PORT_x)
}UBX_CFG_PRT_POLL_PAYLOAD_t;

typedef struct
{
    uint8_t port_id;        //IO port ID (CFG_IO_PORT_x)
    uint8_t     reserved;   //
    union
    {
        struct
        {
            uint16_t    thres : 9;  //Threshold The given threshold is multiplied by 8 bytes.
            uint16_t    pin : 5;    //PIO to be used (must not be in use already by another function)
            uint16_t    pol : 1;    //Polarity (CFG_UART_TX_POL_x)
            uint16_t    en : 1;     //Enable flag
        }details;
        uint16_t    simple;
    }tx_ready;   //reserved (Alwyas set to zero) up to Firmware 7.01
    union
    {
        struct
        {
            struct
            {

                uint32_t    unused4 : 4;
                uint32_t    reserved : 1; //Must be ALWAYS=1 (for backward compatibility)
                uint32_t    unused3 : 1;
                uint32_t    data_len : 2;  //CFG_PRT_UART_DATA_x
                uint32_t    unused2 : 1;
                uint32_t    parity : 3; //CFG_PRT_UART_PARITY_
                uint32_t    stop_bits : 2; //CFG_PRT_UART_SB_x
            }mode;      //A bit mask describing the UART mode
            uint32_t    baud_rate;
        }uart;
        struct
        {
            uint32_t    reserved2[2];   //
        }usb;
        struct
        {
            struct
            {
                uint32_t unused4 : 1;
                uint32_t spi_mode : 2;      // Bitmask (CFG_PRT_SPI_x)
                uint32_t unused3 : 3;       //
                uint32_t flow_ctrl : 1;     //(u-blox 6 only) 0: Flow control disabled, 1: Flow control enabled (9-bit mode)
                uint32_t unused2 : 1;       //
                uint32_t ff_cnt : 8;        //Number of bytes containing 0xFF to receive before switching off reception. Range: 0(mechanism off)-255
            }mode;
            uint32_t    reserved;           //
        }spi;
    }port_specific;
    uint16_t    in_proto_mask;  //CFG_PRT_PROTO_x
    uint16_t    out_proto_mask; //CFG_PRT_PROTO_x
    uint16_t    reserved2[2];  //Always set to zero
}UBX_CFG_PRT_PAYLOAD_t;


//!CFG-RATE (0x06 0x08)
typedef struct
{
    uint16_t    meas_rate;      //ms Measurement Rate, GPS measurements are taken every measRate milliseconds
    uint16_t    nav_rate;       //cycles Navigation Rate, in number of measurement cycles. On u-blox 5 and u-blox 6, this parameter cannot be changed, and is always equals 1.
    uint16_t    time_ref;       //Alignment to reference time: CFG_TIME_REF_x
}UBX_CFG_RATE_PAYLOAD_t;


/*



*/

/*
//!
typedef struct
{

}UBX_xxx_PAYLOAD_t;
*/

#pragma pack()


ubx_crc_t ubx_crc(uint8_t* data, uint16_t length);
uint16_t ubx_make_pck(uint8_t* dest, uint8_t class_id, uint8_t mess_id, uint8_t* data, uint16_t data_size);

#endif // UBX_H
