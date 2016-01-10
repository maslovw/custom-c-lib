#ifndef RingLog_h__
#define RingLog_h__

/************************************************************************/
// * http://cri.atol.ru/confluence/display/ATOLDrive/RingLog
// * RingLog ��� ������ ��� ����������� ����������� � ����� ����������� 
// * ������ � ��������� ������. 
// * 
// * ������������ ��������� �����.
// * � ������ ����������� ������ (RingBuff) ����� ��������� (OldestRec,
// * NewestRec). ��������� ��������� (�.�. ���� - ������ ������ � ������ 
// * ���������).
// * �.�. ������ ����� ������ �����, �� ��� ��������������� ���������� ����� 
// * ������� ������, �� ������� �� ��������� ����� �����������.  
/************************************************************************/
#include "stdint.h"

#define RING_BUFF_SIZE 1024
#define MAX_TASK_NAME 4

#ifdef __cplusplus
extern "C" {
#endif

	extern char lmSaveLogExtStr[];

#	ifdef ARM
	typedef enum TLogRecType{
		LRT_Text = 0,
		LRT_HardFaultData,
	}TLogRecType;
#	endif

#	ifdef EMU
#		define LRT_Text 0
#		define LRT_HardFaultData 1
		typedef unsigned char TLogRecType;
#	endif

#	pragma pack(push, 1)
		typedef unsigned short TOffset;

		typedef struct {
			TOffset Oldest;
			TOffset Teil;
		} TRingLogHeader;

		typedef struct {
			TRingLogHeader *Hdr;
			unsigned char *Buff;
			unsigned int BuffSize;
			const uint32_t *pTimeNow;
		} TRingLog;

		typedef struct {
			TRingLogHeader Hdr;
			unsigned char Data[RING_BUFF_SIZE];
		} TRingLogBuffer;


		typedef struct {
			TLogRecType Type;
			unsigned short Len;
			unsigned int DateTime;
			char TaskName[MAX_TASK_NAME];
			unsigned int Fid;
			unsigned int Line;
			unsigned int StoreId;
		} TRingLogRecHdr;
#	pragma pack(pop)

	void RingLogInit (TRingLog *apLog, void *Buff, unsigned int  BuffSize, const uint32_t *pTimeNow);
	//���������� � ��������� �����
	void RingLogAppend (TRingLog *apLog,
						unsigned int aFid,
						unsigned int aLine,
						const void *const apData, unsigned int  aDataSize,
						enum TLogRecType aType);

	//������� ������
	void RingLogClear (void);
	void ReadRing (TRingLog *apLog, TOffset aOffset, void *apData, unsigned int  aDataSize);

#if UNIT_TESTS
	void RingLogTest (void);  
#endif // UNIT_TESTS

#ifdef __cplusplus
}
#endif

#endif // RingLog_h__



