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

**************************************************
Navi Nodes
4b - INT16[2] - Position (XY), see below
2b - UINT16   - Area ID
2b - UINT16   - Node ID
2b - INT8[2]  - Direction (XY), see below
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
	new Float:x, Float:y, Float:z, Float:dirx, Float:diry, linkid, areaid, nodeid, nodetype, flags;
	new veh_ped_nodes_total = 0;
	new navi_nodes = 0;
	GetHeaderInfo(filename, veh_ped_nodes_total, .navi_nodes = navi_nodes);
	printf("Path nodes: \n");
	for(new i = 0; i < veh_ped_nodes_total; i++)
	{
		ReadPathNode(filename, i, x, y, z, linkid, areaid, nodetype);
		printf("nodeid: %d | areaid: %d | linkid: %d | type: %d | pos(%f, %f, %f)", i, areaid, linkid, nodetype, x, y, z);
	}
	printf("\n\nNavi nodes: ");
	for(new i = 0; i < navi_nodes; i++)
	{
		ReadNaviNodes(filename, i, areaid, nodeid, x, y, dirx, diry, flags);
		printf("nodeid: %d | areaid: %d | pos(%f, %f), dir(%f, %f), flags: 0x%x", nodeid, areaid, x, y, dirx, diry, flags);
	}

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
stock ReadNaviNodes(const filename[], index, &areaid, &nodeid, &Float:posx, &Float:posy, &Float:dirx, &Float:diry, &flags)
{
	new BRW: handle = BRW::Open(filename, bin_read); //The default endianness is: Little endian.

	new total_nodes = 0;
	GetHeaderInfo(filename, total_nodes);
	BRW::Seek(handle, 0);
	//Get the right byte position
	new skip_value = HEADER_SIZE + (total_nodes * PATH_NODE_ENTRY_SIZE) + (index * NAVI_NODE_ENTRY_SIZE);
	BRW::Skip(handle, skip_value);
	posx = floatdiv(float(BRW::ReadInt16(handle)), 8.0);
	posy = floatdiv(float(BRW::ReadInt16(handle)), 8.0);
	areaid = BRW::ReadUInt16(handle);
	nodeid = BRW::ReadUInt16(handle);
	dirx = floatdiv(float(BRW::ReadInt8(handle)), 100.0); //INT 8 is a signed byte
	diry = floatdiv(float(BRW::ReadInt8(handle)), 100.0);
	flags = BRW::ReadUInt32(handle);
	BRW::Close(handle);
	return 0;
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
