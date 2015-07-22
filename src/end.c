/* --------------------------------------------------------------------------
 end.c :

 8 Oct 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

// includes -----------------------------------------------------------------
#include "main.h"

// definitions --------------------------------------------------------------
// prototypes ---------------------------------------------------------------
// global variables ---------------------------------------------------------


/* --------------------------------------------------------------------------
 Free the resources allocated by the application.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
VOID endApplication(VOID) {
   USHORT aus[4];
   if (g.hfWhole != FMO_ERROR) fmClose(g.hfWhole);
   if (g.pBuf) memFree(g.pBuf);
   // if in join mode frees the allocated data
   if ((g.mode & FSJJOIN) && !(g.mode & FSJJOINMASK))
   {
      if (g.join.list.pFileData) memFree(g.join.list.pFileData);
      if (g.join.list.apszPieces) memFree(g.join.list.apszPieces);
   }
   // if the screen size changed reset it to the original size
   if (g.mode & FSJ_RESETSCREEN)
   {
      ((PVIOMODEINFO)aus)->cb = sizeof(aus);
      if (!VioGetMode((PVIOMODEINFO)aus, 0))
      {
         ((PVIOMODEINFO)aus)->col = g.cols;
         ((PVIOMODEINFO)aus)->row = g.rows;
         VioSetMode((PVIOMODEINFO)aus, 0);
      }
   }
   VioSetCurPos(g.rows - 1, 0, 0);
   // print error/success message
   if (g.mode & FSJ_RUNNING)
   {
      if (g.rc)
      {
         if (g.mode & FSJSPLIT)
            printMsg(IROW_CURRENT, SZ_SPLITFAILED, g.split.pszFile, g.cPieces);
         else if (g.mode & FSJJOIN)
            printMsg(IROW_CURRENT, SZ_JOINFAILED, g.split.pszFile, g.cPieces);
      }
      else
      {
         if (g.mode & FSJSPLIT)
            printMsg(IROW_CURRENT, SZ_SPLITOK, g.split.pszFile, g.cPieces);
         else if (g.mode & FSJJOIN)
            printMsg(IROW_CURRENT, SZ_JOINOK, g.split.pszFile, g.cPieces);
      }
      if (g.hfLog != FMO_ERROR) fmClose(g.hfLog);
   }
}


