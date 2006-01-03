using System;
using System.Text;
using System.Collections;

namespace jpmorgan.mina.common
{
    /// The default implementation of {@link IoFilterChainBuilder} which is useful
    /// in most cases.  {@link DefaultIoFilterChainBuilder} has an identical interface
    /// with {@link IoFilter}; it contains a list of {@link IoFilter}s that you can
    /// modify. The {@link IoFilter}s which are added to this builder will be appended
    /// to the {@link IoFilterChain} when {@link #buildFilterChain(IoFilterChain)} is
    /// invoked.
    /// <p>
    /// However, the identical interface doesn't mean that it behaves in an exactly
    /// same way with {@link IoFilterChain}.  {@link DefaultIoFilterChainBuilder}
    /// doesn't manage the life cycle of the {@link IoFilter}s at all, and the
    /// existing {@link IoSession}s won't get affected by the changes in this builder.
    /// {@link IoFilterChainBuilder}s affect only newly created {@link IoSession}s.
    /// 
    /// <pre>
    /// IoAcceptor acceptor = ...;
    /// DefaultIoFilterChainBuilder builder = acceptor.getFilterChain();
    /// builder.addLast( "myFilter", new MyFilter() );
    /// ...
    /// </pre>
    ///
    public class DefaultIoFilterChainBuilder : IFilterChainBuilder
    {
        private IList entries = new ArrayList();
        private readonly Hashtable entriesByName = new Hashtable();
    
        ///
        /// Creates a new instance with an empty filter list.
        ///
        public DefaultIoFilterChainBuilder()
        {
        }        

        ///
        /// @see IoFilterChain#get(String)
        ///
        public IFilter Get(string name)
        {
            /*lock (this)
            {
                IoFilterChainEntry e = GetEntry(name);
                if (e == null)
                {
                    return null;
                }

                return e.Filter;
            }*/
            return null;
        }
        
        ///
        /// @see IoFilterChain#getAll()
        ///
        public ArrayList GetAll()
        {
            return new ArrayList(entries);
        }
        
        ///
        /// @see IoFilterChain#getAllReversed()
        ///
        public ArrayList GetAllReversed()
        {
            ArrayList result = GetAll();
            result.Reverse();
            return result;
        }

        ///
        /// @see IoFilterChain#contains(String)
        ///
        public bool Contains(string name)
        {
            //return GetEntry(name) != null;
            return false;
        }
        
        ///
        /// @see IoFilterChain#contains(IoFilter)
        ///
        public bool contains(IFilter filter)
        {
            /*foreach (IoFilterChainEntry e in entries)
            {                
                if (e.Filter == filter)
                {
                    return true;
                }
            }*/
            
            return false;
        }
        
        ///
        /// @see IoFilterChain#contains(Class)
        ///
        public bool Contains(Type filterType)
        {
            /*foreach (IoFilterChainEntry e in entries)
            {                
                if (filterType.IsAssignableFrom(e.Filter.GetType()))
                {
                    return true;
                }
            }*/
            
            return false;
        }
        
        ///
        /// @see IoFilterChain#addFirst(String, IoFilter)
        ///
        public void addFirst(string name, IFilter filter)
        {
            /*lock (this)
            {
                Register(0, new EntryImpl(name, filter));
            }*/
        }
        
        ///
        /// @see IoFilterChain#addLast(String, IoFilter)
        ///
        public void addLast(string name, IFilter filter)
        {
            /*lock (this)
            {
                Register(entries.Count, new EntryImpl(name, filter));
            }*/
        }

        ///
        /// @see IoFilterChain#addBefore(String, String, IoFilter)
        ///
        public void addBefore(string baseName, string name, IFilter filter)
        {
            /*lock (this)
            {
                CheckBaseName(baseName);
                
                for (int i = 0; i < entries.Count; i++)
                {
                    IoFilterChainEntry baseEntry = (IoFilterChainEntry) entries[i];
                    if (baseEntry.Name.Equals(baseName))
                    {
                        int prevIndex = (i == 0?0:i - 1);
                        Register(prevIndex, new EntryImpl(name, filter));
                        break;
                    }
                }
            }*/
        }

        ///
        /// @see IoFilterChain#addAfter(String, String, IoFilter)
        ///
        public void addAfter(string baseName, string name, IFilter filter)
        {
            /*lock (this)
            {
                CheckBaseName(baseName);
                
                IList entries = new ArrayList(this.entries);
                
                for (int i = 0; i < entries.Count; i++)
                {
                    IoFilterChainEntry baseentry = (IoFilterChainEntry) entries[i];
                    if (baseentry.Name.Equals( baseName))
                    {                        
                        Register(i + 1, new EntryImpl(name, filter));
                        break;
                    }
                }
            }*/
        }

        ///
        /// @see IoFilterChain#remove(String)
        ///
        public IFilter remove(string name)
        {
            /*lock (this)
            {
                if (name == null)
                {
                    throw new ArgumentNullException("name");
                }

                for (int i = 0; i < entries.Count; i++)
                {
                    IoFilterChainEntry e = (IoFilterChainEntry) entries[i];
                    if (e.Name.Equals(name))
                    {
                        int newIndex = (i == 0 ? 0 : i - 1);
                        Deregister(newIndex, e);
                        return e.Filter;
                    }
                }
                
                throw new ArgumentException("Unknown filter name: " + name);
            }*/
            return null;
        }

        ///
        /// @see IoFilterChain#clear()
        ///
        public void Clear()
        {
            lock (this)
            {
                entries = new ArrayList();
                entriesByName.Clear();
            }
        }
        
        public void BuildFilterChain(IFilterChain chain)
        {
            /*foreach (IoFilterChainEntry e in entries)
            {                
                chain.AddLast(e.Name, e.Filter);
            }*/
        }
        
        public override string ToString()
        {
            StringBuilder buf = new StringBuilder();
            buf.Append("{ ");
            
            /*bool empty = true;
            
            foreach (IFilter e in entries)
            {                
                if (!empty)
                {
                    buf.Append(", ");
                }
                else
                {
                    empty = false;
                }
                
                buf.Append('(');
                buf.Append(e.Name);
                buf.Append(':');
                buf.Append(e.Filter);
                buf.Append(')');
            }
            
            if (empty)
            {
                buf.Append("empty");
            }
            
            buf.Append(" }");
            */
            return buf.ToString();
        }

        private void CheckBaseName(string baseName)
        {
            if (baseName == null)
            {
                throw new ArgumentNullException("baseName");
            }
            if (!entriesByName.ContainsKey(baseName))
            {
                throw new ArgumentException("Unknown filter name: " + baseName);
            }
        }

        /*private void Register(int index, IoFilterChainEntry e)
        {
            if (entriesByName.ContainsKey(e.Name))
            {
                throw new ArgumentException( "Other filter is using the same name: " + e.Name);
            }

            ArrayList newEntries = new ArrayList(entries);
            newEntries.Insert(index, e);
            this.entries = newEntries;
            entriesByName[e.Name] = e;
        }*/
        
        /*private void Deregister(int index, IoFilterChainEntry e)
        {
            IList newEntries = new ArrayList( entries );
            newEntries.Remove(index);
            this.entries = newEntries;
            entriesByName.Remove(e.Name);
        }*/
    }
}
