#ifndef _LQOLED_H
#define _LQOLED_H
//#include "derivative.h"
#include "support_common.h"

typedef  unsigned char byte ;
typedef  unsigned int word ;
 //exUBrn uint8 longqiu96x64[768];
 void LCD_Init(void);
 void LCD_CLS(void);
 void LCD_P6x8Str(uint8 x,uint8 y,uint8 ch[]);
 void LCD_P8x16Str(uint8 x,uint8 y,uint8 ch[]);
 void LCD_P8x16Char(uint8 x,uint8 y,uint8 ch);
 void LCD_P14x16Str(uint8 x,uint8 y,uint8 ch[]);
 void LCD_Print(uint8 x, uint8 y, uint8 ch[]);
 void LCD_PutPixel(uint8 x,uint8 y);
 void LCD_Rectangle(uint8 x1,uint8 y1,uint8 x2,uint8 y2,uint8 gif);
 void Draw_LQLogo(void);
 void Draw_LibLogo(void);
 void Draw_BMP(uint8 x0,uint8 y0,uint8 x1,uint8 y1,uint8 bmp[]); 
 void LCD_Fill(uint8 dat);
 void LCD_DLY_ms(uint16 ms);


	typedef union
	{
		byte ByUC;
		struct 
			{unsigned char UC0:1;
			 unsigned char UC1:1;
			 unsigned char UC2:1;
			 unsigned char UC3:1;
			 unsigned char UC4:1;
			 unsigned char UC5:1;
			 unsigned char UC6:1;
			 unsigned char UC7:1;	 
			}Bits;
	}PORUCSTR;
	extern volatile PORUCSTR _PORTUC@(0X40100000);
		
	#define         PORTUC   _PORTUC.ByUC
    #define         UC0   _PORTUC.Bits.UC0
    #define         UC1   _PORTUC.Bits.UC1
    #define         UC2   _PORTUC.Bits.UC2
    #define         UC3   _PORTUC.Bits.UC3    
	    
	#define CLR_LCD_SCL MCF_GPIO_PORTUC&=0XF7  	//D0
	#define SET_LCD_SCL MCF_GPIO_PORTUC|=0X08  	//D0
	#define CLR_LCD_SDA	MCF_GPIO_PORTUC&=0XFB	//D1
	#define SET_LCD_SDA	MCF_GPIO_PORTUC|=0X04	//D1
	#define CLR_LCD_RST	MCF_GPIO_PORTUC&=0XFE		//RST
	#define SET_LCD_RST	MCF_GPIO_PORTUC|=0X01		//RST
	#define CLR_LCD_DC  MCF_GPIO_PORTUC&=0XFD		//DC
	#define SET_LCD_DC  MCF_GPIO_PORTUC|=0X02		//DC
	#define	LCD_IO_PAR 	MCF_GPIO_PUCPAR			//LCD“˝Ω≈≈‰÷√ºƒ¥Ê∆˜
	#define LCD_IO_DDR 	MCF_GPIO_DDRUC			//LCD“˝Ω≈ ‰≥ˆ≈‰÷√	    
	    
#endif