#ifndef MEMORYDRIVER_H
#define MEMORYDRIVER_H
/**
* �ڴ�������Ϣ���������ڴ��������ڴ��С
* ֡��С��֡����
*/

bool initializeMemoryHard();
bool readMemoryConfig();
bool saveMemoryConfig();//*����*//

bool readMemoryUnit(int __offset, unsigned char *__toRead);
bool writeMemoryUnit(int __offset, const unsigned char __toSave);

bool readMemoryByFrame(int __offset, char* __data);
bool writeMemoryByFrame(int __offset, char *__toSave);

#endif
