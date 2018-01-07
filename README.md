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

```pawn
// Returns a valid handle.
BRW: BRW::Open(const filename[], fmode:BRW::type, Endianness:format = little_endian, bool:append = false);
 
 Endianness:
    - little_endian
    - big_endian
    
 fmode:
    - bin_read
    - bin_write
```

```pawn
//Closes a handle that was previously opened and returns the number of bytes read or written.
BRW::Close(BRW:handle);
```


```pawn
//Jumps to a specific location in the file using a certain offset.
BRW::Seek(BRW:handle, offset, seek_whence: whence = seek_start);

  seek_whence
     -  seek_start //start of the file
     -  seek_current //current position in the file
     -  seek_end //end of the file

//Skips a certain number of bytes relative to the current position in the file.
BRW::Skip(BRW:handle, number_of_bytes);
```
### Reading


Values can either be retrieved: called-by-reference or returned. The value returned or called-by-reference is converted to a PAWN cell (4-bytes). 
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

## Example:

Reading nodes from the Node*.dat files in the GTA SA's paths folder:

```pawn
/*

Example using BRW to read binary data contained in the GTA SA Path's folder.
Documentation to read the binary files can be found at: http://gta.wikia.com/wiki/Paths_(GTA_SA)


 ** Header ** 
4b - UINT32 - number of nodes (section 1)
4b - UINT32 - number of vehicle nodes (section 1a)
4b - UINT32 - number of ped nodes (section 1b)
4b - UINT32 - number of navi nodes (section 2)
4b - UINT32 - number of links (section 3/5/6)

**************************************************
** Node entry **
4b - UINT32   - Mem Address, unused
4b - UINT32   - always zero, unused
6b - INT16[3] - Position (XYZ), see below
2b - INT16    - unknown, always 0x7FFE
2b - UINT16   - Link ID
2b - UINT16   - Area ID (same as in filename)
2b - UINT16   - Node ID (increments by 1)
1b - UINT8    - Path Width
1b - UINT8    - Node Type
4b - UINT32   - Flags

*/
#include <a_samp>
#include <BRW>


/* Sizes in Bytes */

#define HEADER_SIZE 				(20)
#define PATH_NODE_ENTRY_SIZE 		(28)
#define NAVI_NODE_ENTRY_SIZE		(14)
#define LINK_ENTRY_SIZE				(4)

main()
{
	new const filename[24] = "Paths/NODES21.dat";
	new Float:x, Float:y, Float:z, linkid, areaid, nodetype;
	new veh_ped_nodes_total = 0;
	new navi_nodes = 0;
	GetHeaderInfo(filename, veh_ped_nodes_total, .navi_nodes = navi_nodes);
	printf("Path nodes: \n");
	for(new i = 0; i < veh_ped_nodes_total; i++)
	{
		ReadPathNode(filename, i, x, y, z, linkid, areaid, nodetype);
		printf("nodeid: %d | areaid: %d | linkid: %d | type: %d | pos(%f, %f, %f)", i, areaid, linkid, nodetype, x, y, z);
	}


}
stock ReadPathNode(const filename[], nodeid, &Float:x, &Float:y, &Float:z, &linkid, &areaid, &nodetype)
{
	new BRW: handle = BRW::Open(filename, bin_read); //The default endianness is: Little endian.

	//Offsets
	#define POSITION_OFFSET			8 //relative to the start of each node entry
	/********************************/
	new totalnodes = BRW::ReadUInt32(handle);
	if(nodeid >= totalnodes)
	{
		printf("ERROR: Invalid nodeid. The max node index is %d.", totalnodes - 1);
		BRW::Close(handle);
		return 0;
	}
	BRW::Seek(handle, HEADER_SIZE + (PATH_NODE_ENTRY_SIZE * nodeid) + POSITION_OFFSET, seek_start); //Position the file on the nodeid's entry relative to the start of the file.
	x = floatdiv(float(BRW::ReadInt16(handle)), 8.0);
	y = floatdiv(float(BRW::ReadInt16(handle)), 8.0);
	z = floatdiv(float(BRW::ReadInt16(handle)), 8.0);
	BRW::Seek(handle, 2, seek_current); //skip 2 bytes | could have used BRW::Skip(handle, 2);
	linkid = BRW::ReadUInt16(handle);
	areaid = BRW::ReadUInt16(handle);
	BRW::Seek(handle, 3, seek_current); //skip 3 bytes (nodeid and path width) could have used BRW::Skip(handle, 3);
	nodetype = BRW::ReadInt8(handle); //Unsigned INT8 (byte)
	BRW::Close(handle);
	return 1;
}

stock GetHeaderInfo(const filename[], &total_nodes, &vehicle_nodes = 0, &ped_nodes = 0, &navi_nodes = 0, &links = 0)
{
	new BRW: handle = BRW::Open(filename, bin_read);//The default endianness is: Little endian.
	total_nodes = BRW::ReadUInt32(handle);
	vehicle_nodes = BRW::ReadUInt32(handle);
	ped_nodes = BRW::ReadUInt32(handle);
	navi_nodes = BRW::ReadUInt32(handle);
	links = BRW::ReadUInt32(handle);
	BRW::Close(handle);
	return 1;
}

```
