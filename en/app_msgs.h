/* --------------------------------------------------------------------------
 app_msgs.h :

 16 Jun 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */


#ifndef _APP_MSGS_H_
   #define _APP_MSGS_H_

// error message prefixes/suffixes

#define SZ_ERRPREFIX \
   "Error: "

#define SZ_READHELP \
   "\r\nType \"FSJ ?\" for details on the program usage.\r\n"

#define SZ_SUFFIX \
   ".\r\n"

// generic errors

#define SZERR_ALLOCATION \
   "allocation of %u bytes failed (DosAllocMem returned %d)"

// argument parsing and initialization errors

#define SZERR_INVALIDCOMMAND \
   "invalid command:\r\n%s\r\n%s"

#define SZERR_INVALIDOPTION \
   "invalid option:\r\n%s.\r\n%s"

#define SZERR_VALIDCMDLIST \
    "The only valid commands are: ? (help), S (split) and J (join)."

#define SZERR_NOSPLITCOUNT \
   "The size/number of file pieces was not specified"

#define SZERR_CANTSPLIT01  \
    "A file cannot be splitted in less than 2 pieces"

#define SZERR_TOOMANYPIECES \
   "The number of file pieces is too large (%u)"

#define SZERR_SPLITUNIT \
   "Unknown split unit of measure"

#define SZERR_MAXPRMEM \
   "failed to get maximum allocatable memory"

#define SZERR_BUFFSIZENOTNUM \
   "the value of the buffer size must be made of decimal digits"

#define SZERR_INVALIDOPT_ONJOIN \
   "invalid option:\r\n%s\r\n"\
   "is not compatible with file-joining"

#define SZERR_OPTDUP \
   "the option % .2s has already been specified.\r\n"\
   "The previous option values are discarded"

#define SZERR_OPTDUP2 \
   "the option % .2s has already been specified"\

#define SZERR_INVALIDOPT_ONSPLIT \
   "invalid option:\r\n%s\r\n"\
   "is not compatible with file-splitting"

#define SZERR_MISSPATH \
   "no path specified for the % .2s option"

#define SZERR_INCOMPATIBLEOPT \
   "this option (% .2s) is not compatible with /%c"

#define SZERR_MISSINGLISTFILE \
   "the name of the file containing the list of the file pieces has not\r\n"\
   "been specified"

#define SZERR_INVALIDLISTFILE \
   "the list of the file pieces contains just one or no entry"

#define SZERR_LISTFILETOOLARGE \
   "the file size of the list of file pieces exceeds %u MBs (%llu)"

#define SZERR_LISTFILETOOMANYENTRIES \
   "the list of file pieces contains too many entries (%u)"

#define SZERR_INVALIDPIECESCOUNT \
   "unspecified or invalid count of file pieces"

#define SZERR_MISSOPTPARMS \
   "Some required option parameters have not been specified or are invalid"

#define SZERR_MASKEDPATH \
   "wildcard characters ('?') are not allowed within the path specification:\r\n"\
   "(%s)\r\n"

#define SZERR_INVALID_FILE_PART_NAME \
   "the mask specifying the file part names:\r\n%s\r\n"\
   "does not contain a valid file part number place holder.\r\n" \
   "Use a block of '?' characters as place holder for the file part number"

#define SZERR_OPTPRIORITY \
   "the process priority change parameter is not valid"

#define SZERR_SETPRIORITY \
   "failed to change the progress priority. DosSetPriority() returned %d"

#define SZERR_OPTPAUSE \
   "no confirmation will be asked before reading/writing a file part"

#define SZERR_OPTPAUSE2 \
   "this option is ignored since is incompatible with /U"

#define SZERR_UNEXPECTEDPARM \
   "unexpected parameter\r\n%s\r\n%s"

#define SZERR_DUPSPLITFILE \
   "duplicate parameter (name of file to be splitted)"

#define SZERR_DUPFILEPIECES \
   "duplicate parameter (file parts specification)"

#define SZERR_TOOMANYPARMS \
   "too many command line parameters"

#define SZERR_UNSPEC_SPLITFILE \
   "the name of the file to be splitted has not been specified"

#define SZERR_SPLIT_EMPTYFILE \
   "the size of the file:\r\n"\
   "%s\r\n"\
   "is %lld byte.\r\n"\
   "No split operation is possible"

#define SZERR_PIECE_GT_WHOLE \
   "nothing to do.\r\n" \
   "file piece size is larger than the size of the whole file"

#define SZERR_PIECESGT_SIZE \
   "the count of file pieces is greater than the size of the whole file"

#define SZERR_CREATE_JOIN_CMD \
   "failed to create the batch file:\r\n%s\r\nto re-join the file pieces"

#define SZERR_MISSJOINPIECES \
   "no file to be joined has been specified"

#define SZERR_JOINONLY1 \
   "only one file to be joined has been specified"

#define SZERR_PATHCONFLICT \
   "cannot use both /%c and /%c options.\r\n"\
   "The path of the files to be joined must be specified with the /%c option"

#define SZERR_QUERYFREESPACE \
   "failed to get the amount of available space for drive %c:.\r\n"\
   "DosQueryFSInfo() returned %d"

#define SZERR_DISKFULL \
   "not enough disk space for the required operation"

// initialization messages

#define SZ_READINGLISTFILE \
   "Getting the list of the file pieces from:\r\n%s."

#define SZ_CALCWHOLEFILESIZE \
   "Calculating the size of:\r\n"\
   "%s\r\n" \
   "to be created from the concatenation of %d files..."

// text message
#define SZ_OPENSPLITFILE \
   "Opening:\r\n %s..."

#define SZ_SPLITTING \
   "Splitting:\r\n %s\r\nin %u pieces of %lld bytes..."

#define SZ_JOINING \
   "Joining %d pieces of %lld bytes into:\r\n %s..."

#define SZ_OPENFILEPART \
   "Writing file part % 5u\r\n"   \
   "(%s)...\r\n"

#define SZ_READFILEPART \
   "Reading file part % 5u\r\n"   \
   "(%s)...\r\n"

#define SZ_READPARTSIZE \
   "Checking the size of %s..."

#define SZ_ALLOCBUF \
   "Allocating %u bytes for the read/write file buffer"

#define SZ_SPLITOK \
   "%s\r\n has been successfully splitted in %d pieces"

#define SZ_JOINOK \
   "%s\r\n has been succesfully created from %d pieces"

#define SZ_SPLITFAILED \
   "Failed to split:\r\n %s\r\nin %d pieces"

#define SZ_JOINFAILED  \
   "Failed to create:\r\n %s\r\nfrom %d pieces"

// menu messages
#define SZ_RETRY \
   "   [%c] ... retry"

#define SZ_TERMINATE \
   "   [%c] ... terminate the program."

#define SZ_DISKFULL \
   "There is not enough disk space...\r\n"\
   SZ_RETRY" (after having freed disk space),\r\n"SZ_TERMINATE

// array of characters used to [R]etry or [T]erminate the program
#define ACH_RETRYABORT       "RT"
#define DISKFULL_RETRY        0
#define DISKFULL_ABORT        1

// message used on non-critical error conditions
#define SZ_IGNOREABORT \
   "This error is not critical...\r\n" \
   "   [%c] ... ignore the error and go on,\r\n"SZ_TERMINATE

// array of characters used to [I]gnore an error or [T]erminate the program
#define ACH_IGNOREABORT      "IT"
#define ERROR_IGNORE         0
#define ERROR_ABORT          1

// message displayed when a file part was not found
#define SZ_PARTNOTFOUND \
   "Failed to get the file size...\r\n" \
   SZ_RETRY" (after having inserted the media with the missing file)\r\n"\
   SZ_TERMINATE

#define ACH_RETRY_ABORT     "SRT"
#define NOCBFILE_RETRY        0
#define NOCBFILE_ABORT        1


// message displayed when a file part could not be opened
#define SZ_PARTNOOPENED \
   "Failed to open the file...\r\n"\
   SZ_RETRY",\r\n"SZ_TERMINATE

// file I/O errors

#define SZERR_TYPEHELPSYS \
   "Type \'HELP SYS%04d\' to get more details on the cause of the problem.\r\n"

#define SZERR_OPENFILE \
   "failed to open the file:\r\n%s.\r\n"

#define SZERR_READFILE \
   "file read operation failed%s.\r\n"

#define SZERR_WRITEFILE \
   "file write operation failed%s.\r\n"

#define SZERR_WRITEFILE2 \
   "write operation failed (had to write %u bytes, wrote %u bytes)"

#define SZERR_FILESIZE \
   "file size query failed%s.\r\n"

#define SZERR_FILESIZE2 \
   "failed to get the size of:\r\n%s.\r\n"

// file I/O debug
#define SZ_FOPEN \
   "File open:\r\n%s\r\nhandle:%d - rc:%d"

#define SZ_FCLOSE \
   "File close: hf:%d"

#define SZ_FREAD \
   "File read: hf:%d - cbRead:%d - cbReadDone:%d - rc:%d"

#define SZ_FWRITE \
   "File write: hf:%d - cbWrite:%d - cbWriteDone:%d - rc:%d"

// showhelp.c messages

#define SZ_TBHEADER \
   " Use the page and direction keys to scroll the text, press Esc to quit."

#define SZ_TBSTATUS \
   " Line %4d of %4d"

#define SZ_TBFILESTART \
   " [ start of file ]"

#define SZ_TBFILEEND \
   " [ end of file ]"

#define SZERR_GETSCREENSIZE \
   "unknown screen size (VioGetMode returned %d)"

// ordinary messages --------------------------------------------------------

#define SZ_PRESSANYKEY \
   "Press any key to continue ...\r\n"

// other messages
#define SZ_PROGRESS \
   "current state : % 6.2f %%"



#endif // #ifndef _APP_MSGS_H_
