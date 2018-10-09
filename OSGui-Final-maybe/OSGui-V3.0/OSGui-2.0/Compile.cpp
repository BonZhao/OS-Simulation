/**
 * MyOS\Compiler.cpp
 *
 * Copyright (c) 2016 LyanQQ
 */

#include "ExeUnit.h"
#include "Basic.h"
#include <string.h>
#include <sstream>
#include <iostream>
using namespace std ;

int variableNumber[INS_NUMBER] = {1, 1, 2, 1, 1, 1, 0, 2, 2, 1, 2, 2} ;
const char * InsMnemonic[INS_NUMBER] = {
    "cfg" ,
    "exc" ,
    "alo" ,
    "fre" ,
    "prt" ,
    "eio" ,
    "end" ,
    "add" ,
    "sub" ,
    "jmp" ,
    "cjp" ,
    "sav"
    } ;

stringstream ss ;

InstructionType whichIns(string ins__) {
    InstructionType it[INS_NUMBER] = {CONFIG, NORMAL, MEM_MALLOC, MEM_FREE_, PRINT, IO, END, ADD, SUB, JUMP, C_JUMP, SAVE};

    for (int i=0; i <= INS_NUMBER-1; i++) {
        if ( ins__.compare(InsMnemonic[i]) == 0 ) {
            return it[i] ;
        }
    }

    return NONE ;
}

bool getVar(InstructionType type__, int * var__)
{
    for (int i=0; i<=variableNumber[type__]-1; i++) {
        if (ss >> var__[i]) {
            ;
        }
        else {
            return false ;
        }
    }

    return true ;
}

int compiler(const char * asm__, unsigned char * code__)
{
    /**
     * �����Դ��������˫�ֽ�ָ��
     * �����ִ�Сд
     */

    bool flag = false ;
    string typeStr ;
    int next = 0;
    char * lowerasm__ = new char [strlen(asm__) + 1] ;
    strcpy(lowerasm__, asm__) ;
    lowercase(lowerasm__);

    ss.clear();
    ss.str("");

    ss << lowerasm__ ;

    while (ss >> typeStr) {
        InstructionType type ;
        int var[2] ;
        int pre ;

        type = whichIns(typeStr) ;
        if (type == NONE) {
            delete []lowerasm__ ;
            return -1 ;
        }
        //cout << type << endl ;
        if (!getVar(type, var)) {
            delete []lowerasm__ ;
            return -1 ;
        }

        switch (type) {
            /* ������� */
            case CONFIG :
                pre = 0 ;

                code__[next] = var[0] % 32 ;
                next ++ ;
                code__[next] = 0 ;
                next ++ ;
            break ;

            /* ��ͨ��� */
            case NORMAL :
                pre = 1 ;

                code__[next] = pre * 32 ;
                next ++ ;
                code__[next] = var[0] % 256 ;
                next ++ ;
            break ;

            /* �����ڴ���� */
            case MEM_MALLOC :
                pre = 2 ;

                code__[next] = pre * 32 + (var[0] % 16) ;
                next ++ ;
                code__[next] = var[1] % 256 ;
                next ++ ;
            break ;

            /* �ͷ��ڴ���� */
            case MEM_FREE_ :
                pre = 2 ;

                code__[next] = pre * 32 + 16 + (var[0] % 16) ;
                next ++ ;
                code__[next] = 0 ;
                next ++ ;
            break ;

            /* ��ӡ��� */
            case PRINT :
                pre = 3 ;

                code__[next] = pre * 32 + (var[0] % 16) ;
                next ++ ;
                code__[next] = 0 ;
                next ++ ;
            break ;

            /* ����IO��� */
            case IO :
                pre = 3 ;

                code__[next] = pre * 32 + 16 ;
                next ++ ;
                code__[next] = var[0] % 256 ;
                next ++ ;
            break ;

            /* ������� */
            case END :
                pre = 4 ;

                code__[next] = pre * 32 ;
                next ++ ;
                code__[next] = 0 ;
                next ++ ;

                flag = true ;
            break ;

            /* �ӷ���� */
            case ADD :
                pre = 5 ;

                code__[next] = pre * 32 ;
                next ++ ;
                code__[next] = (var[0] % 16)*16 +  (var[1] % 16) ;
                next ++ ;
            break ;

            /* ������� */
            case SUB :
                pre = 5 ;

                code__[next] = pre * 32 + 16;
                next ++ ;
                code__[next] = (var[0] % 16)*16 +  (var[1] % 16) ;
                next ++ ;
            break ;

            /* ��ת��� */
            case JUMP :
                pre = 6 ;

                code__[next] = pre * 32 ;
                next ++ ;
                code__[next] = var[0] % 256 ;
                next ++ ;
            break ;

            /* ������ת��� */
            case C_JUMP :
                pre = 6 ;

                code__[next] = pre * 32 + 16 + (var[0] % 16);
                next ++ ;
                code__[next] = var[1] % 256 ;
                next ++ ;
            break ;

            /* д����� */
            case SAVE :
                pre = 7 ;

                code__[next] = pre * 32 + (var[0] % 16);
                next ++ ;
                code__[next] = var[1] % 256 ;
                next ++ ;
            break ;

            default :
                delete []lowerasm__ ;
                return -1 ;
            break ;
        }
    }

    delete []lowerasm__ ;

    if (flag) {
        return next ;
    }
    else {
        return -1 ;
    }
}
