/*
 * lora_mesh_const.h
 *
 *  Created on: 26 èþë. 2019 ã.
 *      Author: User
 */

#ifndef LORA_MESH_LORA_MESH_CONST_H_
#define LORA_MESH_LORA_MESH_CONST_H_
#include <stdint.h>

//!Packet direction
enum
{
	LMPD_NONE = 0,
	LMPD_RX,
	LMPD_TX
};

//! Packet types
enum
{
	LMPT_WELCOME = 1,   //!< LMPT_WELCOME
	LMPT_INDENT,        //!< LMPT_INDENT
	LMPT_CHANNEL_SETUP, //!< LMPT_CHANNEL_SETUP
	LMPT_PING,          //!< LMPT_PING
	LMPT_PINGME,        //!< LMPT_PINGME
	LMPT_GODBY,         //!< LMPT_GODBY

	LMPT_LORA_DATA = 20,//!< LMPT_LORA_DATA
	LMPT_TCP_DATA,      //!< LMPT_TCP_DATA
	LMPT_UDP_DATA,      //!< LMPT_UDP_DATA
	LMPT_HTTP_DATA      //!< LMPT_HTTP_DATA
};


//! Application stage
enum
{
	LMSG_NONE = 0,
	LMSG_REGISTER,
	LMSG_WORKING,
};

//! State
enum
{
	LMST_IDLE = 0,
	LMST_BUSY,
	LMST_ARBITRATION
};

//!Task stage
enum
{
	LMTS_NONE = 0,
	LMTS_START,
	LMTS_WAIT_ARBITRATION,
	LMTS_ARBITRATION_OK,
	LMTS_EXECUTE,
	LMTS_FINISH
};

//! Task types
enum
{
	LMTT_NONE = 0,//!< LMTT_NONE
	LMTT_TX,      //!< LMTT_TX
	LMTT_RX,
	LMTT_WAIT_RX  //!< LMTT_WAIT_RX
};

//! Channels
enum
{
	LMCH_Serve = 1,
	LMCH_Main
};



#endif /* LORA_MESH_LORA_MESH_CONST_H_ */
