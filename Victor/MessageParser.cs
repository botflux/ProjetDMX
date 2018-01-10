using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parser
{
    public static class MessageParser
    {
        public static string Parse (string name, string value)
        {
            return string.Format("{0}={1}", name.ToUpper(), value);
        }

        public static ParsedData Decode (string parsedData)
        {
            if (parsedData.Contains("="))
            {
                string[] exploded = parsedData.Split('=');
                return new ParsedData(exploded[0], exploded[1]);
            }
            else
            {
                throw new Exception("Data not properly formatted !");
            }
        }

        public struct ParsedData
        {
            private string name;
            private string value;

            public string Name
            {
                get
                {
                    return this.name;
                }

                set
                {
                    this.name = value;
                }
            }

            public string Value
            {
                get
                {
                    return this.value;
                }

                set
                {
                    this.value = value;
                }
            }

            public ParsedData (string name, string value)
            {
                this.name = name;
                this.value = value;
            }

            public override bool Equals(object obj)
            {
                return base.Equals(obj);
            }

            public override string ToString()
            {
                return string.Format("Name: {0}; Value: {1}", this.name, this.value);
            }
        }
    }
}
