/*
 * lora_mesh.h
 *
 *  Created on: 26 èþë. 2019 ã.
 *      Author: User
 */

#ifndef LORA_MESH_LORA_MESH_H_
#define LORA_MESH_LORA_MESH_H_
#include "lora_mesh_types.h"
#include "lora_mesh_containers.h"

#define LM_MAX_DATA_LEN			200
#define LM_CHANNEL_FREE_RSSI	-70
#define LM_PROXI_RSSI			-50


void lora_mesh_init(LM_INIT_t* i);
void lora_mesh_step();
uint8_t lora_mesh_transmit_data(uint32_t dest_node_id, uint8_t type, uint8_t* data, uint8_t length);
void lora_mesh_check_encrypt();


//test

#endif /* LORA_MESH_LORA_MESH_H_ */
