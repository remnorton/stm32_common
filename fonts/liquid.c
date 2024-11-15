/*
 * liquid.c
 *
 *  Created on: 16 ����. 2019 �.
 *      Author: Rem Norton
 */

#include "fonts_lib.h"

/*
* Font data for 'Liquid Crystal 26pt'
*/
uint8_t Liquid_Crystal_26pt_Bitmaps[]=
{
/*Bitmap for char '0' (48)*/
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x3f,0xf0,0x00,		//  ##########
	0x7f,0xf8,0x00,		// ############
	0xff,0xfc,0x00,		//##############
	0xc0,0x1e,0x00,		//##         ####
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0c,0x00,		//##          ##
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0xc0,0x0c,0x00,		//##          ##
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x1e,0x00,		//##         ####
	0xff,0xfc,0x00,		//##############
	0x7f,0xf8,0x00,		// ############
	0x3f,0xf0,0x00,		//  ##########
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//

/*Bitmap for char '1' (49)*/
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x40,0x00,		//         #
	0x00,0xe0,0x00,		//        ###
	0x00,0xe0,0x00,		//        ###
	0x02,0xe0,0x00,		//      # ###
	0x06,0xe0,0x00,		//     ## ###
	0x0e,0xe0,0x00,		//    ### ###
	0x0e,0xe0,0x00,		//    ### ###
	0x0e,0xe0,0x00,		//    ### ###
	0x04,0xe0,0x00,		//     #  ###
	0x00,0xe0,0x00,		//        ###
	0x00,0xe0,0x00,		//        ###
	0x00,0xe0,0x00,		//        ###
	0x00,0xc0,0x00,		//        ##
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0xc0,0x00,		//        ##
	0x00,0xe0,0x00,		//        ###
	0x00,0xe0,0x00,		//        ###
	0x00,0xe0,0x00,		//        ###
	0x00,0xe0,0x00,		//        ###
	0x00,0xe0,0x00,		//        ###
	0x00,0xe0,0x00,		//        ###
	0x00,0xe0,0x00,		//        ###
	0x00,0xe0,0x00,		//        ###
	0x00,0xe0,0x00,		//        ###
	0x00,0xe0,0x00,		//        ###
	0x00,0xe0,0x00,		//        ###
	0x00,0x40,0x00,		//         #
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//

/*Bitmap for char '2' (50)*/
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x3f,0xf0,0x00,		//  ##########
	0x7f,0xf8,0x00,		// ############
	0xff,0xfc,0x00,		//##############
	0xc0,0x1e,0x00,		//##         ####
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x3f,0xfc,0x00,		//  ############
	0x7f,0xf8,0x00,		// ############
	0xff,0xf0,0x00,		//############
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xff,0xf0,0x00,		//############
	0xff,0xf8,0x00,		//#############
	0xff,0xfc,0x00,		//##############
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//

/*Bitmap for char '3' (51)*/
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x7f,0xf0,0x00,		// ###########
	0xff,0xf8,0x00,		//#############
	0x7f,0xf4,0x00,		// ########### #
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x1f,0xfc,0x00,		//   ###########
	0x3f,0xf8,0x00,		//  ###########
	0x1f,0xfc,0x00,		//   ###########
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x7f,0xfc,0x00,		// #############
	0xff,0xf8,0x00,		//#############
	0x7f,0xf0,0x00,		// ###########
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//

/*Bitmap for char '4' (52)*/
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x04,0x00,		//             #
	0x00,0x0e,0x00,		//            ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xff,0xfc,0x00,		//##############
	0x7f,0xf8,0x00,		// ############
	0x3f,0xfc,0x00,		//  ############
	0x00,0x1e,0x00,		//           ####
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x04,0x00,		//             #
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//

/*Bitmap for char '5' (53)*/
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0xff,0xfc,0x00,		//##############
	0xff,0xf8,0x00,		//#############
	0xff,0xf0,0x00,		//############
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xff,0xf0,0x00,		//############
	0x7f,0xf8,0x00,		// ############
	0x3f,0xfc,0x00,		//  ############
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xff,0xfc,0x00,		//##############
	0x7f,0xf8,0x00,		// ############
	0x3f,0xf0,0x00,		//  ##########
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//

/*Bitmap for char '6' (54)*/
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x3f,0xf0,0x00,		//  ##########
	0x7f,0xf8,0x00,		// ############
	0xff,0xf0,0x00,		//############
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xc0,0x00,0x00,		//##
	0xff,0xf0,0x00,		//############
	0x7f,0xf8,0x00,		// ############
	0xff,0xfc,0x00,		//##############
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xff,0xfc,0x00,		//##############
	0x7f,0xf8,0x00,		// ############
	0x3f,0xf0,0x00,		//  ##########
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//

/*Bitmap for char '7' (55)*/
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x7f,0xfc,0x00,		// #############
	0xff,0xfe,0x00,		//###############
	0xff,0xfc,0x00,		//##############
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x08,0x00,		//            #
	0x00,0x08,0x00,		//            #
	0x00,0x18,0x00,		//           ##
	0x00,0x38,0x00,		//          ###
	0x00,0x38,0x00,		//          ###
	0x00,0x78,0x00,		//         ####
	0x00,0x70,0x00,		//         ###
	0x00,0x70,0x00,		//         ###
	0x00,0x60,0x00,		//         ##
	0x01,0x80,0x00,		//       ##
	0x03,0x80,0x00,		//      ###
	0x03,0x80,0x00,		//      ###
	0x07,0x80,0x00,		//     ####
	0x07,0x80,0x00,		//     ####
	0x0f,0x00,0x00,		//    ####
	0x0e,0x00,0x00,		//    ###
	0x1e,0x00,0x00,		//   ####
	0x1c,0x00,0x00,		//   ###
	0x3c,0x00,0x00,		//  ####
	0x38,0x00,0x00,		//  ###
	0x38,0x00,0x00,		//  ###
	0x30,0x00,0x00,		//  ##
	0x30,0x00,0x00,		//  ##
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//

/*Bitmap for char '8' (56)*/
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x3f,0xf0,0x00,		//  ##########
	0x7f,0xf8,0x00,		// ############
	0xff,0xfc,0x00,		//##############
	0xc0,0x1e,0x00,		//##         ####
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xff,0xfc,0x00,		//##############
	0x7f,0xf8,0x00,		// ############
	0xff,0xfc,0x00,		//##############
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xff,0xfc,0x00,		//##############
	0x7f,0xf8,0x00,		// ############
	0x3f,0xf0,0x00,		//  ##########
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//

/*Bitmap for char '9' (57)*/
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//
	0x3f,0xf0,0x00,		//  ##########
	0x7f,0xf8,0x00,		// ############
	0xff,0xfc,0x00,		//##############
	0xc0,0x1e,0x00,		//##         ####
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xc0,0x0e,0x00,		//##          ###
	0xff,0xfe,0x00,		//###############
	0x7f,0xfe,0x00,		// ##############
	0x3f,0xfe,0x00,		//  #############
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x3f,0xfc,0x00,		//  ############
	0x7f,0xf8,0x00,		// ############
	0x3f,0xf0,0x00,		//  ##########
	0x00,0x00,0x00,		//
	0x00,0x00,0x00,		//

/*Bitmap for char ':' (58)*/
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x60,				// ##
	0xe0,				//###
	0xe0,				//###
	0x60,				// ##
	0x00,				//
	0x00,				//
	0x00,				//
	0x00,				//
	0x60,				// ##
	0xe0,				//###
	0xe0,				//###
	0xe0,				//###
	0x40,				// #
	0x00,				//
	0x00,				//

}; //Local 8-bit codepage bitmaps



Font_type_t font_Liquid_Crystal_26pt  =
{
	"Liquid Cry~ 26pt", /*name*/
	Liquid_Crystal_26pt_Bitmaps, /*bitmaps*/
	0, /*descr*/
	18, /*width*/
	35, /*height*/
	48, /*start*/
	58, /*stop*/
	0, /*spacing*/
};
