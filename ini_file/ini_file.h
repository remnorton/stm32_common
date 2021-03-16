/*
 * ini_file.h
 *
 *  Created on: 22 θών. 2020 γ.
 *      Author: Rem Norton
 */

#ifndef INI_FILE_INI_FILE_H_
#define INI_FILE_INI_FILE_H_

#include "board.h"
#include "list.h"
#include "ini_types.h"

uint8_t	ini_init();

uint8_t ini_load(const char * file_name);
uint8_t ini_store(const char * file_name);

uint8_t ini_get_param_c(const char * section, const char * param, int max_buf_size, int * fact_count, char * buf);
uint8_t ini_get_param_i(const char * section, const char * param, int need_count, int * fact_count, int * buf);


uint8_t ini_set_param_c(const char * section, const char * param, const char * buf);
uint8_t ini_set_param_i(const char * section, const char * param, int count, const int * buf);

#if (INI_ALLOW_DOUBLE == 1)
uint8_t ini_get_param_d(const char * section, const char * param, int need_count, int * fact_count, double * buf);
uint8_t ini_set_param_d(const char * section, const char * param, int count, const double * buf);
#endif

#endif /* INI_FILE_INI_FILE_H_ */
