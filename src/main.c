/* --------------------------------------------------------------------------
 main.c : FSJ.EXE main module.

 16 Jun 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
 Split: splits a file in pieces. Supports files larger than 2GBs.
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

// #pragma strings(readonly)

// includes -----------------------------------------------------------------
#include "main.h"

// definitions --------------------------------------------------------------
// prototypes ---------------------------------------------------------------
static BOOL displayHelp(VOID);
static BOOL split(VOID);
static INT join(VOID);
static HFILE createSplitFilePart(LONG iPart, LONGLONG cbFile);
static BOOL openJoinFilePart(ULONG iPart, PLONGLONG pSize, PHFILE phf);

// global variables ---------------------------------------------------------
GLOBAL g;

// --------------------------------------------------------------------------

INT main(INT argc, PSZ* argv) {

   if (initApplication(argc, argv))
   {
      switch (g.mode & FSJCOMMAND)
      {
      case FSJSPLIT:
         split();
         break;
      case FSJJOIN:
         join();
         break;
      case FSJHELP:
         displayHelp();
         break;
      }
   }
   endApplication();

   return g.rc;
}


/* --------------------------------------------------------------------------
 Display the program documentation in a scrollable window.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL displayHelp(VOID) {
   TEXTBROWSER tb;
   BOOL rc;

   if (!tbInit(&tb)) return FALSE;
   rc = tbBrowseTextFile(NULL, &tb);
   tbEnd(&tb);

   return rc;
}


/* --------------------------------------------------------------------------
 Split a file in multiple file pieces.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
- Remarks ----------------------------------------------------------------
 The file to be splitted was opened in init.c/isSplitFileValid().
-------------------------------------------------------------------------- */
static
BOOL split(VOID) {
   LONGLONG cbPiece;
   LONG cbAvail, cbWrite;
   PSZ p;
   HFILE hfPart;
   INT i;

   printMsg(IROW_MAINMSG, SZ_SPLITTING, g.split.pszFile, g.cPieces, g.cbPiece);
   // loop through all the file pieces
   for (cbAvail = 0, cbPiece = g.cbPiece, i = 1; i <= g.cPieces; ++i)
   {
      // if this is the last piece its size may be less than the other ones
      if (i == g.cPieces)
         cbPiece = g.cbWholeFile - (g.cbPiece * (g.cPieces - 1));
      // create a new file piece
      hfPart = createSplitFilePart(i, cbPiece);
      if (hfPart == FMO_ERROR) return FALSE;
      for (;;)
      {
         // if the buffer is empty fill it
         if (!cbAvail)
         {
            if (0 > (cbAvail = fmRead(g.hfWhole, g.pBuf, g.cbBuf)))
               goto error_exit;
            p = g.pBuf;
            printProgress(cbAvail);
         }
         // flush the buffer
         cbWrite = cbAvail > cbPiece ? cbPiece : cbAvail;
         if (!fmWrite(hfPart, p, cbWrite)) goto error_exit;
         p += cbWrite;
         cbAvail -= cbWrite;
         cbPiece -= cbWrite;
         printProgress(cbWrite);
         // if the file piece is complete close it and go on with the next file
         if (!cbPiece)
         {
            fmClose(hfPart);
            cbPiece = g.cbPiece;
            break;
         }
      }
   }
   return TRUE;

error_exit:
   fmClose(hfPart);
   return FALSE;
}


/* --------------------------------------------------------------------------
 Join a list of files into a unique file.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 INT 0 (success) or other error code.
-------------------------------------------------------------------------- */
static
INT join(VOID) {
   LONGLONG cbPiece;
   LONG cbAvail, cbRead;
   PSZ p;
   HFILE hfPart;
   INT i;

   // open the output file to fill with the content of the file parts
   printMsg(IROW_MAINMSG, SZ_JOINING, g.cPieces, g.cbPiece, g.join.pszFile);
   g.hfWhole = fmOpen(g.join.pszFile,
                      ((g.mode & FSJ_CHECK1STSIZE) ? 0: g.cbWholeFile),
                      FMO_CREATE | FMO_REPLACE | FMO_SHAREREAD | FMO_WRITE);
   if (g.hfWhole == FMO_ERROR) return FALSE;

   for (cbAvail = g.cbBuf, p = g.pBuf, i = 1; i <= g.cPieces; ++i)
   {
      if (!openJoinFilePart(i, &cbPiece, &hfPart))
         return FALSE;
      // fill the buffer with the content of the current file
      for (;;)
      {
         if (0 > (cbRead = fmRead(hfPart, p, cbAvail))) goto error_exit;
         printProgress(cbRead);
         cbAvail -= cbRead;
         p += cbRead;
         // buffer full : flush it and reset the buffer avail. size and pointer
         if (!cbAvail)
         {
            if (!fmWrite(g.hfWhole, g.pBuf, g.cbBuf)) goto error_exit;
            cbAvail = g.cbBuf, p = g.pBuf;
            printProgress(g.cbBuf);
         }
         cbPiece -= cbRead;
         // if the file has been completely transferred to the buffer
         // close it and go on with the next file
         if (!cbPiece)
         {
            fmClose(hfPart);
            cbPiece = g.cbPiece;
            break;
         }
      }
   }
   // if there is still some unflushed data on the buffer write it
   if (g.cbBuf - cbAvail)
   {
      if (!fmWrite(g.hfWhole, g.pBuf, g.cbBuf - cbAvail)) goto error_exit;
      printProgress(g.cbBuf - cbAvail);
   }
   return TRUE;

error_exit:
   fmClose(hfPart);
   return FALSE;
}


/* --------------------------------------------------------------------------
 Open a new file piece:
 - build the file part name.
 - show details about the file being written, pausing if required.
 - check if there is enough disk space.
 - open the file
- Parameters -------------------------------------------------------------
 LONG iPart      : current file part index.
 LONGLONG cbFile : size of the file.
- Return value -----------------------------------------------------------
 HFILE : file handle or FMO_ERROR.
-------------------------------------------------------------------------- */
static
HFILE createSplitFilePart(LONG iPart, LONGLONG cbFile) {
   CHAR partName[CCHMAXPATH];
   ULONG cb;
   HFILE hf;

   // build the name of the current file piece
   cb = g.split.pszOutPath ? sprintf(partName, "%s\\", g.split.pszOutPath) : 0;
   makePartName(partName + cb, &g.split.mask, iPart);

   // notify the user of the file part which is going to be written
   printMsg(IROW_MSG, SZ_OPENFILEPART, iPart, partName);
   if (g.mode & FSJPAUSE) pause();

   // check if there is enough space for writing the file part
   if (!checkSpaceLoop(cbFile, g.split.pszOutPath)) return FALSE;

   // open the file
   hf = fmOpen(partName, cbFile,
               FMO_CREATE | FMO_REPLACE | FMO_SHARENONE | FMO_WRITE);
   return hf;
}


/* --------------------------------------------------------------------------
 Open a file part returning its handle and its size.
- Parameters -------------------------------------------------------------
 ULONG iPart     : current file part index.
 PLONGLONG pSize : (output) size of the file.
 PHFILE phf      : (output) handle of the opened file.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL openJoinFilePart(ULONG iPart, PLONGLONG pSize, PHFILE phf) {
   CHAR partName[CCHMAXPATH];

   // make the file part name
   makeJoinPartName(partName, iPart, -1);
   printMsg(IROW_MSG, SZ_READFILEPART, iPart, partName);
   if (g.mode & FSJPAUSE) pause();
   while (FMO_ERROR
          ==
          (*phf = fmOpen(partName, 0, FMO_IFEXIST | FMO_SHAREREAD | FMO_READ)))
   {
      // a critical error if the /W (pause) option has not been specified
      if (!(g.mode & FSJPAUSE)) return FALSE;
      // otherwise a retry/abort menu is shown
      if (NOCBFILE_ABORT == showMenu(SZ_PARTNOOPENED, ACH_RETRY_ABORT))
         return FALSE;
   }
   if ((*pSize = fmSize(*phf)) < 0)
   {
      fmClose(*phf);
      return FALSE;
   }
   return TRUE;
}
