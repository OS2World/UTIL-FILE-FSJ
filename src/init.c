/* --------------------------------------------------------------------------
 init.c : program initialization

 22 Jun 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

// includes -----------------------------------------------------------------
#include "main.h"

// definitions --------------------------------------------------------------

// prototypes ---------------------------------------------------------------
static BOOL parmFwdSearch(INT argc, PSZ *argv, INT chOpt, PFN_PARSE_OPT pFunc);
static BOOL parseUnattended(PSZ pszOpt);
static BOOL parseLogFile(PSZ pszOpt);
static BOOL parseCommand(INT argc, PSZ* argv);
static BOOL parseOption(PSZ pszOpt);
static BOOL parseSplitCommand(PSZ pszCmd);
static BOOL parseJoinCommand(PSZ pszCmd);
static BOOL parseBufferOption(PSZ pszOpt);
static BOOL parseBatchFileOption(PSZ pszOpt);
static BOOL parsePiecesPath(PSZ pszOpt);
static BOOL parseFileList(PSZ pszOpt);
static BOOL parseFilePartsMask(PSZ pszOpt);
static BOOL parseOutputOption(PSZ pszOpt);
static BOOL parseRunOption(PSZ pszOpt);
static BOOL parsePauseOption(PSZ pszOpt);
static BOOL parseOrdinaryParm(PSZ parm);
static BOOL areParametersCorrect(VOID);
static BOOL initJobEnv(VOID);
static BOOL checkSplitMode(VOID);
static BOOL checkJoinMode(VOID);
static BOOL isSplitFileValid(VOID);
static BOOL isSpaceEnoughForSplitting(VOID);
static VOID setUnit(INT suffix, LONGLONG cbPiece, ULONG mul1, ULONG mul2);
static LONGLONG formattedNumToLL(PSZ pszNum, PSZ* pEndNum);
static BOOL isFilePartMaskCorrect(PSZ pszName, PFILEMASK pMask);
static VOID checkMaskDigits(PFILEMASK pMask, ULONG cFiles);
static BOOL allocateBuffer(VOID);
static BOOL calcCountOfPieces(VOID);
static BOOL calcSizeOfPiece(VOID);
static BOOL createJoinBatchFile(VOID);

// global variables ---------------------------------------------------------

//---------------------------------------------------------------------------
/* --------------------------------------------------------------------------
 Application initialization:
 - argument parsing and validity check,
 - argument consistency check.
- Parameters -------------------------------------------------------------
 INT argc  : startup parameters count.
 PSZ* argv : startup parameters array.
- Return value -----------------------------------------------------------
 BOOL : TRUE    valid parameter list,
        FALSE   invalid parameter.
-------------------------------------------------------------------------- */
BOOL initApplication(INT argc, PSZ* argv) {
   INT i;

   g.hfWhole = g.hfLog = FMO_ERROR;

   // search the parameters array for the /U (unattended) option to know in
   // advance if prompts for user input have to be disabled
   if (!parmFwdSearch(argc, argv, CHO_UNATTENDED, (PFN_PARSE_OPT)parseUnattended))
      goto error;

   // search the parameters array for the last /L (logfile) option
   if (!parmFwdSearch(argc, argv, CHO_LOGFILE, (PFN_PARSE_OPT)parseLogFile))
      goto error;

   // Check the command parameter (help/split/join)
   if (!parseCommand(argc, argv))
      goto error;
   if (g.mode & FSJHELP) return TRUE;

   // parse all the other parameters
   for (i = 2; i < argc; ++i)
   {
      // option parameters
      if ((argv[i][0] == '-') || (argv[i][0] == '/'))
      {
         if (!parseOption(argv[i])) goto error;
      }
      // non-option parameters
      else
      {
         if (!parseOrdinaryParm(argv[i])) goto error;
      }
   }

   // check if all the needed options have been specified
   if (!areParametersCorrect()) goto error;
   // change the screen settings and allocate the read/write buffer
   if (!initJobEnv()) goto error;

   g.rc = NO_ERROR;
   return TRUE;

error:
   if (!g.rc) g.rc = (APIRET)ERROR_INVALID_PARAMETER;
   return FALSE;
}


/* --------------------------------------------------------------------------
 Search the parameters array for a given option and return the value
 provided by the parse-option callback.
- Parameters -------------------------------------------------------------
 INT argc            : startup parameters count.
 PSZ* argv           : startup parameters array.
 INT chOpt           : character used for the option to be checked.
 PFN_PARSE_OPT pFunc : callback procedure used to parse the option.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL parmFwdSearch(INT argc, PSZ* argv, INT chOpt, PFN_PARSE_OPT pFunc) {

   // search the /opt
   for (--argc; argc > 1; --argc)
   {
      if (   ((argv[argc][0] == '-') || (argv[argc][0] == '/'))
          && ((argv[argc][1] & ~0x20) == chOpt)
         )
      return pFunc(argv[argc]);
   }
   // a missing option cannot have a wrong syntax :-)
   return TRUE;
}


/* --------------------------------------------------------------------------
 Just set the flag corresponding to the /U (unattended) option.
- Parameters -------------------------------------------------------------
 PSZ pszOpt : option string pointer.
- Return value -----------------------------------------------------------
 BOOL : always TRUE.
-------------------------------------------------------------------------- */
static
BOOL parseUnattended(PSZ pszOpt) {
   g.mode |= FSJUNATTENDED;
   return TRUE;
}


/* --------------------------------------------------------------------------
 Store the logfile name. If no log file name was specified use the default
 name (fsj.log) in the environment path "LOGFILES" or in the current path.
- Parameters -------------------------------------------------------------
 PSZ pszOpt : option string pointer.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL parseLogFile(PSZ pszOpt) {
   PSZ p;
   CHAR achLogFile[CCHMAXPATH];
   ULONG cb;

   p = pszOpt + 2;       // point past "/L"
   if (*p == ':') ++p;
   // if no log file name has been specified use the default (fsj.log)
   if (!*p)
   {  // scan the environment for a default logfile path
      if (NO_ERROR == DosScanEnv("LOGFILES", &p))
      {
         cb = strlen(p);
         memcpy(achLogFile, p, cb);
         p = achLogFile + cb - 1;
         if ((*p != '\\') && (*p != '/')) ++p, *p++ = '\\';
      }
      // otherwise just use the current working directory
      else
      {
         p = achLogFile;
      }
      memcpy(p, SZ_DEFLOGFILE, 8);
      p = achLogFile;
   }
   // open the log file
   g.hfLog = fmOpen(p, 0, FMO_CREATE | FMO_REPLACE | FMO_SHAREREAD | FMO_WRITE);
   if (g.hfLog == FMO_ERROR) return FALSE;

   return TRUE;
}


/* --------------------------------------------------------------------------
 Check the command parameter (help/split/join).
- Parameters -------------------------------------------------------------
 INT argc  : startup parameters count.
 PSZ* argv : startup parameters array.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL parseCommand(INT argc, PSZ* argv) {

   // no parameter or H, h, ?
   if ((argc == 1) || memchr(ACH_CMDHELPCHARS, *argv[1], 3))
   {
      g.mode = FSJHELP;
      return TRUE;
   }
   // S - split command
   if ((argv[1][0] & ~0x20) == CHCMD_SPLIT)
      return parseSplitCommand(argv[1]);
   // J - join command
   if ((argv[1][0] & ~0x20) == CHCMD_JOIN)
      return parseJoinCommand(argv[1]);
   // invalid command
   handleError(FALSE, TRUE, SZERR_INVALIDCOMMAND, argv[1], SZERR_VALIDCMDLIST);
   return FALSE;
}


/* --------------------------------------------------------------------------
 Parse all options (i.e. parameters introduced by '-' or '/').
- Parameters -------------------------------------------------------------
 PSZ pszOpt : option string pointer.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL parseOption(PSZ pszOpt) {
   switch (pszOpt[1] & ~0x20)
   {
   case CHO_BUFFER:
      return parseBufferOption(pszOpt);
   case CHO_MAKECMDFILE:
      return parseBatchFileOption(pszOpt);
   case CHO_COMMONPATH:
      return parsePiecesPath(pszOpt);
   case CHO_FILELISTING:
      return parseFileList(pszOpt);
   case CHO_IN_FILEMASK:
      return parseFilePartsMask(pszOpt);
   case CHO_LOGFILE: // log file option has already been processed
      break;
   case CHO_OUT_FILEMASK:
      return parseOutputOption(pszOpt);
   case CHO_PROGRESS:
      g.mode |= FSJPROGRESS;
      break;
   case CHO_QUIET:
      g.mode |= FSJQUIET;
      break;
   case CHO_RUNPRIORITY:
      return parseRunOption(pszOpt);
   case CHO_UNATTENDED: // unattended option has already been processed
      break;
   case CHO_VERBOSE:
      g.mode |= FSJVERBOSE;
      break;
   case CHO_PAUSE:
      return parsePauseOption(pszOpt);
      break;
   }
   return TRUE;
}


/* --------------------------------------------------------------------------
 Parse the split command.
- Parameters -------------------------------------------------------------
 PSZ pszCmd : split command string.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL parseSplitCommand(PSZ pszCmd) {

   PSZ p = pszCmd + 1;    // get past "S"
   LONGLONG sizecount; // size of file piece or count of pieces

   // get the count of pieces or size of each file piece
   if (*p == ':') ++p;
   if (!*p)
      return handleError(FALSE, TRUE,
                         SZERR_INVALIDCOMMAND, pszCmd, SZERR_NOSPLITCOUNT);
   sizecount = formattedNumToLL(p, &p);

   // get the unit of measure
   switch (*p++)
   {
   case '\0':      // divide by pieces
   case 'P':
   case 'p':
      if (sizecount < 2)
         return handleError(FALSE, FALSE,
                            SZERR_INVALIDCOMMAND, pszCmd, SZERR_CANTSPLIT01);
      if (sizecount >= MAX_PIECES_COUNT)
         return handleError(FALSE, TRUE,
                            SZERR_INVALIDCOMMAND, pszCmd,
                            SZERR_TOOMANYPIECES, (ULONG)sizecount);
      g.cPieces = sizecount;
      break;
   case 'B':       // bytes
   case 'b':
      g.mode |=  FSJSPLITSIZE;
      g.cbPiece = sizecount;
      break;
   case 'K':
   case 'k':
      setUnit(*p, sizecount, KB, KiB);
      break;
   case 'M':
   case 'm':
      setUnit(*p, sizecount, MB, MiB);
      break;
   case 'G':
   case 'g':
      setUnit(*p, sizecount, GB, GiB);
      break;
   default:
      return handleError(FALSE, TRUE,
                         SZERR_INVALIDCOMMAND, pszCmd, SZERR_SPLITUNIT);
   }
   g.mode |= FSJSPLIT;
   return TRUE;
}


/* --------------------------------------------------------------------------
 Check the join command.
- Parameters -------------------------------------------------------------
 PSZ pszCmd : join command string.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL parseJoinCommand(PSZ pszCmd) {

   PSZ p = pszCmd + 1;       // point past "J"

   g.mode |= FSJJOIN;

   if (*p == ':') ++p;
   // the name of the file resulting from the join operation has not been
   // specified, use the default name (wholefil.fsj)
   g.join.pszFile = *p ? p : SZ_DEFWHOLEFILE;

   return TRUE;
}


/* --------------------------------------------------------------------------
 Get the read buffer size.
- Parameters -------------------------------------------------------------
 PSZ pszOpt : option string pointer.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL parseBufferOption(PSZ pszOpt) {
   ULONG cbAvailMem;
   PSZ p;

   // consider multiple option specifications a non critical error
   // (keep last specification)
   if (g.cbBuf)
   {
      if (!handleError(TRUE, FALSE, SZERR_OPTDUP, pszOpt))
         return FALSE;
   }

   // check if high memory objects are supported and maximum available memory
   g.rc = DosQuerySysInfo(QSV_VIRTUALADDRESSLIMIT, QSV_VIRTUALADDRESSLIMIT,
                          &cbAvailMem, sizeof(ULONG));
   if (g.rc == NO_ERROR)
   {
      g.mode |= FSJ_MEMHIGHOBJ;
      g.rc = DosQuerySysInfo(QSV_MAXHPRMEM, QSV_MAXHPRMEM,
                             &cbAvailMem, sizeof(ULONG));
   }
   else
   {
      g.rc = DosQuerySysInfo(QSV_MAXPRMEM, QSV_MAXPRMEM,
                             &cbAvailMem, sizeof(ULONG));
   }
   // if cannot get the size of available memory show an error message
   if (g.rc) return handleError(FALSE, FALSE, SZERR_MAXPRMEM);

   cbAvailMem -= 4 * MiB;

   // if "/B" was specified get the parameter value
   if (pszOpt)
   {
      p = pszOpt + 2;
      // check if pszOpt contains a valid numeric value
      if (*p == ':') ++p;
      if ((*p < '0') || (*p > '9'))
         return handleError(FALSE, TRUE,
                            SZERR_INVALIDOPTION, pszOpt, SZERR_BUFFSIZENOTNUM);

      g.cbBuf = (LONG)formattedNumToLL(p, NULL) * MiB;

      if (g.cbBuf <= 0)
         g.cbBuf = CBBUF_MIN;
      else if (g.cbBuf > CBBUF_MAX)
         g.cbBuf = CBBUF_MAX;
   }
   // /B option not specified: use the default value!
   else
   {
      g.cbBuf = CBBUF_DEF;
   }
   g.cbBuf = min(g.cbBuf, cbAvailMem);

   return TRUE;
}


/* --------------------------------------------------------------------------
 Store the name of the batch file to be created.
- Parameters -------------------------------------------------------------
 PSZ pszOpt : option string pointer.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL parseBatchFileOption(PSZ pszOpt) {

   // this option applies only when in split mode (non critical error)
   if (g.mode & FSJJOIN)
      return handleError(TRUE, TRUE, SZERR_INVALIDOPT_ONJOIN, pszOpt);

   // consider multiple option specifications a non critical error
   // (keep last specification)
   if (g.split.pszBatchFile)
   {
      if (!handleError(TRUE, FALSE, SZERR_OPTDUP, pszOpt))
         return FALSE;
   }

   pszOpt += 2;             // get past "/C"
   if (*pszOpt == ':') ++pszOpt;

   g.split.pszBatchFile = (*pszOpt) ? pszOpt : SZ_DEFBATCHFILE;

   return TRUE;
}


/* --------------------------------------------------------------------------
 Parse the /D option (optional common path of file pieces).
- Parameters -------------------------------------------------------------
 PSZ pszOpt : option string pointer.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL parsePiecesPath(PSZ pszOpt) {
   PSZ p = pszOpt;

   // this option applies only when in join mode (non critical error)
   if (g.mode & FSJSPLIT)
      return handleError(TRUE, TRUE, SZERR_INVALIDOPT_ONSPLIT, pszOpt);

   // consider multiple option specifications a non critical error
   // (keep last specification)
   if (g.join.pszPiecesPath)
   {
      if (!handleError(TRUE, FALSE, SZERR_OPTDUP, pszOpt))
         return FALSE;
   }

   p += 2;             // get past "/D"
   if (*p == ':') ++p;
   // path missing
   if (!*p)
      return handleError(FALSE, TRUE, SZERR_MISSPATH, pszOpt);
   g.join.pszPiecesPath = p;

   return TRUE;
}


/* --------------------------------------------------------------------------
 Parse the /F (filelist) option.
- Parameters -------------------------------------------------------------
 PSZ pszOpt : option string pointer.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
- Note -------------------------------------------------------------------
 The allocated memory is freed on program termination (module: end.c,
 procedure: endApplication).
-------------------------------------------------------------------------- */
static
BOOL parseFileList(PSZ pszOpt) {
   HFILE hf;
   LONGLONG cb;
   PSZ p, pLine;
   INT i;

   // this option applies only when in join mode (non critical error)
   if (g.mode & FSJSPLIT)
      return handleError(TRUE, TRUE, SZERR_INVALIDOPT_ONSPLIT, pszOpt);

   // this option is incompatible with /I
   if (g.mode & FSJJOINMASK)
      return handleError(TRUE, TRUE, SZERR_INCOMPATIBLEOPT,
                         pszOpt, CHO_IN_FILEMASK);

   // this option can be specified only once, since allocation is involved
   // consider this a critical error
   if (g.mode & FSJJOINLIST)
      return handleError(FALSE, FALSE, SZERR_OPTDUP2, pszOpt);

   p = pszOpt + 2;               // get past /F
   if (*p == ':') ++p;           // skip ':'
   if (!*p)                      // missing option parameter
   {
      return handleError(FALSE, TRUE, SZERR_MISSINGLISTFILE);
   }
   printMsg(IROW_CURRENT, SZ_READINGLISTFILE, p);
   // open the file
   hf = fmOpen(p, 0, FMO_IFEXIST | FMO_SHAREREAD | FMO_READ);
   if (hf == FMO_ERROR) return FALSE;
   // get the file size (must be > 0 and < 3 MBs)
   cb = fmSize(hf);
   if (cb < 0) goto closeFile;

   if (cb == 0)
   {
      g.rc = ERROR_INVALID_DATA;
      handleError(FALSE, FALSE, SZERR_INVALIDLISTFILE);
      goto closeFile;
   }
   if (cb > CBMAX_FILELIST)
   {
      g.rc = ERROR_INVALID_DATA;
      handleError(FALSE, FALSE, SZERR_LISTFILETOOLARGE, CBMAX_FILELIST/MiB, cb);
      goto closeFile;
   }
   // allocate memory for reading it
   g.join.list.pFileData = memAlloc(cb + 1);
   if (!g.join.list.pFileData) goto closeFile;
   // read the file content
   if (0 >= fmRead(hf, g.join.list.pFileData, cb)) goto closeFile;
   g.join.list.pFileData[cb] = 0;
   // count the file lines (i.e. the names of the file pieces)
   g.cPieces = countLines(g.join.list.pFileData);
   if (g.cPieces > MAX_PIECES_COUNT)
   {
      g.rc = ERROR_INVALID_DATA;
      handleError(FALSE, FALSE, SZERR_LISTFILETOOMANYENTRIES, g.cPieces);
      goto closeFile;
   }
   // allocate an array of pointers to the file lines
   g.join.list.apszPieces = memAlloc(g.cPieces * sizeof(PSZ));
   if (!g.join.list.apszPieces) goto closeFile;
   // scan the file content to find the start of the lines
   for (i = 0, p = pLine = g.join.list.pFileData; ;)
   {
      switch (*p)
      {
      case '\r':
         *p = '\0';
         if (p[1] == '\n') ++p;
      case '\n':
         *p = '\0';
         if (*pLine) g.join.list.apszPieces[i++] = pLine;
         pLine = ++p;
         break;
      case '\0':
         if (*pLine) g.join.list.apszPieces[i++] = pLine;
         goto outOfLoop;
      default:
         ++p;
      }
   }

outOfLoop:
   // update the value of g.cPieces with the value of i to skip empty lines
   g.cPieces = i;
   // check if the file contained just empty lines or only one file name
   if (g.cPieces < 2)
   {
      g.rc = ERROR_INVALID_DATA;
      handleError(FALSE, FALSE, SZERR_INVALIDLISTFILE);
   }
   g.mode |= FSJJOINLIST;

closeFile:
   fmClose(hf);
   return !g.rc;
}


/* --------------------------------------------------------------------------
 Parse the /I option (name of file pieces expressed by a mask containing
 question mark as place holders for the file piece index.
- Parameters -------------------------------------------------------------
 PSZ pszOpt : option string pointer.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL parseFilePartsMask(PSZ pszOpt) {
   PSZ p = pszOpt + 2;

   // this option applies only when in join mode (non critical error)
   if (!(g.mode & FSJJOIN))
      return handleError(TRUE, TRUE, SZERR_INVALIDOPT_ONSPLIT, pszOpt);

   // this option is incompatible with /F
   if (g.mode & FSJJOINLIST)
      return handleError(TRUE, TRUE, SZERR_INCOMPATIBLEOPT,
                         pszOpt, CHO_FILELISTING);

   // this option can be specified only once
   if (g.mode & FSJJOINMASK)
   {
      if (!handleError(TRUE, FALSE, SZERR_OPTDUP, pszOpt))
         return FALSE;
   }

   // check the first sub-parameter (count of file pieces)
   if (*p == ':') ++p; // a missing colon is not critical here
   g.cPieces = strtoul(p, &p, 10);
   // check if the count of pieces is within valid boundaries
   if ((g.cPieces < 2) || (g.cPieces > MAX_PIECES_COUNT))
      return handleError(FALSE, TRUE,
                         SZERR_INVALIDOPTION, pszOpt, SZERR_INVALIDPIECESCOUNT);
   // a missing colon is critical now
   if (*p != ':')
      return handleError(FALSE, TRUE,
                         SZERR_INVALIDOPTION, pszOpt, SZERR_MISSOPTPARMS);

   // the next parameter part is the file mask, check its correctness
   if (!isFilePartMaskCorrect(++p, &g.join.mask))
      return FALSE;
   g.mode |= FSJJOINMASK;

   return TRUE;
}


/* --------------------------------------------------------------------------
 Parse the output option which can specify:
 - just the output path
 - just the output file part name
 - both the output path and the output file name.
- Parameters -------------------------------------------------------------
 PSZ pszOpt : option string pointer.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL parseOutputOption(PSZ pszOpt) {
   PSZ p, pszLastSlash;

   // the output option /O:... has been specified
   if (pszOpt)
   {
      // this option applies only when in split mode (non critical error)
      if (!(g.mode & FSJSPLIT))
         return handleError(TRUE, TRUE, SZERR_INVALIDOPT_ONJOIN, pszOpt);

      // consider multiple option specifications a non critical error
      // (keep last specification)
      if (g.split.mask.cQMarks)
      {
         if (!handleError(TRUE, FALSE, SZERR_OPTDUP, pszOpt))
            return FALSE;
      }

      p = pszOpt + 2;           // get past "/O"
      if (*p == ':') ++p;
      if (!*p)
         return handleError(FALSE, TRUE,
                            SZERR_INVALIDOPTION, pszOpt, SZERR_MISSOPTPARMS);

      // find the last slash ('\' or '/') in pszOptput
      pszLastSlash = lastSlash(p);
      // slash character found: store the path
      if (pszLastSlash != NULL)
      {
         *pszLastSlash = '\0';
         // the path cannot contain wildcards
         if (strchr(p, '?'))
            return handleError(FALSE, TRUE, SZERR_MASKEDPATH, pszOpt);
         g.split.pszOutPath = p;
         p = pszLastSlash + 1;
      }
      // if the name of the file parts has been specified (as a mask) check
      // if it is valid
      if (*p)
         return isFilePartMaskCorrect(p, &g.split.mask);
   }
   // if the output option has not been specified or if only the output
   // path has been specified, use the default specifics for the names
   // of the file parts
   g.split.mask.pStart = SZ_DEFPIECENAMESTART;
   g.split.mask.pPastQMarks = SZ_DEFPIECENAMEEXT;
   g.split.mask.cQMarks = CB_DEFMASK;
   return TRUE;
}


/* --------------------------------------------------------------------------
 Parse the /R (run) option.
- Parameters -------------------------------------------------------------
 PSZ pszOpt : option string pointer.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL parseRunOption(PSZ pszOpt) {
   PSZ p = pszOpt + 2;
   ULONG priority;

   if (*p == ':') ++p;
   switch (*p)
   {
   case '+':
      priority = PRTYC_FOREGROUNDSERVER;
      break;
   case '-':
      priority = PRTYC_IDLETIME;
      break;
   default:
      return handleError(TRUE, TRUE, SZERR_INVALIDOPTION,
                         pszOpt, SZERR_OPTPRIORITY);
   }
   g.rc = DosSetPriority(0, priority, 0, 0);
   if (g.rc)
      return handleError(TRUE, FALSE, SZERR_SETPRIORITY, g.rc);
   return TRUE;
}


/* --------------------------------------------------------------------------
 Parse the /W (wait) option.
- Parameters -------------------------------------------------------------
 PSZ pszOpt : option string pointer.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL parsePauseOption(PSZ pszOpt) {
   PSZ p = pszOpt + 2;

   if (g.mode & FSJUNATTENDED)
      return handleError(FALSE, TRUE, SZERR_INVALIDOPTION,
                         pszOpt, SZERR_OPTPAUSE2, CHO_UNATTENDED);
   g.mode |= FSJPAUSE;
   if (*p == ':') ++p;
   switch (*p)
   {
   case '\0':
      break;
   case '1':
      g.mode |= FSJ_CHECK1STSIZE;
      break;
   case 'N':
   case 'n':
      g.mode |= FSJ_NOSIZECHECK;
      break;
   default:
      return handleError(TRUE, TRUE, SZERR_INVALIDOPTION,
                         pszOpt, SZERR_OPTPAUSE);
   }
   return TRUE;
}


/* --------------------------------------------------------------------------
 Check the correctness of ordinary parameters (i.e. parameters not prefixed
 by / or -).
- Parameters -------------------------------------------------------------
 PSZ parm : current command line parameter.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL parseOrdinaryParm(PSZ parm) {
   // split mode
   if (g.mode & FSJSPLIT)
   {
      // only one parameter expected in this case
      if (g.split.pszFile)
      {
         if (!handleError(FALSE, TRUE,
                          SZERR_UNEXPECTEDPARM, parm, SZERR_DUPSPLITFILE))
            return FALSE;
      }
      g.split.pszFile = parm;
   }
   // join mode
   else
   {
      // parameters are not allowed if the name of the file pieces have
      // already been entered via the /F or /I option
      if ((g.mode & FSJJOINMASK) || (g.mode & FSJJOINLIST))
         return handleError(TRUE, TRUE,
                            SZERR_UNEXPECTEDPARM, parm, SZERR_DUPSPLITFILE);
      // file name array not yet allocated - allow 9999 file names
      if (!g.cPieces)
      {
         g.join.list.apszPieces = memAlloc(MAX_PIECES_PARMS * sizeof(PSZ));
         if (!g.join.list.apszPieces) return FALSE;
      }
      // this is unlikely to happen
      if (g.cPieces == MAX_PIECES_PARMS)
         return handleError(FALSE, FALSE, SZERR_TOOMANYPARMS);
      g.join.list.apszPieces[g.cPieces++] = parm;
   }

   return TRUE;
}


/* --------------------------------------------------------------------------
 Check if, according to the current mode, all the needed options are
 present and compatible.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (missing).
-------------------------------------------------------------------------- */
static
BOOL areParametersCorrect(VOID) {

   // if no buffer options was specified set the buffer to the default value
   if (!g.cbBuf && !parseBufferOption(NULL)) return FALSE;

   if (g.mode & FSJSPLIT)
      return checkSplitMode();
   return checkJoinMode();
}


/* --------------------------------------------------------------------------
 Initialize the screen and allocate the read/write buffer.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL initJobEnv(VOID) {
   USHORT aus[4];
   APIRET rc;

   aus[0] = sizeof(aus);
   rc = VioGetMode((PVIOMODEINFO)aus, 0);
   if (rc == NO_ERROR)
   {
      g.cols = ((PVIOMODEINFO)aus)->col;
      g.rows = ((PVIOMODEINFO)aus)->row;
      if ((g.cols != 80) || (g.rows != 25))
      {
         ((PVIOMODEINFO)aus)->col = 80;
         ((PVIOMODEINFO)aus)->row = 25;
         rc = VioSetMode((PVIOMODEINFO)aus, 0);
         if (rc == NO_ERROR)
            g.mode |= FSJ_RESETSCREEN;
      }
   }
   screenClear(VIOF_GREEN | VIOB_BLACK);
   if (allocateBuffer())
   {
      if (g.mode & FSJPROGRESS)
         VioWrtCharStr(SZ_RANGE, 160, IROW_RANGE, 0, 0);
      g.mode |= FSJ_RUNNING;
      return TRUE;
   }
   return FALSE;
}


/* --------------------------------------------------------------------------
 When in split mode check:
 - if the name of the file to be splitted was specified,
 - if the file to be splitted exists,
 - if the size of the file to be splitted is compatible with the required
   split mode.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL checkSplitMode(VOID) {

   // check if the file to be splitted exists and is not empty
   if (!isSplitFileValid()) return FALSE;
   // calculate count of file pieces and size of file pieces
   // if the size of the file pieces was defined:
   if (g.mode & FSJSPLITSIZE)
   {
      if (!calcCountOfPieces()) return FALSE;
   }
   // the count of pieces was defined
   else
   {
      if (!calcSizeOfPiece()) return FALSE;
   }
   // if the name of file parts has not been specified use the default values
   if (!g.split.mask.pStart) parseOutputOption(NULL);
   // check the correctness of the output file mask
   checkMaskDigits(&g.split.mask, g.cPieces);
   // write the batch to join the files
   if (!createJoinBatchFile()) return FALSE;
   // check if there is enough space to write the file pieces
   return isSpaceEnoughForSplitting();
}


/* --------------------------------------------------------------------------
 Check:
 - if any file pieces were specified
 - if at least the first file piece exists calculate its size
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL checkJoinMode(VOID) {
   CHAR pieceName[CCHMAXPATH];
   INT i, cbPath, cPieces;
   LONGLONG cbPart;

   if (!g.cPieces)
      return handleError(FALSE, FALSE, SZERR_MISSJOINPIECES);
   if (g.cPieces == 1)
      return handleError(FALSE, FALSE, SZERR_JOINONLY1);
   if (g.mode & FSJJOINMASK)
   {
      // check for conflits between the /I and the /D options
      if (g.join.pszPiecesPath)
         return handleError(FALSE, TRUE, SZERR_PATHCONFLICT,
                            CHO_COMMONPATH, CHO_IN_FILEMASK, CHO_IN_FILEMASK);
      // check the correctness of the input file mask
      checkMaskDigits(&g.join.mask, g.cPieces);
   }
   // if no space check is required return TRUE
   if (g.mode & FSJ_NOSIZECHECK)
   {
      g.mode &= ~FSJPROGRESS;
      return TRUE;
   }

   // try to calculate the size of the file resulting from the join operation
   screenClear(VIOB_BLACK);
   printMsg(IROW_MAINMSG, SZ_CALCWHOLEFILESIZE, g.join.pszFile, g.cPieces);
   cbPath = -1;
   // check just the size of the first file part of all of files ?
   cPieces = (g.mode & FSJ_CHECK1STSIZE) ? 1 : g.cPieces;
   for (i = 1; i <= cPieces; ++i)
   {
      cbPath = makeJoinPartName(pieceName, i, cbPath);
      printMsg(IROW_MSG, SZ_READPARTSIZE, pieceName);
      while ((cbPart = fmFileSize(pieceName)) <= 0)
      {
         // a critical error if the /W (pause) option has not been specified
         if (!(g.mode & FSJPAUSE)) return FALSE;
         // otherwise a retry/abort menu is shown
         if (NOCBFILE_ABORT == showMenu(SZ_PARTNOTFOUND, ACH_RETRY_ABORT))
            return FALSE;
      }
      if (i == 1) g.cbPiece = cbPart;
      g.cbWholeFile += cbPart;
   }
   // if checked just the size of the first file calculate the total size
   if (g.mode & FSJ_CHECK1STSIZE)
      g.cbWholeFile *= g.cPieces;

   return TRUE;
}


/* --------------------------------------------------------------------------
 Check if the file to be splitted exists and is not empty.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL isSplitFileValid(VOID) {

   // no file to be splitted was specified
   if (!g.split.pszFile)
      return handleError(FALSE, TRUE, SZERR_UNSPEC_SPLITFILE);
   // open the file
   // open the file to be splitted (the file is closed on application end)
   printMsg(IROW_CURRENT, SZ_OPENSPLITFILE, g.split.pszFile);
   g.hfWhole = fmOpen(g.split.pszFile, 0,
                      FMO_IFEXIST | FMO_SHAREREAD | FMO_READ);
   if (g.hfWhole == FMO_ERROR) return FALSE;
   // get the file size
   if (-1LL == (g.cbWholeFile = fmSize(g.hfWhole))) return FALSE;
   // files smaller than 2 bytes cannot be splitted: nothing to do
   if (!g.cbWholeFile)
      return handleError(FALSE, FALSE,
                         SZERR_SPLIT_EMPTYFILE, g.split.pszFile, g.cbWholeFile);
   return TRUE;
}


/* --------------------------------------------------------------------------
 Check if there is enough space for the files resulting from the split job.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL isSpaceEnoughForSplitting(VOID) {
   LONGLONG cbTot, cbLast;
   ULONG cbSector;

   // no check disk pace option set
   if (g.mode & FSJ_NOSIZECHECK) return TRUE;
   // calculate the size of the last file piece
   cbLast = g.cbWholeFile - (g.cbPiece * (g.cPieces - 1));
   // get the size of a disk sector
   if (0 > fmAvailSpace(g.split.pszOutPath, &cbSector)) return FALSE;
   // calculate the required disk space:
   if (g.mode & FSJ_CHECK1STSIZE)  // check just for the size of a file part
   {
      cbTot = fmFileUsedSpace(g.cbPiece, cbSector);
   }
   else                            // check for the size of all the file parts
   {
      cbTot =   fmFileUsedSpace(g.cbPiece, cbSector) * (g.cPieces - 1)
              + fmFileUsedSpace(cbLast, cbSector);
   }
   return checkSpaceLoop(cbTot, g.split.pszOutPath);
}


/* --------------------------------------------------------------------------
 According to the unit of measure (KB, KiB, MB, MiB, GB or GiB) set the
 unit flag and convert the size of the file pieces into bytes.
- Parameters -------------------------------------------------------------
 INT suffix: if this is 'i' multiply by mul2.
 LONGLONG cbPiece : size of file piece.
 ULONG mul1: power of 10 multiplier.
 ULONG mul2: power of 2 multiplier.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID setUnit(INT suffix, LONGLONG cbPiece, ULONG mul1, ULONG mul2) {
   g.mode |=  FSJSPLITSIZE;
   if ((suffix | 0x20) == 'i')
      g.cbPiece = cbPiece * mul2;
   else
      g.cbPiece = cbPiece * mul1;
}


/* --------------------------------------------------------------------------
 Convert a text string representing a number optionally containing
 underscores as thousands separators to a 64 bits integer.
- Parameters -------------------------------------------------------------
 PSZ pszNum   : number as text string.
 PSZ* pEndNum : optional pointer to the end of the numeric string.
- Return value -----------------------------------------------------------
 LONGLONG : converted number.
-------------------------------------------------------------------------- */
static
LONGLONG formattedNumToLL(PSZ pszNum, PSZ* pEndNum) {
   CHAR buf[32];
   INT i;
   PSZ p;
   for (i = 0, p = buf; *pszNum; ++pszNum)
   {
      if (*pszNum == '_') continue;
      if ((*pszNum < '0') || (*pszNum > '9')) break;
      *p++ = *pszNum;
      ++i;
      if (i == 31) break;    // buffer overflow - error catched by the caller
   }
   *p = '\0';
   if (pEndNum) *pEndNum = pszNum;
   return atoll(buf);
}


/* --------------------------------------------------------------------------
 Check the correctness of file mask specifying the file part names.
- Parameters -------------------------------------------------------------
 PSZ pszName     : file part name containing multiple question marks as digit
                   place holders.
 PFILEMASK pMask : (output) file pieces name mask data.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL isFilePartMaskCorrect(PSZ pszName, PFILEMASK pMask) {
   PSZ p, pEndPreMask;
   ULONG questionMarkBlocks;

   for (p = pszName, questionMarkBlocks = 0; *p;)
   {
      if (*p == '?')
      {
         pEndPreMask = p;
         ++questionMarkBlocks;
         // multiple wildcard blocks (e.g. 'file???text???.ext')
         if (questionMarkBlocks > 1)
            break;
         // no question mark are allowed in the file path
         if (lastSlash(p))
            return handleError(FALSE, TRUE, SZERR_MASKEDPATH, pszName);
         // skip the block
         while (*p == '?') ++p, ++pMask->cQMarks;
         pMask->pPastQMarks = p;
      }
      else
         ++p;
   }
   if (questionMarkBlocks != 1)
      return handleError(FALSE, TRUE, SZERR_INVALID_FILE_PART_NAME, pszName);

   *pEndPreMask = '\0';
   pMask->pStart = pszName;

   return TRUE;
}


/* --------------------------------------------------------------------------
 Check if the count of question mark in the input/output file mask is enough
 for the count of files to be read/written automatically increasing it if
 needed.
- Parameters -------------------------------------------------------------
 PFILEMASK pMask : file mask data.
 ULONG cFiles    : count of files.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID checkMaskDigits(PFILEMASK pMask, ULONG cFiles) {
   ULONG cDigits;

   // count decimal digits of cFiles
   for (cDigits = 0; cFiles; cDigits++, cFiles /= 10) ;
   // adjust the file mask if needed
   if (pMask->cQMarks < cDigits) pMask->cQMarks = cDigits;
}


/* --------------------------------------------------------------------------
 Allocate the memory for the read/write buffer.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL allocateBuffer(VOID) {
   // if the buffer size is greater than the size of the whole file reduce it
   if (g.cbWholeFile && (g.cbBuf > g.cbWholeFile))
   {
      g.cbBuf = g.cbWholeFile;
   }
   // if the buffer size is greater than the size of a file part rounds
   // it down to a multiple of a file part
   else if (g.cbPiece && (g.cbBuf > g.cbPiece))
   {
      g.cbBuf = (g.cbBuf / g.cbPiece) * g.cbPiece;
   }
   // allocate memory
   printMsg(IROW_CURRENT, SZ_ALLOCBUF, g.cbBuf);
   g.pBuf = memAlloc(g.cbBuf);

   return (BOOL)g.pBuf;
}


/* --------------------------------------------------------------------------
 Calculate the count of pieces from the total file size and the size of
 each piece.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL calcCountOfPieces(VOID) {

   g.cPieces = g.cbWholeFile / g.cbPiece;
   if (g.cbWholeFile % g.cbPiece) g.cPieces++;
   if (g.cPieces == 1)               // cannot split into 1 piece
      return handleError(FALSE, FALSE, SZERR_PIECE_GT_WHOLE);
   if (g.cPieces > MAX_PIECES_COUNT) // too many pieces
      return handleError(FALSE, FALSE, SZERR_TOOMANYPIECES, g.cPieces);
   return TRUE;
}


/* --------------------------------------------------------------------------
 Calculate the size of each piece from the total file size and the count of
 pieces.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL calcSizeOfPiece(VOID) {
   // calculate the size of each piece (but the last)
   if (g.cPieces > g.cbWholeFile)    // count of pieces > file size
      return handleError(FALSE, FALSE, SZERR_PIECESGT_SIZE);
   g.cbPiece = g.cbWholeFile / g.cPieces;
   if (g.cbWholeFile % g.cbPiece) g.cbPiece++;
   // if the count of pieces is greater than the size of a single piece
   // recalculate the real count of pieces
   if (g.cPieces > g.cbPiece)
      return calcCountOfPieces();
   return TRUE;
}


/* --------------------------------------------------------------------------
 If the option /C has been specified create a .cmd file to re-join the file
 parts resulting from the split operation.
- Parameters -------------------------------------------------------------
 VOID
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL createJoinBatchFile(VOID) {
   CHAR buf[CBMSG_BUF];
   ULONG cb, cbWritten;
   PSZ pWholeFile;
   HFILE hf;

   // -C option has not been specified
   if (!g.split.pszBatchFile) return TRUE;
   // get the name of the file to be splitted removing its path
   pWholeFile = lastSlash(g.split.pszFile);
   pWholeFile = pWholeFile ? pWholeFile + 1 : g.split.pszFile;
   cb = sprintf(buf,
                "rem file created by fsj.exe\n"
                "@echo off\n"
                "fsj J:%s /I:%d:",
                pWholeFile, g.cPieces);
   if (g.split.pszOutPath)
      cb += sprintf(buf + cb, "%s\\", g.split.pszOutPath);
   cb += sprintf(buf + cb, g.split.mask.pStart);
   memset(buf + cb, '?', g.split.mask.cQMarks);
   cb += g.split.mask.cQMarks;
   cb += sprintf(buf + cb, "%s ", g.split.mask.pPastQMarks);
   // write the file
   hf = fmOpen(g.split.pszBatchFile, cb,
               FMO_CREATE | FMO_REPLACE | FMO_SHAREREAD | FMO_WRITE);
   if (hf != FMO_ERROR)
   {
      cbWritten = fmWrite(hf, buf, cb);
      fmClose(hf);
      if (cbWritten == cb) return TRUE;
   }
   // failed to create the file: ignore (and continue) or quit ?
   return handleError(TRUE, FALSE, SZERR_CREATE_JOIN_CMD, g.split.pszBatchFile);
}


