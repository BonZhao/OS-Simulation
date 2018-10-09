#ifndef DISKDRIVER_H
#define DISKDRIVER_H

bool readDiskConfig();
//bool saveDiskConfig();
int formatting(int __sectionSize);
bool readSection(char * __section, int whichSection);
bool saveSection(char * __section, int whichSection);

#endif