#ifndef VIRTUALMEMORY_H
#define VIRTUALMEMORY_H

/* 虚拟内存帧 */
typedef struct {
	bool isAvailable;              /* 是否被占用 */
	int PID;					   /* 占用它的进程ID */
	int pageNum;                   /* 占用它的进程的第几页 */
}VMFrame;

bool initializeVM();
void resetVMFrameTable();

bool readVMConfig();
bool saveVMConfig();

void withdrawVM(int __PID);
bool allocateVM(int __PID, int __frameCountApply, int sectionNum[]);

void showVirtualMemoryDetial();

int VMframeAllocateCount(int __PID);
int findVMFrameToMemory(int __PID, int __pageNum);
int VMframeNumToSection(int __VMFrameNum);
bool isVMFrameNumAvailable(int __VMFrameNum);

double calVMUtilization();
#endif
