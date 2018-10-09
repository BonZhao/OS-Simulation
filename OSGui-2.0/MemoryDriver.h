#ifndef MEMORYDRIVER_H
#define MEMORYDRIVER_H
/**
* 内存配置信息，包括磁内存描述，内存大小
* 帧大小和帧数量
*/

bool initializeMemoryHard();
bool readMemoryConfig();
bool saveMemoryConfig();//*慎用*//

bool readMemoryUnit(int __offset, unsigned char *__toRead);
bool writeMemoryUnit(int __offset, const unsigned char __toSave);

bool readMemoryByFrame(int __offset, char* __data);
bool writeMemoryByFrame(int __offset, char *__toSave);

#endif
