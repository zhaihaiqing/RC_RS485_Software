#include <stm32f10x_i2c.h>

extern void I2C1_Configuration(void);


extern void I2C1_ReadS_24C(unsigned short addr ,unsigned char * pBuffer,unsigned short Length);
extern unsigned char I2C1_WriteS_24C(unsigned short addr,unsigned char* pBuffer,  unsigned short Length);

extern void EEInit();
extern void EEWrite(unsigned short addr ,unsigned char * pBuffer,unsigned short Length);
extern void EERead(unsigned short addr,unsigned char* pBuffer,  unsigned short Length);
extern void EEErase(unsigned short addr, unsigned short Length);
extern unsigned char EECheck(void);
