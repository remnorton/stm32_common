/*
 * w25q.h
 *
 *  Created on: 7 но€б. 2019 г.
 *      Author: User
 */

#ifndef W25Q_W25Q_H_
#define W25Q_W25Q_H_
#include "board.h"

//W25Q_Details_t
typedef struct
{
	uint8_t		model;
	uint32_t	capacity;
	uint32_t	block_size;
	uint16_t	block_count;
	uint16_t	sector_size;
	uint16_t	sector_count;
	uint8_t		addr_size;
}W25Q_Details_t;


//W25Q_PinDef_t
typedef struct
{
	GPIO_TypeDef* port;
	uint16_t	  pin;
	GPIO_PinState     actState;
}W25Q_PinDef_t;


//W25Q_t
typedef struct
{
	SPI_HandleTypeDef*		spi;
	W25Q_PinDef_t			cs;
	W25Q_Details_t*			detail;
}W25Q_t;

//W25Q_Result_t
typedef enum
{
	W25Q_Error = 0,
	W25Q_Ok,
}W25Q_Result_t;

//Device model
enum
{
	W25Q40 = 0x12,
	W25Q80,
	W25Q16,
	W25Q32,
	W25Q64,
	W25Q128
};

//Status register
typedef struct
{
	uint16_t	busy : 1;		//BUSY
	uint16_t	wel : 1;		//Write Enable Latch
	uint16_t	bp : 3;			//Block Protect Bits
	uint16_t	tb : 1;			//Top/Bottom Block Protect
	uint16_t	sec : 1;		//Sector/Block Protect
	uint16_t	srp : 2;		//Status Register Protect
	uint16_t	qe : 1;			//QUAD ENABLE
	uint16_t	reserved : 1;
	uint16_t	srl : 3;		//SECURITY REGISTER LOCK
	uint16_t	cmp : 1;		//Complement Protect
	uint16_t	sus : 1;		//SUSPEND STATUS
}Status_Reg_t;

W25Q_Result_t w25q_init(W25Q_t* init);
W25Q_Result_t w25q_deinit(W25Q_t* i);
uint64_t w25q_uid(W25Q_t* i);
Status_Reg_t w25q_get_status(W25Q_t* i);
W25Q_Result_t w25q_enable_write(W25Q_t* i, uint8_t enable);
W25Q_Result_t w25q_write(W25Q_t* i, uint32_t addr, void* data, uint16_t len);
W25Q_Result_t w25q_read(W25Q_t* i, uint32_t addr, void* data, uint16_t len);
W25Q_Result_t w25q_sector_is_clear(W25Q_t* i, uint16_t sec_num);
W25Q_Result_t w25q_erase_sector(W25Q_t* i, uint16_t sector);
W25Q_Result_t w25q_erase_block(W25Q_t* i, uint16_t block);
W25Q_Result_t w25q_mass_erase(W25Q_t* i);
uint16_t w25q_sector_num(W25Q_t* i, uint32_t addr);
uint16_t w25q_block_num(W25Q_t* i, uint32_t addr);

#endif /* W25Q_W25Q_H_ */
