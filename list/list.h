/*
 * lora_mesh_containers.h
 *
 *  Created on: 19 рту. 2019 у.
 *      Author: User
 */

#ifndef LIST_H_
#define LIST_H_
#include <stdint.h>

#pragma pack(1)

typedef struct _list_item_
{
	struct	_list_item_*	next;
	void*					data;
}List_item_t;

typedef struct
{
	List_item_t*	item;
}List_t;


#pragma pack()

//! public functions
List_t*			list_create();
List_item_t*	list_first(List_t* list);
List_item_t*	list_last(List_t* list);
List_item_t*	list_append(List_t* list, void* data);
List_item_t*	list_get_item(List_t* list, uint16_t index);
List_item_t*	list_insert_before(List_t* list, List_item_t* item, void* data);
List_item_t*	list_insert_after(List_t* list, List_item_t* item, void* data);
List_item_t*	list_insert_first(List_t* list, void* data);

uint8_t			list_item_count(List_t* list);

void			list_remove_item(List_t* list, List_item_t* item);
void			list_release_item(List_item_t* item);
uint8_t			list_is_empty(List_t* list);


#endif /* LIST_H_ */
