/************************************************************************************************************************
							** BRW.inc (Binary Reader/Writer for PAWN) ** 




    Copyright 2017 Freddy Borja (ThreeKingz aka ThePhenix)


Available at: https://github.com/ThreeKingz/BRW

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.



* PAWN library that allows you to read and write bytes into a file.
* Supports little endian and big endian formats.
* Supports jumping from one position to another position in the file.
* Allows you to read signed and unsigned data types.



************************************************************************************************************************/

#if !defined _inc_brw_
	#define _inc_brw_
#endif


#if !defined _file_included
  #tryinclude <file>
	#if !defined _file_included
		#error "The file library for I/O functions by ITB Compuphase was not found."
	#endif
#endif


#define BRW_DISPLAY_WARNING_MSGS				(true)


#if BRW_DISPLAY_WARNING_MSGS == true 
	#if !defined printf 	
		#error "The native function 'printf was not found."
	#endif
#endif

/* Macros */

//UINT32/INT32 *Unsigned and Signed*
#define BRW_MAX_UINT32							 0xFFFFFFFF //unsigned long (can't really be used in PAWN for values greater than cellmax (0x7FFFFFFF)) 
#define BRW_MAX_INT32							 0x7FFFFFFF //long  (2 147 483 647)
#define BRW_MIN_INT32							-0x7FFFFFFF //		(-2 147 483 647)

//UINT16/INT16 *Unsigned and Signed*
#define BRW_MAX_UINT16							 0xFFFF //unsigned short (65 535) 
#define BRW_MAX_INT16							 0x7FFF  //signed short  (32 767)
#define BRW_MIN_INT16						    -0x7FFF //				 (-32 767)

//UINT8(unsigned byte)/INT8(signed byte)
#define BRW_MAX_UINT8							 0xFF //unsigned byte (255)
#define BRW_MAX_INT8 							 0x7F //signed byte   (127)
#define BRW_MIN_INT8							-0x7F //              (-127)

#define BRW_FLOAT_NAN							(Float:0xFFFFFFFF)
#define BRW_MAX_BRW_HANDLES						10 //Max concurrent handles running.
#define BRW_INVALID_HANDLE						BRW:-1
#define BRW::									BRW_ //C++ scope like


/* BRW::FileWriteRawByte(File:handle, byte) 
 ** Writes a raw byte(not encoded) to a file **
*/
#define BRW_FileWriteRawByte(%0,%1) 				(fputchar(%0,%1,false))

/* BRW::FileReadRawByte(File:handle) 
 ** Returns a raw byte(not encoded) read from a file. **
*/
#define BRW_FileReadRawByte(%0) 					(fgetchar(%0,false))



#if BRW_DISPLAY_WARNING_MSGS == true
#define BRW_SendWarning(%0)      				(printf("[BRW]: " %0))
#endif


enum fmode
{
	bin_write,
	bin_read
}


//Default endianness is little_endian (Intel byte order)
enum Endianness
{
	big_endian,
	little_endian
}

enum BRW::data
{
	bool:BRW::IS_HANDLE_SET,
	fmode:BRW::FMODE,
	File:BRW::INPUT_FILE_HANDLE,
	BRW::BYTE_COUNT,
	Endianness:BRW::ENDIANNESS_SELECTED

};
new static BRW::Info[BRW:BRW_MAX_BRW_HANDLES][BRW::data];


/*****************************************************************************************************
									Internal Functions
******************************************************************************************************/

/*************************************************************
* Signed and unsigned INT32 conversions:
************************************************************/
static stock BRW::BytesToInt32(byte1, byte2, byte3, byte4, Endianness:format = little_endian)
{
	switch(format)
	{
		case big_endian:
		{
			return (byte1<<24)+(byte2<<16)+(byte3<<8)+byte4;
		}
		case little_endian:
		{
			return (byte4<<24)+(byte3<<16)+(byte2<<8)+byte1;
		}
	}
	return 0;
}

static stock BRW::Int32ToBytes(int32, &byte1, &byte2, &byte3, &byte4, Endianness:format = little_endian)
{
	switch(format)
	{
		case big_endian:
		{
			byte1 = int32 >>> 24;
			byte2 = (int32 & 0xFF0000) >>> 16;
			byte3 = (int32 & 0xFF00) >>> 8;
			byte4 = (int32 & 0xFF);
		}
		case little_endian:
		{
			byte4 = int32 >>> 24;
			byte3 = (int32 & 0xFF0000) >>> 16;
			byte2 = (int32 & 0xFF00) >>> 8;
			byte1 = (int32 & 0xFF);
		}
	}
	return 1;
}

/*************************************************************
* Signed and unsigned INT16 conversions:
************************************************************/

static stock BRW::BytesToUInt16(byte1, byte2, Endianness:format = little_endian)
{
	switch(format)
	{
		case big_endian:
		{
			return (byte1<<8)+byte2;
		}
		case little_endian:
		{
			return (byte2<<8)+byte1;
		}
	}
	return 0;
}

static stock BRW::UInt16ToBytes(uint16, &byte1, &byte2, Endianness:format = little_endian)
{
	switch(format)
	{
		case big_endian:
		{
			byte1 = uint16 >> 8;
			byte2 = (uint16 & 0xFF);
		}
		case little_endian:
		{
			byte2 = uint16 >> 8;
			byte1 = (uint16 & 0xFF);
		}
	}
	return 1;
}

static stock BRW::BytesToInt16(byte1, byte2, Endianness:format = little_endian)
{
	switch(format)
	{
		case big_endian:
		{
			if(byte1 & (1 << 7)) 
			{
				return (0xFFFF0000 + (byte1<<8) + byte2); 
			}
			else
			{
				return ((byte1<<8) + byte2);
			}
		}
		case little_endian:
		{
			if(byte2 & (1 << 7)) 
			{
				return (0xFFFF0000 + (byte2<<8) + byte1); 
			}
			else
			{
				return ((byte2<<8) + byte1);
			}
		}
	}
	return 0;
}

static stock BRW::Int16ToBytes(int16, &byte1, &byte2, Endianness:format = little_endian)
{
	if(int16 < 0)
	{
		int16 &= 0xFFFF;
	}
	switch(format)
	{
		case big_endian:
		{
			byte1 = int16 >> 8;
			byte2 = (int16 & 0xFF);
		}
		case little_endian:
		{
			byte2 = int16 >> 8;
			byte1 = (int16 & 0xFF);
		}
	}
	return 1;
}

/*************************************************************
* Signed and unsigned INT8 (Byte/SByte)
************************************************************/

/* BRW::ByteToUInt8(byte) */
#define BRW_ByteToUInt8(%0) (%0)

static stock BRW::UInt8ToByte(uint8, &byte)
{
	byte = uint8;
	return 1;
}

static stock BRW::Int8ToByte(int8, &byte)
{
	if(int8 < 0)
	{
		int8 &= 0xFF;
	}
	byte = int8;
	return 1;
}

static stock BRW::ByteToInt8(byte)
{
	if(byte & (1 << 7)) //negative
	{
		return (0xFFFFFF00 + byte); 
	}
	else
	{
		return byte;
	}
}

/*************************************************************
		4-byte Floating Point - IEEE 754
************************************************************/
static stock BRW::SPFloatToBytes(Float:value, &byte1, &byte2, &byte3, &byte4, Endianness:format = little_endian)
{
	switch(format)
	{
		case big_endian:
		{
			byte1 = (_:value >>> 24);
			byte2 = (_:value & 0xFF0000) >>> 16;
			byte3 = (_:value & 0xFF00) >>> 8;
			byte4 = (_:value & 0xFF);
		}
		case little_endian:
		{
			byte4 = (_:value >>> 24);
			byte3 = (_:value & 0xFF0000) >>> 16;
			byte2 = (_:value & 0xFF00) >>> 8;
			byte1 = (_:value & 0xFF);
		}
	}
	return 1;
}

static stock Float:BRW::SPBytesToFloat(byte1, byte2, byte3, byte4, Endianness:format = little_endian) //IEEE 754
{
	switch(format)
	{
		case big_endian:
		{
			new value = ((byte1 << 24) + (byte2 << 16) + (byte3 << 8) + (byte4));
			new sign = (value & (1 << 31)) ? -1 : 1;
			new mantissa = (value & 0x7FFFFF);
			new exponent = (value & 0x7F800000) >> 23;
			new Float:fmantissa = 1.0;
			for(new i = 0; i != 22; i++)
			{
				fmantissa += ((mantissa & (1 << (22 - i))) ? 1:0) * floatpower(2.0, -float(i + 1));
			}
			return sign * fmantissa * floatpower(2.0, exponent - 127);
		}
		case little_endian:
		{
			new value = ((byte4 << 24) + (byte3 << 16) + (byte2 << 8) + (byte1));
			new sign = (value & (1 << 31)) ? -1 : 1;
			new mantissa = (value & 0x7FFFFF);
			new exponent = (value & 0x7F800000) >> 23;
			new Float:fmantissa = 1.0;
			for(new i = 0; i != 22; i++)
			{
				fmantissa += ((mantissa & (1 << (22 - i))) ? 1:0) * floatpower(2.0, -float(i + 1));
			}
			return sign * fmantissa * floatpower(2.0, exponent - 127);
		}
	}
	return BRW_FLOAT_NAN;
}

static stock BRW: BRW::GetFreeSlot()
{
	new BRW:i = BRW:0;
	while (_:i < sizeof (BRW::Info) && BRW::Info[i][BRW::IS_HANDLE_SET] == true)
	{
		i++;
	}
	if (_:i == sizeof (BRW::Info)) return BRW_INVALID_HANDLE;
	return i;
}

static stock bool:BRW::IsValidHandle(BRW:handle)
{
	if(_:handle >= BRW_MAX_BRW_HANDLES) return false;
	if(handle == BRW_INVALID_HANDLE) return false;
	return BRW::Info[handle][BRW::IS_HANDLE_SET];
}

/*****************************************************************************************************
******************************************************************************************************/




/*****************************************************************************************************
									Actual Library Functions
******************************************************************************************************/

stock BRW: BRW::Open(const filename[], fmode:BRW::type, Endianness:format = little_endian, bool:append = false)
{
	new BRW:handle = BRW::GetFreeSlot();
	if(handle == BRW_INVALID_HANDLE)
	{
		#if BRW_DISPLAY_WARNING_MSGS == true
		BRW::SendWarning("Max concurrent handles limit reached. Increase BRW_MAX_BRW_HANDLES. (BRW::Open).");
		#endif
		return BRW_INVALID_HANDLE;
	}
	switch(BRW::type)
	{
		case bin_write:
		{
			BRW::Info[handle][BRW::FMODE] = bin_write;
			BRW::Info[handle][BRW::INPUT_FILE_HANDLE] = fopen(filename, append ? io_append : io_write);
		}
		case bin_read:
		{
			if(!fexist(filename))
			{
				#if BRW_DISPLAY_WARNING_MSGS == true
				BRW::SendWarning("File '%s' does not exist or could not be opened. (BRW::Open)", filename);
				#endif
				return BRW_INVALID_HANDLE;
			}
			BRW::Info[handle][BRW::FMODE] = bin_read;
			BRW::Info[handle][BRW::INPUT_FILE_HANDLE] = fopen(filename, io_read);
		}
	}
	BRW::Info[handle][BRW::BYTE_COUNT] = 0;
	BRW::Info[handle][BRW::ENDIANNESS_SELECTED] = format;
	BRW::Info[handle][BRW::IS_HANDLE_SET] = true;
	return handle;
}

stock BRW::Close(BRW:handle)
{
	if(!BRW::IsValidHandle(handle))
	{
		#if BRW_DISPLAY_WARNING_MSGS == true
		BRW::SendWarning("An incorrect handle (handle: %d) was passed as an argument. (BRW::Close)", _:handle);
		#endif
		return 0;
	}
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	fclose(fhandle);
	new bytes_written = BRW::Info[handle][BRW::BYTE_COUNT];
	BRW::Info[handle][BRW::IS_HANDLE_SET] = false;
	BRW::Info[handle][BRW::BYTE_COUNT] = 0;
	return bytes_written;
}

stock File:BRW::GetFileHandle(BRW:handle)
{
	if(!BRW::IsValidHandle(handle))
	{
		#if BRW_DISPLAY_WARNING_MSGS == true
		BRW::SendWarning("An incorrect handle (handle: %d) was passed as an argument. (BRW::GetFileHandle)", _:handle);
		#endif
		return File:-1;
	}
	return BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
}

//Could have used fblockread and fblockwrite but those functions only support little_endian
stock BRW::WriteCells(BRW:handle, const buffer[], maxlen = sizeof(buffer))
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_write) return 0;
	new bytes_written = 0;
	for(new i = 0; i < maxlen; i++)
	{
		BRW::WriteInt32(handle, buffer[i]);
		bytes_written += 4;
	}
	return bytes_written;
}

stock BRW::ReadCells(BRW:handle, buffer[], maxlen = sizeof(buffer))
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_read) return 0;
	new bytes_read = 0;
	for(new i = 0; i < maxlen; i++)
	{
		buffer[i] = BRW::ReadInt32(handle);
		bytes_read += 4;
	}
	return bytes_read;
}

stock BRW::Seek(BRW:handle, offset, seek_whence: whence = seek_start)
{
	if(!BRW::IsValidHandle(handle)) return 0;
	return fseek(BRW::Info[handle][BRW::INPUT_FILE_HANDLE], offset, whence);
}

stock BRW::Skip(BRW:handle, number_of_bytes)
{
	if(!BRW::IsValidHandle(handle)) return 0;
	return fseek(BRW::Info[handle][BRW::INPUT_FILE_HANDLE], number_of_bytes, seek_start);
}
/* Data types signed/unsigned */

stock BRW::WriteUInt32(BRW:handle, uint32)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_write) return 0;
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new byte1, byte2, byte3, byte4;
	//If it's greater than 0x7FFFFFFF, it will turn into a negative number
	BRW::Int32ToBytes(uint32, byte1, byte2, byte3, byte4, BRW::Info[handle][BRW::ENDIANNESS_SELECTED]);
	BRW::FileWriteRawByte(fhandle, byte1);
	BRW::FileWriteRawByte(fhandle, byte2);
	BRW::FileWriteRawByte(fhandle, byte3);
	BRW::FileWriteRawByte(fhandle, byte4);
	BRW::Info[handle][BRW::BYTE_COUNT] += 4;
	return 0;
}

//Won't work if value is greater than cellmax (0x7FFFFFFF) but will throw a warning with the value expressed in HEX.
stock BRW::ReadUInt32(BRW:handle, &uint32 = 0)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_read) return 0;
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new byte1, byte2, byte3, byte4;
	byte1 = BRW::FileReadRawByte(fhandle);
	byte2 = BRW::FileReadRawByte(fhandle);
	byte3 = BRW::FileReadRawByte(fhandle);
	byte4 = BRW::FileReadRawByte(fhandle);
	BRW::Info[handle][BRW::BYTE_COUNT] += 4;
	switch(BRW::Info[handle][BRW::ENDIANNESS_SELECTED])
	{
		case little_endian:
		{
			if(byte4 & (1 << 7)) //is greater than 0x7FFFFFFF
			{
				BRW::SendWarning("An unsigned int32 greater than 2147483647 was read and PAWN cannot treat it as such.");
				new ch[4][3];
				format(ch[0], 3, "%x", byte1);
				format(ch[1], 3, "%x", byte2);
				format(ch[2], 3, "%x", byte3);
				format(ch[3], 3, "%x", byte4);
				for(new i = 0; i != 3; i++)
				{
					if(ch[i][0] && ch[i][1] == '\0')
					{
						new tmp = ch[i][0];
						ch[i][0] = '0';
						ch[i][1] = tmp;
						ch[i][2] = '\0';
					}
				}
				BRW::SendWarning("The unsigned int in hex is: 0x%s%s%s%s", ch[3], ch[2], ch[1], ch[0]);
				return 0;
			}
		}
		case big_endian:
		{
			if(byte1 & (1 << 7)) //is greater than 0x7FFFFFFF
			{
				BRW::SendWarning("An unsigned int32 greater than 2147483647 was read and PAWN cannot treat it as such.");
				new ch[4][3];
				format(ch[0], 3, "%x", byte1);
				format(ch[1], 3, "%x", byte2);
				format(ch[2], 3, "%x", byte3);
				format(ch[3], 3, "%x", byte4);
				for(new i = 0; i != 3; i++)
				{
					if(ch[i][0] && ch[i][1] == '\0')
					{
						new tmp = ch[i][0];
						ch[i][0] = '0';
						ch[i][1] = tmp;
						ch[i][2] = '\0';
					}
				}
				BRW::SendWarning("The unsigned int in hex is: 0x%s%s%s%s", ch[0], ch[1], ch[2], ch[3]);
			}
		}
	}
	uint32 = BRW::BytesToInt32(byte1, byte2, byte3, byte4, BRW::Info[handle][BRW::ENDIANNESS_SELECTED]);
	return uint32;
}

stock BRW::ReadInt32(BRW:handle, &int32 = 0)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_read) return 0;
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new byte1, byte2, byte3, byte4;
	byte1 = BRW::FileReadRawByte(fhandle);
	byte2 = BRW::FileReadRawByte(fhandle);
	byte3 = BRW::FileReadRawByte(fhandle);
	byte4 = BRW::FileReadRawByte(fhandle);
	BRW::Info[handle][BRW::BYTE_COUNT] += 4;
	int32 = BRW::BytesToInt32(byte1, byte2, byte3, byte4, BRW::Info[handle][BRW::ENDIANNESS_SELECTED]);
	return int32;
}

stock BRW::WriteInt32(BRW:handle, int32)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_write) return 0;
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new byte1, byte2, byte3, byte4;
	BRW::Int32ToBytes(int32, byte1, byte2, byte3, byte4, BRW::Info[handle][BRW::ENDIANNESS_SELECTED]);
	BRW::FileWriteRawByte(fhandle, byte1);
	BRW::FileWriteRawByte(fhandle, byte2);
	BRW::FileWriteRawByte(fhandle, byte3);
	BRW::FileWriteRawByte(fhandle, byte4);
	BRW::Info[handle][BRW::BYTE_COUNT] += 4;
	return 1;
}
stock BRW::WriteFloat(BRW:handle, Float:value)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_write) return 0;
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new byte1, byte2, byte3, byte4;
	BRW::SPFloatToBytes(value, byte1, byte2, byte3, byte4, BRW::Info[handle][BRW::ENDIANNESS_SELECTED]);
	BRW::FileWriteRawByte(fhandle, byte1);
	BRW::FileWriteRawByte(fhandle, byte2);
	BRW::FileWriteRawByte(fhandle, byte3);
	BRW::FileWriteRawByte(fhandle, byte4);
	BRW::Info[handle][BRW::BYTE_COUNT] += 4;
	return 1;
}
stock Float:BRW::ReadFloat(BRW:handle, &Float:value = BRW_FLOAT_NAN)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_read) return BRW_FLOAT_NAN;
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new byte1, byte2, byte3, byte4;
	byte1 = BRW::FileReadRawByte(fhandle);
	byte2 = BRW::FileReadRawByte(fhandle);
	byte3 = BRW::FileReadRawByte(fhandle);
	byte4 = BRW::FileReadRawByte(fhandle);
	value = BRW::SPBytesToFloat(byte1, byte2, byte3, byte4, BRW::Info[handle][BRW::ENDIANNESS_SELECTED]);
	BRW::Info[handle][BRW::BYTE_COUNT] += 4;
	return value;
}


stock BRW::WriteUByte(BRW:handle, uint8)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_write) return 0;
	if(uint8 > BRW_MAX_UINT8 || uint8 < 0)
	{
		#if BRW_DISPLAY_WARNING_MSGS == true
		BRW::SendWarning("An incorrect value was passed as an argument (argument: %d). (BRW::WriteUByte).", uint8);
		BRW::SendWarning("An unsigned byte (uint8) cannot be negative and cannot be greater than %d.", BRW_MAX_UINT8);
		#endif
		return 0;
	}
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	BRW::FileWriteRawByte(fhandle, uint8);
	BRW::Info[handle][BRW::BYTE_COUNT]++;
	return 1;
}

stock BRW::WriteUInt8(BRW:handle, uint8)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_write) return 0;
	if(uint8 > BRW_MAX_UINT8 || uint8 < 0)
	{
		#if BRW_DISPLAY_WARNING_MSGS == true
		BRW::SendWarning("An incorrect value was passed as an argument (argument: %d). (BRW::WriteUInt8).", uint8);
		BRW::SendWarning("An unsigned byte (uint8) cannot be negative and cannot be greater than %d.", BRW_MAX_UINT8);
		#endif
		return 0;
	}
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	BRW::FileWriteRawByte(fhandle, uint8);
	BRW::Info[handle][BRW::BYTE_COUNT]++;
	return 1;
}

stock BRW::ReadUByte(BRW:handle, &uint8 = 0)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_read) return 0;
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new byte;
	byte = BRW::FileReadRawByte(fhandle);
	BRW::Info[handle][BRW::BYTE_COUNT]++;
	uint8 = byte;
	return byte;
}

stock BRW::ReadUInt8(BRW:handle, &uint8 = 0)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_read) return 0;
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new byte;
	byte = BRW::FileReadRawByte(fhandle);
	BRW::Info[handle][BRW::BYTE_COUNT]++;
	uint8 = byte;
	return byte;
}

stock BRW::WriteInt8(BRW:handle, int8)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_write) return 0;
	if(int8 > BRW_MAX_INT8 || int8 < BRW_MIN_INT8)
	{
		#if BRW_DISPLAY_WARNING_MSGS == true
		BRW::SendWarning("An incorrect value was passed as an argument (argument: %d). (BRW::WriteInt8).", int8);
		BRW::SendWarning("An signed byte (int8) cannot be greater than %d or less than %d.", BRW_MAX_INT8, BRW_MIN_INT8);
		#endif
		return 0;
	}
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new sbyte = 0;
	BRW::Int8ToByte(int8, sbyte);
	BRW::FileWriteRawByte(fhandle, sbyte);
	BRW::Info[handle][BRW::BYTE_COUNT]++;
	return 1;
}

stock BRW::WriteSByte(BRW:handle, int8)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_write) return 0;
	if(int8 > BRW_MAX_INT8 || int8 < BRW_MIN_INT8)
	{
		#if BRW_DISPLAY_WARNING_MSGS == true
		BRW::SendWarning("An incorrect value was passed as an argument (argument: %d). (BRW::WriteSbyte).", int8);
		BRW::SendWarning("An signed byte (int8) cannot be greater than %d or less than %d.", BRW_MAX_INT8, BRW_MIN_INT8);
		#endif
		return 0;
	}
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new sbyte = 0;
	BRW::Int8ToByte(int8, sbyte);
	BRW::FileWriteRawByte(fhandle, sbyte);
	BRW::Info[handle][BRW::BYTE_COUNT]++;
	return 1;
}


stock BRW::ReadInt8(BRW:handle, &int8 = 0)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_read) return 0;
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new byte = 0;
	byte = BRW::ByteToInt8(BRW::FileReadRawByte(fhandle));
	BRW::Info[handle][BRW::BYTE_COUNT]++;
	int8 = byte;
	return byte;
}

stock BRW::ReadSByte(BRW:handle, &int8 = 0)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_read) return 0;
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new byte = 0;
	byte = BRW::ByteToInt8(BRW::FileReadRawByte(fhandle));
	BRW::Info[handle][BRW::BYTE_COUNT]++;
	int8 = byte;
	return byte;
}
stock BRW::WriteUInt16(BRW:handle, uint16)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_write) return 0;
	if(uint16 > BRW_MAX_UINT16 || uint16 < 0)
	{
		#if BRW_DISPLAY_WARNING_MSGS == true
		BRW::SendWarning("An incorrect value was passed as an argument (argument: %d). (BRW::WriteUInt16).", uint16);
		BRW::SendWarning("An unsigned int16 cannot be negative and cannot be greater than %d.", BRW_MAX_UINT16);
		#endif
		return 0;
	}
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new byte1 = 0, byte2 = 0;
	BRW::UInt16ToBytes(uint16, byte1, byte2, BRW::Info[handle][BRW::ENDIANNESS_SELECTED]);
	BRW::FileWriteRawByte(fhandle, byte1);
	BRW::FileWriteRawByte(fhandle, byte2);
	BRW::Info[handle][BRW::BYTE_COUNT]+=2;
	return 1;
}

stock BRW::ReadUInt16(BRW:handle, &uint16 = 0)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_read) return 0;
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new byte1, byte2;
	byte1 = BRW::FileReadRawByte(fhandle);
	byte2 = BRW::FileReadRawByte(fhandle);
	BRW::Info[handle][BRW::BYTE_COUNT] += 2;
	uint16 = BRW::BytesToUInt16(byte1, byte2, BRW::Info[handle][BRW::ENDIANNESS_SELECTED]);
	return uint16;
}
stock BRW::WriteInt16(BRW:handle, int16)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_write) return 0;
	if(int16 > BRW_MAX_INT16 || int16 < BRW_MIN_INT16)
	{
		#if BRW_DISPLAY_WARNING_MSGS == true
		BRW::SendWarning("An incorrect value was passed as an argument (argument: %d). (BRW::WriteInt16).", int16);
		BRW::SendWarning("An signed int16 cannot be greater than %d or less than %d.", BRW_MAX_INT16, BRW_MIN_INT16);
		#endif
		return 0;
	}
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new byte1, byte2;
	BRW::Int16ToBytes(int16, byte1, byte2, BRW::Info[handle][BRW::ENDIANNESS_SELECTED]);
	BRW::FileWriteRawByte(fhandle, byte1);
	BRW::FileWriteRawByte(fhandle, byte2);
	BRW::Info[handle][BRW::BYTE_COUNT]+=2;
	return 1;
}

stock BRW::ReadInt16(BRW:handle, &int16 = 0)
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_read) return 0;
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new byte1, byte2;
	byte1 = BRW::FileReadRawByte(fhandle);
	byte2 = BRW::FileReadRawByte(fhandle);
	BRW::Info[handle][BRW::BYTE_COUNT] += 2;
	int16 = BRW::BytesToInt16(byte1, byte2, BRW::Info[handle][BRW::ENDIANNESS_SELECTED]);
	return int16;
}

/*

Not used for now...
stock BRW::WriteString(BRW:handle, const string[])
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_write) return 0;
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	for(new i = 0; i < strlen(string); i++)
	{
		BRW::FileWriteRawByte(fhandle, string[i]);
		BRW::Info[handle][BRW::BYTE_COUNT]++;
	}
	BRW::FileWriteRawByte(fhandle, '\0'); //NULL terminator
	BRW::Info[handle][BRW::BYTE_COUNT]++;
	return 1;
}

stock BRW::ReadString(BRW:handle, output[], len = sizeof(output))
{
	if(!BRW::IsValidHandle(handle) || BRW::Info[handle][BRW::FMODE] != bin_read) return 0;
	new File:fhandle = BRW::Info[handle][BRW::INPUT_FILE_HANDLE];
	new idx = 0;
	for( ; ; )
	{
		output[idx] = BRW::FileReadRawByte(fhandle);
		if(output[idx] == '\0' || idx == len - 1)
		{
			break;
		}
		idx++;
	}
	return 1;
}*/

#if BRW_DISPLAY_WARNING_MSGS == true
#undef BRW_SendWarning
#endif
