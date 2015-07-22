/* --------------------------------------------------------------------------
 utils.c : general utility procedures module.

 16 Jun 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

#pragma strings(readonly)

// includes -----------------------------------------------------------------
#include "main.h"

// definitions --------------------------------------------------------------
// prototypes ---------------------------------------------------------------
// global variables ---------------------------------------------------------


/* --------------------------------------------------------------------------
 DosAllocMem wrapper. Print an error message in case of failure.
- Parameters -------------------------------------------------------------
 ULONG cb  : memory to be allocated.
- Return value -----------------------------------------------------------
 PVOID : address of allocated memory.
-------------------------------------------------------------------------- */
PVOID memAlloc(ULONG cb) {
   PVOID pmem;
   ULONG flags;

   flags = PAG_READ | PAG_WRITE | PAG_COMMIT;
   if (g.mode & FSJ_MEMHIGHOBJ) flags |= OBJ_ANY;

   g.rc = DosAllocMem((PPVOID)&pmem, cb, flags);
   if (g.rc)
      return (PVOID)handleError(FALSE, TRUE, SZERR_ALLOCATION, cb, g.rc);

   return pmem;
}


/* --------------------------------------------------------------------------
 Allocate a tiled memory object via DosAllocMem().
- Parameters -------------------------------------------------------------
 ULONG cb  : memory to be allocated.
- Return value -----------------------------------------------------------
 PVOID : address of allocated memory.
-------------------------------------------------------------------------- */
PVOID memAllocTiled(ULONG cb) {
   PVOID pmem;

   g.rc = DosAllocMem((PPVOID)&pmem, cb,
                      PAG_READ | PAG_WRITE | PAG_COMMIT | OBJ_TILE);
   if (g.rc)
      return (PVOID)handleError(FALSE, TRUE, SZERR_ALLOCATION, cb, g.rc);

   return pmem;

}


/* --------------------------------------------------------------------------
 DosFreeMem wrapper.
- Parameters -------------------------------------------------------------
 PVOID pmem : address of memory to be freed.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
VOID memFree(PVOID pmem) {
   DosFreeMem(pmem);
}


// /* --------------------------------------------------------------------------
//  Duplicate a text string in allocated memory.
// - Parameters -------------------------------------------------------------
//  PSZ text : text string to be duplicated.
// - Return value -----------------------------------------------------------
//  PSZ : address of the allocated memory or NULL in case of error.
// -------------------------------------------------------------------------- */
// PSZ strDup(PSZ text) {
//    ULONG cb;
//    PSZ pDup;
//
//    cb = strlen(text) + 1;
//    if (NULL != (pDup = memAlloc(cb)))
//       memcpy(pDup, text, cb);
//    return pDup;
// }
//
//
/* --------------------------------------------------------------------------
 Get the last slash ('\' or '/') in a string.
- Parameters -------------------------------------------------------------
 PSZ pszIn  : string to be parsed.
- Return value -----------------------------------------------------------
 PSZ : pointer to the last slash or NULL.
-------------------------------------------------------------------------- */
PSZ lastSlash(PSZ pszIn) {
   PSZ pszEnd;
   ULONG cb = strlen(pszIn);
   for (pszEnd = pszIn + cb - 1; pszEnd >= pszIn; --pszEnd)
   {
      if ((*pszEnd == '\\') || (*pszEnd == '/'))
         return pszEnd;
   }
   return NULL;
}


/* --------------------------------------------------------------------------
 Handle error events.
- Parameters -------------------------------------------------------------
 BOOL bContinue : TRUE/FALSE (non-critical/critical error).
 BOOL bHelp : show the 'type FSJ ?' message.
 PSZ msg    : error message holding formatting characters.
 ...        : variable number of parameters.
- Return value -----------------------------------------------------------
 BOOL : FALSE when the program has to terminate, TRUE for non-critical errors.
-------------------------------------------------------------------------- */
BOOL handleError(BOOL bContinue, BOOL bHelp, PSZ msg, ...) {
   CHAR buf[CBMSG_BUF];
   ULONG cb;
   va_list ap;

   // build the error message: "Error: " + msg + ".\r\n"
   cb = sprintf(buf, SZ_ERRPREFIX);
   va_start(ap, msg);
   cb += vsprintf(buf + cb, msg, ap);
   va_end(ap);
   cb += sprintf(buf + cb, (bHelp ? SZ_READHELP : SZ_SUFFIX));
   if (bContinue)
   {
      if (FMO_ERROR != g.hfLog)
         DosPutMessage(g.hfLog, cb, buf);
      if (g.mode & FSJUNATTENDED) return FALSE;
      sprintf(buf + cb, "\r\n%s", SZ_IGNOREABORT);
      if (ERROR_ABORT == showMenu(buf, ACH_IGNOREABORT))
         return FALSE;
      g.rc = NO_ERROR;
      return TRUE;
   }
   printError(buf, cb);
   return FALSE;
}


/* --------------------------------------------------------------------------
 Print an error message to the standard error output and to the log file.
- Parameters -------------------------------------------------------------
 PSZ pszMsg  : message to be printed.
 ULONG cbMsg : length of the message.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
VOID printError(PSZ pszMsg, ULONG cbMsg) {

   if (!(g.mode & FSJQUIET))
   {
      if (g.mode & FSJ_RUNNING) VioSetCurPos(20, 0, 0);
      DosPutMessage((HFILE)2, cbMsg, pszMsg);
   }
   if (FMO_ERROR != g.hfLog)
   {
      pszMsg[cbMsg++] = '\r', pszMsg[cbMsg++] = '\n';
      DosPutMessage(g.hfLog, cbMsg, pszMsg);
   }
}


/* --------------------------------------------------------------------------
 Print a text message on the screen and on standard output.
- Parameters -------------------------------------------------------------
 LONG irow : row position of the message. If this is < 0 the cursor
              position is not changed.
 PSZ pszMsg : text message with formatting tags.
 ...        : variable number of parameters.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
VOID printMsg(ULONG irow, PSZ pszMsg, ...) {
   CHAR buf[CBMSG_BUF];
   ULONG cb;
   va_list ap;

   va_start(ap, pszMsg);
   cb = vsprintf(buf, pszMsg, ap);
   va_end(ap);
   cb +=sprintf(buf + cb, "\r\n");
   if (!(g.mode & FSJQUIET))
   {
      if (irow > 0)
      {
         VioScrollUp(irow, 0, irow + 5, 80, 6, " \x00", 0);
         VioSetCurPos(irow, 0, 0);
      }
      DosPutMessage((HFILE)1, cb, buf);
   }
   if (g.hfLog) DosPutMessage(g.hfLog, cb, buf);
}


/* --------------------------------------------------------------------------
 Print the progress bar.
- Parameters -------------------------------------------------------------
 ULONG cbData : amount of data processed in the current step.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
VOID printProgress(ULONG cbData) {
   ULONG curstep;
   CHAR buf[80];
   CHAR bar[] = "Û°±²";
   INT len;

   if (g.mode & FSJPROGRESS)
   {
      g.cbProcData += cbData;
      curstep = (ULONG)(g.cbProcData * 160LL / g.cbWholeFile);
      if (curstep > g.prevstep)
      {
         while (g.prevstep < curstep)
         {
            g.prevstep++;
            VioWrtCharStr(bar + (g.prevstep % 4), 1,
                          IROW_BAR, (g.prevstep - 1) / 4, 0);
         }
         len = sprintf(buf, SZ_PROGRESS, ((curstep * 5.0) / 16.0));
         VioWrtCharStr(buf, len, IROW_PERCENT, 1, 0);
      }
   }
}


/* --------------------------------------------------------------------------
 Return details about the last pressed key as an ULONG value.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 ULONG : selected data from the KBDKEYINFO data structure.
-------------------------------------------------------------------------- */
ULONG kbdKey(VOID) {
   KBDKEYINFO kki;
   if (KbdCharIn(&kki, 0, 0)) return 0;
   return kki.chChar | (kki.chScan<<8) | (kki.fbStatus<<16) | (kki.fsState<<24);
}


// /* --------------------------------------------------------------------------
//  Get the current cursor line.
// - Parameters -------------------------------------------------------------
//  VOID
// - Return value -----------------------------------------------------------
//  INT : current cursor line (0 is the top line, -1 in case of error).
// -------------------------------------------------------------------------- */
// INT screenLine(VOID) {
//    USHORT row, col;
//    if (VioGetCurPos(&row, &col, 0))
//       return -1;
//    return row;
// }
//

/* --------------------------------------------------------------------------
 Display the message press any key to continue.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
VOID pause(VOID) {
   DosPutMessage(1, strlen(SZ_PRESSANYKEY), SZ_PRESSANYKEY);
   kbdKey();
}


/* --------------------------------------------------------------------------
 Get console screen size in columns (cx) and rows (cy).
- Parameters -------------------------------------------------------------
 PCONSIZE
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
BOOL screenSize(PSIZEL psize) {
   USHORT aus[4];
   aus[0] = 8;
   g.rc = VioGetMode((PVIOMODEINFO)aus, 0);
   if (g.rc)
      return handleError(FALSE, TRUE, SZERR_GETSCREENSIZE, g.rc);

   psize->cx = ((PVIOMODEINFO)aus)->col;
   psize->cy = ((PVIOMODEINFO)aus)->row;
   return TRUE;
}


/* --------------------------------------------------------------------------
 Clear the screen.
- Parameters -------------------------------------------------------------
 ULONG attribute : foreground/background colors attribute.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
BOOL screenClear(ULONG attribute) {
   CHAR buf[4];
   buf[0] = ' ';
   buf[1] = attribute;
   if (VioScrollUp(0, 0, 0xffff, 0xffff, 0xffff, buf, 0))
      return FALSE;
   VioSetCurPos(0, 0, 0);
   return TRUE;
}


/* --------------------------------------------------------------------------
 Count the lines contained in a file.
- Parameters -------------------------------------------------------------
 PSZ pszData : file data.
- Return value -----------------------------------------------------------
 ULONG count of lines.
-------------------------------------------------------------------------- */
ULONG countLines(PSZ pszData) {
   ULONG cLines;

   for (cLines = 1; *pszData; ++pszData)
   {
      if (*pszData == '\r')
      {
         if (pszData[1] == '\n') ++pszData;
         cLines++;
      }
      else if (*pszData == '\n')
      {
         cLines++;
      }
   }
   return cLines;
}


/* --------------------------------------------------------------------------
 Handle file I/O errors.
- Parameters -------------------------------------------------------------
 ULONG retv : value to be returned.
 PSZ msg    : error message holding formatting characters.
 ...        : variable number of parameters.
- Return value -----------------------------------------------------------
 ULONG : the value of the 'retv' parameter.
-------------------------------------------------------------------------- */
ULONG handleFileIOError(ULONG retv, PSZ msg, PSZ pszFile, APIRET rc) {
   CHAR buf[CBMSG_BUF];
   ULONG cb;

   cb = sprintf(buf, SZ_ERRPREFIX);
   cb += sprintf(buf + cb, msg, pszFile);
   cb += sprintf(buf + cb, SZERR_TYPEHELPSYS, rc);
   printError(buf, cb);

   return retv;
}


/* --------------------------------------------------------------------------
 Open a file according to the required opening mode.
 Display an error message in case of error.
- Parameters -------------------------------------------------------------
 PSZ pszFile : file name.
 LONGLONG cb : size of the file (used only when creating a new file).
 ULONG flag  : opening mode flag (see constants defined in fileUtil.h).
- Return value -----------------------------------------------------------
 HFILE : handle of the opened file or FMO_ERROR in case of error.
-------------------------------------------------------------------------- */
HFILE fmOpen(PSZ pszFile, LONGLONG cb, ULONG flag) {
   HFILE hf;
   ULONG ul;

   g.rc = DosOpenL(pszFile, &hf, &ul, cb, 0,
                   MKOPENFLAG(flag), MKOPENMODE(flag), NULL);
   if (g.mode & FSJVERBOSE)
      printMsg(IROW_DEBUG, SZ_FOPEN, pszFile, hf, g.rc);
   if (g.rc == 110) g.rc = ERROR_FILE_NOT_FOUND;
   if (g.rc)
      return (HFILE)handleFileIOError(FMO_ERROR, SZERR_OPENFILE, pszFile, g.rc);

   return hf;
}


/* --------------------------------------------------------------------------
 Close a file.
- Parameters -------------------------------------------------------------
 PSZ pszFile : file name.
 ULONG flag  : opening mode flag (see constants defined in fileUtil.h).
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
BOOL fmClose(HFILE hf) {
   if (g.mode & FSJVERBOSE)
      printMsg(IROW_DEBUG, SZ_FCLOSE, hf);
   return !DosClose(hf);
}


/* --------------------------------------------------------------------------
 Read 'cb' bytes from the file 'hf' into the buffer 'pBuffer'.
 Display an error message in case of error.
- Parameters -------------------------------------------------------------
 HFILE hf      : file handle.
 PVOID pBuffer : read buffer.
 ULONG cb      : count of bytes to be read.
- Return value -----------------------------------------------------------
 LONG : count of succesfully read bytes (-1 in case of error).
-------------------------------------------------------------------------- */
LONG fmRead(HFILE hf, PVOID pBuffer, ULONG cb) {
   ULONG cbRead;

   g.rc = DosRead(hf, pBuffer, cb, &cbRead);
   if (g.mode & FSJVERBOSE)
      printMsg(IROW_DEBUG, SZ_FREAD, hf, cb, cbRead, g.rc);
   if (g.rc)
      return handleFileIOError(-1, SZERR_READFILE, "", g.rc);

   return cbRead;
}


/* --------------------------------------------------------------------------
 Write 'cb' bytes from the buffer 'pBuffer' to the file 'hf'.
 Display an error message in case of error.
- Parameters -------------------------------------------------------------
 HFILE hf      : file handle.
 PVOID pBuffer : read buffer.
 ULONG cb      : count of bytes to be read.
- Return value -----------------------------------------------------------
 ULONG : count of successfully written bytes (0 in case of error).
-------------------------------------------------------------------------- */
ULONG fmWrite(HFILE hf, PVOID pBuffer, ULONG cb) {
   ULONG cbWritten;

   g.rc = DosWrite(hf, pBuffer, cb, &cbWritten);
   if (g.mode & FSJVERBOSE)
      printMsg(IROW_DEBUG, SZ_FWRITE, hf, cb, cbWritten, g.rc);
   if (g.rc)
      return handleFileIOError(0, SZERR_WRITEFILE, "", g.rc);
   if (cbWritten != cb)
   {
      g.rc = ERROR_WRITE_FAULT;
      handleError(FALSE, FALSE, SZERR_WRITEFILE2, cb, cbWritten);
   }
   return cbWritten;
}


/* --------------------------------------------------------------------------
 Return the size of a file.
- Parameters -------------------------------------------------------------
 PSZ pszFile : file name.
- Return value -----------------------------------------------------------
 LONGLONG : file size or -1 in case of error.
-------------------------------------------------------------------------- */
LONGLONG fmFileSize(PSZ pszFile) {
   FILESTATUS3L fs;

   g.rc = DosQueryPathInfo(pszFile, FIL_STANDARDL, &fs, sizeof(fs));
   if (g.rc == NO_ERROR) return fs.cbFile;

   handleFileIOError(0, SZERR_FILESIZE2, pszFile, g.rc);
   return -1LL;
}


/* --------------------------------------------------------------------------
 Return the size of a file from its handle.
- Parameters -------------------------------------------------------------
 HFILE hf      : file handle.
 PVOID pBuffer : read buffer.
 ULONG cb      : count of bytes to be read.
- Return value -----------------------------------------------------------
 LONGLONG : file size or -1 in case of error.
 ------------------------------------------------------------------------- */
LONGLONG fmSize(HFILE hf) {
   FILESTATUS3L fs;

   g.rc = DosQueryFileInfo(hf, FIL_STANDARDL, &fs, sizeof(fs));
   if (g.rc == NO_ERROR) return fs.cbFile;

   handleFileIOError(0, SZERR_FILESIZE, "", g.rc);
   return -1LL;
}


/* --------------------------------------------------------------------------
 Return the amount of available space on a disk volume.
- Parameters -------------------------------------------------------------
 PSZ pszPath : path to be queried for the amount of free space.
- Return value -----------------------------------------------------------
 LONGLONG amount of available free space (-1 in case of error).
-------------------------------------------------------------------------- */
LONGLONG fmAvailSpace(PSZ pszPath, PULONG pCbSector) {
   FSALLOCATE fsa;
   ULONG ulDrvNum, ulDrvMap;

   if (!pszPath || (pszPath[1] != ':'))
      DosQueryCurrentDisk(&ulDrvNum, &ulDrvMap);
   else
      ulDrvNum = *pszPath & ~0x20 - 'A' + 1;
   g.rc = DosQueryFSInfo(ulDrvNum, FSIL_ALLOC, &fsa, sizeof(fsa));
   if (g.rc)
   {
      handleError(FALSE, FALSE, SZERR_QUERYFREESPACE, ulDrvNum + 'A' - 1, g.rc);
      return -1LL;
   }
   if (pCbSector) *pCbSector = fsa.cbSector;
   return   (LONGLONG)fsa.cbSector * (LONGLONG)fsa.cSectorUnit
          * (LONGLONG)fsa.cUnitAvail;
}


/* --------------------------------------------------------------------------
 Calculate the space used by a file according to the size of the disk sector.
- Parameters -------------------------------------------------------------
 LONGLONG cbFile : file size.
 ULONG cbSector  : sector size.
- Return value -----------------------------------------------------------
 LONGLONG : disk space used by the file.
-------------------------------------------------------------------------- */
LONGLONG fmFileUsedSpace(LONGLONG cbFile, ULONG cbSector) {
   return (cbFile / cbSector) * cbSector + ((cbFile % cbSector) > 0);
}


/* --------------------------------------------------------------------------
 Build the name of a file part.
- Parameters -------------------------------------------------------------
 PSZ pBuf        : buffer for the name of the file part.
 PFILEMASK pMask : mask containing the specification for the file part name.
 ULONG iPart     : part index.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
VOID makePartName(PSZ pBuf, PFILEMASK pMask, ULONG iPart) {

   sprintf(pBuf, "%s%0*d%s",
           pMask->pStart, pMask->cQMarks, iPart, pMask->pPastQMarks);
}


/* --------------------------------------------------------------------------
 Make a join file part name from the data in the JOINDATA structure.
- Parameters -------------------------------------------------------------
 PSZ pszBuf : buffer where to write the name of 'iFile'-th file part.
 INT iFile  : current file part index.
 INT cbPath : -1 on initialization, or length of the common path.
- Return value -----------------------------------------------------------
 INT : lenght of the optional common path, 0 if there is no common path.
-------------------------------------------------------------------------- */
INT makeJoinPartName(PSZ pszBuf, INT iFile, INT cbPath) {

   if (g.mode & FSJJOINMASK)
   {
      makePartName(pszBuf, &g.join.mask, iFile);
   }
   else
   {
      if (cbPath < 0) // initialization
      {
         if (g.join.pszPiecesPath)
         {
            cbPath = strlen(g.join.pszPiecesPath);
            memcpy(pszBuf, g.join.pszPiecesPath, cbPath);
            // add a trailing slash if needed
            if ((pszBuf[cbPath - 1] != '\\') && (pszBuf[cbPath - 1] != '/'))
               pszBuf[cbPath++] = '\\';
         }
         else
         {
            cbPath = 0;
         }
      }
      strcpy(pszBuf + cbPath, g.join.list.apszPieces[iFile - 1]);
   }
   return cbPath;
}


/* --------------------------------------------------------------------------
 Display a menu of choices, returning the ordinal (0 based) of the selected
 choice.
- Parameters -------------------------------------------------------------
 INT iRow      : line position (0 = top of screen, -1 = current position).
 PSZ pszPrompt : prompt message.
 PSZ choices   : array of choice characters (max 5 characters).
 PMENUFN pfunc : optional callback function used when the menu has a
                 'retry' item. When pfunc() returns FALSE the loop is
                 terminated.
 PVOID pparm   : optional pfunc() parameters.
- Return value -----------------------------------------------------------
 ULONG : number of the selected choice.
- Note: ------------------------------------------------------------------
 If pszPrompt is NULL the area where the prompt message is printed is
 cleared and no other action is performed.
-------------------------------------------------------------------------- */
ULONG showMenu(PSZ pszPrompt, PSZ choices) {
   CHAR achscr[1760];
   CHAR achmenu[1760];
   CHAR buf[4];
   ULONG key;
   USHORT row, col, irow, cb;
   PSZ pChoice, pLine, pEnd;

   buf[0] = ' ';
   buf[1] = VIOB_GRAY | VIOF_DARKBLUE;
   DosBeep(220, 50);
   DosBeep(440, 50);
   DosBeep(880, 50);
   cb = sizeof(achscr);
   VioReadCellStr(achscr, &cb, 7, 0, 0);
   VioScrollUp(7, 0, 17, 80, 11, buf, 0);
   sprintf(achmenu, pszPrompt,
           choices[0], choices[1], choices[2], choices[3], choices[4]);
   VioGetCurPos(&row, &col, 0);
   for (pLine = pEnd = achmenu, irow = 8; pEnd; ++irow)
   {
      if (NULL != (pEnd = strchr(pLine, '\r')))
      {
         *pEnd = '\0';
         if (*++pEnd == '\n') ++pEnd;
      }
      // skip empty lines
      if (*pLine)
         VioWrtCharStrAtt(pLine, strlen(pLine), irow, 0, buf + 1, 0);
      pLine = pEnd;
   }
   VioSetCurPos(irow, 0, 0);
//   VioWrtCharStrAtt(pszPrompt, strlen(pszPrompt), 8, 0, buf + 1, 0);
   for (;;)
   {
      key = kbdKeyChar(kbdKey()) & ~0x20;
      if (NULL != (pChoice = strchr(choices, (INT)key)))
         break;
      DosBeep(440, 100);
   }
   VioWrtCellStr(achscr, cb, 7, 0, 0);
   VioSetCurPos(row, col, 0);
   return (ULONG)(pChoice - choices);
}


// /* --------------------------------------------------------------------------
//  Return the amount of milliseconds since the OS boot.
// - Parameters -------------------------------------------------------------
//  VOID
// - Return value -----------------------------------------------------------
//  ULONG time in milliseconds.
// -------------------------------------------------------------------------- */
// ULONG uptime(VOID) {
//    ULONG ul;
//    DosQuerySysInfo(QSV_MS_COUNT, QSV_MS_COUNT, &ul, sizeof(ULONG));
//    return ul;
// }
//
//
// /* --------------------------------------------------------------------------
//  Returns TRUE if the file 'pszFile' exists.
//  If 'pszPath' is not NULL and the file exists gets its full path.
// - Parameters -------------------------------------------------------------
//  PSZ pszFile : name of the file to be checked for existance.
// - Return value -----------------------------------------------------------
//  BOOL : TRUE/FALSE (existing/non existing file)
// -------------------------------------------------------------------------- */
// BOOL fmFileExists(PSZ pszFile) {
//    HDIR hdir = HDIR_CREATE;
//    FILEFINDBUF3 ffb;
//    ULONG ul = 1;
//    g.rc = DosFindFirst(pszFile, &hdir, FILE_READONLY | FILE_ARCHIVED,
//                        &ffb, sizeof(ffb), &ul, FIL_STANDARD);
//    if (g.rc) return FALSE;
//    DosFindClose(hdir);
//    return TRUE;
// }
//
//
/* --------------------------------------------------------------------------
 Check if there is enough available disk space.
- Parameters -------------------------------------------------------------
 LONGLONG cbReq  : required disk space.
 PSZ pOutPath    : output path (used to determine which disk must be checked).
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
BOOL checkSpaceLoop(LONGLONG cbReq, PSZ pOutPath) {
   LONGLONG cbAvail;

   while (cbReq > (cbAvail = fmAvailSpace(pOutPath, NULL)))
   {
      // unattended mode this is a critical error
      if (g.mode & FSJUNATTENDED)
      {
         g.rc = ERROR_DISK_FULL;
         return handleError(FALSE, FALSE, SZERR_DISKFULL);
      }
      // let the user retry after having freed disk space
      if (DISKFULL_ABORT == showMenu(SZ_DISKFULL, ACH_RETRYABORT))
         return FALSE;
   }
   return (cbAvail > 0);  // since fmAvailSpace() returns -1 in case of error
}


