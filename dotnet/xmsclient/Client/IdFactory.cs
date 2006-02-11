namespace OpenAMQ.XMS.Client
{
    public class IdFactory
    {
        private object[] _locks = new object[1];

        private ushort[] _counters = new ushort[1];

        private const int CHANNEL_ID = 0;

        public IdFactory()
        {
            for (int i = 0; i < _locks.Length; i++)
            {
                _locks[i] = new object();
                // need to initialise our ids to 1
                _counters[i] = 1;
            }
        }

        public ushort ChannelId
        {
            get
            {
                lock (_locks[CHANNEL_ID])
                {
                    return _counters[CHANNEL_ID]++;
                }
            }            
        }
    }
}
