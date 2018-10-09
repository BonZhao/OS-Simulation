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
 * ����������ͱ�ǣ��ļ����ļ��У�
 */
#define FILE_TYPE 0					/* �ļ� */
#define FOLDER_TYPE 1				/* �ļ��� */
#define ROOT_TYPE 2					/* ��Ŀ¼ */

/* ��ָ�� */
#define NULL_POINTER 65535			/* �������еĿ�ָ���ʾ */

/* λͼ��Ϣ */
#define SECTION_AVAILABLE 48		/* �������� */
#define SECTION_TAKEN 49			/* ���������� */

/**
 * ���ֶ�����������ռ�ĳ��ȣ����ֽ�Ϊ��λ
 */
#define LENGTH_FLAG 1				/* ���ռ 1 �ֽ� */
#define LENGTH_NAME 38				/* �ļ����У���ռ 38 �ֽ� */
#define LENGTH_TIME_STAMP 7			/* ʱ���ռ 7 �ֽ� */
#define LENGTH_LENGTH 4				/* �ļ�����ռ 4 �ֽ� */
#define LENGTH_POINTER_NUMBER 2		/* ����ָ������ռ 2 �ֽ� */
#define LENGTH_POINTER 2			/* ����ָ��ռ 2 �ֽ� */
#define MAX_DATASEC_NUMBER 477		/* �ļ������������������ */
#define MAX_CHILD_NUMBER 486		/* �ļ����ӽڵ��������� */

/**
 * ���ֶ���ʼλ���������е�ƫ����
 */
/* �ļ� */
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

/* �ļ��� */
#define FOLDER_OFFSET_FLAG 0
#define FOLDER_OFFSET_NAME 1
#define FOLDER_OFFSET_CREATE_TIME 39
#define FOLDER_OFFSET_CHILD_NUMBER 46
#define FOLDER_OFFSET_FATHER_NODE 48
#define FOLDER_OFFSET_CHILD_NODE 50
#define FOLDER_OFFSET_ATTRIBUTE 1022

/**
 * �ļ����Խṹ
 */
struct FileAttribute {
	bool hide ;					/* ���� */
	bool readOnly ;				/* ֻ�� */
} ;

/**
 * �ļ������Խṹ
 */
struct FolderAttribute {
	bool hide ;					/* ���� */
	bool protect ;				/* ���� */
} ;

/**
 * �ļ����������ݽṹ
 */
struct FileIndex {
	char flag ;					/*  1B	���������ļ����ļ��еı�� */
	char * name ;				/* 38B	�ļ����� */
	TimeStamp createTime ;		/*  7B	�ļ�����ʱ�� */
	TimeStamp lastModifyTime ;	/*  7B	�ļ��ϴ��޸�ʱ�� */
	TimeStamp lastOpenTime ;	/*  7B	�ļ��ϴδ�ʱ�� */
	int length ;				/*  4B	�ļ����� */
	int dataSectionNumber ;		/*  2B	���ݿ������ */
	int fatherNode ;			/*  2B	���ڵ�λ�� */
	int * dataNode ;			/* 2nB	���ݿ�ڵ�λ�� */
	FileAttribute attribute ;	/* 	2B	�ļ����� */
} ;

/**
 * �ļ������������ݽṹ
 */
struct FolderIndex {
	char flag ;					/*  1B	���������ļ����ļ��еı�� */
	char * name ;				/* 38B	�ļ������� */
	TimeStamp createTime ;		/*  7B	�ļ��д���ʱ�� */
	int childNodeNumber ;		/*  2B	�ӽڵ������ */
	int fatherNode ;			/*  2B	���ڵ�λ�� */
	int * childNode ;			/* 2nB	�ӽڵ�λ�� */
	FolderAttribute attribute ;	/* 	2B	�ļ������� */
} ;

/**
 * �ļ�������
 */
struct FileStream {
	char * path ;					/* �ļ�·�� */
	int indexSectionNumber ;		/* �ļ������������� */
	int readPtr ;					/* �ļ���ȡָ�� */
	int writePtr ;					/* �ļ�д��ָ�� */
} ;

/**
 * �ļ�������
 */
#define BEGIN_PTR -1				/* �ļ���ʼλ��ָ�� */
#define CURRENT_PTR -2				/* �ļ���ǰλ��ָ�� */
#define END_PTR -3					/* �ļ�����λ��ָ�� */
#define WHOLE_FILE -1

/**
 * ��������
 * private���������ṩ���ⲿ
 */
/* ʱ������� */
int compareTimeStamp(TimeStamp __time_1, TimeStamp __time_2);
void extractTimeStamp(char * buf, TimeStamp * __ts);
void packTimeStamp(char * buf, TimeStamp __ts);

/* �ļ���������� */
void initFileIndex(FileIndex * __fi);
void destroyFileIndex(FileIndex * __fi);
void extractFileIndex(char * section, FileIndex * __fi);
void packFileIndex(char * section, FileIndex __fi);
void newFileIndex(FileIndex * __fi, char * __name, int __father);
bool moreDataNode(FileIndex * __fi, int __dsn, int * __secNum);
bool releaseDataNode(FileIndex * __fi, int __dsn);

/* �ļ������������ */
void initFolderIndex(FolderIndex * __foi);
void destroyFolderIndex(FolderIndex * __foi);
void extractFolderIndex(char * __sec, FolderIndex * __foi);
void packFolderIndex(char * __sec, FolderIndex __foi);
void newFolderIndex(FolderIndex * __foi, char * __name, int __father);
bool moreChildNode(FolderIndex * __foi, int __dsn, int * __secNum);
bool releaseChildNode(FolderIndex * __foi, int __dsn);
bool deleteChildNode(FolderIndex * __foi, int __secNum);

/* �������� */
bool newBitVector(int __sectionNumber);		/* private */
bool getBitVector(char * __bitMap);			/* private */
bool saveBitVector(char * __bitMap);		/* private */
bool readFSConfig();						/* private */
bool initFileSystem();

/* ���пռ������ */
int queryFreeSpace();
bool applyForFreeSpace(int __many, int * __allocated);
bool releaseFreeSpace(int __many, const int * __allocated);
bool recycleFileSpace(FileIndex __fi, int __secNum);

/* �����ײ�ӿ� */
bool copySection(int __aim, int __from);
bool checkFileName(char * __fn);
bool extractName(char * __name, int __secNum);
void extractFileName(char * __name, char * __ex, const char * __whole);
bool extractFilePath(const char * __whole_path, char * __path, int __which);
bool createRootDirectory();
bool getIndexSection(char * __sec, int * __secNum, const char * __whole_path);
bool fileNotOpen(const char * __path);
bool folderNotOpen(const char * __path);

/* �ļ�ϵͳAPI */
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