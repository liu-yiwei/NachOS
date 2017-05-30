/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/file.h>
#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include <signal.h>
#include <sys/types.h>
#include <pthread.h>
//#include <synchconsole.h>




void SysHalt()
{
  kernel->interrupt->Halt();
}


int SysAdd(int op1, int op2)
{
  return op1 + op2;
}


int SysWrite(char *buffer, int size, OpenFileId id)
{
	int ch;		//从物理内存读需要用整数作为传递单位
	int i=0;
	while ( i < size ){
		kernel->machine->ReadMem((int)buffer,1, &ch);		//由于物理内存以int为单位进行读写，所以要对buffer作强制类型转换
		write(id, (char *)&ch, 1);				////从C系统调用write写出的ch是一个字符
		buffer ++;
		i++; 
	}
	return i;
}

int SysRead(char *buffer, int size, OpenFileId id)
{
	char ch;
	int i=0;
	while (i < size){
		Read(id, &ch, 1);			//从C系统调用Read读到ch中的是一个字符
		kernel->machine->WriteMem((int)buffer,1, ch);    //由于物理内存以int为单位进行读写，所以要对buffer作强制类型转换
		buffer ++;
		i++; 
	}
	return i;
}

SpaceId SysExec(char* exec_name)
{
	int count = 0;
	int ch;
	char cmd[60];
	do {
		kernel->machine->ReadMem((int)exec_name, 1, &ch);
		exec_name ++;
		cmd[count]=(char)ch;
	}while (ch !='\0' && count++ <59);
	cmd[count]='\0';
	pid_t child;
	child = vfork();
	if(child == 0) {
		execl ("/bin/sh", "/bin/sh", "-c", cmd, NULL);
		_exit (EXIT_FAILURE);
	} else if(child < 0){
		_exit (EXIT_FAILURE);
		return EPERM;
	}
	return child;
}

int SysJoin(SpaceId id)
{
	pid_t retid;
	retid = waitpid((pid_t) id, (int*) 0, 0);
	return retid;
}

int SysPrint(char * msg)
{
	int IntValue = 0;
	int IntMem = (int)msg;
	do
	{
		/* 从用户程序内存空间读取字符串的值 */
		kernel->machine->ReadMem(int(IntMem++), 1, &IntValue);
		if ( 0 == IntValue ) 
			break;
		write(1, (char *)&IntValue, 1);
		//kernel->synchConsoleOut->PutChar( (char)IntValue );
	}while(true);
	return 0;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
