000100!                                                                 POS00049
000200!   lrschema.cob - Schema for COBOL programming                   95/03/31
000300!                                                                 95/03/31
000500!   Copyright (c) 1991-2009 iMatix Corporation                    95/03/31
000600!                                                                 95/03/31
000700!  Usage:      Generates single copybook, with extension '.cob'.  95/03/31
000800!              By default, generates ANSI 74 COBOL, for a main    95/03/31
000900!              program (without linkage).  Accept these options:  95/03/31
001000!                                                                 95/03/31
001100!              -opt:model=main   - generate main program (default)95/03/31
001200!              -opt:model=called - generate called program        95/06/25
001300!              -opt:level=ansi74 - generate ANSI74 code (default) 95/03/31
001400!              -opt:level=ansi85 - generate ANSI85 code           95/03/31
001500!              -opt:ext=cbl      - use extension '.cbl' (default) 95/03/31
001600!              -opt:ext=xxx      - use extension '.xxx'           95/03/31
001700!              -opt:console=""   - suffix for DISPLAY verb        95/10/01
001800!              -opt:stack_max=n  - subdialog stack size (20)      95/11/02
001900!              -opt:template=xxx - template file (TEMPLATE.cob)   95/12/18
002000!                                                                 95/03/27
002100!              I recommend that your dialog carries the program   95/03/27
002200!              name followed by 'd', with extension '.l'.         95/03/27
002300!                                                                 95/03/27
002400!              Assumes linkage section in $SOURCE\R.$ext.         95/12/08
002500!                                                                 95/03/27
002800!  ------------------ GPL Licensed Source Code ------------------ 95/03/29
002800!  iMatix makes this software available under the GNU General     95/03/29
002800!  Public License (GPL) license for open source projects.  For    95/03/29
002800!  details of the GPL license please see www.gnu.org or read the  95/03/29
002800!  file license.gpl provided in this package.                     95/03/29
002800!                                                                 95/03/29
002800!  This program is free software; you can redistribute it and/or  95/03/29
002800!  modify it under the terms of the GNU General Public License as 95/03/29
002800!  published by the Free Software Foundation; either version 2 of 95/03/29
002800!  the License, or (at your option) any later version.            95/03/29
002800!                                                                 95/03/29
002800!  This program is distributed in the hope that it will be useful,95/03/29
002800!  but WITHOUT ANY WARRANTY; without even the implied warranty of 95/03/29
002800!  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  95/03/29
002800!  GNU General Public License for more details.                   95/03/29
002800!                                                                 95/03/29
002800!  You should have received a copy of the GNU General Public      95/03/29
002800!  License along with this program in the file 'license.gpl'; if  95/03/29
002800!  not, write to the Free Software Foundation, Inc., 59 Temple    95/03/29
002800!  Place - Suite 330, Boston, MA 02111-1307, USA.                 95/03/29
002800!                                                                 95/03/29
002800!  You can also license this software under iMatix's General Terms95/03/29
002800!  of Business (GTB) for commercial projects.  If you have not    95/03/29
002800!  explicitly licensed this software under the iMatix GTB you may 95/03/29
002800!  only use it under the terms of the GNU General Public License. 95/03/29
002800!                                                                 95/03/29
002800!  For more information, send an email to info@imatix.com.        95/03/29
002800!  -------------------------------------------------------------- 95/03/29
004200!=================================================================95/03/27
004300                                                                  95/03/31
004400:set array_base = 1                                               95/12/08
004500:set row_width  = 48                 # If $row is longer, wrap    95/12/08
004600                                                                  95/12/08
004700:declare string ext = "cbl"          # works best on Unix         95/12/08
004800:declare string level = "ansi74"     # most portable code         95/12/08
004900:declare string model = "main"       # main program               95/12/08
005000:declare string console = ""         # e.g. 'UPON TERMINAL'       95/12/08
005100:declare string template="TEMPLATE.cob"                           95/12/18
005200:option -style=cobol -noidle                                      95/12/08
005300                                                                  95/12/08
005400!  Check that program name and dialog name are different          95/03/29
005500                                                                  95/03/31
005600:if "$source" = "$dialog"                                         95/10/01
005700:  if "$ext" = "cob"                                              98/07/14
005800:    echo "lr E: dialog cannot have same name as source file"     95/04/01
005900:    exit 1                                                       95/04/01
006000:  endif                                                          98/07/14
006100:endif                                                            95/03/31
006200                                                                  95/03/31
006300!  Generate skeleton program if none already exists               95/03/29
006400                                                                  95/03/31
006500:if not exist $SOURCE.cob                                         95/04/01
006600:echo "lr I: creating skeleton program $SOURCE.cob..."            95/04/01
006700:output $SOURCE.cob                                               95/04/01
006800 IDENTIFICATION DIVISION.                                         95/03/29
006900 PROGRAM-ID.    $SOURCE.                                          95/03/29
007000                                                                  95/03/29
007100 AUTHOR.        $AUTHOR.                                          95/04/01
007200 DATE-WRITTEN.  $DATE                                             95/03/29
007300     -REVISED:  $DATE.                                            95/03/29
007400:include optional $template "<HEADER>" "<END>"                    95/12/18
007500                                                                  95/12/18
007600 ENVIRONMENT DIVISION.                                            95/03/29
007700                                                                  95/03/29
007800 CONFIGURATION SECTION.                                           95/03/29
007900 SOURCE-COMPUTER. PORTABLE.                                       95/03/29
008000 OBJECT-COMPUTER. PORTABLE.                                       95/03/29
008100                                                                  95/03/29
008200 DATA DIVISION.                                                   95/03/29
008300                                                                  95/03/29
008400 WORKING-STORAGE SECTION.                                         95/03/29
008500:include optional $template "<DATA>" "<END>"                      95/12/18
008600                                                                  95/12/18
008700*DIALOG DATA AND INTERPRETER.                                     95/03/29
008800 COPY $DIALOG.                                                    95/03/29
008900                                                                  95/03/29
009000*******************   INITIALISE THE PROGRAM   *******************95/08/07
009100                                                                  95/03/29
009200 INITIALISE-THE-PROGRAM.                                          95/03/29
009300     MOVE OK-EVENT TO THE-NEXT-EVENT                              95/03/29
009400:include optional $template "<Initialise-The-Program>" "<END>"    95/12/18
009500     .                                                            95/03/29
009600                                                                  95/03/29
009700*********************   GET EXTERNAL EVENT   *********************95/03/31
009800                                                                  95/03/31
009900 GET-EXTERNAL-EVENT.                                              95/03/31
010000:include optional $template "<Get-External-Event>" "<END>"        95/12/18
010100:if $included = 0                                                 95/12/18
010200     EXIT                                                         95/03/31
010300:endif                                                            95/12/18
010400     .                                                            95/03/31
010500                                                                  95/03/31
010600********************   TERMINATE THE PROGRAM   *******************95/08/07
010700                                                                  95/03/29
010800 TERMINATE-THE-PROGRAM.                                           95/03/29
010900     MOVE TERMINATE-EVENT TO THE-NEXT-EVENT                       95/03/29
011000:include optional $template "<Terminate-The-Program>" "<END>"     95/12/18
011100     .                                                            95/03/29
011200:close                                                            95/04/01
011300:endif                                                            95/04/01
011400:if "$model" = "called"                                           96/04/03
011500:if not exist "$SOURCE\R.$ext"                                    95/12/08
011600:echo "lr I: creating linkage copybook $SOURCE\R.$ext..."         95/12/08
011700:output $SOURCE\R.$ext                                            95/12/08
011800*    Copybook for calling $SOURCE                                 95/12/08
011900*                                                                 95/12/08
012000*    Generated: $date     Libero $version                         95/12/08
012100*    Revised:   $date     $author                                 95/12/08
012200*                                                                 95/12/08
012300*    To use:    place operation code in $SOURCE-CONTROL and       95/12/08
012400*               CALL "$SOURCE"                                    95/12/08
012500*                   USING $SOURCE-CONTROL                         95/12/08
012600*                                                                 95/12/08
012700*    Returns:   If $SOURCE-FEEDBACK = SPACE, there were no        95/12/08
012800*               errors.  Else $SOURCE-FEEDBACK indicates the      95/12/08
012900*               cause or nature of the error.                     95/12/08
013000*                                                                 95/12/08
013100 01  $SOURCE-CONTROL.                                             95/12/08
013200*CONTENTS                                                         95/12/08
013300     02  $SOURCE-OPERATION       PIC X      VALUE SPACE.          95/12/08
013400     02  $SOURCE-FEEDBACK        PIC X      VALUE SPACE.          95/12/08
013500:close                                                            95/12/08
013600:endif                                                            95/12/08
013700:endif                                                            96/04/03
013800                                                                  95/03/31
013900:output $DIALOG.$ext                                              95/03/29
014000:echo "lr I: building $DIALOG.$ext..."                            95/03/31
014100*----------------------------------------------------------------*95/03/28
014200*  $DIALOG.$ext - Libero dialog definitions for $SOURCE          *95/03/29
014300*  Generated by Libero $version on $fulldate, $time.             *95/03/29
014400*  Schema file used: $schema                                     *95/10/01
014500*----------------------------------------------------------------*95/10/01
014600                                                                  95/03/27
014700 01  LR--DIALOG-CONSTANTS.                                        95/03/29
014800     02  TERMINATE-EVENT         PIC S9(3)  COMP VALUE -1.        95/10/01
014900:if check                                                         95/03/30
015000     02  LR--NULL-EVENT          PIC S9(3)  COMP VALUE ZERO.      95/10/01
015100:endif                                                            95/03/30
015200:do event                                                         95/03/30
015300     02  $NAME                   PIC S9(3)  COMP VALUE +$number.  95/03/29
015400:enddo                                                            95/03/30
015500     02  LR--DEFAULTS-STATE      PIC S9(3)  COMP VALUE +$defaults.95/04/01
015600:do state                                                         95/10/01
015700     02  LR--STATE-$NAME         PIC S9(3)  COMP VALUE +$number.  95/05/03
015800:enddo                                                            95/03/30
015900                                                                  95/03/27
016000 01  LR--DIALOG-VARIABLES.                                        95/03/29
016100     02  LR--EVENT               PIC S9(3)  COMP VALUE ZERO.      95/03/29
016200     02  LR--STATE               PIC S9(3)  COMP VALUE ZERO.      95/03/29
016300     02  LR--SAVEST              PIC S9(3)  COMP VALUE ZERO.      95/03/29
016400     02  LR--INDEX               PIC S9(3)  COMP VALUE ZERO.      95/03/29
016500     02  LR--VECPTR              PIC S9(3)  COMP VALUE ZERO.      95/03/29
016600     02  LR--MODNBR              PIC S9(3)  COMP VALUE ZERO.      95/03/29
016700     02  THE-NEXT-EVENT          PIC S9(3)  COMP VALUE ZERO.      95/03/29
016800     02  THE-EXCEPTION-EVENT     PIC S9(3)  COMP VALUE ZERO.      95/03/29
016900     02  EXCEPTION-RAISED        PIC X      VALUE SPACE.          95/03/29
017000         88  EXCEPTION-IS-RAISED            VALUE "Y".            95/03/29
017100:if module "Dialog-Call"                                          95/11/02
017200:  if not event "Return"                                          95/11/02
017300:     echo "lr E: you must define the 'Return' event              95/11/02
017400:     exit 1                                                      95/11/02
017500:  endif                                                          95/11/02
017600:  declare int stack_max = 20                                     95/11/02
017700     02  LR--STACK-SIZE          PIC S9(3)  COMP.                 95/11/02
017800     02  LR--STACK               PIC S9(3)  COMP                  95/11/02
017900                                            OCCURS $stack_max.    95/11/04
018000:endif                                                            95/11/04
018100                                                                  95/11/04
018200:declare int iw                  # size of item in row            95/03/31
018300:declare int rw                  # size of this row               95/03/31
018400:if $states < 10                                                  95/03/30
018500:  set iw=1                                                       95/03/31
018600:else                                                             95/03/30
018700:if $states < 100                                                 95/03/31
018800:  set iw=2                                                       95/03/31
018900:else                            #  assume max 999 states :-0     95/03/31
019000:  set iw=3                                                       95/03/31
019100:endif all                                                        95/03/31
019200:set number_fmt = "%ld"                                           95/03/31
019300:set row_first  = "%0$iw\ld"                                      95/03/31
019400:set row_after  = "%0$iw\ld"                                      95/03/31
019500:set number_fmt = "%03ld"                                         95/03/31
019600:                                                                 95/03/30
019700 01  LR--NEXT-STATES.                                             95/03/29
019800:do nextst                                                        95/03/30
019900:  set rw=$tally * $iw                                            95/03/31
020000:  if $rw > 12                                                    95/03/31
020100     02  FILLER                  PIC X($rw) VALUE                 95/03/31
020200         "$row".                                                  95/03/29
020300:  else                                                           95/03/31
020400     02  FILLER                  PIC X($rw) VALUE "$row".         95/03/31
020500:  endif                                                          95/03/31
020600:  do overflow                                                    95/03/31
020700:    set rw=$tally * $iw                                          95/03/31
020800:    if $rw > 12                                                  95/03/31
020900     02  FILLER                  PIC X($rw) VALUE                 95/03/31
021000         "$row".                                                  95/03/31
021100:    else                                                         95/03/31
021200     02  FILLER                  PIC X($rw) VALUE "$row".         95/03/31
021300:    endif                                                        95/03/31
021400:  enddo                                                          95/03/31
021500:enddo                                                            95/03/30
021600 01  FILLER                      REDEFINES  LR--NEXT-STATES.      95/03/29
021700     02  FILLER                             OCCURS $states TIMES. 95/03/29
021800         03  LR--NEXTST          PIC 9($iw) OCCURS $events TIMES. 95/03/31
021900                                                                  95/03/29
022000:if $vectors < 10                                                 95/03/31
022100:  set iw=1                                                       95/03/31
022200:else                                                             95/03/31
022300:if $vectors < 100                                                95/03/31
022400:  set iw=2                                                       95/03/31
022500:else                                                             95/03/31
022600:  set iw=3                                                       95/03/31
022700:endif all                                                        95/03/31
022800:set number_fmt = "%ld"                                           95/03/31
022900:set row_first  = "%0$iw\ld"                                      95/03/31
023000:set row_after  = "%0$iw\ld"                                      95/03/31
023100:set number_fmt = "%03ld"                                         95/03/31
023200:                                                                 95/03/31
023300 01  LR--ACTIONS.                                                 95/03/29
023400:do action                                                        95/03/30
023500:  set rw = $tally * $iw                                          95/03/31
023600:  if $rw > 12                                                    95/03/31
023700     02  FILLER                  PIC X($rw) VALUE                 95/03/31
023800         "$row".                                                  95/03/31
023900:  else                                                           95/03/31
024000     02  FILLER                  PIC X($rw) VALUE "$row".         95/03/31
024100:  endif                                                          95/03/31
024200:  do overflow                                                    95/03/31
024300:    set rw=$tally * $iw                                          95/03/31
024400:    if $rw > 12                                                  95/03/31
024500     02  FILLER                  PIC X($rw) VALUE                 95/03/31
024600         "$row".                                                  95/03/31
024700:    else                                                         95/03/31
024800     02  FILLER                  PIC X($rw) VALUE "$row".         95/03/31
024900:    endif                                                        95/03/31
025000:  enddo                                                          95/03/31
025100:enddo                                                            95/03/30
025200 01  FILLER                      REDEFINES  LR--ACTIONS.          95/03/29
025300     02  FILLER                             OCCURS $states TIMES. 95/03/29
025400         03  LR--ACTION          PIC 9($iw) OCCURS $events TIMES. 95/03/31
025500                                                                  95/03/29
025600 01  LR--OFFSETS.                                                 95/03/29
025700:do vector                                                        95/04/09
025800     02  FILLER                  PIC S9(3)  COMP VALUE +$offset.  95/04/09
025900:enddo                                                            95/03/30
026000 01  FILLER                      REDEFINES  LR--OFFSETS.          95/03/29
026100     02  LR--OFFSET              PIC S9(3)  OCCURS $vectors COMP. 95/11/04
026200                                                                  95/03/29
026300:declare int tblsize = 0         # total size of table            95/03/31
026400:declare string null                                              95/03/31
026500:if $modules < 10                                                 95/03/31
026600:  set iw=1                                                       95/03/31
026700:  set null="0"                                                   95/03/31
026800:else                                                             95/03/31
026900:if $modules < 100                                                95/03/31
027000:  set iw=2                                                       95/03/31
027100:  set null="00"                                                  95/03/31
027200:else                                                             95/03/31
027300:  set iw=3                                                       95/03/31
027400:  set null="000"                                                 95/03/31
027500:endif all                                                        95/03/31
027600:set number_fmt = "%ld"                                           95/03/31
027700:set row_first  = "%0$iw\ld"                                      95/03/31
027800:set row_after  = "%0$iw\ld"                                      95/03/31
027900:set number_fmt = "%03ld"                                         95/03/31
028000:                                                                 95/03/31
028100 01  LR--MODULES.                                                 95/03/29
028200:do vector                                                        95/03/30
028300:  set rw = $tally * $iw                                          95/03/31
028400:  if $rw > 28                                                    95/03/31
028500     02  FILLER                  PIC X($rw) VALUE                 95/03/31
028600:    if "$row" = ""                                               95/12/11
028700         "$null".                                                 95/12/11
028800:    else                                                         95/12/11
028900         "$row$null".                                             95/12/11
029000:    endif                                                        95/12/11
029100:  else                                                           95/03/30
029200:    if "$row" = ""                                               95/12/11
029300     02  FILLER  PIC X($rw) VALUE "$null".                        95/12/11
029400:    else                                                         95/12/11
029500     02  FILLER  PIC X($rw) VALUE "$row$null".                    95/12/11
029600:    endif                                                        95/12/11
029700:  endif                                                          95/03/30
029800:  set tblsize = $tblsize + $tally                                95/03/31
029900:enddo                                                            95/03/30
030000 01  FILLER                      REDEFINES  LR--MODULES.          95/03/31
030100     02  LR--MODULE              PIC 9($iw) OCCURS $tblsize TIMES.95/11/04
030200                                                                  95/03/29
030300:if animate                                                       95/10/01
030400:push $style                     #  Set temporary animation style 95/11/18
030500:option -style=normal                                             95/11/05
030600 01  LR--MNAMES.                                                  95/10/01
030700:  do module                                                      95/10/01
030800     02  FILLER  PIC X(30) VALUE "$name".                         95/11/04
030900:  enddo                                                          95/10/01
031000 01  FILLER           REDEFINES  LR--MNAMES.                      95/10/01
031100     02  LR--MNAME    PIC X(30)  OCCURS $modules TIMES.           95/10/01
031200                                                                  95/10/01
031300 01  LR--SNAMES.                                                  95/10/01
031400:  do state                                                       95/10/01
031500     02  FILLER  PIC X(30) VALUE "$name".                         95/11/04
031600:  enddo                                                          95/10/01
031700 01  FILLER           REDEFINES  LR--SNAMES.                      95/10/01
031800     02  LR--SNAME    PIC X(30)  OCCURS $states TIMES.            95/10/01
031900                                                                  95/10/01
032000 01  LR--ENAMES.                                                  95/10/01
032100:  do event                                                       95/10/01
032200     02  FILLER  PIC X(30) VALUE "$name".                         95/11/04
032300:  enddo                                                          95/10/01
032400 01  FILLER           REDEFINES  LR--ENAMES.                      95/10/01
032500     02  LR--ENAME    PIC X(30)  OCCURS $events TIMES.            95/10/02
032600                                                                  95/10/01
032700:pop $style                                                       95/11/18
032800:option -style=$style                                             95/11/18
032900:endif                                                            95/10/01
033000:if "$model" = "main"                                             95/03/31
033100 PROCEDURE DIVISION.                                              95/03/29
033200:else                                                             95/03/31
033300:if "$model" = "called"                                           95/06/25
033400 LINKAGE SECTION.                                                 95/03/31
033500                                                                  95/03/31
033600 01  PROGRAM-CONTROL.                                             95/03/31
033700:include "$SOURCE\R.$ext" "*CONTENTS"                             95/04/26
033800                                                                  95/03/31
033900 PROCEDURE DIVISION                                               95/03/31
034000     USING PROGRAM-CONTROL                                        95/03/31
034100     .                                                            95/03/31
034200:else                                                             95/03/31
034300:  echo "lr E: invalid /option - use /opt:model=[main|called]"    95/06/25
034400:  exit 1                                                         95/03/31
034500:endif all                                                        95/03/31
034600                                                                  95/03/29
034700 LR--BEGIN-PROGRAM.                                               95/03/29
034800     MOVE  +1  TO LR--STATE                                       95/11/02
034900:if module "Dialog-Call"                                          95/11/02
035000     MOVE ZERO TO LR--STACK-SIZE                                  95/11/02
035100:endif                                                            95/11/02
035200     PERFORM INITIALISE-THE-PROGRAM                               95/03/29
035300     PERFORM LR--EXECUTE-DIALOG                                   95/03/29
035400       UNTIL THE-NEXT-EVENT = TERMINATE-EVENT                     95/03/29
035500     .                                                            95/03/29
035600 LR--END-PROGRAM.                                                 95/10/01
035700     EXIT PROGRAM                                                 95/03/29
035800     .                                                            95/03/29
035900 LR--STOP-PROGRAM.                                                95/10/01
036000     STOP RUN                                                     95/03/29
036100     .                                                            95/03/29
036200                                                                  95/10/01
036300 LR--EXECUTE-DIALOG.                                              95/03/29
036400     MOVE THE-NEXT-EVENT TO LR--EVENT                             95/03/29
036500:if check                                                         95/03/29
036600     IF LR--EVENT > $events OR LR--EVENT < 1                      95/03/29
036700         DISPLAY "State " LR--STATE " - event " LR--EVENT         95/04/03
036800                 " is out of range"                               95/10/01
036900:  if "$console" != ""                                            95/10/01
037000                 $console                                         95/10/01
037100:  endif                                                          95/10/01
037200         PERFORM LR--STOP-PROGRAM                                 95/11/04
037300     .                                                            95/03/29
037400:endif                                                            95/03/29
037500     MOVE LR--STATE                         TO LR--SAVEST         95/03/29
037600     MOVE LR--ACTION (LR--STATE, LR--EVENT) TO LR--INDEX          95/03/29
037700:if defaults                                                      95/03/29
037800*    IF NO ACTION FOR THIS EVENT, TRY THE DEFAULTS STATE          95/03/29
037900     IF LR--INDEX = 0                                             95/03/29
038000         MOVE LR--DEFAULTS-STATE                TO LR--STATE      95/04/01
038100         MOVE LR--ACTION (LR--STATE, LR--EVENT) TO LR--INDEX      95/03/29
038200     .                                                            95/03/29
038300:endif                                                            95/03/29
038400:if animate                                                       95/10/01
038500     DISPLAY " "                                                  95/11/05
038600     DISPLAY LR--SNAME (LR--STATE) ":"                            95/10/01
038700:  if "$console" != ""                                            95/10/01
038800             $console                                             95/10/01
038900:  endif                                                          95/10/01
039000     DISPLAY "    (--) " LR--ENAME (LR--EVENT)                    95/10/02
039100:  if "$console" != ""                                            95/10/01
039200             $console                                             95/10/01
039300:  endif                                                          95/10/01
039400:endif                                                            95/10/01
039500:if check                                                         95/03/29
039600     IF LR--INDEX = ZERO                                          95/03/29
039700         DISPLAY "State " LR--STATE " - event " LR--EVENT         95/04/03
039800                 " is not accepted"                               95/10/01
039900:  if "$console" != ""                                            95/10/01
040000                 $console                                         95/10/01
040100:  endif                                                          95/10/01
040200         PERFORM LR--STOP-PROGRAM                                 95/11/04
040300     .                                                            95/03/29
040400     MOVE     LR--NULL-EVENT     TO THE-NEXT-EVENT                95/03/31
040500:endif                                                            95/03/29
040600     MOVE     LR--NULL-EVENT     TO THE-EXCEPTION-EVENT           95/03/29
040700     MOVE         SPACE          TO EXCEPTION-RAISED              95/03/29
040800     MOVE LR--OFFSET (LR--INDEX) TO LR--VECPTR                    95/03/29
040900     PERFORM LR--EXECUTE-ACTION-VECTOR                            95/03/29
041000       VARYING LR--VECPTR FROM LR--VECPTR BY 1                    95/03/29
041100         UNTIL LR--MODULE (LR--VECPTR) = ZERO                     95/03/29
041200            OR EXCEPTION-IS-RAISED                                95/03/29
041300                                                                  95/03/29
041400     IF EXCEPTION-IS-RAISED                                       95/03/29
041500         PERFORM LR--GET-EXCEPTION-EVENT                          95/03/29
041600     ELSE                                                         95/03/29
041700         MOVE LR--NEXTST (LR--STATE, LR--EVENT) TO LR--STATE      95/03/29
041800     .                                                            95/03/29
041900:if defaults                                                      95/05/18
042000     IF LR--STATE = LR--DEFAULTS-STATE                            95/05/18
042100         MOVE LR--SAVEST TO LR--STATE                             95/05/18
042200     .                                                            95/05/18
042300:endif                                                            95/05/18
042400     IF THE-NEXT-EVENT = LR--NULL-EVENT                           95/03/29
042500         PERFORM GET-EXTERNAL-EVENT                               95/03/31
042600:if check                                                         96/02/03
042700         IF THE-NEXT-EVENT = LR--NULL-EVENT                       95/03/31
042800             DISPLAY "No event set after event " LR--EVENT        95/03/31
042900                     " in state " LR--STATE                       95/10/01
043000:  if "$console" != ""                                            95/10/01
043100                     $console                                     95/10/01
043200:  endif                                                          95/10/01
043300             PERFORM LR--STOP-PROGRAM                             95/11/04
043400:endif                                                            96/02/03
043500     .                                                            95/03/29
043600:                                                                 95/03/29
043700:declare int    modto            # last of group of 10            95/08/07
043800:declare int    modfrom          # first of group of 10           95/08/07
043900:declare int    modbase          # last of previous group, or     95/08/07
044000:declare int    modloop          # loop counter                   95/08/07
044100:declare string modelse          # 'else' or spaces               95/08/07
044200:set comma_before="ELSE"                                          95/03/29
044300:set comma_last=""                                                95/03/29
044400                                                                  95/03/29
044500 LR--EXECUTE-ACTION-VECTOR.                                       95/03/29
044600     MOVE LR--MODULE (LR--VECPTR) TO LR--MODNBR                   95/04/26
044700:if animate                                                       95/10/01
044800     DISPLAY "          + " LR--MNAME (LR--MODNBR)                95/10/01
044900:  if "$console" != ""                                            95/10/01
045000             $console                                             95/10/01
045100:  endif                                                          95/10/01
045200:endif                                                            95/10/01
045300:set number_fmt = "%02ld"                                         95/03/31
045400:if "$LEVEL" = "ANSI74"                                           95/03/29
045500:if $modules > 10                # do gymnastics if > 10 modules  95/03/29
045600:  set modto = $modules                                           95/03/29
045700:  do while $modto > 10                                           95/03/29
045800:    set modbase = ($modto - 1) / 10 * 10                         95/03/29
045900:    set modfrom = $modbase + 1                                   95/03/29
046000     IF LR--MODNBR > $modbase                                     95/03/29
046100         PERFORM LR--EXECUTE-$modfrom-$modto                      95/03/29
046200     ELSE                                                         95/03/29
046300:    set modto = $modbase                                         95/03/29
046400:  enddo                                                          95/03/29
046500         PERFORM LR--EXECUTE-01-$modto                            95/03/29
046600:endif                                                            95/03/29
046700!    Calculate if we need to print a split header                 95/03/29
046800:set modfrom = 1                                                  95/03/29
046900:set modloop = 0                                                  95/03/29
047000:do module                                                        95/03/29
047100:  set modto = $modfrom + 9                                       95/03/29
047200:  if $modto > $modules                                           95/03/29
047300:    set modto = $modules                                         95/03/29
047400:  endif                                                          95/03/29
047500:  if $modules > 10                                               95/03/29
047600:    if $modloop = 0                                              95/03/29
047700     .                                                            95/03/29
047800                                                                  95/03/29
047900 LR--EXECUTE-$modfrom-$modto.                                     95/03/29
048000:      set modfrom = $modfrom + 10                                95/03/29
048100:      set modloop = 10                                           95/03/29
048200:    endif                                                        95/03/29
048300:    set modloop = $modloop - 1                                   95/03/29
048400:  endif                                                          95/03/29
048500     IF LR--MODNBR = $number                                      95/03/29
048600:  set modelse="$comma"                                           95/03/29
048700:  if $modules > 10                                               95/03/29
048800:    if $modloop = 0                                              95/03/29
048900:      set modelse=""                                             95/03/29
049000:    endif                                                        95/03/29
049100:  endif                                                          95/03/29
049200         PERFORM $NAME                            $MODELSE        95/03/29
049300:enddo                                                            95/03/29
049400:else                                                             95/03/29
049500:if "$LEVEL" = "ANSI85"                                           95/03/29
049600     EVALUATE LR--MODNBR                                          95/03/29
049700:  do module                                                      95/03/29
049800         WHEN $number PERFORM $NAME                               95/03/29
049900:  enddo                                                          95/03/29
050000     END-EVALUATE                                                 95/03/29
050100:else                                                             95/03/29
050200:  echo "lr E: invalid /option - use /opt:level=[ansi74|ansi85]"  95/03/31
050300:  exit 1                                                         95/03/29
050400:endif all                                                        95/03/29
050500     .                                                            95/03/29
050600                                                                  95/03/29
050700 LR--GET-EXCEPTION-EVENT.                                         95/03/29
050800     IF THE-EXCEPTION-EVENT NOT = LR--NULL-EVENT                  95/03/29
050900         MOVE THE-EXCEPTION-EVENT TO LR--EVENT                    95/03/29
051000     .                                                            95/03/29
051100     MOVE LR--EVENT TO THE-NEXT-EVENT                             95/03/29
051200:if animate                                                       95/10/02
051300     DISPLAY "    (=>) " LR--ENAME (LR--EVENT)                    95/10/02
051400:  if "$console" != ""                                            95/10/02
051500             $console                                             95/10/02
051600:  endif                                                          95/10/02
051700:endif                                                            95/10/02
051800     .                                                            95/03/29
051900                                                                  95/03/31
052000:if module "Dialog-Call"                                          95/11/02
052100 DIALOG-CALL.                                                     95/11/02
052200     IF LR--STACK-SIZE < $stack_max                               95/11/02
052300         ADD 1 TO LR--STACK-SIZE                                  95/11/05
052400         MOVE LR--STATE TO LR--STACK (LR--STACK-SIZE)             95/11/02
052500     ELSE                                                         95/11/02
052600         DISPLAY "State " LR--STATE " - Dialog-Call overflow"     95/11/02
052700         PERFORM LR--STOP-PROGRAM                                 95/11/04
052800     .                                                            95/11/02
052900                                                                  95/11/02
053000 DIALOG-RETURN.                                                   95/11/02
053100     IF LR--STACK-SIZE > ZERO                                     95/11/02
053200         MOVE LR--STACK (LR--STACK-SIZE) TO LR--STATE             95/11/02
053300         MOVE        RETURN-EVENT        TO THE-EXCEPTION-EVENT   95/11/05
053400         MOVE           "YES"            TO EXCEPTION-RAISED      95/11/05
053500         ADD -1 TO LR--STACK-SIZE                                 95/11/05
053600     ELSE                                                         95/11/02
053700         DISPLAY "State " LR--STATE " - Dialog-Return underflow"  95/11/02
053800         PERFORM LR--STOP-PROGRAM                                 95/11/04
053900     .                                                            95/11/02
054000:endif                                                            95/11/02
054100:close                                                            95/03/29
054200!  Generate stubs for all modules not yet defined in source       95/03/29
054300                                                                  95/03/31
054400:internal "initialise_the_program"                                95/05/19
054500:internal "get_external_event"                                    95/05/19
054600:set stub_first   = "*"                                           95/11/03
054700:set stub_between = "*"                                           95/11/03
054800:set stub_last    = "*"                                           95/11/03
054900:set stub_width   = 66                                            95/11/03
055000:set module_line = " %s."                                         95/11/03
055100:do stubs $SOURCE.cob $DIALOG.$ext                                95/11/02
055200                                                                  95/03/29
055300 $NAME.                                                           95/03/31
055400:include optional $template "<$module_name>" "<END>"              95/12/18
055500:if $included = 0                                                 95/12/18
055600     EXIT                                                         95/12/18
055700:endif                                                            95/12/18
055800     .                                                            95/03/29
055900:enddo                                                            95/03/29
