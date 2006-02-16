using System;
using System.Collections;
using jpmorgan.mina.common;
using jpmorgan.mina.common.support;
using log4net;

namespace OpenAMQ.Framing
{    

    ///
    /// From the protocol document:
    /// field-table      = short-integer *field-value-pair
    /// field-value-pair = field-name field-value
    /// field-name       = short-string
    /// field-value      = 'S' long-string
    /// 'I' long-integer
    /// 'D' decimal-value
    /// 'T' long-integer
    /// decimal-value    = decimals long-integer
    /// decimals         = OCTET    
    public class FieldTable : LinkedHashtable
    {
        private uint _encodedSize = 0;

        public FieldTable() : base()
        {            
        }
        
        /**
         * Construct a new field table.
         * @param buffer the buffer from which to read data. The length byte must be read already
         * @param length the length of the field table. Must be > 0.
         * @throws AMQFrameDecodingException if there is an error decoding the table
         */
        public FieldTable(ByteBuffer buffer, uint length)
            : base()
        {            
            if (length < 0)
            {
                throw new ArgumentException("length must not be negative", "length");
            }
            _encodedSize = length;
            int sizeRead = 0;
            while (sizeRead < _encodedSize)
            {
                int sizeRemaining = buffer.Remaining;
                string key = EncodingUtils.ReadShortString(buffer);
                // TODO: use proper charset decoder
                char type = (char)buffer.Get();
                object value;
                switch (type)
                {
                    case 'S':
                        value = EncodingUtils.ReadLongString(buffer);
                        break;
                    case 'I':
                        value = buffer.GetUnsignedInt();
                        break;
                    default:
                        throw new AMQFrameDecodingException("Unsupported field table type: " + type);
                }
                sizeRead += (sizeRemaining - buffer.Remaining);
                // we deliberately want to call put in the parent class since we do
                // not need to do the size calculations
                InnerHashtable[key] = value;
                base.OnSetComplete(key, null, value);
            }
        }

        public uint EncodedSize
        {
            get
            {
                return _encodedSize;
            }
        }

        public void WriteToBuffer(ByteBuffer buffer)
        {
            // write out the total length, which we have kept up to date as data is added
            buffer.Put(_encodedSize);
            WritePayload(buffer);            
        }

        private void WritePayload(ByteBuffer buffer)
        {
            foreach (LinkedDictionaryEntry lde in this)
            {
                string key = (string) lde.key;
                EncodingUtils.WriteShortStringBytes(buffer, key);
                object value = lde.value;
                if (value is byte[])
                {
                    buffer.Put((byte) 'S');
                    EncodingUtils.WriteLongstr(buffer, (byte[]) value);
                }
                else if (value is string)
                {
                    // TODO: look at using proper charset encoder
                    buffer.Put((byte) 'S');
                    EncodingUtils.WriteLongStringBytes(buffer, (string) value);
                }
                else if (value is uint)
                {
                    // TODO: look at using proper charset encoder
                    buffer.Put((byte) 'I');
                    buffer.Put((uint) value);
                }
                else
                {
                    // Should never get here 
                    throw new Exception("Fatal: unsupported type in FieldTable: " + value.GetType());
                }
            }
        }

        public byte[] GetDataAsBytes()
        {
            ByteBuffer buffer = ByteBuffer.Allocate((int)_encodedSize);
            WritePayload(buffer);
            byte[] result = new byte[_encodedSize];
            buffer.Flip();
            buffer.Get(result);
            //buffer.Release();
            return result;
        }

        /// <summary>
        /// Adds all the items from one field table in this one. Will overwrite any items in the current table
        /// with the same key.
        /// </summary>
        /// <param name="ft">the source field table</param>
        public void AddAll(FieldTable ft)
        {
            foreach (DictionaryEntry de in ft.InnerHashtable)
            {
                this[de.Key] = de.Value;
            }
        }
        
        private void CheckKey(object key)
        {
            if (key == null)
            {
                throw new ArgumentException("All keys must be Strings - was passed: null");
            }
            else if (!(key is string))
            {
                throw new ArgumentException("All keys must be Strings - was passed: " + key.GetType());
            }
        }

        private void CheckValue(object value)
        {
            if (!(value is string || value is uint || value is int || value is long))
            {
                throw new ArgumentException("All values must be type string or int or long or uint, was passed: " +
                                            value.GetType());
            }
        }

        protected override void OnSet(object key, object oldValue, object newValue)            
        {
            CheckKey(key);
            CheckValue(newValue);
            if (InnerHashtable.ContainsKey(key))
            {
                RemoveKeyValue(key, oldValue);
            }
            base.OnSet(key, oldValue, newValue);
        }

        protected override void OnInsert(object key, object newValue)
        {
            CheckKey(key);
            CheckValue(newValue);
            base.OnInsert(key, newValue);
        }
        
        protected override void OnSetComplete(object key, object oldValue, object value)
        {            
            _encodedSize += EncodingUtils.EncodedShortStringLength((string) key);
            // the extra byte if for the type indicator what is written out
            if (value is string)
            {
                _encodedSize += 1 + EncodingUtils.EncodedLongStringLength((string) value);
            }
            else if (value is int || value is uint || value is long)
            {
                _encodedSize += 1 + 4;
            }            
            else
            {
                // Should never get here since was already checked
                throw new Exception("Unsupported value type: " + value.GetType());
            }

            base.OnSetComplete(key, oldValue, value);
        }

        private void RemoveKeyValue(object key, object value)
        {
            _encodedSize -= EncodingUtils.EncodedShortStringLength((string) key);
            if (value != null)
            {
                if (value is string)
                {
                    _encodedSize -= 1 + EncodingUtils.EncodedLongStringLength((string) value);
                }
                else if (value is int || value is uint || value is long)
                {
                    _encodedSize -= 5;
                }
                else
                {
                    // Should never get here 
                    throw new Exception("Illegal value type: " + value.GetType());
                }
            }
        }

        protected override void OnRemove(object key, object value)
        {
            if (InnerHashtable.ContainsKey(key))
            {
                RemoveKeyValue(key, value);                
            } 
            base.OnRemove(key, value);
        }
    }
}
