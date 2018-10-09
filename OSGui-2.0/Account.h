/**
 * MyOS\Account.h
 *
 * Copyright (c) 2016 LyanQQ
 */
#ifndef ACCOUNT_H
#define ACCOUNT_H

enum AccountLevelType {MANAGER_USER, GUEST_USER} ;


bool initAccount() ;
bool checkAccount(const char * __acc, const char * __pass) ;

#endif
