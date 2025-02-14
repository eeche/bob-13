//==============================================================================
// Title : Example of a relocatable shellcode
// Author : South
// Date : 2017.5.2
// Description :
// This shellcode is for System and Network Security lab's seminar.
// I try to show that there are a lots of ways to build shellcode.
// With this example shellcode, you can found out that it is possible
// to make shellcode without *.asm file
// I hope that after this seminar, all System and Netowrk Security lab's
// members understand the principal of relocatable shellcode and
// have their own shellcode framework.
//==============================================================================

#include <windows.h>
#include <stdio.h>

extern void MoveShellCode(PVOID ShellCodeBuf);

// This global variable will be usee in sc.cc
char *gTARGET_PROG_PATH = "C:\\Windows\\System32\\notepad.exe";
ULONG gTARGET_PROG_LEN = 0;
        
int main(int argc, char *argv[])
{
  char *sc = NULL;
  
  // allocate a virtual memory for shellcode
  // porperty : read/write/execute
  sc = (char *) VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

  if (sc == NULL) {
    printf("[-] VirtualAlloc fails: Error Code[%x]\n", GetLastError());
    return -1;
  }

  // initialize the buffer and global variable
  ZeroMemory(sc, 0x1000);
  gTARGET_PROG_LEN = (ULONG)strlen(gTARGET_PROG_PATH) + 1; //including null-char
  
  // move shellcode from text section to buffer memory
  MoveShellCode(sc);

  // type casting from char buffer to function pointer
  // then just use this variable like function.

  ((void (*)(void))sc)();
  printf("[+] Execute shellcode\n");
  printf("[+] Execute shellcode\n");
  printf("[+] Execute shellcode\n");
  printf("[+] Execute shellcode\n");
  printf("[+] Execute shellcode\n");
  printf("[+] Execute shellcode\n");
  printf("[+] Execute shellcode\n");
  printf("[+] Execute shellcode\n");
  printf("[+] Execute shellcode\n");
  printf("[+] Execute shellcode\n");
  printf("[+] Execute shellcode\n");
  printf("[+] Execute shellcode\n");
  char ch = getchar();
  return 0;
}//end of main
