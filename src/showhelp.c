/* --------------------------------------------------------------------------
 showhelp.c :

 28 Sep 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

// includes -----------------------------------------------------------------
#include "main.h"

// definitions --------------------------------------------------------------

// prototypes ---------------------------------------------------------------
static VOID tbFree(PTEXTBROWSER ptb);
static BOOL makeLinesArrray(PTEXTBROWSER ptb);
static BOOL readFile(PSZ pszFile, PTEXTBROWSER ptb);
static VOID printHeader(ULONG cols);
static VOID printStatusLine(PTEXTBROWSER ptb);
static BOOL scrollUp(PTEXTBROWSER ptb, ULONG flag);
static BOOL scrollDown(PTEXTBROWSER ptb, ULONG flag);
static VOID printLines(PTEXTBROWSER ptb, INT iPrint, INT iRow, INT cPrint);
static VOID showFile(PTEXTBROWSER ptb);

// global variables ---------------------------------------------------------


/* --------------------------------------------------------------------------
 Store the background intensity/blink flag.
- Parameters -------------------------------------------------------------
 PTEXTBROWSER ptb : text browser data.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
BOOL tbInit(PTEXTBROWSER ptb) {
   VIOINTENSITY vi;

   memset(ptb, 0, sizeof(TEXTBROWSER));

   // get the screen size in row and columns
   if (!screenSize((PSIZEL)ptb))
      return FALSE;

   vi.cb = sizeof(vi);
   vi.type = 2;
   vi.fs = 0;
   // this call is valid only when in full screen
   if (NO_ERROR == VioGetState(&vi, 0))
   {
      // if the screen is in blink mode set it to high intensity background
      if (!vi.fs)
      {
         vi.fs = 1;
         VioSetState(&vi, 0);
         ptb->flag |= TBRESET_INTENSITY;
      }
   }
   return TRUE;
}


/* --------------------------------------------------------------------------
 Free the allocated resources, reset the intensity flag to its original
 state and clear the screen.
- Parameters -------------------------------------------------------------
 PTEXTBROWSER ptb : text browser data.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
VOID tbEnd(PTEXTBROWSER ptb) {
   VIOINTENSITY vi;

   tbFree(ptb);

   if (ptb->flag & TBRESET_INTENSITY)
   {
      vi.cb = sizeof(vi);
      vi.type = 2;
      vi.fs = 0;
      VioSetState(&vi, 0);
   }

   if (ptb->flag & TBCLEAR_SCREEN)
   {
      screenClear(VIOB_BLACK | VIOF_BLACK);
   }
}


/* --------------------------------------------------------------------------
 Free the resources allocated via DosAllocMem.
- Parameters -------------------------------------------------------------
 PTEXTBROWSER ptb : text browser data.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static VOID tbFree(PTEXTBROWSER ptb) {
   if (ptb->pData) memFree(ptb->pData);
   if (ptb->aLines) memFree(ptb->aLines);
}


/* --------------------------------------------------------------------------
 Build the array of text line pointers.
- Parameters -------------------------------------------------------------
 PTEXTBROWSER ptb : text browser data.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static BOOL makeLinesArrray(PTEXTBROWSER ptb) {
   PSZ p;
   ULONG i;

   ptb->cLines = countLines(ptb->pData);
   ptb->aLines = memAllocTiled(sizeof(PSZ) * ptb->cLines);
   if (ptb->aLines)
   {
      ptb->iLine = 0;
      for (i = 1, p = ptb->aLines[0] = ptb->pData; *p; ++p)
      {
         if (*p == '\r')
         {
            *p = '\0';
            if (p[1] == '\n') ++p;
            ptb->aLines[i++] = p + 1;
         }
         else if (*p == '\n')
         {
            *p = '\0';
            ptb->aLines[i++] = p + 1;
         }
      }
   }
   return (BOOL)ptb->aLines;
}


/* --------------------------------------------------------------------------
 Read a text file returning the file content, the count of lines, and an
 array of pointers to the text lines.
- Parameters -------------------------------------------------------------
 PSZ pszFile      : name of the file to be read.
 PTEXTBROWSER ptb : text browser data.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static BOOL readFile(PSZ pszFile, PTEXTBROWSER ptb) {
   HFILE hf;
   LONGLONG cb;

   g.rc = NO_ERROR;
   if (FMO_ERROR == (hf = fmOpen(pszFile, 0, FMO_IFEXIST | FMO_SHAREREAD)))
      return FALSE;

   if (0 > (cb = fmSize(hf))) goto exit_0;
   if (NULL == (ptb->pData = memAllocTiled(cb + 1))) goto exit_0;
   if (0 > fmRead(hf, ptb->pData, cb)) goto exit_1;
   ptb->pData[cb] = '\0';
   if (makeLinesArrray(ptb)) goto exit_0;

exit_1:
   tbFree(ptb);
exit_0:
   fmClose(hf);
   return !g.rc;
}


/* --------------------------------------------------------------------------
 Print the file browsing header.
- Parameters -------------------------------------------------------------
 ULONG cols : screen width in columns.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static VOID printHeader(ULONG cols) {
   CHAR buf[256];
   ULONG cb;
   BYTE attribute;

   cb = strlen(SZ_TBHEADER);
   memcpy(buf, SZ_TBHEADER, cb);
   memset(buf + cb, ' ', 256 - cb);
   attribute = VIOF_BLACK | VIOB_GRAY;
   VioWrtCharStrAtt(buf, cols, 0, 0, &attribute, 0);
}


/* --------------------------------------------------------------------------
 Print the file browsing status line.
- Parameters -------------------------------------------------------------
 PTEXTBROWSER ptb : text browser data.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static VOID printStatusLine(PTEXTBROWSER ptb) {
   CHAR buf[256];
   ULONG cb;
   BYTE attribute;

   cb = sprintf(buf, SZ_TBSTATUS, ptb->iLine + 1, ptb->cLines);
   if (!ptb->iLine)
   {
      cb += sprintf(buf + cb, SZ_TBFILESTART);
   }
   else if (ptb->iLine + ptb->rows - 2 == ptb->cLines)
   {
      cb += sprintf(buf + cb, SZ_TBFILEEND);
   }
   memset(buf + cb, ' ', 256 - cb);
   attribute = VIOF_BLACK | VIOB_GRAY;
   VioWrtCharStrAtt(buf, ptb->cols, ptb->rows - 1, 0, &attribute, 0);
}


/* --------------------------------------------------------------------------
 Scroll the text up according to the amount specified by flag.
- Parameters -------------------------------------------------------------
 PTEXTBROWSER ptb : text browser data.
 ULONG flag       : amount to scroll (line/page/all).
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (scroll done/aborted).
-------------------------------------------------------------------------- */
static BOOL scrollUp(PTEXTBROWSER ptb, ULONG flag) {
   APIRET rc;
   CHAR buf[4];
   LONG cScroll, iPrint, cRows;

   cRows = ptb->rows - 2;    // leave 2 lines for header and status bar

   buf[0] = ' ';
   buf[1] = VIOF_BLUE | VIOB_WHITE;

   // count how many lines are below the last visible line
   cScroll = ptb->cLines - (ptb->iLine + cRows);
   if (cScroll <= 0) return FALSE;

   iPrint = ptb->iLine + cRows;
   // count how many lines are to be scrolled
   switch (flag)
   {
   case SCROLL_ONE:              // scroll one line
      cScroll = 1;
      ptb->iLine++;
      break;
   case SCROLL_PAGE:             // scroll one page less one line
      if (cScroll >= cRows) cScroll = cRows - 1;
      ptb->iLine += cScroll;
      break;
   case SCROLL_ALL:              // scroll to the end of the file
      iPrint = ptb->iLine + cRows;
      ptb->iLine += cScroll;
      if (cScroll > cRows)
      {
         cScroll = cRows;
         iPrint = ptb->cLines - cRows;
      }
      break;
   }

   rc = VioScrollUp(1, 0, cRows, ptb->cols, cScroll, buf, 0);
   printLines(ptb, iPrint, cRows - cScroll + 1, cScroll);

   return TRUE;
}


/* --------------------------------------------------------------------------
 Scroll the text down according to the amount specified by flag.
- Parameters -------------------------------------------------------------
 PTEXTBROWSER ptb : text browser data.
 ULONG flag       : amount to scroll (line/page/all).
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (scroll done/aborted).
-------------------------------------------------------------------------- */
static BOOL scrollDown(PTEXTBROWSER ptb, ULONG flag) {
   APIRET rc;
   CHAR buf[4];
   LONG cScroll, cRows;

   cRows = ptb->rows - 2;    // leave 2 lines for header and status bar

   buf[0] = ' ';
   buf[1] = VIOF_BLUE | VIOB_WHITE;

   // if already at the start of the file there is nothing to do
   if (!ptb->iLine) return FALSE;

   // count how many lines are to be scrolled
   switch (flag)
   {
   case SCROLL_ONE:              // scroll one line
      cScroll = 1;
      break;
   case SCROLL_PAGE:             // scroll one page less one line
      cScroll = cRows - 1;
      if (cScroll > ptb->iLine) cScroll = ptb->iLine;
      break;
   case SCROLL_ALL:              // scroll to the start of the file
      cScroll = ptb->iLine;
      break;
   }

   rc = VioScrollDn(1, 0, cRows, ptb->cols, cScroll, buf, 0);
   printLines(ptb, ptb->iLine - cScroll, 1, min(cScroll, cRows));
   ptb->iLine -= cScroll;

   return !rc;
}


/* --------------------------------------------------------------------------
 Print a few lines of text.
- Parameters -------------------------------------------------------------
 PTEXTBROWSER ptb : text browser data.
 INT iPrint       : index of the first text line to be printed.
 INT iRow         : row position of the first line to be printed
 INT cPrint       : count of lines to be printed.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static VOID printLines(PTEXTBROWSER ptb, INT iPrint, INT iRow, INT cPrint) {
   INT i;
   BYTE attribute;
   ULONG cb;
   attribute = VIOF_BLUE | VIOB_WHITE;
   for (i = 0; i < cPrint; ++i, ++iPrint, ++iRow)
   {
      cb = strlen(ptb->aLines[iPrint]);
      if (cb > ptb->cols) cb = ptb->cols;
      VioWrtCharStrAtt(ptb->aLines[iPrint], cb, iRow, 0, &attribute, 0);
   }
}


/* --------------------------------------------------------------------------
 Show the file allowing to scroll it via the arrow keys.
- Parameters -------------------------------------------------------------
 PTEXTBROWSER ptb : text browser data.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static VOID showFile(PTEXTBROWSER ptb) {
   ULONG keycode;
   BOOL scrollDone;

   ptb->flag |= TBCLEAR_SCREEN;
   // clear the screen
   screenClear(VIOF_BLACK | VIOB_WHITE);
   // print the header
   printHeader(ptb->cols);
   // print the status line
   printStatusLine(ptb);
   // print the first page of the file
   printLines(ptb, 0, 1, min(ptb->cLines, ptb->rows - 2));
   VioSetCurPos(1, 0, 0);
   // enter in the browse loop until the user press the Esc key
   for (;;)
   {
      keycode = kbdKey();
      switch (keycode & KEY_SCANCODEMASK)
      {
      case KEY_ESC:
         return;
      case KEY_HOME:
         scrollDone = scrollDown(ptb, SCROLL_ALL);
         break;
      case KEY_UP:
         scrollDone = scrollDown(ptb, SCROLL_ONE);
         break;
      case KEY_PAGE_UP:
         scrollDone = scrollDown(ptb, SCROLL_PAGE);
         break;
      case KEY_END:
         scrollDone = scrollUp(ptb, SCROLL_ALL);
         break;
      case KEY_DOWN:
         scrollDone = scrollUp(ptb, SCROLL_ONE);
         break;
      case KEY_PAGE_DOWN:
         scrollDone = scrollUp(ptb, SCROLL_PAGE);
         break;
      default :
         scrollDone = FALSE;;
      }
      if (scrollDone)
         printStatusLine(ptb);
      else
         DosBeep(440, 100);
   }
}


/* --------------------------------------------------------------------------
 Open a text file and load it in the text browser.
- Parameters -------------------------------------------------------------
 PSZ pszFile      : name of the file to be read and displayed in the browser.
 PTEXTBROWSER ptb : text browser data.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
BOOL tbBrowseTextFile(PSZ pszFile, PTEXTBROWSER ptb) {
   CHAR buf[CCHMAXPATH];
   PPIB ppib;
   PTIB ptib;
   PSZ pExt;

   // file name not provided, assume it is the executable name with a .txt
   // extension
   if (!pszFile)
   {
      DosGetInfoBlocks(&ptib, &ppib);
      DosQueryModuleName(ppib->pib_hmte, CCHMAXPATH, buf);
      // point to .exe handling the case of an extension-less executable
      pExt = strrchr(buf, '\\');
      if (!pExt) pExt = buf;
      pExt = strrchr(pExt, '.');
      if (!pExt) pExt = buf + strlen(buf);
      memcpy(pExt, ".txt", 5);
      pszFile = buf;
   }
   // read the file and display it
   if (!readFile(pszFile, ptb)) return FALSE;
   showFile(ptb);
   return TRUE;
}
