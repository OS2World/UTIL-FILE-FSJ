/* --------------------------------------------------------------------------
 prototypes.h :

 16 Jun 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

#ifndef _PROTOTYPES_H_
   #define _PROTOTYPES_H_

// end.c
VOID endApplication(VOID);

// init.c
BOOL initApplication(INT argc, PSZ* argv);

// showhlep.c
BOOL tbInit(PTEXTBROWSER ptb);
VOID tbEnd(PTEXTBROWSER ptb);
BOOL tbBrowseTextFile(PSZ pszFile, PTEXTBROWSER ptb);

// utils.c
PVOID memAlloc(ULONG cb);
PVOID memAllocTiled(ULONG cb);
VOID memFree(PVOID pmem);
PSZ lastSlash(PSZ pszIn);
BOOL handleError(BOOL bContinue, BOOL bHelp, PSZ msg, ...);
VOID printError(PSZ pszMsg, ULONG cbMsg);
VOID printMsg(ULONG irow, PSZ pszMsg, ...);
VOID printProgress(ULONG cbData);
ULONG kbdKey(VOID);
VOID pause(VOID);
BOOL screenSize(PSIZEL psize);
BOOL screenClear(ULONG attribute);
ULONG countLines(PSZ pszData);
ULONG handleFileIOError(ULONG retv, PSZ msg, PSZ pszFile, APIRET rc);
HFILE fmOpen(PSZ pszFile, LONGLONG cb, ULONG flag);
BOOL fmClose(HFILE hf);
LONG fmRead(HFILE hf, PVOID pBuffer, ULONG cb);
ULONG fmWrite(HFILE hf, PVOID pBuffer, ULONG cb);
LONGLONG fmFileSize(PSZ pszFile);
LONGLONG fmSize(HFILE hf);
LONGLONG fmAvailSpace(PSZ pszPath, PULONG pCbSector);
LONGLONG fmFileUsedSpace(LONGLONG cbFile, ULONG cbSector);
VOID makePartName(PSZ pBuf, PFILEMASK pMask, ULONG iPart);
INT makeJoinPartName(PSZ pszBuf, INT iFile, INT cbPath);
ULONG showMenu(PSZ pszPrompt, PSZ choices);
BOOL checkSpaceLoop(LONGLONG cbReq, PSZ pOutPath);

#endif // #ifndef _PROTOTYPES_H_