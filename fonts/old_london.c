/*
 * old_london.c
 *
 *  Created on: 6 ���. 2019 �.
 *      Author: User
 */
#include"fonts_lib.h"

uint8_t Old_London_40pt_Bitmaps[]=
{
/*Bitmap for char '0' (48)*/
	0x00,0x20,0x00,		//          #
	0x00,0xf8,0x00,		//        #####
	0x01,0xfc,0x00,		//       #######
	0x03,0xff,0x00,		//      ##########
	0x07,0xff,0x80,		//     ############
	0x0c,0x3f,0xc0,		//    ##    ########
	0x08,0x1f,0xc0,		//    #      #######
	0x18,0x07,0xe0,		//   ##        ######
	0x30,0x03,0xf0,		//  ##          ######
	0x30,0x01,0xf0,		//  ##           #####
	0x70,0x00,0xf0,		// ###            ####
	0x60,0x00,0xf8,		// ##             #####
	0x60,0x00,0x78,		// ##              ####
	0xe0,0x00,0x78,		//###              ####
	0xe0,0x00,0x78,		//###              ####
	0xe0,0x00,0x38,		//###               ###
	0xe0,0x00,0x38,		//###               ###
	0xe0,0x00,0x38,		//###               ###
	0xf0,0x00,0x38,		//####              ###
	0xf0,0x00,0x38,		//####              ###
	0xf0,0x00,0x30,		//####              ##
	0xf8,0x00,0x30,		//#####             ##
	0x7c,0x00,0x30,		// #####            ##
	0x7c,0x00,0x60,		// #####           ##
	0x7e,0x00,0x60,		// ######          ##
	0x3f,0x00,0x40,		//  ######         #
	0x3f,0x80,0x80,		//  #######       #
	0x1f,0xe1,0x80,		//   ########    ##
	0x0f,0xfd,0x00,		//    ########## #
	0x07,0xfe,0x00,		//     ##########
	0x03,0xfc,0x00,		//      ########
	0x00,0xf8,0x00,		//        #####
	0x00,0x30,0x00,		//          ##

/*Bitmap for char '1' (49)*/
	0x04,0x00,			//     #
	0x0e,0x00,			//    ###
	0x1e,0x00,			//   ####
	0x3f,0x00,			//  ######
	0x5f,0x00,			// # #####
	0x8f,0x00,			//#   ####
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x00,			//     ###
	0x07,0x10,			//     ###   #
	0x0f,0xd8,			//    ###### ##
	0x0f,0xe0,			//    #######
	0x07,0xc0,			//     #####
	0x03,0x80,			//      ###
	0x01,0x00,			//       #

/*Bitmap for char '2' (50)*/
	0x00,0x60,0x00,		//         ##
	0x00,0xf0,0x00,		//        ####
	0x01,0xfc,0x00,		//       #######
	0x03,0xfe,0x00,		//      #########
	0x07,0xf3,0x80,		//     #######  ###
	0x1f,0xe1,0xc0,		//   ########    ###
	0x3f,0xc1,0xf0,		//  ########     #####
	0x7f,0x00,0xf8,		// #######        #####
	0x3e,0x00,0xf8,		//  #####         #####
	0x1c,0x00,0xf8,		//   ###          #####
	0x08,0x00,0xf8,		//    #           #####
	0x00,0x00,0xf8,		//                #####
	0x00,0x00,0xf8,		//                #####
	0x00,0x01,0xf0,		//               #####
	0x00,0x01,0xf0,		//               #####
	0x00,0x03,0xe0,		//              #####
	0x00,0x03,0xe0,		//              #####
	0x00,0x07,0xc0,		//             #####
	0x00,0x0f,0x80,		//            #####
	0x00,0x1f,0x00,		//           #####
	0x00,0x3c,0x00,		//          ####
	0x00,0x78,0x00,		//         ####
	0x00,0xe0,0x00,		//        ###
	0x01,0xc0,0x00,		//       ###
	0x03,0x00,0x00,		//      ##
	0x06,0x00,0x00,		//     ##
	0x0c,0x00,0x00,		//    ##
	0x18,0x7f,0xc0,		//   ##    #########
	0x33,0xff,0xf8,		//  ##  ###############
	0x3f,0xff,0xf8,		//  ###################
	0x7f,0xff,0xf0,		// ###################
	0x7f,0xff,0xe0,		// ##################
	0xf8,0x01,0xc0,		//#####          ###
	0x80,0x00,0x00,		//#

/*Bitmap for char '3' (51)*/
	0x00,0x80,0x00,		//        #
	0x01,0xe0,0x00,		//       ####
	0x07,0xf8,0x00,		//     ########
	0x0f,0xe6,0x00,		//    #######  ##
	0x1f,0xc3,0x80,		//   #######    ###
	0x7f,0x03,0xe0,		// #######      #####
	0x3e,0x03,0xe0,		//  #####       #####
	0x1c,0x03,0xe0,		//   ###        #####
	0x10,0x03,0xe0,		//   #          #####
	0x00,0x03,0xe0,		//              #####
	0x00,0x07,0xc0,		//             #####
	0x00,0x07,0xc0,		//             #####
	0x00,0x0f,0x80,		//            #####
	0x00,0x3f,0x00,		//          ######
	0x00,0xfc,0x00,		//        ######
	0x07,0xf8,0x00,		//     ########
	0x00,0x3e,0x00,		//          #####
	0x00,0x0f,0x80,		//            #####
	0x00,0x07,0xe0,		//             ######
	0x00,0x03,0xf0,		//              ######
	0x00,0x01,0xf0,		//               #####
	0x00,0x01,0xf8,		//               ######
	0x20,0x00,0xf8,		//  #             #####
	0x30,0x00,0xf8,		//  ##            #####
	0x7c,0x00,0xf8,		// #####          #####
	0xfe,0x00,0xf8,		//#######         #####
	0x7f,0x00,0xf8,		// #######        #####
	0x3f,0x81,0xf0,		//  #######      #####
	0x1f,0xc1,0xe0,		//   #######     ####
	0x0f,0xe1,0xc0,		//    #######    ###
	0x07,0xfb,0x80,		//     ######## ###
	0x03,0xfe,0x00,		//      #########
	0x00,0xf0,0x00,		//        ####

/*Bitmap for char '4' (52)*/
	0x00,0x08,0x00,		//            #
	0x00,0x0c,0x00,		//            ##
	0x00,0x1c,0x00,		//           ###
	0x00,0x3e,0x00,		//          #####
	0x00,0x3e,0x00,		//          #####
	0x00,0x7e,0x00,		//         ######
	0x00,0xce,0x00,		//        ##  ###
	0x00,0x8e,0x00,		//        #   ###
	0x01,0x8e,0x00,		//       ##   ###
	0x03,0x0e,0x00,		//      ##    ###
	0x02,0x0e,0x00,		//      #     ###
	0x04,0x0e,0x00,		//     #      ###
	0x0c,0x0e,0x00,		//    ##      ###
	0x08,0x0e,0x00,		//    #       ###
	0x10,0x0e,0x00,		//   #        ###
	0x20,0x0e,0x00,		//  #         ###
	0x20,0x0e,0x00,		//  #         ###
	0x40,0x0e,0x00,		// #          ###
	0x80,0x0e,0x00,		//#           ###
	0xff,0xff,0xf0,		//####################
	0xff,0xff,0xf0,		//####################
	0x7f,0xff,0xf0,		// ###################
	0x7f,0xff,0xf0,		// ###################
	0x7f,0xff,0xf0,		// ###################
	0x00,0x0e,0x08,		//            ###     #
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x0e,0x00,		//            ###
	0x00,0x1e,0x00,		//           ####
	0x00,0x1f,0xe0,		//           ########
	0x00,0x3f,0xc0,		//          ########
	0x00,0x0f,0x00,		//            ####
	0x00,0x02,0x00,		//              #

/*Bitmap for char '5' (53)*/
	0x07,0xff,0xe0,		//     ##############
	0x07,0xff,0xc0,		//     #############
	0x0f,0xff,0xc0,		//    ##############
	0x0f,0xff,0xc0,		//    ##############
	0x0f,0xff,0x80,		//    #############
	0x08,0x00,0x00,		//    #
	0x08,0x00,0x00,		//    #
	0x08,0x00,0x00,		//    #
	0x08,0x00,0x00,		//    #
	0x18,0x08,0x00,		//   ##       #
	0x18,0x1e,0x00,		//   ##      ####
	0x10,0x3f,0x80,		//   #      #######
	0x10,0xff,0xc0,		//   #    ##########
	0x11,0x9f,0xe0,		//   #   ##  ########
	0x16,0x0f,0xe0,		//   # ##     #######
	0x3c,0x03,0xf0,		//  ####        ######
	0x30,0x01,0xf0,		//  ##           #####
	0x40,0x00,0xf0,		// #              ####
	0x00,0x00,0xf8,		//                #####
	0x00,0x00,0x78,		//                 ####
	0x00,0x00,0x78,		//                 ####
	0x00,0x00,0x70,		//                 ###
	0x00,0x00,0x70,		//                 ###
	0x10,0x00,0x70,		//   #             ###
	0x38,0x00,0x70,		//  ###            ###
	0x7e,0x00,0xe0,		// ######         ###
	0xff,0x80,0xe0,		//#########       ###
	0x3f,0xc1,0xc0,		//  ########     ###
	0x1f,0xf1,0x80,		//   #########   ##
	0x07,0xff,0x00,		//     ###########
	0x01,0xfc,0x00,		//       #######
	0x00,0xe0,0x00,		//        ###

/*Bitmap for char '6' (54)*/
	0x00,0x60,0x00,		//         ##
	0x00,0xf8,0x00,		//        #####
	0x01,0xfe,0x00,		//       ########
	0x03,0xff,0x80,		//      ###########
	0x06,0xff,0xc0,		//     ## ##########
	0x04,0x3f,0x80,		//     #    #######
	0x0c,0x0f,0x00,		//    ##      ####
	0x18,0x03,0x00,		//   ##         ##
	0x18,0x00,0x00,		//   ##
	0x30,0x00,0x00,		//  ##
	0x30,0x00,0x00,		//  ##
	0x70,0x07,0x00,		// ###         ###
	0x60,0x0f,0x80,		// ##         #####
	0x60,0x3f,0xc0,		// ##       ########
	0xe0,0x7f,0xe0,		//###      ##########
	0xe1,0xe7,0xf0,		//###    ####  #######
	0xe7,0x81,0xf0,		//###  ####      #####
	0xef,0x00,0xf8,		//### ####        #####
	0xfe,0x00,0xf8,		//#######         #####
	0xf8,0x00,0x78,		//#####            ####
	0xfc,0x00,0x38,		//######            ###
	0xfc,0x00,0x38,		//######            ###
	0xfe,0x00,0x38,		//#######           ###
	0x7e,0x00,0x30,		// ######           ##
	0x7f,0x00,0x30,		// #######          ##
	0x7f,0x00,0x20,		// #######          #
	0x3f,0xc0,0x40,		//  ########       #
	0x1f,0xe0,0xc0,		//   ########     ##
	0x0f,0xf9,0x80,		//    #########  ##
	0x07,0xff,0x00,		//     ###########
	0x03,0xfc,0x00,		//      ########
	0x01,0xf8,0x00,		//       ######
	0x00,0x30,0x00,		//          ##

/*Bitmap for char '7' (55)*/
	0x1f,0xff,0xf0,		//   #################
	0x1f,0xff,0xf0,		//   #################
	0x1f,0xff,0xe0,		//   ################
	0x3f,0xff,0xe0,		//  #################
	0x3f,0xff,0xc0,		//  ################
	0x70,0x00,0x80,		// ###            #
	0x60,0x01,0x00,		// ##            #
	0xc0,0x03,0x00,		//##            ##
	0x80,0x06,0x00,		//#            ##
	0x00,0x0c,0x00,		//            ##
	0x00,0x0c,0x00,		//            ##
	0x00,0x18,0x00,		//           ##
	0x00,0x38,0x00,		//          ###
	0x00,0x30,0x00,		//          ##
	0x00,0x70,0x00,		//         ###
	0x00,0xf0,0x00,		//        ####
	0x00,0xe0,0x00,		//        ###
	0x01,0xe0,0x00,		//       ####
	0x01,0xe0,0x00,		//       ####
	0x03,0xc0,0x00,		//      ####
	0x03,0xc0,0x00,		//      ####
	0x07,0xc0,0x00,		//     #####
	0x07,0xc0,0x00,		//     #####
	0x07,0x80,0x00,		//     ####
	0x0f,0x80,0x00,		//    #####
	0x0f,0x80,0x00,		//    #####
	0x0f,0x80,0x00,		//    #####
	0x0f,0x80,0x00,		//    #####
	0x1f,0x80,0x00,		//   ######
	0x1f,0x80,0x00,		//   ######
	0x1e,0x00,0x00,		//   ####
	0x10,0x00,0x00,		//   #

/*Bitmap for char '8' (56)*/
	0x00,0xf0,0x00,		//        ####
	0x01,0xfe,0x00,		//       ########
	0x03,0xff,0x80,		//      ###########
	0x07,0xff,0xc0,		//     #############
	0x0f,0xff,0xe0,		//    ###############
	0x1c,0x0f,0xf0,		//   ###      ########
	0x18,0x01,0xf0,		//   ##          #####
	0x30,0x00,0xf0,		//  ##            ####
	0x30,0x00,0x70,		//  ##             ###
	0x70,0x00,0x60,		// ###             ##
	0x70,0x00,0x60,		// ###             ##
	0x78,0x00,0xc0,		// ####           ##
	0x7e,0x01,0x00,		// ######        #
	0x7f,0xf2,0x00,		// ###########  #
	0x3f,0xff,0x80,		//  ###############
	0x3f,0xff,0xe0,		//  #################
	0x0f,0xff,0xf0,		//    ################
	0x08,0x3f,0xf0,		//    #     ##########
	0x10,0x03,0xf8,		//   #          #######
	0x10,0x00,0xf8,		//   #            #####
	0x20,0x00,0x78,		//  #              ####
	0x60,0x00,0x38,		// ##               ###
	0x60,0x00,0x38,		// ##               ###
	0xe0,0x00,0x38,		//###               ###
	0xf0,0x00,0x30,		//####              ##
	0xf0,0x00,0x70,		//####             ###
	0xfc,0x00,0xe0,		//######          ###
	0xff,0x03,0xc0,		//########      ####
	0x7f,0xff,0x80,		// ################
	0x3f,0xff,0x00,		//  ##############
	0x3f,0xfe,0x00,		//  #############
	0x0f,0xfc,0x00,		//    ##########
	0x03,0xf0,0x00,		//      ######

/*Bitmap for char '9' (57)*/
	0x00,0x60,0x00,		//         ##
	0x00,0xfc,0x00,		//        ######
	0x01,0xfe,0x00,		//       ########
	0x07,0xff,0x80,		//     ############
	0x0f,0xff,0xc0,		//    ##############
	0x18,0x7f,0xc0,		//   ##    #########
	0x10,0x1f,0xe0,		//   #       ########
	0x30,0x0f,0xf0,		//  ##        ########
	0x60,0x07,0xf0,		// ##          #######
	0x60,0x03,0xf0,		// ##           ######
	0xe0,0x03,0xf8,		//###           #######
	0xe0,0x01,0xf8,		//###            ######
	0xe0,0x01,0xf8,		//###            ######
	0xf0,0x00,0xf8,		//####            #####
	0xf0,0x01,0xf8,		//####           ######
	0xf8,0x03,0xf8,		//#####         #######
	0xfc,0x06,0x78,		//######       ##  ####
	0x7e,0x18,0x78,		// ######    ##    ####
	0x7f,0xf0,0x78,		// ###########     ####
	0x3f,0xc0,0xf0,		//  ########      ####
	0x1f,0x80,0xf0,		//   ######       ####
	0x06,0x00,0xf0,		//     ##         ####
	0x00,0x00,0xe0,		//                ###
	0x00,0x00,0xe0,		//                ###
	0x00,0x01,0xc0,		//               ###
	0x0c,0x01,0x80,		//    ##         ##
	0x1f,0x03,0x80,		//   #####      ###
	0x1f,0xc7,0x00,		//   #######   ###
	0x3f,0xf6,0x00,		//  ########## ##
	0x5f,0xfc,0x00,		// # ###########
	0x07,0xf8,0x00,		//     ########
	0x01,0xf0,0x00,		//       #####
	0x00,0x40,0x00,		//         #

}; //Local 8-bit codepage bitmaps

Font_Char_info_t Old_London_40pt_Descriptors[]=
{
	{0, 21, 33, 1},	//Descriptor for '0' (48)
	{99, 13, 33, 1},	//Descriptor for '1' (49)
	{165, 21, 34, 0},	//Descriptor for '2' (50)
	{267, 21, 33, 1},	//Descriptor for '3' (51)
	{366, 21, 33, 1},	//Descriptor for '4' (52)
	{465, 21, 32, 1},	//Descriptor for '5' (53)
	{561, 21, 33, 1},	//Descriptor for '6' (54)
	{660, 20, 32, 1},	//Descriptor for '7' (55)
	{756, 21, 33, 1},	//Descriptor for '8' (56)
	{855, 21, 33, 1},	//Descriptor for '9' (57)
}; //Local 8-bit codepage table descriptors

Font_type_t font_Old_London_40pt  =
{
	"Old London 40pt", /*name*/
	Old_London_40pt_Bitmaps, /*bitmaps*/
	Old_London_40pt_Descriptors, /*descr*/
	0, /*width*/
	34, /*height*/
	48, /*start*/
	57, /*stop*/
	1, /*spacing*/
};
