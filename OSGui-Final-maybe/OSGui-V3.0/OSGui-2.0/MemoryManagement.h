#ifndef MEMORYMANAGEMENT_H
#define MEMORYMANAGEMENT_H
//typedef enum bools{ false, true }bool;
typedef struct {
	bool isAvailable;              /* 是否被占用 */
	int PID;					   /* 占用它的进程ID */
	int pageNum;                   /* 占用它的进程的第几页 */
	int FIFOcount;                 /* FIFO计数器 */
	int LRUcount;                  /* LRU算法计数器 */
    bool isDirty;                  /* 脏位，表示是否被修改 */
}Frame;

typedef struct {
	bool isAvailable;              /* 是否有效 */
	int PID;
	int frameNum;                   /* 此页占用的帧号 */
}Page, *PageTable;


bool initializeMemory();          /* 初始化内存 */
void showFrameTable();            /* 展示帧表 */
int VMToMemory(int __PID, int __pageNum);
bool MemoryToVM(int __frameNum);
bool allocateMemory(int __PID, int __frameNeed, int __hasUsed);     /* 分配内存 */
void withdrawMemory(int __PID);            /* 收回内存 */
bool readMemoryByChar(int __pid, int __frameNum, int __offsetIn, unsigned char * __toRead);  /* 读内存 */
bool writeMemoryByChar(int __pid, int __frameNum, int __offsetIn, const unsigned char __toSave); /* 写内存 */

/*FIFO算法调页*/
void FIFOUse(int __frameNum, bool isFrameAvailable);
bool withDrawFIFO(int __frameNum);
int FIFOPageReplace();

/*LRU算法调页*/
void LRUUse(int __frameNum);
bool withDrawLRU(int __frameNum);
int LRUPageReplace();

bool addPageErrorLog(int  __frameNum, int __type);

double calMemUtilization();
void test();
#endif
