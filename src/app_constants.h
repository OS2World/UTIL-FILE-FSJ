/* --------------------------------------------------------------------------
 app_constants.h :

 13 Nov 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------
 - mode, option and state flags
 - unit of measures mulitpliers
 - limits and defaults
 - default file names
 - other constants
-------------------------------------------------------------------------- */

#ifndef _APP_CONSTANTS_H_
   #define _APP_CONSTANTS_H_


/* ---------------------------------------------------------------------------
 mode, option and state flags
*/
// operating mode flags (help/split/join)
#define FSJSPLIT         0x00000001       // split mode
#define FSJJOIN          0x00000002       // join mode
#define FSJHELP          0x00000004       // help mode
#define FSJCOMMAND      (FSJSPLIT | FSJJOIN | FSJHELP)

// join modes
#define FSJJOINMASK      0x00000010       // file parts defined via '?' mask
#define FSJJOINLIST      0x00000020       // file parts defined via a text file

// other options
#define FSJPAUSE         0x00000100       // pause before writing a new piece
#define FSJQUIET         0x00000200       // quiet mode
#define FSJPROGRESS      0x00000400       // show the progress bar
#define FSJUNATTENDED    0x00000800       // disable all confirmation requests
#define FSJVERBOSE       0x00001000       // verbose: display file (I/O details)

// split mode (by pieces count or by piece size)
#define FSJSPLITPIECES   0x00000000       // split by pieces
#define FSJSPLITSIZE     0x00010000       // split by size

// higm memory object support flags
#define FSJ_MEMDEFAULT   0x00000000
#define FSJ_MEMHIGHOBJ   0x00100000

// do not perform overall needed disk space calculation
#define FSJ_NOSIZECHECK  0x00200000
// only check the size of the first file part
#define FSJ_CHECK1STSIZE 0x00400000

// reset the screen size on program termination
#define FSJ_RESETSCREEN  0x10000000
// the split/join operation started
#define FSJ_RUNNING      0x20000000


/* ---------------------------------------------------------------------------
 unit of measures mulitpliers
*/
#define KB                     1000
#define MB                  1000000
#define GB               1000000000

#define KiB                    1024
#define MiB                 1048576
#define GiB              1073741824

/* ---------------------------------------------------------------------------
 limits and defaults
*/
// limit max count of file pieces
#define MAX_PIECES_COUNT      99999
#define MAX_PIECES_PARMS       9999

// minimum/maximum read buffer size
#define CBBUF_MIN          (64 * KiB)
#define CBBUF_DEF          MiB
#define CBBUF_MAX          (1024 * MiB)

// output messages buffer size
#define CBMSG_BUF          2048

// maximum size for the file containing the list of file pieces
#define CBMAX_FILELIST     (3*MiB)

#define CBNUMBUF           32           // buffer used for numeric unformatting


/* ---------------------------------------------------------------------------
 default file names
*/
#define SZ_DEFLOGFILE            "fsj.log"
#define SZ_DEFWHOLEFILE          "wholefil.fsj"
#define SZ_DEFBATCHFILE          "join.cmd"
#define SZ_DEFPIECENAMESTART     "pie"
#define SZ_DEFPIECENAMEEXT       ".fsj"
#define CB_DEFMASK               5

/* ---------------------------------------------------------------------------
 other constants
*/
// progress bar caption
#define SZ_RANGE \
"0%    10%     20%     30%     40%     50%     60%     70%     80%     90%   100%"\
"---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+"

// row position indexes
#define IROW_CURRENT            -1     // current row
#define IROW_MAINMSG             1     // main message
#define IROW_MSG                 6     // secondary message
#define IROW_RANGE              13     // progress scale
#define IROW_BAR                14     // scrollbar
#define IROW_PERCENT            15     // progress as percentage
#define IROW_DEBUG              17     // file I/O debug messages

#endif // #ifndef _APP_CONSTANTS_H_