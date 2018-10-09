/**
 * MyOS\FileSystem.h
 *
 * Copyright (c) 2016 LyanQQ
 */

/**
 * Working log:
 * 2016/03/16 20:45		Created				by LyanQQ ;
 * 2016/03/16 22:28		1st time coding		by LyanQQ :
 * 		-Finish all definitions and data structures ;
 * 2016/03/18 21:48		2nd time coding		by LyanQQ :
 *		-Add some constant definitions ;
 * 2016/05/14 20:01		3rd time coding		by LyanQQ :
 *		-Add file\Folder attributes definitions ;
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Basic.h"
 
/**
 * 索引块的类型标记（文件，文件夹）
 */
#define FILE_TYPE 0					/* 文件 */
#define FOLDER_TYPE 1				/* 文件夹 */
#define ROOT_TYPE 2					/* 根目录 */

/* 空指针 */
#define NULL_POINTER 65535			/* 索引块中的空指针表示 */

/* 位图信息 */
#define SECTION_AVAILABLE 48		/* 扇区可用 */
#define SECTION_TAKEN 49			/* 扇区不可用 */

/**
 * 各字段在扇区中所占的长度，以字节为单位
 */
#define LENGTH_FLAG 1				/* 标记占 1 字节 */
#define LENGTH_NAME 38				/* 文件（夹）名占 38 字节 */
#define LENGTH_TIME_STAMP 7			/* 时间戳占 7 字节 */
#define LENGTH_LENGTH 4				/* 文件长度占 4 字节 */
#define LENGTH_POINTER_NUMBER 2		/* 扇区指针数量占 2 字节 */
#define LENGTH_POINTER 2			/* 扇区指针占 2 字节 */
#define MAX_DATASEC_NUMBER 477		/* 文件数据扇区的最大数量 */
#define MAX_CHILD_NUMBER 486		/* 文件夹子节点的最大数量 */

/**
 * 各字段起始位置在扇区中的偏移量
 */
/* 文件 */
#define FILE_OFFSET_FLAG 0
#define FILE_OFFSET_NAME 1
#define FILE_OFFSET_CREATE_TIME 39
#define FILE_OFFSET_LAST_MODIFY_TIME 46
#define FILE_OFFSET_LAST_OPEN_TIME 53
#define FILE_OFFSET_LENGTH 60
#define FILE_OFFSET_DATASEC_NUMBER 64
#define FILE_OFFSET_FATHER_NODE 66
#define FILE_OFFSET_DATA_NODE 68
#define FILE_OFFSET_ATTRIBUTE 1022

/* 文件夹 */
#define FOLDER_OFFSET_FLAG 0
#define FOLDER_OFFSET_NAME 1
#define FOLDER_OFFSET_CREATE_TIME 39
#define FOLDER_OFFSET_CHILD_NUMBER 46
#define FOLDER_OFFSET_FATHER_NODE 48
#define FOLDER_OFFSET_CHILD_NODE 50
#define FOLDER_OFFSET_ATTRIBUTE 1022

/**
 * 文件属性结构
 */
struct FileAttribute {
	bool hide ;					/* 隐藏 */
	bool readOnly ;				/* 只读 */
} ;

/**
 * 文件夹属性结构
 */
struct FolderAttribute {
	bool hide ;					/* 隐藏 */
	bool protect ;				/* 保护 */
} ;

/**
 * 文件索引块数据结构
 */
struct FileIndex {
	char flag ;					/*  1B	用于区分文件和文件夹的标记 */
	char * name ;				/* 38B	文件名称 */
	TimeStamp createTime ;		/*  7B	文件创建时间 */
	TimeStamp lastModifyTime ;	/*  7B	文件上次修改时间 */
	TimeStamp lastOpenTime ;	/*  7B	文件上次打开时间 */
	int length ;				/*  4B	文件长度 */
	int dataSectionNumber ;		/*  2B	数据块的数量 */
	int fatherNode ;			/*  2B	父节点位置 */
	int * dataNode ;			/* 2nB	数据块节点位置 */
	FileAttribute attribute ;	/* 	2B	文件属性 */
} ;

/**
 * 文件夹索引块数据结构
 */
struct FolderIndex {
	char flag ;					/*  1B	用于区分文件和文件夹的标记 */
	char * name ;				/* 38B	文件夹名称 */
	TimeStamp createTime ;		/*  7B	文件夹创建时间 */
	int childNodeNumber ;		/*  2B	子节点的数量 */
	int fatherNode ;			/*  2B	父节点位置 */
	int * childNode ;			/* 2nB	子节点位置 */
	FolderAttribute attribute ;	/* 	2B	文件夹属性 */
} ;

/**
 * 文件流类型
 */
struct FileStream {
	char * path ;					/* 文件路径 */
	int indexSectionNumber ;		/* 文件索引块扇区号 */
	int readPtr ;					/* 文件读取指针 */
	int writePtr ;					/* 文件写入指针 */
} ;

/**
 * 文件流常量
 */
#define BEGIN_PTR -1				/* 文件开始位置指针 */
#define CURRENT_PTR -2				/* 文件当前位置指针 */
#define END_PTR -3					/* 文件结束位置指针 */
#define WHOLE_FILE -1

/**
 * 函数声明
 * private函数不能提供给外部
 */
/* 时间戳操作 */
int compareTimeStamp(TimeStamp __time_1, TimeStamp __time_2);
void extractTimeStamp(char * buf, TimeStamp * __ts);
void packTimeStamp(char * buf, TimeStamp __ts);

/* 文件索引块操作 */
void initFileIndex(FileIndex * __fi);
void destroyFileIndex(FileIndex * __fi);
void extractFileIndex(char * section, FileIndex * __fi);
void packFileIndex(char * section, FileIndex __fi);
void newFileIndex(FileIndex * __fi, char * __name, int __father);
bool moreDataNode(FileIndex * __fi, int __dsn, int * __secNum);
bool releaseDataNode(FileIndex * __fi, int __dsn);

/* 文件夹索引块操作 */
void initFolderIndex(FolderIndex * __foi);
void destroyFolderIndex(FolderIndex * __foi);
void extractFolderIndex(char * __sec, FolderIndex * __foi);
void packFolderIndex(char * __sec, FolderIndex __foi);
void newFolderIndex(FolderIndex * __foi, char * __name, int __father);
bool moreChildNode(FolderIndex * __foi, int __dsn, int * __secNum);
bool releaseChildNode(FolderIndex * __foi, int __dsn);
bool deleteChildNode(FolderIndex * __foi, int __secNum);

/* 环境函数 */
bool newBitVector(int __sectionNumber);		/* private */
bool getBitVector(char * __bitMap);			/* private */
bool saveBitVector(char * __bitMap);		/* private */
bool readFSConfig();						/* private */
bool initFileSystem();

/* 空闲空间管理函数 */
int queryFreeSpace();
bool applyForFreeSpace(int __many, int * __allocated);
bool releaseFreeSpace(int __many, const int * __allocated);
bool recycleFileSpace(FileIndex __fi, int __secNum);

/* 其他底层接口 */
bool copySection(int __aim, int __from);
bool checkFileName(char * __fn);
bool extractName(char * __name, int __secNum);
void extractFileName(char * __name, char * __ex, const char * __whole);
bool extractFilePath(const char * __whole_path, char * __path, int __which);
bool createRootDirectory();
bool getIndexSection(char * __sec, int * __secNum, const char * __whole_path);
bool fileNotOpen(const char * __path);
bool folderNotOpen(const char * __path);

/* 文件系统API */
bool newFolder(char * __name, const char * __path);
bool newFile(char * __name, const char * __path);
bool deleteFileFolder(const char * __path);
bool renameFileFolder(char * __new_name, const char * __path);
bool moveFileFolder(const char * __dest_path, const char * __source_path);
bool copyFile(const char * __dest_path, const char * __source_path);
bool copyFolder(char * __dest_path, char * __source_path);
bool setFileAttribute(FileAttribute __attr, const char * __path);
bool setFolderAttribute(FolderAttribute __attr, const char * __path);
/* bool openFile(const char * __path, FileStream * * __fs);
void closeFile(FileStream * * __fs);
int setFilePointer(int __from, int __offset, FileStream * __fs);
int getFileLength(FileStream * __fs);
int readWholeFile(char * __buf, FileStream * __fs);
bool saveWholeFile(char * __buf, int __count, FileStream * * __fs); */
FileStream * openFile(const char * __path) ;
void closeFile(FileStream * __fs) ;
int getFileLength(FileStream * __fs) ;
int readWholeFile(char * __buf, FileStream * __fs) ;
bool saveWholeFile(char * __buf, int __count, FileStream * __fs) ;
#endif