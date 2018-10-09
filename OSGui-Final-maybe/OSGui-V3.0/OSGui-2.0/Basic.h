/**
 * MyOS\Basic.h
 *
 * last modification time : 2016/3/18 16:37		by LyanQQ
 */

/**
 * Working log:
 * 2016/03/18 16:37		Created				by LyanQQ ;
 * 2016/03/18 16:59		1st time coding		by LyanQQ :
 * 		-Finish TimeStamp definition and its operating functions ;
 */

/**
 * Basic.h���������л��������ݽṹ�������ͺ���
 * ��Щ�����ڲ���ϵͳ���κ�һ��ģ�鶼�п����õ�
 * ������������ʱ������ַ�������������
 */ 

#ifndef BASIC_H
#define BASIC_H

/**
 * �����������
 */
enum ErrorType {  NO_ENOUGH_DISK_SPACE
				, NO_ENOUGH_CHILD_NODE
				, PATH_NOT_EXIST
				, SAME_NAME
				, READ_WRITE_DISK_ERROR
				, SYSTEM_ERROR
				, ILLEGAL_ACCESS_SYSTEM_DISK
				, DISK_SPACE_ERROR
				, ILLEGAL_CHAR_IN_FILE_NAME
				, FILE_NAME_TOO_LONG
				, FILE_NAME_EMPTY
				, FILE_TOO_BIG
				, FILE_READ_ERROR
				, FILE_WRITE_ERROR
				, MEMORY_CONFIG_NOT_EXIST,		                /*�ڴ������ļ�������*/
				 MEMORY_CONFIG_DESC_TOO_LONG_OR_SHORT,          /*�ڴ�������Ϣ��ʽ����������Ϣ���������)*/
				 MEMORY_CONFIG_MEMORYSIZE_ERROR,                /*�ڴ�������Ϣ�ڴ��С��ʽ����*/
				 MEMORY_NOT_EXIST,                              /*�ڴ治����*/
				 MEMORYADDRESS_OUT_OF__BOUND,                   /*�ڴ��ַԽ��*/
				 VMEMORY_CONFIG_NOT_EXIST,                      /*�����ڴ������ļ�������*/
				 VMEMORY_CONFIG_FORM_ERROR,                     /*�����ڴ������ļ���ʽ����*/
				 VMEMORY_APPLY_TOO_MUCH,                        /*���������ڴ�֡������*/
				 VMEMORY_NOT_ENOUGH,                            /*�����ڴ治��*/
				 VMEMORY_FRAMENUM_MAP_SECTION_NOT_FOUND,        /*������֡�Ŷ�Ӧ������������*/
				 MEMORYMANAGEMENT_CONFIG_NOT_EXIST,             /*�ڴ���������ļ�������*/
				 MEMORYMANAGEMENT_CONFIG_FORM_ERROR,            /*�ڴ���������ļ���ʽ����ȷ*/
				 MEMORYMANAGEMENT_CONFIG_NOT_READ,              /*δ��ȡ�ڹ����������Ϣ*/
				 MEMORY_APPLY_TOO_MUCH,                         /*�����ڴ�֡������*/
				 VM_FRAMENUM_NOT_AVAILABLE,                     /*����֡��Ч*/
				 FRAME_QUEUE_INI_ERROR,                         /*֡���г�ʼ��ʧ��*/
				 FRAME_NOT_AVAILABLE,                           /*�ڴ�֡��Ч*/
				 MEMORYLOGPATH_NOT_EXIST
				, NOT_RUNNABLE_FILE
				, CAN_NOT_RUN_FILE
				, UNKNOW_ERROR
				, UNAVAILABLE_PROCESS
				, PID_NOT_MAP_FRAME
				, ADDRESS_OVERFLOW
				, IO_REQUEST_INVALID
				, FILE_ALREADY_OPEN
				, FOLDER_ALREADY_OPEN
				, CAN_NOT_MODIFY_READ_ONLY_FILE
                , CAN_NOT_DELETE_PROTECT_FOLDER
                , ILLEGAL_ADDRESS_VISIT
                , DEST_FOLDER_IS_SUB_FOLDER
                , PERMISSION_IS_RESTRICTED
                , ACCOUNT_INI_INVALID
                , ACCOUNT_PASSWORD_WRONG
                , NO_ENOUGH_HEAP_SPACE
                , ILLEGAL_PARAMETER
                , DEFRAGMENT_FAILED} ;

/**
 * ʱ����ṹ
 */
struct TimeStamp {
	int year ;		/* �꣬4λ��, 2B */
	int month ;		/* �£�2λ��, 1B */
	int day ;		/* �գ�2λ��, 1B */
	int hour ;		/* ʱ��2λ��, 1B */
	int min ;		/* �֣�2λ��, 1B */
	int sec ;		/* �룬2λ��, 1B */
} ;

void getSystemTime(TimeStamp * __ts);
void showTime(char * __buf, TimeStamp __ts);
char * logTime(char * __buf, TimeStamp __ts);
int copyString(char * aim, const char * from, int howMuch);
void uppercase(char * __str);
void lowercase(char * __str);
int strToHex(char *ch, char *hex);
int strToHex(char *ch, char *hex, int length);

/* ������ */
extern ErrorType errorCode;
void inline setError(ErrorType __e)
{
	errorCode = __e ;
}
void printError();

#endif
