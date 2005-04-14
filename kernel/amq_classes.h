/*===========================================================================
 *
 *  amq_classes.h - OpenAMQ classes
 *
 *  This header file wraps all amq classes and should be used by all openamq
 *  programs that need to access one or more classes.  It includes amq_core.h
 *  as a first step.
 *
 *  Copyright (c) 2004 JPMorgan
 *===========================================================================*/

/*  We don't protect inclusion since that breaks iCL's chains in some cases  */

#include "amq_core.h"
#include "amq_frames.h"

/*  Private OpenAMQ class headers                                            */

#include "amq_browser.h"
#include "amq_bucket.h"
#include "amq_channel.h"
#include "amq_connection.h"
#include "amq_consumer.h"
#include "amq_dispatch.h"
#include "amq_field.h"
#include "amq_global.h"
#include "amq_handle.h"
#include "amq_message.h"
#include "amq_smessage.h"
#include "amq_queue.h"
#include "amq_topic.h"
#include "amq_subsc.h"
#include "amq_user.h"
#include "amq_vhost.h"
