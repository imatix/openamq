using System;
using System.Collections;
using jpmorgan.mina.common;

namespace jpmorgan.mina.handler.demux
{
    public class DemuxingIoHandler : IoHandlerAdapter
    {
        private readonly Hashtable _findHandlerCache = new Hashtable();
        private readonly Hashtable _type2Handler = new Hashtable();        

        public IMessageHandler this[Type type]
        {
            get
            {
                return (IMessageHandler) _type2Handler[type];
            }
            set
            {
                _type2Handler[type] = value;
            }
        }

        public void RemoveMessageHandler(Type type)
        {
            _findHandlerCache.Clear();
            _type2Handler.Remove(type);
        }
    
        public override void MessageReceived(ISession session, object message)
        {
            IMessageHandler handler = FindHandler(message.GetType());
            if (handler != null)
            {
                handler.MessageReceived(session, message);
            }
            else
            {
                throw new UnknownMessageTypeException("No message handler found for message: " + message);
            }
        }
        
        private IMessageHandler FindHandler(Type type)
        {
            return FindHandler(type, null);
        }
        
        private IMessageHandler FindHandler(Type type, Hashtable triedClasses)
        {
            IMessageHandler handler;
            
            if (triedClasses != null && triedClasses.ContainsKey(type))
            {
                return null;
            }
            
            //
            // Try the cache first
            //
            handler = (IMessageHandler) _findHandlerCache[type];
            if (handler != null)
            {
                return handler;
            }
            
            //
            // Try the registered handlers for an immediate match
            //
            handler = (IMessageHandler) _type2Handler[type];
            
            if (handler == null)
            {
                //
                // No immediate match could be found. Search the type's interfaces
                //
                
                if (triedClasses == null)
                {
                    triedClasses = new Hashtable();
                }
                triedClasses[type] = 1;
                Type[] interfaces = type.GetInterfaces();
                foreach (Type interfac in interfaces)
                {
                    handler = FindHandler(interfac, triedClasses);
                    if (handler != null)
                    {
                        break;
                    }
                }
            }
            
            if (handler == null)
            {
                //
                // No match in type's interfaces could be found. Search the superclass.
                //

                Type superclass = type.BaseType;
                if (superclass != null)
                {
                    handler = FindHandler(superclass);
                }                
            }
            
            //
            // Make sure the handler is added to the cache. By updating the cache
            // here all the types (superclasses and interfaces) in the path which
            // led to a match will be cached along with the immediate message type
            if (handler != null)
            {
                _findHandlerCache[type] = handler;
            }
            return handler;
        }
    }
}
