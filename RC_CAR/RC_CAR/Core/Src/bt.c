
#include "bt.h"

uint8_t rxData[10];

volatile uint8_t Flag1;
volatile uint8_t Flag2;
volatile uint8_t Flag3;
volatile uint8_t Flag4;

volatile uint8_t isAutoMode = 0;

void Check()
{
	  if(rxData[0] =='g')
	  {
		 isAutoMode = 1;
		 rxData[0]=0;
	  }
	  if(rxData[0] =='e')
	  {
		 isAutoMode = 0;
		 rxData[0]=0;
	  }
		  if(rxData[0] =='w')
		  {
			 Flag1= 1;
			 rxData[0]=0;
		  }
		if(rxData[0] == 's')
		  {
			Flag2= 1;
			rxData[0]=0;
		  }
		if(rxData[0] == 'a')
		  {
			Flag3= 1;
			rxData[0]=0;
		  }
		if(rxData[0] == 'd')
		  {
			Flag4= 1;
			rxData[0]=0;
		  }
}
