/* --------------------------------------------------------------------------
 main.h : FSJ.EXE main header.

 16 Jun 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------
 - list of include definitions and include files
 - extern global variable declaration
-------------------------------------------------------------------------- */


// get rid of that stupid 'No newline at end of file' warning !
//#pragma disable_message(138);

#ifndef _FSJ_MAIN_H_
   #define _FSJ_MAIN_H_
   #define INCL_DOS
   #define INCL_DOSERRORS
   #define INCL_DOSMISC
   #define INCL_DOSMODULEMGR
   #define INCL_DOSFILEMGR
   #define INCL_DOSPROCESS
   #define INCL_VIO
   #define INCL_KBD
   #define INCL_LONGLONG

   #include <os2.h>
   #include <stdarg.h>
   #include <stdlib.h>
   #include <stdio.h>
   #include <malloc.h>
   #include <string.h>

//   #include <ApiExBase.h>
   #include "app_types.h"
   #include "app_options.h"
   #include "app_constants.h"
   #include "app_util.h"
   #include "app_msgs.h"
   #include "prototypes.h"

extern GLOBAL g;


#endif // #ifndef _FSJ_MAIN_H_
