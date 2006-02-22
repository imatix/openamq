using System;
using System.Collections;

namespace jpmorgan.mina.common
{
    /// Represents network transport types.
    /// MINA provides three transport types by default:
    /// <ul>
    ///   <li>{@link #SOCKET} - TCP/IP</li>
    ///   <li>{@link #DATAGRAM} - UDP/IP</li>
    ///   <li>{@link #VM_PIPE} - in-VM pipe support (only available in protocol
    ///       layer</li>
    /// </ul>
    /// <p>
    /// You can also create your own transport type.  Please refer to
    /// {@link #TransportType(String[], boolean)}. 
    [Serializable]
    public class TransportType
    {
        private static readonly Hashtable name2type = new Hashtable();
    
        private static void Register(string[] names, TransportType type)
        {
            lock (name2type.SyncRoot)
            {
                for (int i = names.Length - 1; i >= 0; i--)
                {
                    if (name2type.Contains(names[i]))
                    {
                        throw new ArgumentException(
                                "Transport type name '" + names[i] + "' is already taken." );
                    }
                }

                for (int i = names.Length - 1; i >= 0; i--)
                {
                    name2type[names[i].ToUpper()] = type;
                }
            }
        }

        ///
        /// Transport type: TCP/IP (Registry name: <tt>"SOCKET"</tt> or <tt>"TCP"</tt>)
        ///
        public static readonly TransportType SOCKET =
            new TransportType(new String[]{"SOCKET", "TCP"}, false);

        ///
        /// Transport type: UDP/IP (Registry name: <tt>"DATAGRAM"</tt> or <tt>"UDP"</tt>)
        ///
        public static readonly TransportType DATAGRAM =
            new TransportType(new String[] {"DATAGRAM", "UDP"}, true);

        ///
        /// Transport type: in-VM pipe (Registry name: <tt>"VM_PIPE"</tt>) 
        /// Please refer to
        /// <a href="../protocol/vmpipe/package-summary.htm"><tt>org.apache.mina.protocol.vmpipe</tt></a>
        /// package.         
        public static readonly TransportType VM_PIPE =
            new TransportType(new String[] {"VM_PIPE"}, typeof(Object), false );
        

        ///
        /// Returns the transport type of the specified name.
        /// All names are case-insensitive.
        /// 
        /// @param name the name of the transport type
        /// @return the transport type
        /// @throws IllegalArgumentException if the specified name is not available.         
        public static TransportType GetInstance(string name)
        {
            TransportType type = (TransportType) name2type[name.ToUpper()];
            if (type != null)
            {
                return type;
            }
            
            throw new ArgumentException("Unknown transport type name: " + name);
        }

        private readonly string[] names;
        
        [NonSerialized]
        private readonly bool connectionless;
        
        [NonSerialized]
        private readonly Type envelopeType;

        ///
        /// Creates a new instance.  New transport type is automatically registered
        /// to internal registry so that you can look it up using {@link #getInstance(String)}.
        /// 
        /// @param names the name or aliases of this transport type
        /// @param connectionless <tt>true</tt> if and only if this transport type is connectionless
        /// 
        /// @throws IllegalArgumentException if <tt>names</tt> are already registered or empty         
        public TransportType(string[] names, bool connectionless) :
            this(names, typeof(ByteBuffer), connectionless)
        {
        }

        ///
        /// Creates a new instance.  New transport type is automatically registered
        /// to internal registry so that you can look it up using {@link #getInstance(String)}.
        ///
        /// @param names the name or aliases of this transport type
        /// @param connectionless <tt>true</tt> if and only if this transport type is connectionless
        /// 
        /// @throws ArgumentException if <tt>names</tt> are already registered or empty         
        public TransportType(string[] names, Type envelopeType, bool connectionless)
        {
            if (names == null)
            {
                throw new ArgumentNullException("names");
            }
            if (names.Length == 0)
            {
                throw new ArgumentException("names is empty");
            }
            if (envelopeType == null)
            {
                throw new ArgumentNullException("envelopeType");
            }

            for (int i = 0; i < names.Length; i++)
            {
                if (names[i] == null)
                {
                    throw new ArgumentNullException("strVals[" + i + "]");
                }
                
                names[i] = names[i].ToUpper();
            }

            Register(names, this);
            this.names = names;
            this.connectionless = connectionless;
            this.envelopeType = envelopeType;
        }

        ///
        /// Returns <code>true</code> if the session of this transport type is
        /// connectionless.
        public bool Connectionless
        {
            get            
            {
                return connectionless;
            }
        }
        
        public Type EnvelopeType
        {
            get
            {
                return envelopeType;
            }
        }
        
        ///
        /// Returns the known names of this transport type.
        ///
        public Hashtable Names
        {
            get
            {       
                Hashtable result = new Hashtable();
                for (int i = names.Length - 1; i >= 0; i--)
                {
                    result[names[i]] = 1;
                }
                
                return result;
            }
        }

        public override string ToString()
        {
            return names[0];
        }
        
        /*private object ReadResolve()
        {
            for (int i = names.length - 1; i >= 0; i --)
            {                
                return GetInstance(names[i]);             
            }
            
            throw new InvalidObjectException( "Unknown transport type." );
        }*/
    }
}
