/**
 * MyOS\Account.cpp
 *
 * Copyright (c) 2016 LyanQQ
 */

#include "Basic.h"
#include "Account.h"
#include "FileSystem.h"
#include <sstream>
#include <string.h>
#include <iostream>
using namespace std;
 
const char * accountFilePath = "C:\\system\\account.ini" ;

AccountLevelType accountLevel ; 

bool initAccount()
{
	FileStream * fs ;
	
	fs = openFile(accountFilePath) ;
	
	if (fs != NULL) {
		closeFile(fs);
		return true ;
	}
	else {
		char sys[] = "system" ;
		char acc[] = "account.ini" ;
		
		newFolder(sys, "C:") ;
		newFile(acc, "C:\\system") ;
		
		fs = openFile(accountFilePath) ;
		
		if (fs == NULL) {
			return false ;
		}
		
		char buf[] = "user root" ;
		
		if (saveWholeFile(buf, strlen(buf), fs)) {
			setFileAttribute((FileAttribute){true, true}, accountFilePath);
			setFolderAttribute((FolderAttribute){true, true}, "C:\\system");
			closeFile(fs) ;
			
			return true ;
		}
		else {
            closeFile(fs) ;
			return false ;
		}
	}
}

bool checkAccount(const char * __acc, const char * __pass)
{
	char * buf ;
	int len ;
	
	FileStream * fs ;
	
	fs = openFile(accountFilePath) ;
	
	if (fs == NULL) {
		setError(ACCOUNT_INI_INVALID) ;
		return false ;
	}
	
	len = getFileLength(fs) ;
	buf = new char[len + 1] ;
	
	readWholeFile(buf, fs) ;
	buf[len] = '\0' ;
    closeFile(fs) ;
	
	stringstream ss ;
	string acc = "", pass = "", guest = "" ;
	
	ss.clear();
	ss.str("");
	ss << buf ;
	
	ss >> acc >> pass >> guest ;
	
    if (strlen(__acc) == 0 && pass.length() != 0) {
		/* 访客账户 */
		if (guest.compare(__pass) == 0) {
			accountLevel = GUEST_USER ;
			return true ;
		}
		else {
			setError(ACCOUNT_PASSWORD_WRONG) ;
			return false ;
		}
	}
	else {
		/* 管理员账户 */
		if ( acc.compare(__acc) == 0
			&& pass.compare(__pass) == 0 ) {
			
			accountLevel = MANAGER_USER ;
			return true ;
		}
		else {
			setError(ACCOUNT_PASSWORD_WRONG) ;
			return false ;
		}
	}
}
/* 
bool updateManagerAccount(const char * __newAcc)
{
	if (accountLevel == GUEST_USER) {
		setError(PERMISSION_IS_RESTRICTED) ;
		return false ;
	}
	
	if (strlen(__newAcc) == 0) {
		return false ;
	}
	
	char * buf, * newbuf ;
	int len ;
	
	FileStream * fs ;
	
	fs = openFile(accountFilePath) ;
	
	if (fs == NULL) {
		return false ;
	}
	
	len = getFileLength(fs) ;
	buf = new char[len + 1] ;
	
	readWholeFile(buf, fs) ;
	buf[len] = '\0' ;
	
	len = 
} */
