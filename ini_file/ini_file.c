/*
 * ini_file.c
 *
 *  Created on: 22 θών. 2020 γ.
 *      Author: Rem Norton
 */

#include <string.h>
#include "ini_file.h"
#include "ff.h"
#include "crc.h"

#if defined(_FATFS)

/*Private defines*/
#ifndef INI_MAX_BUF_SIZE
#define INI_MAX_BUF_SIZE        256
#endif


/*Private variables*/

List_t*     free_text = 0;
List_t*     content = 0;
uint8_t		modified = 0;

/*Private forwards*/

int                 compare_str(const void *str1, const void* str2, uint16_t len);
uint32_t            calc_hash(void *text, uint16_t len);
INI_Section_t*      allocate_section(uint8_t *name, uint16_t name_len);
INI_Parameter_t*    allocate_param(INI_Section_t* section, uint8_t *name, uint16_t name_len, void* data, uint8_t data_size);
INI_TextBlock_t*    allocate_text_block(INI_Section_t* section, uint8_t *text, uint16_t len, uint8_t type);
void                release_parameter(INI_Parameter_t* param);
void                release_text_block(INI_TextBlock_t* block);
void                clear_section(INI_Section_t* section);
INI_Section_t*      extract_section(uint8_t *line, uint16_t *pos);
INI_Parameter_t*    extract_parameter(INI_Section_t* section, uint8_t *line, uint16_t *pos);
INI_TextBlock_t*    extract_text_block(INI_Section_t* section, uint8_t *line, uint16_t *pos, uint8_t type);
INI_Section_t*      find_section(const char* section);
INI_Parameter_t*    find_param(INI_Section_t* section, const char * param);
void                store_section_content(FIL* file, INI_Section_t* section);
uint8_t             set_param_data(INI_Parameter_t* p, void* data, uint16_t len);

/*Public members*/

uint8_t	ini_init()
{
	free_text = list_create();
	if (!free_text) return 0;
	content = list_create();
	if (!content)
	{
		free(free_text);
		free_text = 0;
		return 0;
	}
	return 1;
}

uint8_t ini_load(const char * file_name)
{
	FIL fl;
	if (f_open(&fl, file_name, FA_READ) != FR_OK) return 0;

	INI_Section_t* cur_sec = 0;
	uint8_t res = 1;

	while(!f_eof(&fl))
	{
		char line[INI_MAX_BUF_SIZE] = {0};
		f_gets(line, sizeof(line), &fl);
		uint16_t len = strlen(line);
		uint16_t idx = 0;
		int16_t line_start = -1;
		uint8_t parsed = 0;
		while ((idx < len) && res)
		{
			uint8_t ch = line[idx];
			if (ch > 32)
			{
				if (line_start < 0) line_start = idx;
				switch(ch)
				{
					case ';':
					{
						if (extract_text_block(cur_sec, (uint8_t*)line, &idx, INI_CT_COMMENT))
							parsed = 1;
						break;
					}
					case '[':
					{
						cur_sec = extract_section((uint8_t*)line, &idx);
						if (cur_sec) parsed = 1;
						else res = 0;
						break;
					}
					case '=':
					{
						if (cur_sec && (line_start > -1))
						{
							if (extract_parameter(cur_sec, (uint8_t*)&line[line_start], &idx)) parsed = 1;
							else res = 0;
						}
						break;
					}
				}
			}
			idx++;
		}
		if (!parsed)
			allocate_text_block(cur_sec, (uint8_t*)line, strlen(line), INI_CT_TEXT);
	}

	f_close(&fl);
	return res;
}

uint8_t ini_store(const char * file_name)
{
	if (list_is_empty(content)) return 0;
	if (!modified) return 1; //nothing to save
	FIL fl;
	if (f_open(&fl, file_name, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
		return 0;

	List_item_t* item = 0;

	//save leading comments
	item = list_first(free_text);
	while (item)
	{
		INI_TextBlock_t* t = (INI_TextBlock_t*)item->data;
		if (t->hash)
		{
			char buf[INI_MAX_BUF_SIZE] = {0};
			memcpy(buf, t->text, t->size);
			if (t->type == INI_CT_COMMENT) f_printf(&fl, ";%s\n", buf);
			else f_printf(&fl, "%s", buf);
		}
		item = item->next;

	}

	item =  list_first(content);
	while(item)
	{
		INI_Section_t* s = (INI_Section_t*)item->data;
		if (s->hash)
		{
			char buf[INI_MAX_BUF_SIZE] = {0};
			memcpy(buf, s->name, s->name_len);
			f_printf(&fl, "[%s]\n", buf);
			store_section_content(&fl, s);
		}
		item = item->next;
	}

	f_close(&fl);
	modified = 0;
	return 1;
}

uint8_t ini_get_param_c(const char * section, const char * param, int max_buf_size, int * fact_count, char * buf)
{
	if (!section || !param || !fact_count || !buf) return 0;
	INI_Section_t* sec = find_section(section);
	if (!sec) return 0;
	INI_Parameter_t* par = find_param(sec, param);
	if (!par) return 0;
	(*fact_count) = (par->data_size < max_buf_size)?par->data_size:max_buf_size;
	if ((*fact_count)) memcpy(buf, par->data, (*fact_count));
	return 1;
}

uint8_t ini_get_param_i(const char * section, const char * param, int need_count, int * fact_count, int * buf)
{
	if (!section || !param || !fact_count || !buf) return 0;
	INI_Section_t* sec = find_section(section);
	if (!sec) return 0;
	INI_Parameter_t* par = find_param(sec, param);
	if (!par) return 0;
	uint16_t head = 0;
	uint16_t tail = 0;
	(*fact_count) = 0;
	char* data = (char*)par->data;
	while ((head <= par->data_size) && ((*fact_count) < need_count))
	{
		if ((data[head] == ',') || (head == (par->data_size*1)))
		{
			char tmp[INI_MAX_BUF_SIZE] = {0};
			uint16_t len = head-tail;
			memcpy(tmp, &data[tail], len);
			uint16_t offset = 0;
			while (((tmp[offset] < '0') || (tmp[offset] > '9')) && (offset < len)) offset++;
			while (((tmp[offset+len] < '0') || (tmp[offset+len] > '9')) && len)
			{
				tmp[offset+len] = 0;
				len--;
			}
			sscanf((char*)&tmp[offset], "%i", &buf[(*fact_count)++]);
			tail = head;
		}
		head++;
	}
	return 1;
}


uint8_t ini_set_param_c(const char * section, const char * param, const char * buf)
{
	INI_Section_t* s = find_section(section);
	if (!s) s = allocate_section((uint8_t*)section, strlen(section));
	if (!s) return 0;
	INI_Parameter_t* p = find_param(s, param);
	if (!p) p = allocate_param(s, (uint8_t*)param, strlen(param), 0, 0);
	if (!p) return 0;
	uint8_t res = set_param_data(p, (void*)buf, strlen(buf));
	if (res) modified = 1;
	return res;
}

uint8_t ini_set_param_i(const char * section, const char * param, int count, const int * buf)
{
	INI_Section_t* s = find_section(section);
	if (!s) s = allocate_section((uint8_t*)section, strlen(section));
	if (!s) return 0;
	INI_Parameter_t* p = find_param(s, param);
	if (!p) p = allocate_param(s, (uint8_t*)param, strlen(param), 0, 0);
	if (!p) return 0;
	char tmp[INI_MAX_BUF_SIZE] = {0};
	for (int i = 0; i < count; i++)
	{
		if (strlen(tmp)) strcat(tmp, ", ");
		sprintf(&tmp[strlen(tmp)], "%i", buf[i]);
	}
	uint8_t res = set_param_data(p, tmp, strlen(tmp));
	if (res) modified = 1;
	return res;
}

#if (INI_ALLOW_DOUBLE == 1)
 uint8_t ini_get_param_d(const char * section, const char * param, int need_count, int * fact_count, double * buf)
 {
	 if (!section || !param || !fact_count || !buf) return 0;
	 INI_Section_t* sec = find_section(section);
	 if (!sec) return 0;
	 INI_Parameter_t* par = find_param(sec, param);
	 if (!par) return 0;
	 uint16_t head = 0;
	 uint16_t tail = 0;
	 (*fact_count) = 0;
	 char* data = (char*)par->data;
	 while ((head <= par->data_size) && ((*fact_count) < need_count))
	 {
		 if ((data[head] == ',') || (head == (par->data_size*1)))
		 {
			 char tmp[32] = {0};
			 uint16_t len = head-tail;
			 memcpy(tmp, &data[tail], len);
			 uint16_t offset = 0;
			 while ((((tmp[offset] < '0') || (tmp[offset] > '9')) && (tmp[offset] != '.')) && (offset < len)) offset++;
			 while (((tmp[offset+len] < '0') || (tmp[offset+len] > '9')) && len)
			 {
				 tmp[offset+len] = 0;
				 len--;
			 }
			 double dv = 0;
			 sscanf((char*)&tmp[offset], "%f", &dv);
			 buf[(*fact_count)++] = dv;
			 tail = head;
		 }
		 head++;
	 }
	 return 1;
 }

 uint8_t ini_set_param_d(const char * section, const char * param, int count, const double * buf)
 {
	 INI_Section_t* s = find_section(section);
	 if (!s) s = allocate_section((uint8_t*)section, strlen(section));
	 if (!s) return 0;
	 INI_Parameter_t* p = find_param(s, param);
	 if (!p) p = allocate_param(s, (uint8_t*)param, strlen(param), 0, 0);
	 if (!p) return 0;
	 char tmp[INI_MAX_BUF_SIZE] = {0};
	 for (int i = 0; i < count; i++)
	 {
		 if (strlen(tmp)) strcat(tmp, ", ");
		 sprintf(&tmp[strlen(tmp)], "%g", buf[i]);
	 }
	 return set_param_data(p, tmp, strlen(tmp));
 }
#endif


 /*Private members*/

 int compare_str(const void *str1, const void* str2, uint16_t len)
 {
	 int res = 0;
	 uint8_t* s1 = (uint8_t*)str1;
	 uint8_t* s2 = (uint8_t*)str2;
	 for (int i = 0; (i < len) && !res; i++) res = (s1[i] == s2[i])?0:i;
	 return res;
 }

 uint32_t calc_hash(void *text, uint16_t len)
 {
	 return crc32_ether((uint8_t*)text, len);
 }

 INI_Section_t* allocate_section(uint8_t *name, uint16_t name_len)
 {
	 //create section
	 INI_Section_t*  s = (INI_Section_t*)malloc(sizeof(INI_Section_t));
	 if (!s) return 0;
	 s->content = list_create();
	 if (!s->content)
	 {
		 free(s);
		 return 0;
	 }
	 s->hash = calc_hash(name, name_len);
	 s->name = (uint8_t*)malloc(name_len);
	 s->name_len = name_len;
	 memcpy(s->name, name, name_len);

	 //allocate section in list
	 if (!list_append(content, s))
	 {
		 free(s->name);
		 free(s->content);
		 return 0;
	 }
	 return s;
 }

 INI_Parameter_t* allocate_param(INI_Section_t* section, uint8_t *name, uint16_t name_len, void* data, uint8_t data_size)
 {
	 //create param
	 INI_Parameter_t* p = (INI_Parameter_t*)malloc(sizeof(INI_Parameter_t));
	 if (!p) return 0;
	 p->data = 0;
	 p->name = (uint8_t*)malloc(name_len);
	 if (!p->name)
	 {
		 free(p);
		 return 0;
	 }

	 if (data_size)
	 {
		 p->data = malloc(data_size);
		 if (!p->data)
		 {
			 free(p->name);
			 free(p);
			 return 0;
		 }
		 memcpy(p->data, data, data_size);
	 }

	 p->type = INI_CT_PARAM;
	 memcpy(p->name, name, name_len);
	 p->data_size = data_size;
	 p->hash = calc_hash(name, name_len);
	 p->name_len = name_len;

	 //allocate parameter in section
	 if (!list_append(section->content, p))
	 {
		 free(p->name);
		 free(p->data);
		 free(p);
		 return 0;
	 }

	 return p;
 }

 INI_TextBlock_t* allocate_text_block(INI_Section_t* section, uint8_t *text, uint16_t len, uint8_t type)
 {
	 //create text block
	 INI_TextBlock_t* t = (INI_TextBlock_t*)malloc(sizeof(INI_TextBlock_t));
	 if (!t) return 0;
	 t->text = (uint8_t*)malloc(len);
	 if (!t)
	 {
		 free(t);
		 return 0;
	 }
	 t->type = type;
	 t->size = len;
	 t->hash = 1;
	 memcpy(t->text, text, len);

	 //allocate text block in section
	 List_t* list = section?section->content:free_text;
	 if (!list_append(list, t))
	 {
		 free(t->text);
		 free(t);
		 return 0;
	 }
	 return t;
 }

 void release_parameter(INI_Parameter_t* param)
 {
	 free(param->name);
	 if (param->data_size) free(param->data);
 }

 void release_text_block(INI_TextBlock_t* block)
 {
	 free(block->text);
 }

 void clear_section(INI_Section_t* section)
 {
	 while(!list_is_empty(section->content))
	 {
		 List_item_t* item = list_first(section->content);
		 uint8_t* type = (uint8_t*)item->data;
		 switch(*type)
		 {
			 case INI_CT_TEXT:
			 case INI_CT_COMMENT:
			 {
				 release_text_block((INI_TextBlock_t*)item->data);
				 break;
			 }
			 case INI_CT_PARAM:
			 {
				 release_parameter((INI_Parameter_t*)item->data);
				 break;
			 }
		 }

		 list_remove_item(section->content, item);
		 list_release_item(item);
	 }
 }

 INI_Section_t* extract_section(uint8_t *line, uint16_t *pos)
 {
	 uint16_t start = (*pos)+1;
	 while ((line[*pos] != 0) && (line[*pos] != ']')) (*pos)++;
	 if (line[*pos] != ']') return 0; //invalid
	 return allocate_section(&line[start], (*pos)-start);
 }

 INI_Parameter_t* extract_parameter(INI_Section_t* section, uint8_t *line, uint16_t *pos)
 {
	 uint16_t delim = (*pos);
	 while(line[*pos] && (line[*pos] != '\n')) (*pos)++;
	 //trim name
	 uint16_t name_trim_right, name_trim_left;
	 name_trim_right = delim-1;
	 name_trim_left = 0;
	 while (line[name_trim_left] < 33) name_trim_left++;
	 while (line[name_trim_right] < 33) name_trim_right--;

	 //trim value
	 uint16_t val_trim_left, val_trim_right;
	 val_trim_left = delim+1;
	 val_trim_right = (*pos)-1;
	 uint8_t aux = 0;
	 if (val_trim_right < val_trim_left) val_trim_right = val_trim_left;
	 if (val_trim_left <= val_trim_right)
	 {
		 aux = 1;
		 while ((line[val_trim_right] < 33) && (val_trim_left < val_trim_right)) val_trim_right--;
		 while ((line[val_trim_left] < 33) && (val_trim_left < val_trim_right)) val_trim_left++;
	 }
	 return allocate_param(section, &line[name_trim_left], name_trim_right - name_trim_left+1, &line[val_trim_left], val_trim_right-val_trim_left+aux);
 }

 INI_TextBlock_t* extract_text_block(INI_Section_t* section, uint8_t *line, uint16_t *pos, uint8_t type)
 {
	 uint16_t start = (*pos)+1;
	 while(line[*pos] && (line[*pos] != '\n')) (*pos)++;
	 return allocate_text_block(section, &line[start], (*pos)-start, type);
 }

 INI_Section_t* find_section(const char* section)
 {
	 if (list_is_empty(content)) return 0;
	 uint16_t len = strlen(section);
	 uint32_t hash = calc_hash((void*)section, len);

	 List_item_t* item = list_first(content);
	 INI_Section_t* res = (INI_Section_t*)item->data;
	 if(res->hash != hash) res = 0;
	 else
		 if (compare_str(res->name, section, len) != 0) res = 0;
	 while (item->next && !res)
	 {
		 item = item->next;
		 res = (INI_Section_t*)item->data;
		 if(res->hash != hash) res = 0;
		 else
			 if (compare_str(res->name, section, len) != 0) res = 0;
	 }
	 if (res)
		 if (res->hash == 0) res = 0;
	 return res;
 }

 INI_Parameter_t* find_param(INI_Section_t* section, const char * param)
 {
	 if (list_is_empty(section->content)) return 0;
	 uint16_t len = strlen(param);
	 uint32_t hash = calc_hash((void*)param, len);

	 List_item_t* item = list_first(section->content);
	 INI_Parameter_t* res = (INI_Parameter_t*)item->data;
	 if (res->hash != hash) res = 0;
	 else
		 if (compare_str(res->name, param, len) != 0) res = 0;
	 while (item->next && !res)
	 {
		 item = item->next;
		 res = (INI_Parameter_t*)item->data;
		 if (res->hash != hash) res = 0;
		 else
			 if (compare_str(res->name, param, len) != 0) res = 0;
	 }
	 if (res)
		 if (res->hash == 0) res = 0;
	 return res;
 }

 void store_section_content(FIL* file, INI_Section_t* section)
 {
	 List_item_t* item = list_first(section->content);
	 while (item)
	 {
		 char buf[INI_MAX_BUF_SIZE] = {0};
		 uint8_t* type = (uint8_t*)item->data;
		 switch (type[0])
		 {
			 case INI_CT_COMMENT:
			 case INI_CT_TEXT:
			 {
				 INI_TextBlock_t* t = (INI_TextBlock_t*)item->data;
				 if (t->hash)
				 {
					 memcpy(&buf[strlen(buf)], t->text, t->size);
					 if (type[0] == INI_CT_COMMENT) f_printf(file, ";%s\n", buf);
					 else f_printf(file, "%s", buf);
				 }
				 break;
			 }
			 case INI_CT_PARAM:
			 {
				 INI_Parameter_t* p = (INI_Parameter_t*)item->data;
				 if (p->hash)
				 {
					 memcpy(buf, p->name, p->name_len);
					 f_printf(file, "%s=", buf);
					 if(p->name_len > p->data_size) memset(buf, 0, p->name_len);
					 memcpy(buf, p->data, p->data_size);
					 f_printf(file, "%s\n", buf);
				 }
			 }
		 }
		 item = item->next;
	 }
 }

 uint8_t set_param_data(INI_Parameter_t* p, void* data, uint16_t len)
 {
	 if (p->data_size != len)
	 {
		 uint8_t* t = (uint8_t*)malloc(len);
		 if (!t) return 0;
		 if (p->data_size) free(p->data);
		 p->data = t;
		 p->data_size = len;
	 }
	 memcpy(p->data, data, len);
	 return 1;
 }

#endif //_FATFS

