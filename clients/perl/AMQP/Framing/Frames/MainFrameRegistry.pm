#!/usr/bin/perl
# MainFrameRegistry.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::MainFrameRegistry;

use strict;
use warnings;

sub new() {
    my $class = ref($_[0]) || $_[0];
    my $registry = {};

    $registry{10010} = "AMQP::Framing::Frames::ConnectionStart";
    $registry{10011} = "AMQP::Framing::Frames::ConnectionStartOk";
    $registry{10020} = "AMQP::Framing::Frames::ConnectionSecure";
    $registry{10021} = "AMQP::Framing::Frames::ConnectionSecureOk";
    $registry{10030} = "AMQP::Framing::Frames::ConnectionTune";
    $registry{10031} = "AMQP::Framing::Frames::ConnectionTuneOk";
    $registry{10040} = "AMQP::Framing::Frames::ConnectionOpen";
    $registry{10041} = "AMQP::Framing::Frames::ConnectionOpenOk";
    $registry{10050} = "AMQP::Framing::Frames::ConnectionRedirect";
    $registry{10060} = "AMQP::Framing::Frames::ConnectionClose";
    $registry{10061} = "AMQP::Framing::Frames::ConnectionCloseOk";
    $registry{20010} = "AMQP::Framing::Frames::ChannelOpen";
    $registry{20011} = "AMQP::Framing::Frames::ChannelOpenOk";
    $registry{20020} = "AMQP::Framing::Frames::ChannelFlow";
    $registry{20021} = "AMQP::Framing::Frames::ChannelFlowOk";
    $registry{20030} = "AMQP::Framing::Frames::ChannelAlert";
    $registry{20040} = "AMQP::Framing::Frames::ChannelClose";
    $registry{20041} = "AMQP::Framing::Frames::ChannelCloseOk";
    $registry{30010} = "AMQP::Framing::Frames::AccessRequest";
    $registry{30011} = "AMQP::Framing::Frames::AccessRequestOk";
    $registry{40010} = "AMQP::Framing::Frames::ExchangeDeclare";
    $registry{40011} = "AMQP::Framing::Frames::ExchangeDeclareOk";
    $registry{40020} = "AMQP::Framing::Frames::ExchangeDelete";
    $registry{40021} = "AMQP::Framing::Frames::ExchangeDeleteOk";
    $registry{50010} = "AMQP::Framing::Frames::QueueDeclare";
    $registry{50011} = "AMQP::Framing::Frames::QueueDeclareOk";
    $registry{50020} = "AMQP::Framing::Frames::QueueBind";
    $registry{50021} = "AMQP::Framing::Frames::QueueBindOk";
    $registry{50030} = "AMQP::Framing::Frames::QueuePurge";
    $registry{50031} = "AMQP::Framing::Frames::QueuePurgeOk";
    $registry{50040} = "AMQP::Framing::Frames::QueueDelete";
    $registry{50041} = "AMQP::Framing::Frames::QueueDeleteOk";
    $registry{60010} = "AMQP::Framing::Frames::BasicQos";
    $registry{60011} = "AMQP::Framing::Frames::BasicQosOk";
    $registry{60020} = "AMQP::Framing::Frames::BasicConsume";
    $registry{60021} = "AMQP::Framing::Frames::BasicConsumeOk";
    $registry{60030} = "AMQP::Framing::Frames::BasicCancel";
    $registry{60031} = "AMQP::Framing::Frames::BasicCancelOk";
    $registry{60040} = "AMQP::Framing::Frames::BasicPublish";
    $registry{60050} = "AMQP::Framing::Frames::BasicReturn";
    $registry{60060} = "AMQP::Framing::Frames::BasicDeliver";
    $registry{60070} = "AMQP::Framing::Frames::BasicGet";
    $registry{60071} = "AMQP::Framing::Frames::BasicGetOk";
    $registry{60072} = "AMQP::Framing::Frames::BasicGetEmpty";
    $registry{60080} = "AMQP::Framing::Frames::BasicAck";
    $registry{60090} = "AMQP::Framing::Frames::BasicReject";
    $registry{60100} = "AMQP::Framing::Frames::BasicRecover";
    $registry{70010} = "AMQP::Framing::Frames::FileQos";
    $registry{70011} = "AMQP::Framing::Frames::FileQosOk";
    $registry{70020} = "AMQP::Framing::Frames::FileConsume";
    $registry{70021} = "AMQP::Framing::Frames::FileConsumeOk";
    $registry{70030} = "AMQP::Framing::Frames::FileCancel";
    $registry{70031} = "AMQP::Framing::Frames::FileCancelOk";
    $registry{70040} = "AMQP::Framing::Frames::FileOpen";
    $registry{70041} = "AMQP::Framing::Frames::FileOpenOk";
    $registry{70050} = "AMQP::Framing::Frames::FileStage";
    $registry{70060} = "AMQP::Framing::Frames::FilePublish";
    $registry{70070} = "AMQP::Framing::Frames::FileReturn";
    $registry{70080} = "AMQP::Framing::Frames::FileDeliver";
    $registry{70090} = "AMQP::Framing::Frames::FileAck";
    $registry{70100} = "AMQP::Framing::Frames::FileReject";
    $registry{80010} = "AMQP::Framing::Frames::StreamQos";
    $registry{80011} = "AMQP::Framing::Frames::StreamQosOk";
    $registry{80020} = "AMQP::Framing::Frames::StreamConsume";
    $registry{80021} = "AMQP::Framing::Frames::StreamConsumeOk";
    $registry{80030} = "AMQP::Framing::Frames::StreamCancel";
    $registry{80031} = "AMQP::Framing::Frames::StreamCancelOk";
    $registry{80040} = "AMQP::Framing::Frames::StreamPublish";
    $registry{80050} = "AMQP::Framing::Frames::StreamReturn";
    $registry{80060} = "AMQP::Framing::Frames::StreamDeliver";
    $registry{90010} = "AMQP::Framing::Frames::TxSelect";
    $registry{90011} = "AMQP::Framing::Frames::TxSelectOk";
    $registry{90020} = "AMQP::Framing::Frames::TxCommit";
    $registry{90021} = "AMQP::Framing::Frames::TxCommitOk";
    $registry{90030} = "AMQP::Framing::Frames::TxRollback";
    $registry{90031} = "AMQP::Framing::Frames::TxRollbackOk";
    $registry{100010} = "AMQP::Framing::Frames::DtxSelect";
    $registry{100011} = "AMQP::Framing::Frames::DtxSelectOk";
    $registry{100020} = "AMQP::Framing::Frames::DtxStart";
    $registry{100021} = "AMQP::Framing::Frames::DtxStartOk";
    $registry{110010} = "AMQP::Framing::Frames::TunnelRequest";

    bless $registry, $class;
}

1;
