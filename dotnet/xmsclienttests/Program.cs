using System;
using System.Collections.Generic;
using System.Text;

namespace xmsclienttests
{
    class Program
    {
        static string getit()
        {
            string s = null;
            return s;
        }

        static void Main(string[] args)
        {
            StringBuilder builder = new StringBuilder("Value is: ");
            builder.Append(getit());
            Console.WriteLine("Value is " + builder);
            Console.WriteLine("Pies");
        }
    }
}
