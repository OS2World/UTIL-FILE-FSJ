/* --------------------------------------------------------------------------
 app_options.h :

 13 Nov 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

#ifndef _APP_OPTIONS_H_
   #define _APP_OPTIONS_H_

// valid help command characters array
#define ACH_CMDHELPCHARS        "hH?"

// split command character
#define CHCMD_SPLIT            'S'

// join command character
#define CHCMD_JOIN             'J'

// valid option characters
// in case of modifications avoid duplicates !!!
#define CHO_BUFFER             'B'
#define CHO_MAKECMDFILE        'C'
#define CHO_COMMONPATH         'D'
#define CHO_FILELISTING        'F'
#define CHO_IN_FILEMASK        'I'
#define CHO_LOGFILE            'L'
#define CHO_OUT_FILEMASK       'O'
#define CHO_PROGRESS           'P'
#define CHO_QUIET              'Q'
#define CHO_RUNPRIORITY        'R'
#define CHO_UNATTENDED         'U'
#define CHO_VERBOSE            'V'
#define CHO_PAUSE              'W'


// callback parse-option typedef
typedef BOOL (FN_PARSE_OPT)(PSZ);
typedef FN_PARSE_OPT * PFN_PARSE_OPT;


#endif // #ifndef _APP_OPTIONS_H_