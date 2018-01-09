/*

Example using BRW to read and write binary data.

*/

#include <a_samp>
#include <BRW>


main()
{
	new const file[24] = "Binary/bin.dat";
	new BRW:handle = BRW::Open(file, bin_write);

	//Writes a signed INT32
	BRW::WriteInt32(handle, 124241245);
	BRW::WriteInt32(handle, 23);

	//Writes an usigned INT32 (PAWN really cannot handle numbers greater than cellmax)

	BRW::WriteUInt32(handle, 2344);
	//Writes a signed INT16

	BRW::WriteInt16(handle, BRW_MAX_INT16);
	BRW::WriteInt16(handle, BRW_MIN_INT16);

	//Writes an unsigned INT16

	BRW::WriteUInt16(handle, BRW_MAX_UINT16);

	//Writes a signed INT8 (sbyte)

	BRW::WriteInt8(handle, 127); //same function
	BRW::WriteSByte(handle, -127);  //same function

	//Writes an usigned INT8 (unsigned byte)
	BRW::WriteUInt8(handle, 255); //same function
	BRW::WriteUByte(handle, 255);  //same function

	//writes a 4-byte floating point 

	BRW::WriteFloat(handle, 99.99);
	BRW::WriteFloat(handle, -3.141516);

	//Writes a PAWN cell array

	new cells[4] = {23449, 1293993, 1283874, 129394};

	BRW::WriteCells(handle, cells);
	printf("Bytes written: %d bytes.", BRW::Close(handle));


	handle = BRW::Open(file, bin_read);
	printf("%d", BRW::ReadInt32(handle));
	printf("%d", BRW::ReadInt32(handle));
	printf("%d", BRW::ReadUInt32(handle));
	printf("%d", BRW::ReadInt16(handle));
	printf("%d", BRW::ReadInt16(handle));
	printf("%d", BRW::ReadUInt16(handle));
	printf("%d", BRW::ReadInt8(handle));
	printf("%d", BRW::ReadSByte(handle));
	printf("%d", BRW::ReadUInt8(handle));
	printf("%d", BRW::ReadUByte(handle));
	printf("%f", BRW::ReadFloat(handle));
	printf("%f", BRW::ReadFloat(handle));
	new dest[4];
	BRW::ReadCells(handle, dest, sizeof(dest));
	for(new i = 0; i < sizeof(dest); i++)
		printf("dest[%d] = %d", i, dest[i]);
	printf("Bytes read: %d bytes.", BRW::Close(handle));

	/* Output:
		Bytes written: 46 bytes.
		2147483647
		-2147483647
		2147483647
		32767
		-32767
		65535
		127
		-127
		255
		255
		234.234436
		-32.132995
		dest[0] = 23449
		dest[1] = 1293993
		dest[2] = 1283874
		dest[3] = 129394
		Bytes read: 46 bytes.
	*/


}
