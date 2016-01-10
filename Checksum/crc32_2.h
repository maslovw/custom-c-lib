#ifndef CRC_32_H
	#define CRC_32_H
#ifdef  __cplusplus
extern  "C" {
#endif
	
#define uLong unsigned int
#define uLongf unsigned int
#define Bytef unsigned char
#define uInt unsigned int

#define ZEXPORT

#define Z_NULL 0L

uLong crc32(uLong crc, Bytef *buf, uInt len);
unsigned int eval_crc(char *b, int len);
#ifdef  __cplusplus
}
#endif
#endif /* CRC_32_H */

