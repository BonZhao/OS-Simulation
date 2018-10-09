#ifndef VIRTUALMEMORY_H
#define VIRTUALMEMORY_H

/* �����ڴ�֡ */
typedef struct {
	bool isAvailable;              /* �Ƿ�ռ�� */
	int PID;					   /* ռ�����Ľ���ID */
	int pageNum;                   /* ռ�����Ľ��̵ĵڼ�ҳ */
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
