using System;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.XMS.Client
{
    public class ConnectionTuneParameters
    {
        private uint _frameMax;

        private ushort _channelMax;

        private uint _hearbeat;

        private uint _txnLimit;

        public uint FrameMax
        {
            get
            {
                return _frameMax;
            }
            set
            {
                _frameMax = value;
            }
        }

        public ushort ChannelMax
        {
            get
            {
                return _channelMax;
            }
            set
            {
                _channelMax = value;
            }
        }

        public uint Heartbeat
        {
            get
            {
                return _hearbeat;
            }
            set
            {
                _hearbeat = value;
            }
        }

        public uint TxnLimit
        {
            get
            {
                return _txnLimit;
            }
            set
            {
                _txnLimit = value;
            }
        }
    }
}
