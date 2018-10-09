#ifndef MEMORYMANAGEMENT_H
#define MEMORYMANAGEMENT_H
//typedef enum bools{ false, true }bool;
typedef struct {
	bool isAvailable;              /* �Ƿ�ռ�� */
	int PID;					   /* ռ�����Ľ���ID */
	int pageNum;                   /* ռ�����Ľ��̵ĵڼ�ҳ */
	int FIFOcount;                 /* FIFO������ */
	int LRUcount;                  /* LRU�㷨������ */
    bool isDirty;                  /* ��λ����ʾ�Ƿ��޸� */
}Frame;

typedef struct {
	bool isAvailable;              /* �Ƿ���Ч */
	int PID;
	int frameNum;                   /* ��ҳռ�õ�֡�� */
}Page, *PageTable;


bool initializeMemory();          /* ��ʼ���ڴ� */
void showFrameTable();            /* չʾ֡�� */
int VMToMemory(int __PID, int __pageNum);
bool MemoryToVM(int __frameNum);
bool allocateMemory(int __PID, int __frameNeed, int __hasUsed);     /* �����ڴ� */
void withdrawMemory(int __PID);            /* �ջ��ڴ� */
bool readMemoryByChar(int __pid, int __frameNum, int __offsetIn, unsigned char * __toRead);  /* ���ڴ� */
bool writeMemoryByChar(int __pid, int __frameNum, int __offsetIn, const unsigned char __toSave); /* д�ڴ� */

/*FIFO�㷨��ҳ*/
void FIFOUse(int __frameNum, bool isFrameAvailable);
bool withDrawFIFO(int __frameNum);
int FIFOPageReplace();

/*LRU�㷨��ҳ*/
void LRUUse(int __frameNum);
bool withDrawLRU(int __frameNum);
int LRUPageReplace();

bool addPageErrorLog(int  __frameNum, int __type);

double calMemUtilization();
void test();
#endif
