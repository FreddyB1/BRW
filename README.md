# BRW
Binary reader and writer for PAWN that allows you to work with various data types.



## Features
 * Endianness: Supports little endian and big endian formats.
 * Supports jumping from one position to another position relative to (start, current position and end of file).
 * Allows you to read and write signed and unsigned data types.
 * Allows you write and read 4-byte floating point values (IEEE 754).
 
 

## Installation:
Simply include BRW.inc in your file.

```pawn
#include <BRW>
```

## Functions:

### Reading


Values can either be called-by-reference or returned. The value returned or called-by-reference is converted to a PAWN cell (4-bytes). 
Each time a function is called, the specific amount of bytes each data type holds are advanced on the file, e.g, INT16 will advance 2 bytes.

```pawn
//Signed INT8 (signed byte)
BRW::ReadInt8(BRW:handle, &int8 = 0); //Range [-127, 127]
BRW::ReadSByte(BRW:handle, &int8 = 0); //Range [-127, 127]

//Unsigned INT8(unsigned byte)
BRW::ReadUInt8(BRW:handle, &uint8 = 0); //Range [0, 255]
BRW::ReadUByte(BRW:handle, &uint8 = 0); //Range [0, 255]

//Signed INT16 / Unsigned INT 16
BRW::ReadInt16(BRW:handle, &int16 = 0); //Range [-32767, 32767]
BRW::ReadUInt16(BRW:handle, &uint16 = 0); //Range [0, 65535]

//Signed INT32 / Unsigned INT32 (doesn't support values greater than 0x7FFFFFFF - if greater, prints a warning with the value in HEX)
BRW::ReadInt32(BRW:handle, &int32 = 0); //Range [-2147483647, 2147483647]
BRW::ReadUInt32(BRW:handle, &uint32 = 0); 

//PAWN cell array (4 bytes)
BRW::ReadCells(BRW:handle, buffer[], maxlen = sizeof(buffer)); //Range [-2147483647, 2147483647]

//4-byte floating point
Float:BRW::ReadFloat(BRW:handle, &Float:value = BRW_FLOAT_NAN)
```


### Writing


The input values are PAWN cells that are converted to the specific amount of bytes each data type holds, the values must be bound by the ranges for each data type.
Each time a function is called, the specific amount of bytes each data type holds are advanced on the file, e.g, INT16 will advance 2 bytes.

```pawn
//Signed INT8 (signed byte)
BRW::WriteInt8(BRW:handle, int8); //Range [-127, 127]
BRW::WriteSByte(BRW:handle, int8); //Range [-127, 127]

//Unsigned INT8(unsigned byte)
BRW::WriteUInt8(BRW:handle, uint8); //Range [0, 255]
BRW::WriteUByte(BRW:handle, uint8); //Range [0, 255]

//Signed INT16 / Unsigned INT 16
BRW::WriteInt16(BRW:handle, int16); //Range [-32767, 32767]
BRW::WriteUInt16(BRW:handle, uint16); //Range [0, 65535]

//Signed INT32 / Unsigned INT32 (could write values greater than 0x7FFFFFFF but those cannot be read.)
BRW::WriteInt32(BRW:handle, int32); //Range [-2147483647, 2147483647]
BRW::WriteUInt32(BRW:handle, uint32)

//PAWN cell array (4 bytes)
BRW::WriteCells(BRW:handle, const buffer[], maxlen = sizeof(buffer)); //Range [-2147483647, 2147483647]

//4-byte floating point
BRW::WriteFloat(BRW:handle, Float:value);

```
