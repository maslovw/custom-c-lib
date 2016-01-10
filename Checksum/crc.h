#include <stdint.h>
#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif 

	#ifdef EMU
		#define CRC32_SW	1
		// на ARM'е используем аппаратный CRC
	#endif


	uint16_t update_crc_16 (uint16_t crc, char data);
	uint16_t crc16_calc(uint16_t crc, unsigned char * buff, int len);


	uint8_t GetLRC (const void* apBuff, size_t len);
	// CRC для пенала Скаут
	uint8_t Scout_update_crc8 (uint8_t crc, uint8_t data);
	// CRC для нашего пенала
	uint8_t update_crc8 (uint8_t crc, uint8_t data);

	typedef unsigned char TCrc8;
	typedef unsigned short TCrc16;
	typedef unsigned long TCrc32;

	TCrc32 Crc32Reset(void);
	TCrc32 Crc32CalcBlock (const void *apData, size_t aLength);

#ifdef __cplusplus
}
#endif 
