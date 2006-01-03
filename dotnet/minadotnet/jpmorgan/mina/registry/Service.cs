using System;
using System.Net;

namespace jpmorgan.mina.registry
{
    class Service : ICloneable
    {
        private readonly string name;

        private readonly TransportType transportType;

        private readonly IPEndPoint address;

        ///
        /// Creates a new instance with the specified protocol name, transport type,
        /// and port number to be bound.         
        public Service(string name, TransportType transportType, int port) : 
            this(name, transportType, new IPEndPoint(IPAddress.Loopback, port))
        {
        }

        ///
        ///Creates a new instance with the specified protocol name, transport type,
        /// and socket address to be bound.         
        public Service(string name, TransportType transportType, IPEndPoint address)
        {
            if (name == null)
                throw new ArgumentNullException("name");            
            if (address == null)
                throw new ArgumentNullException("address");

            /*if( transportType == TransportType.VM_PIPE &&
                !( address instanceof VmPipeAddress ) )
            {
                throw new IllegalArgumentException(
                        "VM_PIPE transport type accepts only VmPipeAddress: " + address.getClass() );
            }*/

            this.name = name;
            this.transportType = transportType;
            this.address = address;
        }

        ///
        /// Returns the name of this service (protocol).
        ///
        public string Name
        {
            get
            {
                return name;
            }
        }

        ///
        /// Returns the transport type this service uses.         
        public TransportType TransportType
        {
            get
            {
                return transportType;
            }
        }

        ///
        /// Returns the socket address this service is bound on.
        ///
        public IPEndPoint Address
        {
            get
            {
                return address;
            }
        }

        public override int GetHashCode()
        {
            return ( ( name.GetHashCode() * 37 ) ^ transportType.GetHashCode() * 37 )
                    ^ address.GetHashCode();
        }

        public override bool Equals(object o)
        {
            if (o == null)
                return false;
            if (this == o)
                return true;
            if(!(o is Service))
                return false;

            Service that = (Service) o;
            return this.name.Equals(that.name)
                    && this.transportType == that.transportType
                    && this.address.Equals(that.address);
        }

        public object Clone()
        {            
            return new Service(name, transportType, address);     
        }
        
        public override string ToString()
        {
            return "(" + transportType + ", " + name + ", " + address + ')';
        }
    }
}
