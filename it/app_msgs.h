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
   "Errore: "

#define SZ_READHELP \
   "\r\nDigitare \"FSJ ?\" per informazioni sull'uso del programma.\r\n"

#define SZ_SUFFIX \
   ".\r\n"

// generic errors

#define SZERR_ALLOCATION \
   "allocazione di %u byte fallita (DosAllocMem riporta %d)"

// argument parsing and initialization errors

#define SZERR_INVALIDCOMMAND \
   "comando non valido:\r\n%s\r\n%s"

#define SZERR_INVALIDOPTION \
   "opzione non valida:\r\n%s.\r\n%s"

#define SZERR_VALIDCMDLIST \
    "I soli comandi validi sono: ? (aiuto), S (divisione) e J (unione)."

#define SZERR_NOSPLITCOUNT \
   "La dimensione/quantit… dei pezzi di file non Š stata specificata"

#define SZERR_CANTSPLIT01  \
    "Un file non pu• essere diviso in meno di 2 pezzi"

#define SZERR_TOOMANYPIECES \
   "Il numero di pezzi di file Š troppo grande (%u)"

#define SZERR_SPLITUNIT \
   "Unit… di misura sconosciuta"

#define SZERR_MAXPRMEM \
   "impossibile rilevare la massima quantit… di memoria disponibile"

#define SZERR_BUFFSIZENOTNUM \
   "la dimensione del buffer deve essere espressa in cifre"

#define SZERR_INVALIDOPT_ONJOIN \
   "opziona non valida:\r\n%s\r\n"\
   "non Š compatibile con il concatenamento di file"

#define SZERR_OPTDUP \
   "l'opzione % .2s Š stata gi… specificata.\r\n"\
   "I valori precedenti verranno ignorati"

#define SZERR_OPTDUP2 \
   "l'opzione % .2s Š stata gi… specificata"

#define SZERR_INVALIDOPT_ONSPLIT \
   "opzione non valida:\r\n%s\r\n"\
   "non Š compatibile con la divisione di file"

#define SZERR_MISSPATH \
   "nessun percorso specificato nell'opzione % .2s"

#define SZERR_INCOMPATIBLEOPT \
   "questa opzione (% .2s) non Š compatibile con /%c"

#define SZERR_MISSINGLISTFILE \
   "il name del file contente l'elenco dei file da concatenare non Š stato\r\n"\
   "specificato"

#define SZERR_INVALIDLISTFILE \
   "l'elenco dei file da concatenare contiene solo una voce o nessuna voce"

#define SZERR_LISTFILETOOLARGE \
   "la dimensione dell'elenco dei file da concatenare eccede %u MBs (%llu)"

#define SZERR_LISTFILETOOMANYENTRIES \
   "l'elenco dei file da concatenare contiene troppe voci (%u)"

#define SZERR_INVALIDPIECESCOUNT \
   "numero di file non specificato o non valido"

#define SZERR_MISSOPTPARMS \
   "I sottoparametri dell'opzione non sono stati specificati o non sono validi"

#define SZERR_MASKEDPATH \
   "i caratteri jolly ('?') non sono permessi all'interno del percorso:\r\n"\
   "(%s)\r\n"

#define SZERR_INVALID_FILE_PART_NAME \
   "la maschera specificante i nomi delle porzioni di file:\r\n%s\r\n"\
   "non contiene un segnaposto valido per il numero del file.\r\n" \
   "Usare un blocco di caratteri '?' come segnaposto per il numero del file"

#define SZERR_OPTPRIORITY \
   "il formato per il cambiamento di priorit… del processo non Š valido"

#define SZERR_SETPRIORITY \
   "impossibile cambiare la priorit… del processo. DosSetPriority() riporta %d"

#define SZERR_OPTPAUSE \
   "nessuna conferma verr… richiesta prima della lettura/scrittura dei file"

#define SZERR_OPTPAUSE2 \
   "questa opzione viene ignorata per incompatibilit… con /U"

#define SZERR_UNEXPECTEDPARM \
   "parametro inaspettato\r\n%s\r\n%s"

#define SZERR_DUPSPLITFILE \
   "parametro duplicato (nome del file da dividere)"

#define SZERR_DUPFILEPIECES \
   "parametro duplicato (specifiche delle parti di file)"

#define SZERR_TOOMANYPARMS \
   "troppi parametri di riga di comando"

#define SZERR_UNSPEC_SPLITFILE \
   "il nome del file da dividere non Š stato specificato"

#define SZERR_SPLIT_EMPTYFILE \
   "la dimensione del file:\r\n"\
   "%s\r\n"\
   "Š di %lld byte.\r\n"\
   "Nessuna operazione di divisione Š possibile"

#define SZERR_PIECE_GT_WHOLE \
   "nessuna operazione possibile.\r\n" \
   "una parte di file Š di dimensione maggiore dell'intero file"

#define SZERR_PIECESGT_SIZE \
   "il numero delle parti di file Š superiore alla dimensione del file intero"

#define SZERR_CREATE_JOIN_CMD \
   "impossibile creare il file batch:\r\n%s\r\nper concatenare le parti di file"

#define SZERR_MISSJOINPIECES \
   "nessun file da concatenare Š stato specificato"

#define SZERR_JOINONLY1 \
   "solo un file da concatenare Š stato specificato"

#define SZERR_PATHCONFLICT \
   "le opzioni /%c e /%c sono incompatibili.\r\n"\
   "Il percorso dei file da concatenare deve essere specificato con "\
   "l'opzione /%c"

#define SZERR_QUERYFREESPACE \
   "controllo spazio libero su disco %c: fallito.\r\n"\
   "DosQueryFSInfo() riporta %d"

#define SZERR_DISKFULL \
   "spazio disco insufficiente per l'operazione richiesta"

// initialization messages

#define SZ_READINGLISTFILE \
   "Lettura elenco file da dividere da:\r\n%s."

#define SZ_CALCWHOLEFILESIZE \
   "Calcolo in corso della dimensione di:\r\n"\
   "%s\r\n" \
   "che sar… creato dalla concatenazione di %d file..."

// text message
#define SZ_OPENSPLITFILE \
   "Apertura di:\r\n %s..."

#define SZ_SPLITTING \
   "Divisione do:\r\n %s\r\nin %u parti di %lld byte..."

#define SZ_JOINING \
   "Unione di %d parti di %lld byte in:\r\n %s..."

#define SZ_OPENFILEPART \
   "Scrittura della parte di file % 5u\r\n"   \
   "(%s)...\r\n"

#define SZ_READFILEPART \
   "Lettura della parte di file % 5u\r\n"   \
   "(%s)...\r\n"

#define SZ_READPARTSIZE \
   "Controllo della dimensione di %s..."

#define SZ_ALLOCBUF \
   "Allocazione di %u byte per il buffer di lettura/scrittura"

#define SZ_SPLITOK \
   "%s\r\n Š stato diviso con successo in %d parti"

#define SZ_JOINOK \
   "%s\r\n Š stato creato con successo da %d parti"

#define SZ_SPLITFAILED \
   "Fallimento divisione di:\r\n %s\r\nin %d parti"

#define SZ_JOINFAILED  \
   "Fallimento della creazione di :\r\n %s\r\nda %d parti"

// menu messages
#define SZ_RETRY \
   "   [%c] ... riprovare"

#define SZ_TERMINATE \
   "   [%c] ... terminare il programma."

#define SZ_DISKFULL \
   "Lo spazio libero sul disco non Š sufficiente...\r\n"\
   SZ_RETRY" (dopo aver creato pi— spazio libero),\r\n"SZ_TERMINATE

// array of characters used to [R]etry or [T]erminate the program
#define ACH_RETRYABORT       "RT"
#define DISKFULL_RETRY        0
#define DISKFULL_ABORT        1

// message used on non-critical error conditions
#define SZ_IGNOREABORT \
   "Questo errore non Š critico\r\n" \
   "   [%c] ... ignorare l'errore e continuare,\r\n"SZ_TERMINATE

// array of characters used to [I]gnore an error or [T]erminate the program
#define ACH_IGNOREABORT      "IT"
#define ERROR_IGNORE         0
#define ERROR_ABORT          1

// message displayed when a file part was not found
#define SZ_PARTNOTFOUND \
   "Rilevamento dimensione file fallito...\r\n" \
   SZ_RETRY" (dopo aver inserito il supporto con il file mancante)\r\n"\
   SZ_TERMINATE

#define ACH_RETRY_ABORT     "SRT"
#define NOCBFILE_RETRY        0
#define NOCBFILE_ABORT        1


// message displayed when a file part could not be opened
#define SZ_PARTNOOPENED \
   "L'apertura del file Š fallita...\r\n"\
   SZ_RETRY",\r\n"SZ_TERMINATE

// file I/O errors

#define SZERR_TYPEHELPSYS \
   "Digitare \'HELP SYS%04d\' per maggiori dettagli sulle cause del problema.\r\n"

#define SZERR_OPENFILE \
   "impossibile aprire il file:\r\n%s.\r\n"

#define SZERR_READFILE \
   "operazione lettura file fallita%s.\r\n"

#define SZERR_WRITEFILE \
   "operazione scrittura file fallita%s.\r\n"

#define SZERR_WRITEFILE2 \
   "operazione scrittura fallita (da scrivere %u byte, scritti %u byte)"

#define SZERR_FILESIZE \
   "richiesta dimensione file fallita%s.\r\n"

#define SZERR_FILESIZE2 \
   "impossibile ottenere dimensione di:\r\n%s.\r\n"

// file I/O debug
#define SZ_FOPEN \
   "Apertura file:\r\n%s\r\nhandle:%d - rc:%d"

#define SZ_FCLOSE \
   "Chiusura file: hf:%d"

#define SZ_FREAD \
   "Lettura file: hf:%d - cbRead:%d - cbReadDone:%d - rc:%d"

#define SZ_FWRITE \
   "Scrittura file: hf:%d - cbWrite:%d - cbWriteDone:%d - rc:%d"

// showhelp.c messages

#define SZ_TBHEADER \
   " Usare i tasti direzionali per scollare il testo, premere Esc per terminare."

#define SZ_TBSTATUS \
   " Linea %4d di %4d"

#define SZ_TBFILESTART \
   " [ inizio file ]"

#define SZ_TBFILEEND \
   " [ fine del file ]"

#define SZERR_GETSCREENSIZE \
   "dimensione schermo sconosciuta (VioGetMode riporta %d)"

// ordinary messages --------------------------------------------------------

#define SZ_PRESSANYKEY \
   "Premere un tasto qualsiasi per continuare ...\r\n"

// other messages
#define SZ_PROGRESS \
   "stato corrente: % 6.2f %%"



#endif // #ifndef _APP_MSGS_H_
