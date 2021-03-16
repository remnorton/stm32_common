/*
 * lora_mesh_containers.c
 *
 *  Created on: 19 рту. 2019 у.
 *      Author: User
 */

#include <stdlib.h>
#include "hal_includes.h"
#include "lora_mesh_containers.h"

//
//Private forwards
//
List_item_t* create_list_item();
void kill_list_item(List_item_t* item);

//
//Public members
//
List_t* list_create()
{
	List_t* res = (List_t*)malloc(sizeof(List_t));
	if (res)  res->item = 0;
	return res;
}

List_item_t* list_first(List_t* list)
{
	if (!list) return 0;
	return list->item;
}

List_item_t* list_last(List_t* list)
{
	if (!list) return 0;
	if (!list->item) return 0;
	List_item_t* res = list->item;
	while (res->next)
		res = res->next;
	return res;
}

List_item_t* list_append(List_t* list, void* data)
{
	if (!list || !data) return 0;
	List_item_t* last = list_last(list);
	if (!last)
	{
		list->item = create_list_item();
		list->item->data = data;
		return list->item;
	}

	last->next = create_list_item();
	last->next->data = data;
	return last->next;
}

List_item_t* list_get_item(List_t* list, uint16_t index)
{
	if (!list) return 0;
	List_item_t* res = list->item;
	while (res && index)
	{
		res = res->next;
		index--;
	}
	return (index?0:res);
}

List_item_t*	list_insert_before(List_t* list, List_item_t* item, void* data)
{
	if (list->item == item) return list_insert_first(list, data);
	List_item_t* pos = list->item;
	List_item_t* res = 0;
	while (pos && pos->next)
	{
		if (pos->next == item)
		{
			res = list_insert_after(list, pos, data);
			pos = 0;
		}
		else pos = pos->next;

	}
	return res;
}

List_item_t* list_insert_after(List_t* list, List_item_t* item, void* data)
{
	List_item_t* res = create_list_item();
	res->data = data;
	res->next = item->next;
	item->next = res;
	return res;
}

List_item_t*	list_insert_first(List_t* list, void* data)
{
	List_item_t* item = list->item;
	list->item = create_list_item();
	list->item->data = data;
	list->item->next = item;
	return list->item;
}

uint8_t list_item_count(List_t* list)
{
	uint8_t res = 0;
	if (list)
	{
		List_item_t* item = list->item;
		while(item)
		{
			res++;
			item = item->next;
		}
	}
	return res;
}

void list_remove_item(List_t* list, List_item_t* item)
{
	if (!list || !item) return;
	List_item_t* del = 0;
	List_item_t* pos = list->item;
	if (list->item == item) list->item = item->next;
	else
	{
		while (!del && pos->next)
		{
			if (item == pos->next) del = pos;
			else pos = pos->next;
		}

		if (del) del->next = del->next->next;
	}
}

void list_release_item(List_item_t* item)
{
	__disable_irq();
	if (item->data) free(item->data);
	free(item);
	__enable_irq();
}

uint8_t list_is_empty(List_t* list)
{
	if (!list) return 1;
	return (list->item)?0:1;
}

//
//Private members
//

List_item_t* create_list_item()
{
	List_item_t* res = (List_item_t*)malloc(sizeof(List_item_t));
	__disable_irq();
	if (res)
	{
		res->next = 0;
		res->data = 0;
	}
	__enable_irq();
	return res;
}

void kill_list_item(List_item_t* item)
{
	if (!item) return ;
	__disable_irq();
	if (item->data) free(item->data);
	free(item);
	__enable_irq();
}
