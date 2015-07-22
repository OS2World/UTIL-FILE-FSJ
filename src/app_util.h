/* --------------------------------------------------------------------------
 app_util.h :

 5 Jul 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

#ifndef _APP_UTIL_H_
   #define _APP_UTIL_H_


// file management macros ---------------------------------------------------

#define MKOPENFLAG(_flag_)  \
   ((_flag_) & (FMO_CREATE | FMO_IFEXIST | FMO_REPLACE))
#define MKOPENMODE(_flag_) \
   (((_flag_) & 0x7fff0000) >> 16)

// fileIO definitions -------------------------------------------------------

#define FILEATTR_ANY \
   (FILE_ARCHIVED | FILE_SYSTEM | FILE_HIDDEN | FILE_READONLY)
#define FILEATTR_RECUR       (FILEATTR_ANY | FILE_DIRECTORY)
// file open error
#define FMO_ERROR            ((HFILE)-1)
// creating flags
#define FMO_FAILIFNEW        0
#define FMO_CREATE           OPEN_ACTION_CREATE_IF_NEW
// mutually exclusive
#define FMO_NOTIFEXIST       0
#define FMO_IFEXIST          OPEN_ACTION_OPEN_IF_EXISTS
#define FMO_REPLACE          OPEN_ACTION_REPLACE_IF_EXISTS
// sharing flags (mutually exclusive)
#define FMO_SHARENONE        (OPEN_SHARE_DENYREADWRITE << 16)
#define FMO_SHAREREAD        (OPEN_SHARE_DENYWRITE << 16)
#define FMO_SHAREWRITE       (OPEN_SHARE_DENYREAD << 16)
#define FMO_SHAREALL         (OPEN_SHARE_DENYNONE << 16)
// access flags (mutually exclusive)
#define FMO_READ             (OPEN_ACCESS_READONLY << 16)
#define FMO_WRITE            (OPEN_ACCESS_WRITEONLY << 16)
#define FMO_READWRITE        (OPEN_ACCESS_READWRITE << 16)
// #define FMO_APPEND           (0x80000000 | FMO_WRITE)

// vio utility definitions --------------------------------------------------

// key definitions
#define KEY_NOTCHAR       0x00020000
#define KEY_CHARMASK      0x000000ff
#define KEY_SCANCODEMASK  0x0000ff00
#define KEY_EVENTMASK     0x00ff0000
#define KEY_SHIFTMASK     0xff000000

// scancodes (as returned by kbdKey()
#define KEY_ESC           0x00000100  // Esc - quit the key retrieving loop
#define KEY_UP            0x00004800  // up arrow - scroll down one line
#define KEY_LEFT          0x00004b00  // left arrow - scroll right one column
#define KEY_RIGHT         0x00004d00  // right arrow - scroll left one column
#define KEY_DOWN          0x00005000  // down arrow - scroll up one line
#define KEY_HOME          0x00004700  // go to the first line
#define KEY_END           0x00004f00  // go the last line
#define KEY_PAGE_DOWN     0x00005100  // page down - scroll up one page
#define KEY_PAGE_UP       0x00004900  // page up - scroll down one page

// keyboard macros
#define kbdKeyChar(_key_)        ((_key_) & KEY_CHARMASK)
#define kbdKeyScancode(_key_)    (((_key_) & KEY_SCANCODEMASK) >> 8)
#define kbdKeyEvent(_key_)       (((_key_) & KEY_EVENTMASK) >> 16)
#define kbdKeyShift(_key_)       (((_key_) & KEY_SHIFTMASK) >> 24)
#define kbdIsEscKey(_key_)       (((_key_) & KEY_SCANCODEMASK) == KEY_ESC)


// foreground colors
#define VIOF_BLACK       0x00
#define VIOF_DARKBLUE    0x01
#define VIOF_DARKGREEN   0x02
#define VIOF_DARKCYAN    0x03
#define VIOF_DARKRED     0x04
#define VIOF_DARKPINK    0x05
#define VIOF_BROWN       0x06
#define VIOF_GRAY        0x07
#define VIOF_GREY        VIOF_GRAY
#define VIOF_DARKGRAY    0x08
#define VIOF_DARKGREY    VIOF_DARKGRAY
#define VIOF_BLUE        0x09
#define VIOF_GREEN       0x0a
#define VIOF_CYAN        0x0b
#define VIOF_RED         0x0c
#define VIOF_PINK        0x0d
#define VIOF_YELLOW      0x0e
#define VIOF_WHITE       0x0f

// background colors
#define VIOB_BLACK       0x00
#define VIOB_DARKBLUE    0x10
#define VIOB_DARKGREEN   0x20
#define VIOB_DARKCYAN    0x30
#define VIOB_DARKRED     0x40
#define VIOB_DARKPINK    0x50
#define VIOB_BROWN       0x60
#define VIOB_GRAY        0x70
#define VIOB_GREY        VIOB_GRAY
#define VIOB_DARKGRAY    0x80
#define VIOB_DARKGREY    VIOB_DARKGRAY
#define VIOB_BLUE        0x90
#define VIOB_GREEN       0xa0
#define VIOB_CYAN        0xb0
#define VIOB_RED         0xc0
#define VIOB_PINK        0xd0
#define VIOB_YELLOW      0xe0
#define VIOB_WHITE       0xf0

// scroll flags
#define SCROLL_ONE          0
#define SCROLL_PAGE         1
#define SCROLL_ALL          2

// resource release flags
#define TBRESET_INTENSITY   0x0001
#define TBCLEAR_SCREEN      0x0002


typedef struct {
   ULONG cols;                      // screen size in columns
   ULONG rows;                      // screen size in rows
   ULONG cLines;                    // count of lines
   ULONG iLine;                     // current top line index
   PSZ pData;                       // text file data
   PSZ* aLines;                     // array of pointers to text lines
   ULONG flag;                      // resource release flag
} TEXTBROWSER, * PTEXTBROWSER;

// callback function used by showmenu
typedef BOOL (APIENTRY MENUFN)(ULONG, PVOID);
typedef MENUFN *PMENUFN;

#endif // #ifndef _APP_UTIL_H_