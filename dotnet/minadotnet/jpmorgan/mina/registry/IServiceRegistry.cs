using System;
using Iesi.Collections;
using jpmorgan.mina.common;

namespace jpmorgan.mina.registry
{
    interface IServiceRegistry
    {
        ///
        /// Binds the specified I/O handler to the specified service.
        /// @throws IOException
        void bind(Service service, IHandler ioHandler);

        ///
        /// Binds the specified I/O handler to the specified service.
        /// @throws IOException
        void bind(Service service, IHandler ioHandler, IFilterChainBuilder filterChainBuilder );

        ///
        /// Unbinds the specified service (and its aggregated I/O handler or
        /// protocol provider).         
        void unbind(Service service);
    
        ///
        /// Unbinds all services (and their aggregated I/O handlers or
        /// protocol providers). 
        ///
        void unbindAll();

        ///
        /// Returns {@link Set} of all services bound in this registry.
        ///
        Set getAllServices();
    
        ///
        /// Returns {@link Set} of services bound in this registry with the
        /// specified service(or protocol) name.
        ///
        Set getServices(String name);

        ///
        /// Returns {@link Set} of services bound in this registry with the
        /// specified transport type.
        ///
        Set getServices(TransportType transportType);
    
        ///
        /// Returns {@link Set} of services bound in this registry with the
        /// specified port number.
        Set getServices(int port);

        IAcceptor getAcceptor(TransportType transportType);
    }
}
