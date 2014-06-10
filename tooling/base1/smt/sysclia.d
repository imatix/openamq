/*---------------------------------------------------------------------------*
 *  sysclia.d - LIBERO dialog data definitions for sysclia.c.                *
 *  Generated by LIBERO 2.4 on 11 Oct, 2010, 17:15.                          *
 *  Schema file used: smtschm.c.                                             *
 *---------------------------------------------------------------------------*/

/*- Symbolic constants and event numbers ------------------------------------*/

#define _LR_STOP                        0xFFFFL
#define _LR_NULL_EVENT                  -2
#define _LR_NULL_STATE                  -1
#define terminate_event                 -1
#define cancel_event                    0
#define empty_event                     1
#define error_event                     2
#define exception_event                 3
#define exit_event                      4
#define halt_event                      5
#define halting_event                   6
#define help_event                      7
#define list_event                      8
#define ok_event                        9
#define pause_event                     10
#define pause_error_event               11
#define pause_ok_event                  12
#define ready_event                     13
#define refresh_event                   14
#define shutdown_event                  15
#define sock_closed_event               16
#define sock_error_event                17
#define start_event                     18
#define start_error_event               19
#define start_ok_event                  20
#define status_event                    21
#define stop_event                      22
#define stop_all_event                  23
#define stop_error_event                24
#define stop_ok_event                   25
#define task_id_event                   26
#define task_nf_event                   27
#define task_paused_event               28
#define task_running_event              29
#define task_stopped_event              30
#define version_event                   31
#define _LR_STATE_after_init            0
#define _LR_STATE_have_connection       1
#define _LR_STATE_making_connection     2
#define _LR_STATE_daemon_message        3
#define _LR_STATE_user_command          4
#define _LR_STATE_before_stop           5
#define _LR_STATE_before_halt           6
#define _LR_STATE_daemon_halting        7
#define _LR_STATE_defaults              8
#define _LR_defaults_state              8


/*- Function prototypes and macros ------------------------------------------*/

#ifndef MODULE
#define MODULE  static void             /*  Libero dialog modules            */
#endif

MODULE initialise_the_thread        (THREAD *thread);
MODULE connect_to_sysman_daemon     (THREAD *thread);
MODULE terminate_the_thread         (THREAD *thread);
MODULE store_connection_data        (THREAD *thread);
MODULE get_daemon_message           (THREAD *thread);
MODULE check_daemon_message         (THREAD *thread);
MODULE signal_connected_okay        (THREAD *thread);
MODULE get_user_command_input       (THREAD *thread);
MODULE show_task_name               (THREAD *thread);
MODULE signal_task_started_ok       (THREAD *thread);
MODULE signal_task_not_started      (THREAD *thread);
MODULE signal_task_paused_ok        (THREAD *thread);
MODULE signal_task_not_paused       (THREAD *thread);
MODULE signal_task_stopped_ok       (THREAD *thread);
MODULE signal_task_not_stopped      (THREAD *thread);
MODULE signal_task_not_known        (THREAD *thread);
MODULE signal_task_running          (THREAD *thread);
MODULE signal_task_paused           (THREAD *thread);
MODULE signal_task_stopped          (THREAD *thread);
MODULE send_daemon_list_command     (THREAD *thread);
MODULE send_daemon_refresh_command  (THREAD *thread);
MODULE send_daemon_status_command   (THREAD *thread);
MODULE send_daemon_start_command    (THREAD *thread);
MODULE send_daemon_pause_command    (THREAD *thread);
MODULE send_daemon_stop_command     (THREAD *thread);
MODULE get_confirmation_for_stop    (THREAD *thread);
MODULE get_confirmation_for_halt    (THREAD *thread);
MODULE show_version_information     (THREAD *thread);
MODULE show_help_information        (THREAD *thread);
MODULE signal_invalid_command       (THREAD *thread);
MODULE send_daemon_halt_command     (THREAD *thread);
MODULE signal_socket_closed         (THREAD *thread);
MODULE signal_socket_error          (THREAD *thread);
MODULE signal_sysman_fatal_error    (THREAD *thread);
MODULE signal_unexpected_message    (THREAD *thread);

#define the_next_event              _the_next_event
#define the_exception_event         _the_exception_event
#define the_external_event          _the_external_event
#define exception_raised            _exception_raised
#define io_completed                _io_completed


/*- Static areas shared by all threads --------------------------------------*/

static word _LR_nextst [9][32] =
{
    { 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
     ,0,0
    },
    { 0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
     ,0,0
    },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
     ,0,0
    },
    { 0,0,0,0,0,0,0,0,0,0,0,3,3,4,0,0,0,0,0,3,3,0,0,0,3,3,3,3,3,3
     ,3,0
    },
    { 0,4,4,0,4,6,0,4,3,0,3,0,0,0,3,0,0,0,3,0,0,3,3,5,0,0,0,0,0,0
     ,0,4
    },
    { 4,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
     ,0,0
    },
    { 4,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
     ,0,0
    },
    { 0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
     ,0,0
    },
    { 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
     ,8,8
    }
};

static word _LR_action [9][32] =
{
    { 0,0,2,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
     ,0,0
    },
    { 0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
     ,0,0
    },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
     ,0,0
    },
    { 0,0,0,0,0,0,0,0,0,0,0,10,9,5,0,0,0,0,0,8,7,0,0,0,12,11,6,13
     ,15,14,16,0
    },
    { 0,5,27,0,2,24,0,26,17,0,21,0,0,0,18,0,0,0,20,0,0,19,22,23,0
     ,0,0,0,0,0,0,25
    },
    { 5,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
     ,0,0
    },
    { 5,0,0,0,0,0,0,0,0,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
     ,0,0
    },
    { 0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
     ,0,0
    },
    { 32,32,31,2,32,32,32,32,32,32,32,32,32,32,32,2,29,30,32,32,32
     ,32,32,32,32,32,32,32,32,32,32,32
    }
};

#if (defined (SMT_PROFILE))
static word _LR_count [34];
static long _LR_time  [34];
static word _LR_flow  [9][32];
#endif

static word _LR_offset [] =
{
    0,
    0,
    2,
    4,
    8,
    11,
    13,
    17,
    21,
    25,
    29,
    33,
    37,
    41,
    45,
    49,
    53,
    57,
    61,
    65,
    69,
    73,
    77,
    81,
    83,
    85,
    88,
    91,
    94,
    98,
    101,
    104,
    107
};

static word _LR_vector [] =
{
    0,_LR_STOP,
    1,_LR_STOP,
    2,3,4,_LR_STOP,
    5,6,_LR_STOP,
    6,_LR_STOP,
    7,3,4,_LR_STOP,
    8,3,4,_LR_STOP,
    9,3,4,_LR_STOP,
    10,3,4,_LR_STOP,
    11,3,4,_LR_STOP,
    12,3,4,_LR_STOP,
    13,3,4,_LR_STOP,
    14,3,4,_LR_STOP,
    15,3,4,_LR_STOP,
    16,3,4,_LR_STOP,
    17,3,4,_LR_STOP,
    18,3,4,_LR_STOP,
    19,3,4,_LR_STOP,
    20,3,4,_LR_STOP,
    21,3,4,_LR_STOP,
    22,3,4,_LR_STOP,
    23,3,4,_LR_STOP,
    24,_LR_STOP,
    25,_LR_STOP,
    26,6,_LR_STOP,
    27,6,_LR_STOP,
    28,6,_LR_STOP,
    29,3,4,_LR_STOP,
    30,1,_LR_STOP,
    31,1,_LR_STOP,
    32,1,_LR_STOP,
    33,1,_LR_STOP
};

static HOOK *_LR_module [34] = {
    connect_to_sysman_daemon,
    terminate_the_thread,
    store_connection_data,
    get_daemon_message,
    check_daemon_message,
    signal_connected_okay,
    get_user_command_input,
    show_task_name,
    signal_task_started_ok,
    signal_task_not_started,
    signal_task_paused_ok,
    signal_task_not_paused,
    signal_task_stopped_ok,
    signal_task_not_stopped,
    signal_task_not_known,
    signal_task_running,
    signal_task_paused,
    signal_task_stopped,
    send_daemon_list_command,
    send_daemon_refresh_command,
    send_daemon_status_command,
    send_daemon_start_command,
    send_daemon_pause_command,
    send_daemon_stop_command,
    get_confirmation_for_stop,
    get_confirmation_for_halt,
    show_version_information,
    show_help_information,
    signal_invalid_command,
    send_daemon_halt_command,
    signal_socket_closed,
    signal_socket_error,
    signal_sysman_fatal_error,
    signal_unexpected_message
};

#if (defined (DEBUG))
static char *_LR_mname [34] =
{
     "Connect-To-Sysman-Daemon",
     "Terminate-The-Thread",
     "Store-Connection-Data",
     "Get-Daemon-Message",
     "Check-Daemon-Message",
     "Signal-Connected-Okay",
     "Get-User-Command-Input",
     "Show-Task-Name",
     "Signal-Task-Started-Ok",
     "Signal-Task-Not-Started",
     "Signal-Task-Paused-Ok",
     "Signal-Task-Not-Paused",
     "Signal-Task-Stopped-Ok",
     "Signal-Task-Not-Stopped",
     "Signal-Task-Not-Known",
     "Signal-Task-Running",
     "Signal-Task-Paused",
     "Signal-Task-Stopped",
     "Send-Daemon-List-Command",
     "Send-Daemon-Refresh-Command",
     "Send-Daemon-Status-Command",
     "Send-Daemon-Start-Command",
     "Send-Daemon-Pause-Command",
     "Send-Daemon-Stop-Command",
     "Get-Confirmation-For-Stop",
     "Get-Confirmation-For-Halt",
     "Show-Version-Information",
     "Show-Help-Information",
     "Signal-Invalid-Command",
     "Send-Daemon-Halt-Command",
     "Signal-Socket-Closed",
     "Signal-Socket-Error",
     "Signal-Sysman-Fatal-Error",
     "Signal-Unexpected-Message"
};

static char *_LR_sname [9] =
{
     "After-Init",
     "Have-Connection",
     "Making-Connection",
     "Daemon-Message",
     "User-Command",
     "Before-Stop",
     "Before-Halt",
     "Daemon-Halting",
     "Defaults"
};

static char *_LR_ename [32] =
{
     "Cancel-Event",
     "Empty-Event",
     "Error-Event",
     "Exception-Event",
     "Exit-Event",
     "Halt-Event",
     "Halting-Event",
     "Help-Event",
     "List-Event",
     "Ok-Event",
     "Pause-Event",
     "Pause-Error-Event",
     "Pause-Ok-Event",
     "Ready-Event",
     "Refresh-Event",
     "Shutdown-Event",
     "Sock-Closed-Event",
     "Sock-Error-Event",
     "Start-Event",
     "Start-Error-Event",
     "Start-Ok-Event",
     "Status-Event",
     "Stop-Event",
     "Stop-All-Event",
     "Stop-Error-Event",
     "Stop-Ok-Event",
     "Task-Id-Event",
     "Task-Nf-Event",
     "Task-Paused-Event",
     "Task-Running-Event",
     "Task-Stopped-Event",
     "Version-Event"
};
#else
static char *_LR_mname [34] =
{
     "0",
     "1",
     "2",
     "3",
     "4",
     "5",
     "6",
     "7",
     "8",
     "9",
     "10",
     "11",
     "12",
     "13",
     "14",
     "15",
     "16",
     "17",
     "18",
     "19",
     "20",
     "21",
     "22",
     "23",
     "24",
     "25",
     "26",
     "27",
     "28",
     "29",
     "30",
     "31",
     "32",
     "33"
};

static char *_LR_sname [9] =
{
     "0",
     "1",
     "2",
     "3",
     "4",
     "5",
     "6",
     "7",
     "8"
};

static char *_LR_ename [32] =
{
     "0",
     "1",
     "2",
     "3",
     "4",
     "5",
     "6",
     "7",
     "8",
     "9",
     "10",
     "11",
     "12",
     "13",
     "14",
     "15",
     "16",
     "17",
     "18",
     "19",
     "20",
     "21",
     "22",
     "23",
     "24",
     "25",
     "26",
     "27",
     "28",
     "29",
     "30",
     "31"
};
#endif
