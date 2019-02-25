//
//	ITEMS
//


#if defined _inc_items_
	#endinput
#endif 

#define _inc_items_


#include <a_samp>
#include <YSI_Data\y_iterate>



//
//	DEFINES
//


#if !defined MAX_ITEM_TYPES 
	#define MAX_ITEM_TYPES					16
#endif 


#if !defined MAX_ITEMS 						
	#define MAX_ITEMS 						2048
#endif


#if !defined MAX_ITEM_NAME
	#define MAX_ITEM_NAME					32
#endif


#define INVALID_ITEM 						(-1)


//
//	DATA
//


const Float:ITEM_STREAM_DIST = 50.0;
static const ITEM_REFERENCE = 0x7165;


static enum e_ITEM_TYPES
{
	itmTypeName[MAX_ITEM_NAME],
	itmTypeModel,
	Float:itmTypeRX,
	Float:itmTypeRY
}

static ItemType[MAX_ITEM_TYPES][e_ITEM_TYPES];
static ItemTypeCount;



static enum e_ITEM_DATA
{
	itmType,
	itmArea,
	itmObjt,
	Float:itmX,
	Float:itmY,
	Float:itmZ,
	itmWorld,
	itmInterior
}	

static Item[MAX_ITEMS][e_ITEM_DATA];
static Iterator:C_Items<MAX_ITEMS>;
static Iterator:S_Items<MAX_ITEMS>;


//
//	FUNCTIONS
//


stock IsValidItemType(number)
{
	return (-1 < number < ItemTypeCount);
}


stock IsValidItem(index)
{
	return Iter_Contains(C_Items, index);
}


stock IsItemInWorld(index)
{
	return Iter_Contains(S_Items, index);
}


stock DefineItem(const name[], modelid, Float:rx, Float:ry)
{
	if(ItemTypeCount + 1 >= MAX_ITEM_TYPES)
	{
		print("DefineItem reached your limit.");
		return INVALID_ITEM;
	}

	new index = ItemTypeCount++;

	strcat(ItemType[index][itmTypeName], name, MAX_ITEM_NAME);
	ItemType[index][itmTypeModel] = modelid;
	ItemType[index][itmTypeRX] = rx;
	ItemType[index][itmTypeRY] = ry;

	return index;
}


stock CreateItem(type, bool:spawn, Float:x, Float:y, Float:z, worldid = -1, interiorid = 0)
{
	if(!IsValidItemType(type))
		return INVALID_ITEM;

	new index;
	if((index = Iter_Alloc(C_Items)) > -1)
	{
		Item[index][itmObjt] = Item[index][itmArea] = INVALID_STREAMER_ID;
		Item[index][itmType] = type;
		Item[index][itmX] = x;
		Item[index][itmY] = y;
		Item[index][itmZ] = z;
		Item[index][itmWorld] = worldid;
		Item[index][itmInterior] = interiorid;

		if(spawn)
			ItemWorldSpawn(index);

		return index;
	}
	return INVALID_ITEM;		
}


static ItemWorldSpawn(index)
{
	new type = Item[index][itmType];
	new Float:rx = ItemType[type][itmTypeRX];
	new Float:ry = ItemType[type][itmTypeRY];
	new worldid = Item[index][itmWorld];
	new interiorid = Item[index][itmInterior];

	new data[2];
	data[0] = ITEM_REFERENCE;
	data[1] = index;

	new objectid = CreateDynamicObject(ItemType[type][itmTypeModel], Item[index][itmX], Item[index][itmY], Item[index][itmZ], rx, ry, float(random(360)), worldid, interiorid, _, ITEM_STREAM_DIST, ITEM_STREAM_DIST);
	new areaid = CreateDynamicSphere(0.0, 0.0, 0.0, 1.1, worldid, interiorid);

	AttachDynamicAreaToObject(areaid, objectid);

	Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, data);
	Streamer_SetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, data);

	Item[index][itmArea] = areaid;
	Item[index][itmObjt] = objectid;

	Iter_Add(S_Items, index);
}


stock DestroyItem(index)
{
	if(!Iter_Contains(C_Items, index))
		return 0;

    if(IsValidDynamicObject(Item[index][itmObjt]))
    	DestroyDynamicObject(Item[index][itmObjt]);

    if(IsValidDynamicArea(Item[index][itmArea]))
    	DestroyDynamicArea(Item[index][itmArea]);

	Item[index][itmObjt] = Item[index][itmArea] = INVALID_STREAMER_ID;
	Item[index][itmType] = -1;

	return Iter_Remove(C_Items, index);
}


stock AddItemInWorld(index)
{
	if(!Iter_Contains(C_Items, index))
		return 0;

	if(Iter_Contains(S_Items, index))
		return 0;

	ItemWorldSpawn(index);
	return 1;
}


stock RemoveItemFromWorld(index)
{
	if(!Iter_Contains(C_Items, index))
		return 0;

	if(!Iter_Contains(S_Items, index))
		return 0;

    if(IsValidDynamicObject(Item[index][itmObjt]))
    	DestroyDynamicObject(Item[index][itmObjt]);

    if(IsValidDynamicArea(Item[index][itmArea]))
    	DestroyDynamicArea(Item[index][itmArea]);

	Item[index][itmObjt] = Item[index][itmArea] = INVALID_STREAMER_ID;
	Item[index][itmType] = -1;

	return Iter_Remove(S_Items, index);
}


stock GetItemTypeName(type, name[], size = sizeof name)
{
	if(!IsValidItemType(type))
		return;

	strcpy(name, ItemType[type][itmTypeName], size);
}


stock GetItemTypeModel(type)
{
	if(!IsValidItemType(type))
		return 0;

	return ItemType[type][itmTypeModel];		
}


stock GetItemTypeRotation(type, &Float:rx, &Float:ry)
{
	if(!IsValidItemType(type))
		return;

	rx = ItemType[type][itmTypeRX];		
	ry = ItemType[type][itmTypeRY];		
}


stock GetItemType(index)
{
	if(!Iter_Contains(C_Items, index))
		return -1;

	return Item[index][itmType];
}


stock GetItemObject(index)
{
	if(!Iter_Contains(C_Items, index))
		return INVALID_STREAMER_ID;
	
	return Item[index][itmObjt];
}


stock GetItemArea(index)
{
	if(!Iter_Contains(C_Items, index))
		return INVALID_STREAMER_ID;
	
	return Item[index][itmArea];
}


stock GetItemPos(index, &Float:x, &Float:y, &Float:z)
{
	if(!Iter_Contains(C_Items, index))
		return;

	x = Item[index][itmX];
	y = Item[index][itmY];
	z = Item[index][itmZ];		
}


stock GetItemWorld(index)
{
	if(!Iter_Contains(C_Items, index))
		return -1;

	return Item[index][itmWorld];
}


stock GetItemInterior(index)
{
	if(!Iter_Contains(C_Items, index))
		return -1;
			
	return Item[index][itmInterior];
}
