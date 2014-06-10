!============================================================================*
!                                                                            *
!   lrschema.pas LIBERO code generation for Delphi FSM Object                *
!                                                                            *
!   Written: Friday, October 01, 1999 Nyimbi Odero nyimbi@thinkingsys.com    *
!                                                                            *
!   FSM Code Generator.  Copyright (c) 1991-99 iMatix.                       *
!                                                                            *
!   This program is free software; you can redistribute it and/or modify     *
!   it under the terms of the GNU General Public License as published by     *
!   the Free Software Foundation; either version 2 of the License, or        *
!   (at your option) any later version.                                      *
!                                                                            *
!   This program is distributed in the hope that it will be useful,          *
!   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
!   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
!   GNU General Public License for more details.                             *
!                                                                            *
!   You should have received a copy of the GNU General Public License        *
!   along with this program; if not, write to the Free Software              *
!   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                *
!============================================================================*

:declare string $template = "template.vas"
:declare string model     = "object"
:declare string extend    = ""
:declare int $stack_max = 20

:if "$model" = "object"
:else
:if "$model" = "console"
:else
:if "$model" = "applet"
:else
:   echo "lr E: invalid model '$model'; use -opt:model=[object|console|applet]"
:   exit 1
:endif all


:if not exist $source.pas
:echo "lr I: creating skeleton program $source.pas..."
:output $source.pas
{===================================================================+
|   COPYRIGHT (C):  Thinking Systems Ltd, 1999
|   PROJECT      :
|   FILE         :  $source
|   PURPOSE      :  Finite State Machine to do ....
|   VERSION      :  0.4
|   AUTHOR       :  $author
|   LANGUAGE     :  Object Pascal (Delphi 4.0)
|   TARGET       :  Win 32 (95/98/NT 4.0)
|   TABS/WRAP    :  4/70
|   DATE         :  $fulldate $fulltime
|   COMMENTS     :  Declares following types and Constants
|                   Default State =  $defaults
|
+===================================================================} 
:include optional $template "<HEADER>" "<END>"

:include optional $template "<DATA>" "<END>"
unit $source ;
interface
uses
    SysUtils,
    Classes;

const
: declare int $debugLogLevel = 5
  Log_Debug = $debugLogLevel;

: declare int $defLogLevel = 3
  Log_Default = $defLogLevel; //Default Logging Level

: declare int $errorLogLevel=0
  Log_ErrorLvl = $errorLogLevel;

{Define Base Class}
type
  TLogWrite = procedure (aMsg: String; aLevel: Integer) of object;
  TParamLessProcedure = procedure of object;

  T$source = Class(TObject)
  private
    {Private Deeclarations}
    //FContextOwner is the class that will own this state machine and will implement any actions
    //defined in the state machine
    //OwnerType needs to be defined using the -option:name=value conditional
:   declare string $ownerType
    FContextOwner : TObject;
    FOnLogWrite   : TLogWrite;
    FEvent        : Integer;    //  Event for state transition
    FState        : Integer;    //  Current dialog state
    FSavest       : Integer;     //  Saved dialog state
    FIndex        : Integer;     //  Index of methods function
    FNextEvent    : Integer;    //  Next event from module
    FExceptionEvent : Integer;  //  Exception event from module
    FExceptionRaised :  Boolean;
    FOnGetExternalEvent : TParamLessProcedure;
    procedure LogWrite(aMsg : String; aLevel: Integer = Log_Default);
    procedure setExceptionEvent(aEvent: Integer);
  protected
    {Protected Declarations}
    // declare variables
    LR_sname: Array[0..$states] of String; //State Names
:declare count = $events - 1
!:set count = $events - 1
    LR_ename: Array[0..$events] of String; //Event names
    LR_mname: Array[0..$modules] of String; //Module Names
    LR_action: Array[0..$states,0..$events] of Integer; //Actions
    LR_nextst: Array[0..$states,0..$events] of Integer; //Next State Table
    _LR_vector : Array of Array of Integer; //[1..$vectors,1..$maxaction+1] of Integer; //Vector
    LR_offset: Array[0..$vectors] of Integer;
    // declare the procedures that are part of the object
:  do module
    procedure $name;
:  enddo
  public
    {Public Declarations}
    Constructor Create(aContextOwner : TObject);
    destructor Destroy; override;
    procedure Init;
    procedure $"initialise_the_program";
    procedure $"get_external_event";
    function execute: Integer;
    procedure LR_initialise;
    property OnLogWrite : TLogWrite read FOnLogWrite write FOnLogWrite;
    property NextEvent : Integer read FNextEvent write FNextEvent;
    property ExceptionEvent : Integer read FExceptionEvent write setExceptionEvent;
    property ExceptionRaised : Boolean read FExceptionRaised;
    property State : Integer read FState;
    property OnGetExternalEvent : TParamLessProcedure read FOnGetExternalEvent write FOnGetExternalEvent;
  published
      {Published declarations}
  end;

implementation

uses
//ownerUnit is the name of the unit that contains the owner of this state machine
//so that callbacks can be made
//ownerUnit must be defined using -option:name=value conditional
:declare string $ownerUnit
  $ownerUnit,
  $source\i;

//////   C O N T R U C T O R //// D E S T R U C T O R ///////////////////////////
procedure T$source.init;
begin
   // Add your custom init here
end;

constructor T$source.Create(aContextOwner : TObject);
begin
  inherited Create;
  LR_initialise;
  FContextOwner := aContextOwner;
  $"initialise_the_program";
end;


destructor T$source.destroy;
begin
  FContextOwner := nil;
  inherited;
end;

procedure T$source.setExceptionEvent (aEvent: Integer);
begin
  FExceptionRaised := (aEvent >= 0);
  
  if FExceptionRaised then
    FExceptionEvent := aEvent
  else
    FExceptionEvent := LR_NULL_EVENT;
end;

//////////////////////////   INITIALISE THE PROGRAM   /////////////////////////

procedure T$source.$"initialise_the_program";
begin
  NextEvent := $"ok_event";
:include optional $template "<Initialise-The-Program>" "<END>"
end;

///////////////////////////  OPTIONAL LOGGING    //////////////////////////////
procedure T$source.LogWrite(aMsg : String; aLevel: Integer);
begin
  if assigned(OnLogWrite) then
    OnLogWrite(aMsg, aLevel);
end;

////////////////////////////   GET EXTERNAL EVENT   ///////////////////////////

procedure T$source.$"get_external_event";
begin
:include optional $template "<Get-External-Event>" "<END>"
  if assigned(OnGetExternalEvent) then
    OnGetExternalEvent;
end;
//////////////////////////// LR_INITAILIZE /////////////////////////////////
Procedure T$source.LR_initialise;
const
  values_loaded: Boolean = False;         // First-time flag

		procedure LoadVect(args : array of Integer; j : Integer);
    var
      I : Integer;
		begin
      for I := 0 to high(args) do
			  _LR_vector[j][I] := args[I];
		end;
begin
    If values_loaded Then Exit;        //  If we already initialised tables
    values_loaded := True;
    SetLength(_LR_vector,$vectors+1);//,$maxaction+1); //0 ?
    SetLength(_LR_vector[0], 1);
    _LR_vector[0][0] := 0;
:
:push $row_first
:push $row_after
:set $row_first = "%ld"
:set $row_after = ",%ld"
:declare offsetidx = $number + 1
:declare int $cnt = 0
:do vector
:set offsetidx = $number + 2
:  if "$row" = ""
     SetLength(_LR_vector[$offsetidx-1], 1);
     _LR_vector[$offsetidx-1][0] := _LR_STOP;
     //LoadVect(IntToStr(_LR_STOP)+','+'$row',$offsetidx-1);
:  else
     setlength(_LR_vector[$offsetidx-1], $tally);
     LoadVect([ $row, _LR_STOP ],$offsetidx-1);
:  endif
:enddo
:pop $row_first
:pop $row_after
// LR_offset Initialization
:do vector
:set offsetidx = $number + 1
    LR_offset[$offsetidx] := $offset ;
:enddo  #vector


:#if animate

:push $style
:option -style=normal
//    private static String _LR_mname [] = {
:  do module
        LR_mname[$number] := '$name';
:  enddo

//    private static String _LR_sname [] = {
:  do state
       LR_sname[$number] := '$name';
:  enddo


//    private static String LR_ename [] = {
:  do event
        LR_ename[$number] := '$name';
:  enddo
end; //LR_Initialize

:push $style
:option -style=plain
////////////////////////////   E X E C U T E   ///////////////////////////
function T$source.execute: Integer;
var
  feedback: Integer;
  Index: Integer;
  next_module : Integer;
// _LR_stack : Array [0..$stack_max] of Integer;
  buffer : String;
begin
  feedback := 0;
:if module "Dialog-Call"
  _LR_stack_size := 0;             //  Clear subdialog stack
:endif

  while (FNextEvent <> LR_NULL_EVENT) do
  begin
      FEvent := FNextEvent;
:if check
      if (FEvent >= $events) or (FEvent < 0) then
      begin
          buffer  := 'State ' + IntToStr(FState) + ' - event ' + IntToStr(FEvent);
          buffer  := buffer + ' is out of range';
          LogWrite(buffer, Log_ErrorLvl);
          break;
      end;
:endif
      FSavest := FState;
      FIndex  := _LR_action [FState][FEvent];
:if defaults
      //  If no action for this event, try the defaults state
      if (FIndex = 0) then
      begin
          FState := _LR_defaults_state;
          FIndex := _LR_action [FState][FEvent];
      end;
:endif
:if check
      if (FIndex = 0) then
      begin
          buffer  := 'State ' + IntToStr(FState) + ' - event ' + IntToStr(FEvent);
          buffer  := buffer + ' is not accepted';
          LogWrite(buffer, Log_ErrorLvl);
          break;
       end;
      NextEvent       := LR_NULL_EVENT;
:endif
      ExceptionEvent  := LR_NULL_EVENT; //resets ExceptionRaised property

      next_module             := 0;

    //fire off actions related to transition
    while (true) do
    begin
      index := _LR_vector [FIndex][next_module];
      if (index = _LR_STOP) or (ExceptionRaised) then
        break;

      case index of
:declare $index = 0
:push $style
:option -style=plain
:do module
        $index: $name ;
:set $index = $index + 1
:enddo
      end;
:pop $style
      inc(next_module);
    end; //actions

    if (ExceptionRaised) then
    begin
        if (FExceptionEvent <> LR_NULL_EVENT) then
            FEvent := FExceptionEvent;
        NextEvent := FEvent;
    end  else
    begin
        FState := _LR_nextst [FState][FEvent];
        LogWrite(Format('%s: %s --> %s', [LR_ename[FEvent], LR_sname[FSavest], LR_sname[FState]]));
    end;

:if defaults
    if (FState = _LR_defaults_state) then
        FState := FSavest;
:endif
    if (NextEvent = LR_NULL_EVENT) then
    begin
      $"get_external_event";
:if check
      if (NextEvent = LR_NULL_EVENT) then
      begin
          buffer  := 'No event set after event ' + IntToStr(FEvent);
          buffer  := buffer + ' in state ' + IntToStr(FState);
          LogWrite (buffer, Log_Debug);
          break;
      end;
:endif
    end;
  end; {while not terminate-the-program}
  result := $"feedback";
end; //proc execute


//%START MODULE
//%END MODULE

:#endif
:push $style
:option -style=$style

:echo "lr I: generating $source\i.pas..."
:output $source\i.pas
{===================================================================+
|   COPYRIGHT (C):  Thinking Systems Ltd, 1999
|   PROJECT      :
|   FILE         :  $source\i
|   PURPOSE      :  Finite State Machine support file for $source
|   VERSION      :  0.4
|   AUTHOR       :  $author
|   LANGUAGE     :  Object Pascal (Delphi 4.0)
|   TARGET       :  Win 32 (95/98/NT 4.0)
|   TABS/WRAP    :  4/70
|   DATE         :  $date
|   COMMENTS     :  Declares Constants
|
+===================================================================}
unit $source\i;
// Defines the constants used in the FSM
interface
:option -style=plain
:include optional $template "<DATA>" "<END>"

:if module "Dialog-Call"
:  if not event "Return"
:     echo "lr E: you must define the 'Return' event"
:     exit 1
:  endif
:  declare int $stack_max = 20
   _LR_stack:Array [1..$stack_max] of Integer;     //  Subdialog call stack
   _LR_stack_size:Integer;             //  Subdialog stack size; 0=empty
:endif

    //- Symbolic constants and event numbers --------------------------------
  const
!:if check
  _LR_STOP           : Integer = $$FFFF;
  LR_NULL_EVENT      : Integer = -2;
!:endif

:do state
  _LR_STATE_$name     :integer = $number;
:enddo
:if defaults
  _LR_defaults_state  :integer = $defaults;
:endif

:do event
  $name   : Integer =    $number;
:enddo
  $"terminate_event"  = -1;

    //- Static areas --------------------------------------------------------
  _LR_nextst: Array[0..$states-1,0..$events-1] of Integer = (
:do nextst
:  if $overflow = 0
        ( $row )$comma
        //_LR_nextst[$row,$offset] := $comma;
:  else
        ( $row
:    do overflow
         $row
:    enddo
        )$comma
:  endif
:enddo
    );

    _LR_action: Array[0..$states-1,0..$events-1] of Integer = (
:set array_base=1
:do action
:  if $overflow = 0
        ( $row )$comma
:  else
        ( $row
:    do overflow
          $row
:    enddo
        )$comma
:  endif
:enddo
:set array_base=0
);





:pop $style
:option -style=$style
:endif

:if module "Dialog-Call"
procedure $"dialog_call";
begin
    if (_LR_stack_size < $stack_max) then
        _LR_stack [_LR_stack_size++] = FState
    else
    begin
        LogWrite ('State ' + FState + ' - Dialog-Call overflow', Log_ErrorLvl);
    end;
end;

procedure $"dialog_return";
begin
    if (_LR_stack_size > 0)then
    begin
        FState = _LR_stack [--_LR_stack_size];
        ExceptionEvent := $"return_event";
    end else
    begin
        LogWrite('State ' + _LR_state + ' - Dialog-Return underflow', Log_ErrorLvl);
    end;
end;

:endif
implementation

end.

:close

:output $source.001
:include $source.pas "//%START MODULE" "//%END MODULE"
:close $source.001
:output $source.002
:include $source.pas "" "//%START MODULE"
:close $source.002

!   Generate stubs for all modules not yet defined in source file

:set stub_before=""
:set stub_first="{"
:set stub_between="-"
:set stub_last=""
:set module_line="Procedure %s ;"
:do stubs $source.001 $source\i.pas
 Procedure: $name
 Purpose:
 Implementation & Comments:
     (1):
-------------------------------------------------------------------------------}
procedure T$source.$name;
begin
:include optional $template "<$module_name>" "<END>"
  assert(FContextOwner is $ownerType);
  (FContextOwner as $ownerType).$name;
end; //proc $name


:enddo

:output $source.pas
:include $source.002
:delete  $source.002
//%START MODULE
:include $source.001
:delete  $source.001
//%END MODULE
end.
