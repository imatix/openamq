!----------------------------------------------------------------------------!
!   sysmana.l - sysman agent dialog                                          !
!                                                                            !
!   Copyright (c) 1991-2009 iMatix Corporation                               !
!                                                                            !
!   ------------------ GPL Licensed Source Code ------------------           !
!   iMatix makes this software available under the GNU General               !
!   Public License (GPL) license for open source projects.  For              !
!   details of the GPL license please see www.gnu.org or read the            !
!   file license.gpl provided in this package.                               !
!                                                                            !
!   This program is free software; you can redistribute it and/or            !
!   modify it under the terms of the GNU General Public License as           !
!   published by the Free Software Foundation; either version 2 of           !
!   the License, or (at your option) any later version.                      !
!                                                                            !
!   This program is distributed in the hope that it will be useful,          !
!   but WITHOUT ANY WARRANTY; without even the implied warranty of           !
!   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            !
!   GNU General Public License for more details.                             !
!                                                                            !
!   You should have received a copy of the GNU General Public                !
!   License along with this program in the file 'license.gpl'; if            !
!   not, write to the Free Software Foundation, Inc., 59 Temple              !
!   Place - Suite 330, Boston, MA 02111-1307, USA.                           !
!                                                                            !
!   You can also license this software under iMatix's General Terms          !
!   of Business (GTB) for commercial projects.  If you have not              !
!   explicitly licensed this software under the iMatix GTB you may           !
!   only use it under the terms of the GNU General Public License.           !
!                                                                            !
!   For more information, send an email to info@imatix.com.                  !
!----------------------------------------------------------------------------!
-schema=smtschm.c

After-Init:
!   Control-Master thread manages control connection requests
    (--) Control-Master                     -> Control-Master-Input
          + Load-Global-Configuration
          + Open-Master-Log-File
          + Create-Task-Threads
          + Open-Control-Master-Socket
          + Wait-For-Socket-Input
!   Control threads handle control connections
    (--) Control                            -> Control-Message
          + Control-Ready-For-Command
          + Read-Control-Message
          + Check-Control-Message
!   Task threads handle individual tasks
    (--) Task                               -> Task-Stopped
          + Load-Task-Values
          + Run-Task-Idle-Command
          + Refresh-Logical-System-State

Control-Master-Input:
    (--) Ok                                 -> Control-Master-Input
          + Accept-Control-Connection
          + Wait-For-Socket-Input
    (--) Sock-Timeout                       -> Control-Master-Input
          + Wait-For-Socket-Input

!  These states define the control threads

Control-Message:
    (--) List                               -> Control-Message
          + Control-List-Of-Tasks
          + Control-Ready-For-Command
          + Read-Control-Message
          + Check-Control-Message
    (--) Halt                               ->
          + Control-Halting
          + Commit-Outgoing-Replies
          + Shutdown-The-Application
          + Terminate-The-Thread
    (--) Status                             -> Control-Status-Task
          + Get-First-Affected-Task
    (--) Start                              -> Control-Start-Task
          + Get-First-Affected-Task
    (--) Pause                              -> Control-Pause-Task
          + Get-First-Affected-Task
    (--) Stop                               -> Control-Stop-Task
          + Get-First-Affected-Task
    (--) Refresh                            -> Control-Refresh-Task-Stopping
          + Force-All-Tasks
          + Get-First-Affected-Task
    (--) Error                              -> Control-Message
          + Control-Unrecognised-Control
          + Control-Ready-For-Command
          + Read-Control-Message
          + Check-Control-Message

Control-Refresh-Task-Stopping: <Control-Status-Task>
    (--) Ok                                 -> Control-Refresh-Task-Wait-Stop
          + Pass-Stop-To-Task-Thread
    (--) No-More                            -> Control-Message
          + Destroy-Task-Threads
          + Close-Manager-Log-File
          + Load-Global-Configuration
          + Open-Master-Log-File
          + Create-Task-Threads
          + Control-Ready-For-Command
          + Read-Control-Message
          + Check-Control-Message

Control-Refresh-Task-Wait-Stop:
    (--) Ok                                 -> Control-Refresh-Task-Stopping
          + Control-Task-Stopped-Ok
          + Get-Next-Affected-Task
    (--) Error                              -> Control-Refresh-Task-Stopping
          + Control-Task-Stopped-Error
          + Get-Next-Affected-Task

Control-Status-Task:
    (--) Ok                                 -> Control-Status-Task-Wait
          + Pass-Status-To-Task-Thread
    (--) No-More                            -> Control-Message
          + Control-Ready-For-Command
          + Read-Control-Message
          + Check-Control-Message

Control-Status-Task-Wait:
    (--) Running                            -> Control-Status-Task
          + Control-Task-Running
          + Get-Next-Affected-Task
    (--) Paused                             -> Control-Status-Task
          + Control-Task-Paused
          + Get-Next-Affected-Task
    (--) Stopped                            -> Control-Status-Task
          + Control-Task-Stopped
          + Get-Next-Affected-Task
    (--) Error                              -> Control-Status-Task
          + Get-Next-Affected-Task

Control-Start-Task:  <Control-Status-Task>
    (--) Ok                                 -> Control-Start-Task-Wait
          + Pass-Start-To-Task-Thread

Control-Start-Task-Wait:
    (--) Ok                                 -> Control-Start-Task
          + Control-Task-Started-Ok
          + Get-Next-Affected-Task
    (--) Error                              -> Control-Start-Task
          + Control-Task-Started-Error
          + Get-Next-Affected-Task

Control-Pause-Task:  <Control-Status-Task>
    (--) Ok                                 -> Control-Task-Wait-Pause
          + Pass-Pause-To-Task-Thread

Control-Task-Wait-Pause:
    (--) Ok                                 -> Control-Pause-Task
          + Control-Task-Paused-Ok
          + Get-Next-Affected-Task
    (--) Error                              -> Control-Pause-Task
          + Control-Task-Paused-Error
          + Get-Next-Affected-Task

Control-Stop-Task: <Control-Status-Task>
    (--) Ok                                 -> Control-Task-Wait-Stop
          + Pass-Stop-To-Task-Thread

Control-Task-Wait-Stop:
    (--) Ok                                 -> Control-Stop-Task
          + Control-Task-Stopped-Ok
          + Get-Next-Affected-Task
    (--) Error                              -> Control-Stop-Task
          + Control-Task-Stopped-Error
          + Get-Next-Affected-Task


!  These states define the task threads, one for each task defined
!  in the SYSMAN initialisation file.

Task-Stopped:
    !  These events come from a control thread, and get a reply
    (--) Status-Req                         -> Task-Stopped
          + Signal-Task-Stopped
    (--) Start-Req                          -> Task-Running
          + Load-Task-Values
          + Load-Task-Time-Slots
          + Run-Task-Startup-Command
          + Refresh-Logical-System-State
          + Monitor-Active-Task-Status
          + Signal-Task-Started-Okay
    (--) Pause-Req                          -> Task-Stopped
          + Signal-Task-Not-Running
    (--) Stop-Req                           -> Task-Stopped
          + Signal-Task-Not-Running
    !  These events come from the timer agent, and may be ignored
    (--) Start-Auto                         -> Task-Stopped
          +
    (--) Pause-Auto                         -> Task-Stopped
          +
    !  These events come from another thread, and do not get a reply
    (--) Stop-Force                         -> Task-Stopped
          +
    (--) Start-Force                        -> Task-Running
          + Load-Task-Values
          + Load-Task-Time-Slots
          + Run-Task-Startup-Command
          + Refresh-Logical-System-State
          + Monitor-Active-Task-Status

Task-Running:
    !  These events come from a control thread, and get a reply
    (--) Status-Req                         -> Task-Running
          + Signal-Task-Running
    (--) Start-Req                          -> Task-Running
          + Signal-Task-Already-Running
    (--) Pause-Req                          -> Task-Paused
          + Shut-Down-Child-Task
          + Run-Task-Cancel-Command
          + Run-Task-Idle-Command
          + Refresh-Logical-System-State
          + Signal-Task-Paused-Okay
    (--) Stop-Req                           -> Task-Stopped
          + Shut-Down-Child-Task
          + Run-Task-Cancel-Command
          + Run-Task-Idle-Command
          + Refresh-Logical-System-State
          + Signal-Task-Stopped-Okay
    !  These events come from the timer agent, and may be ignored
    (--) Start-Auto                         -> Task-Running
          +
    (--) Pause-Auto                         -> Task-Paused
          + Shut-Down-Child-Task
          + Run-Task-Cancel-Command
          + Run-Task-Idle-Command
          + Refresh-Logical-System-State
    !  These events come from another thread, and do not get a reply
    (--) Stop-Force                         -> Task-Stopped
          + Shut-Down-Child-Task
          + Run-Task-Cancel-Command
          + Run-Task-Idle-Command
          + Refresh-Logical-System-State
    (--) Start-Force                        -> Task-Running
          +
    !  This timer event lets us monitor the progress of the task
    (--) Task-Alarm                         -> Task-Running
          + Check-Task-Still-Running
          + Monitor-Active-Task-Status
    !  This exception means the task stopped spontaneously
    (--) Task-Halted                        -> Task-Not-Running
          + Check-If-Restartable-Task
    !  Shutdown handling for an active task is a special case
    (--) Shutdown                           ->
          + Shut-Down-Child-Task
          + Run-Task-Cancel-Command
          + Run-Task-Idle-Command
          + Refresh-Logical-System-State
          + Destroy-The-Task
          + Terminate-The-Thread

Task-Paused:
    !  These events come from a control thread, and get a reply
    (--) Status-Req                         -> Task-Paused
          + Signal-Task-Paused
    (--) Start-Req                          -> Task-Running
          + Load-Task-Values
          + Load-Task-Time-Slots
          + Run-Task-Startup-Command
          + Refresh-Logical-System-State
          + Monitor-Active-Task-Status
          + Signal-Task-Started-Okay
    (--) Pause-Req                          -> Task-Paused
          + Signal-Task-Not-Running
    (--) Stop-Req                           -> Task-Paused
          + Signal-Task-Not-Running
    !  These events come from the timer agent, and may be ignored
    (--) Start-Auto                         -> Task-Running
          + Load-Task-Values
          + Run-Task-Startup-Command
          + Refresh-Logical-System-State
          + Monitor-Active-Task-Status
    (--) Pause-Auto                         -> Task-Paused
          +
    !  These events come from another thread, and do not get a reply
    (--) Stop-Force                         -> Task-Stopped
          +
    (--) Start-Force                        -> Task-Running
          + Load-Task-Values
          + Load-Task-Time-Slots
          + Run-Task-Startup-Command
          + Refresh-Logical-System-State
          + Monitor-Active-Task-Status

!   We get to this state if a task either failed to start, or
!   ended by itself.  If the task was mandatory, this means
!   that all tasks have to end.
!
Task-Not-Running:
    (--) Restart                            -> Task-Running
          + Run-Task-Startup-Command
          + Refresh-Logical-System-State
          + Monitor-Active-Task-Status
    (--) Required                           -> Task-Stopped
          + Broadcast-Stop-All-Tasks
          + Run-Task-Idle-Command
          + Refresh-Logical-System-State
    (--) Normal                             -> Task-Paused
          + Run-Task-Idle-Command
          + Refresh-Logical-System-State

Defaults:
    (--) Sock-Timeout                       -> End-Thread
          + Check-Thread-Type
    (--) Sock-Error                         -> End-Thread
          + Signal-Socket-Error
          + Check-Thread-Type
    (--) Sock-Closed                        -> End-Thread
          + Check-Thread-Type
    (--) Exception                          -> End-Thread
          + Check-Thread-Type
    (--) Shutdown                           -> End-Thread
          + Check-Thread-Type
    (--) Fatal                              ->
          + Shutdown-The-Application
    (--) $other                             ->
          + Log-Invalid-Dialog-Event
    (--) Task-Failed                        -> Task-Not-Running
          + Signal-Task-Started-Error
          + Check-If-Required-Task
    (--) Task-Alarm                         ->
          +

End-Thread:
    (--) Control-Master                     ->
          + Close-Manager-Log-File
          + Shutdown-The-Application
          + Terminate-The-Thread
    (--) Control                            ->
          + Terminate-The-Thread
    (--) Task                               ->
          + Destroy-The-Task
          + Terminate-The-Thread
