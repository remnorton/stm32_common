/*
 * xpt2046.c
 *
 *  Created on: 26 мая 2019 г.
 *      Author: Rem Norton
 */

#include "xpt2046.h"

#define  CHX   0x90
#define  CHY   0xd0

#define THRESHOLD 2

static XP2048_Instance_t* instance = 0;

void DelayUS(uint32_t cnt)
{
  uint32_t i;
  i = cnt * 4;
  while(i--);
}

void WR_CMD (XP2048_Instance_t* instance, uint8_t cmd)
{ 
  HAL_SPI_Transmit(instance->spi,&cmd,1,1000);
} 

int RD_AD(XP2048_Instance_t* instance)
{ 
  uint8_t buf[2];
  int value;
  HAL_SPI_Receive(instance->spi, buf, 2, 100);
  value = (uint16_t)((buf[0] << 8) + buf[1]) >> 3;
  return value;
} 

int Read_X(XP2048_Instance_t* instance)
{  
  int i; 
  HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
  DelayUS(1); 
  WR_CMD(instance, CHX);
  DelayUS(1); 
  i=RD_AD(instance);
  HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, !instance->cs.activeState);
  return i;    
} 

int Read_Y(XP2048_Instance_t* instance)
{  
  int i; 
  HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
  DelayUS(1); 
  WR_CMD(instance, CHY);
  DelayUS(1); 
  i=RD_AD(instance);
  HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, !instance->cs.activeState);
  return i;     
} 

void TP_GetAdXY(XP2048_Instance_t* instance, int *x,int *y)
{ 
  int adx,ady; 
  //DelayUS(1);
  adx=Read_X(instance);
  DelayUS(1); 
  ady=Read_Y(instance);
  *x=adx; 
  *y=ady; 
} 
  
uint8_t Read_Ads7846(XP2048_Instance_t* instance, TP_POINT_t* screen)
{
  int m0,m1,m2,TP_X[1],TP_Y[1],temp[3];
  uint8_t count=0;
  int buffer[2][9]={{0},{0}};
  
  do
  {       
    TP_GetAdXY(instance,TP_X,TP_Y);
    buffer[0][count]=TP_X[0];  
    buffer[1][count]=TP_Y[0];
    count++;  
  }
  while((HAL_GPIO_ReadPin(instance->irq.port, instance->irq.pin) == instance->irq.activeState) && (count < 9));

  if(count==9)   /* Average X Y  */ 
  {
    /* Average X  */
    temp[0]=(buffer[0][0]+buffer[0][1]+buffer[0][2])/3;
    temp[1]=(buffer[0][3]+buffer[0][4]+buffer[0][5])/3;
    temp[2]=(buffer[0][6]+buffer[0][7]+buffer[0][8])/3;
   
    m0=temp[0]-temp[1];
    m1=temp[1]-temp[2];
    m2=temp[2]-temp[0];
   
    m0=m0>0?m0:(-m0);
    m1=m1>0?m1:(-m1);
    m2=m2>0?m2:(-m2);
   
    if( m0>THRESHOLD  &&  m1>THRESHOLD  &&  m2>THRESHOLD ) return 0;
   
    if(m0<m1)
    {
      if(m2<m0) 
        screen->x=(temp[0]+temp[2])/2;
      else 
        screen->x=(temp[0]+temp[1])/2;
    }
    else if(m2<m1) 
      screen->x=(temp[0]+temp[2])/2;
    else 
      screen->x=(temp[1]+temp[2])/2;
   
    /* Average Y  */
    temp[0]=(buffer[1][0]+buffer[1][1]+buffer[1][2])/3;
    temp[1]=(buffer[1][3]+buffer[1][4]+buffer[1][5])/3;
    temp[2]=(buffer[1][6]+buffer[1][7]+buffer[1][8])/3;
    m0=temp[0]-temp[1];
    m1=temp[1]-temp[2];
    m2=temp[2]-temp[0];
    m0=m0>0?m0:(-m0);
    m1=m1>0?m1:(-m1);
    m2=m2>0?m2:(-m2);
    if(m0>THRESHOLD&&m1>THRESHOLD&&m2>THRESHOLD) return 0;
   
    if(m0<m1)
    {
      if(m2<m0) 
        screen->y=(temp[0]+temp[2])/2;
      else 
        screen->y=(temp[0]+temp[1])/2;
      }
    else if(m2<m1) 
       screen->y=(temp[0]+temp[2])/2;
    else
       screen->y=(temp[1]+temp[2])/2;
   
    return 1;
  }
  return 0; 
}

//
//Public members
//
void tp_init(XP2048_Instance_t* init)
{
	instance = init;
}

uint8_t tp_getPoint(TP_POINT_t* point)
{
	if (!instance) return 0;
	if (!Read_Ads7846(instance, point)) return 0;
	return 1;
}
