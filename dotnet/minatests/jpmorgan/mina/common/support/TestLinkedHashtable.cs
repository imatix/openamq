using System;
using jpmorgan.mina.common.support;

namespace jpmorgan.mina.common.support
{
    public class TestLinkedHashtable
    {
        private static void Test1()
        {
            LinkedHashtable table = new LinkedHashtable();
            table["Super"] = "Ayr";
            table["Ayr"] = "United";
            table["Dumbarton"] = "Gubbed";

            foreach (LinkedHashtable.LinkedDictionaryEntry o in table)
            {
                Console.WriteLine("Item: " + o.key);
            }

            table.MoveToHead("Dumbarton");
            foreach (LinkedHashtable.LinkedDictionaryEntry o in table)
            {
                Console.WriteLine("Item: " + o.key);
            }
            Console.WriteLine("Done");
        }

        public static void Main(string[] args)
        {
            Test1();
        }
    }
}
