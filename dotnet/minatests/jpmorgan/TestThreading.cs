using System;
using System.Collections.Generic;
using System.Threading;
using NUnit.Framework;

namespace jpmorgan.mina.jpmorgan
{
    [TestFixture]
    public class TestThreading
    {
        private int _closed;

        private const int CLOSED_TRUE = 1;
        private const int CLOSED_FALSE = 2;

        [Test]
        public void TestBooleanInterlocked()
        {
            _closed = CLOSED_FALSE;
            Interlocked.Exchange(ref _closed, CLOSED_TRUE);
            Assert.AreEqual(_closed, CLOSED_TRUE);
        }
    }
}
