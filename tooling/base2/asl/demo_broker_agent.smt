<?xml?>
<!---------------------------------------------------------------------------
    demo_broker_agent.smt - demo_broker component

    Generated from demo_broker.icl by smt_object_gen using GSL/4.

    
    Copyright (c) 1996-2009 iMatix Corporation
    
    This file is licensed under the GPL as follows:
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.
    
    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.
    
    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 ----------------------------------------------------------------------------->

<agent
    name = "demo_broker_agent"
    script = "smt_gen.gsl"
    animate = "0"
    >
<import class = "demo_broker" />

<method name = "destroy">
    <argument name = "file" type = "char *" />
    <argument name = "line" type = "size_t" />
</method>

<method name = "start">
</method>

<method name = "shutdown">
</method>

<thread name = "class">
    <context>
        demo_broker_t
            *demo_broker;
    </context>
    
    <handler name = "thread new">
        <argument name = "self" type = "demo_broker_t *">The iCL object</argument>
        tcb->demo_broker = self;
    </handler>
        
    <state name = "main">

        <method name = "destroy"  nextstate = ""  >
            <action name = "destroy">

#define self tcb->demo_broker
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
smt_signal_deregister (SIGINT);
smt_signal_deregister (SIGTERM);
ipr_meter_destroy (&self->imeter);
ipr_meter_destroy (&self->ometer);
demo_server_connection_table_destroy (&self->connections);
demo_server_connection_list_destroy (&self->connection_list);
smt_thread_unlink (&self->agent_thread);
demo_server_agent_term ();
if (self->stats)
    fclose (self->stats);
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->demo_broker
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on demo_broker object");
    demo_broker_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    demo_broker_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "start"  >
            <action name = "start">

#define self tcb->demo_broker

        {
//  We will run the broker monitor once per second, unconditionally
smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
//  Register to receive interrupt event on SIGINT, SIGTERM
smt_signal_register (SIGINT, thread, interrupt_event);
smt_signal_register (SIGTERM, thread, interrupt_event);
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "shutdown"  >
            <action name = "shutdown">

#define self tcb->demo_broker

        {
//  Tell demo_server_agent to close master socket
demo_server_agent_shutdown (self->agent_thread);
//  TODO: Shutdown any peerings or other objects that would malfunction
//  Start graceful shutdown sequence
smt_thread_set_next_event (thread, waiting_for_shutdown_event);
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <event name = "monitor"  >
            <action name = "monitor">
            
#define self tcb->demo_broker

        {
//  Cycle log files at midnight
if (ipr_time_now () > 235900) {
    if (!self->cycled) {
        self->cycled = TRUE;        //  Don't cycle again today
        smt_log_cycle (self->debug_log);
        smt_log_cycle (self->daily_log);
        smt_log_cycle (self->alert_log);
    }
}
else
    self->cycled = FALSE;

if (self->monitor_timer) {
    self->monitor_timer--;
    if (self->monitor_timer == 0) {
        Bool
            new_stats = FALSE;      //  Do we have new stats to print?

        self->monitor_timer = demo_server_config_monitor (demo_server_config);
        if (ipr_meter_mark (self->imeter, demo_server_config_monitor (demo_server_config))) {
            smt_log_print (self->debug_log,
                "I: incoming rate=%d mean=%d peak=%d",
                self->imeter->current,
                self->imeter->average,
                self->imeter->maximum);
            new_stats = TRUE;
        }
        if (ipr_meter_mark (self->ometer, demo_server_config_monitor (demo_server_config))) {
            smt_log_print (self->debug_log,
                "I: outgoing rate=%d mean=%d peak=%d iomean=%d",
                self->ometer->current,
                self->ometer->average,
                self->ometer->maximum,
                self->ometer->average + self->imeter->average);
            new_stats = TRUE;
        }
        //  Record statistics if necessary
        if (self->stats && new_stats) {
            fprintf (self->stats, "%d\t%zu\t%d\t%d\t%d\t%d\n",
                demo_server_connection_count (),
                ipr_bucket_used () / 1024,
                self->imeter->current, self->ometer->current,
                self->imeter->average, self->ometer->average);
        }
    }
}
smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
        }
        
#undef self
            </action>
        </event>
        <event name = "waiting for shutdown"  >
            <action name = "shutdown active connections">
            
#define self tcb->demo_broker

        {
//  Spend a maximum of 30 seconds shutting down
s_shutdown_time_spent++;
if (s_shutdown_time_spent > 30)
    smt_thread_set_next_event (thread, finished_with_shutdown_event);
//  For each active connection, tell it's demo_server_agent instance
//  to shut it down.
s_active_connections = 0;
demo_server_connection_table_apply (
    self->connections, s_shutdown_connection, NULL);
if (s_active_connections) {
    if (s_shutdown_time_spent == 1)
        //  Only print message once
        smt_log_print (self->alert_log,
            "W: closing %d connection%s, please wait up to 30 seconds",
            s_active_connections,
            s_active_connections > 1? "s": "");
    //  Loop and try again in one second
    smt_timer_request_delay (thread, 1000000,
        waiting_for_shutdown_event);
}
else
    //  Finished, proceed with broker shutdown
    smt_thread_set_next_event (thread, finished_with_shutdown_event);
        }
        
#undef self
            </action>
        </event>
        <event name = "finished with shutdown"  >
            <action name = "shutdown broker">
            
#define self tcb->demo_broker

        {
smt_shut_down ();
        }
        
#undef self
            </action>
        </event>
        <event name = "interrupt"  >
            <action name = "report server interrupted">
            
#define self tcb->demo_broker

        {
s_interrupted++;
if (s_interrupted == 1) {
    smt_log_print (self->alert_log,
        "W: caught signal, shutting down");
    //  Start graceful shutdown
    demo_broker_shutdown (self);
}
else
if (s_interrupted > 1) {
    smt_log_print (self->alert_log,
        "W: caught another signal, shutting down immediately");
    smt_shut_down ();
}
        }
        
#undef self
            </action>
        </event>
        <event name = "shutdown" nextstate = "shutting down" />
    </state>
    
    <state name = "shutting down">

        <method name = "destroy"  nextstate = ""  >
            <action name = "destroy">

#define self tcb->demo_broker
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
smt_signal_deregister (SIGINT);
smt_signal_deregister (SIGTERM);
ipr_meter_destroy (&self->imeter);
ipr_meter_destroy (&self->ometer);
demo_server_connection_table_destroy (&self->connections);
demo_server_connection_list_destroy (&self->connection_list);
smt_thread_unlink (&self->agent_thread);
demo_server_agent_term ();
if (self->stats)
    fclose (self->stats);
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->demo_broker
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on demo_broker object");
    demo_broker_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    demo_broker_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <method name = "start" />
        <method name = "shutdown" />
        <event name = "monitor" />
        <event name = "waiting for shutdown" />
        <event name = "finished with shutdown" />
        <event name = "interrupt" />
    </state>
</thread>

<private name = "types">
static void
    s_shutdown_connection (demo_server_connection_t *connection_p, void *argument);
static int
    s_interrupted = 0,                  //  Server interrupted?
    s_shutdown_time_spent = 0;          //  Time spent in graceful shutdown
static dbyte
    s_active_connections = 0;           //  Used during server shutdown
</private>

<private name = "functions">
//  Shutdown a single connection
static void
s_shutdown_connection (demo_server_connection_t *connection_p, void *argument)
{
    demo_server_connection_t
        *connection;

    connection = demo_server_connection_link (connection_p);
    if (connection) {
        if (demo_server_agent_shutdown (connection->thread) == 0)
            s_active_connections++;
        demo_server_connection_unlink (&connection);
    }
}
</private>

</agent>
