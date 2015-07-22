/* --------------------------------------------------------------------------
 app_types.h :

 13 Nov 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

#ifndef _APP_TYPES_H_
   #define _APP_TYPES_H_

/* --------------------------------------------------------------------------
 File piece names can be specified via a name containing a block of question
 marks as place holders for the ordinal number of the file piece (e.g.
 file???.txt).
 This structure defines the various parts of such file names.
*/
typedef struct {
   PSZ pStart;               // start of name containing a quest.marks block
   PSZ pPastQMarks;          // first character after the quest.marks block
   ULONG cQMarks;            // count of question marks in the block
} FILEMASK, * PFILEMASK;


/* --------------------------------------------------------------------------
 A list of file pieces can also be entered as command line parameters or
 as a list in a text file (one file name per line).
 This structure holds a pointer to the data of the text file (if needed)
 and a dynamically allocated array of pointers to the names of the file
 pieces.
*/
typedef struct {
   PSZ pFileData;
   PSZ* apszPieces;
} PIECESLIST, * PPIECESLIST;


/* --------------------------------------------------------------------------
 Data structure used when splitting files.
*/
typedef struct {
   PSZ pszFile;              // name of the file to be splitted
   PSZ pszBatchFile;         // optional name of the batch file to be generated
   PSZ pszOutPath;           // path where to write the file pieces
   FILEMASK mask;            // FILEMASK structure (see above)
} SPLITDATA, * PSPLITDATA;


/* --------------------------------------------------------------------------
 Data used when joining files.
*/
typedef struct {
   PSZ pszFile;              // name of the file to be created
   PSZ pszPiecesPath;        // path containing the file pieces
   union {                   // file pieces details:
      FILEMASK mask;         // file pieces specified as a mask
      PIECESLIST list;       // file pieces explicitly specified
   } ;
} JOINDATA, * PJOINDATA;


/* --------------------------------------------------------------------------
 Application data.
*/

typedef struct {
   union {
      SPLITDATA split;             // data used to split a file
      JOINDATA join;               // data used to join files
   } ;
   // common data
   PSZ pszLogFile;                 // name of the log file
   ULONG cPieces;                  // count of file pieces
   LONGLONG cbPiece;               // size of a file piece
   LONGLONG cbWholeFile;           // size of the file to be splitted
   LONG cbBuf;                     // read/write buffer size
   PBYTE pBuf;                     // read/write buffer address
   HFILE hfWhole;                  // file to be splitted/join result. file
   HFILE hfLog;                    // handle of the log file
   ULONG mode;                     // command, options and state flags
   ULONG cols, rows;               // previous screen size
   APIRET rc;                      // API return code
   ULONG prevstep;                 // previous state of the progress
   ULONGLONG cbProcData;           // count of processed data (for progress)
} GLOBAL, *PGLOBAL;

#endif // #ifndef _APP_TYPES_H_