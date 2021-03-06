/**
 *  Plastic Inquisitor
 *  Copyright (C) 2015 The Plastic Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef INVENTORY_H_
#define INVENTORY_H_

#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include "IGData.h"
#include "vmodel.h"
#include "vsprite.h"
#include "actortypes.h"
#include "aichip.h"


/* Kinds of inventory objects */
enum EInventoryKind {
	INVK_BASE,
	INVK_WEARABLE,
	INVK_VOXEL,
	INVK_AIPART,
};


/* Basic inventory item */
class InventoryObject : public IGData {
protected:
	EInventoryKind kind;
	std::string name,desc;
	int weight,cond,cost,count;
	SPABase boost;

	//Internal basic variables initialization.
	void InitVars();

	//Basic comparative function to be able to use comparison in child's operators.
	const bool BaseEqualTo(const InventoryObject & obj);

	//Game data I/O
	virtual void PushPrivateData(FILE* f);
	virtual bool PullPrivateData(FILE* f);

public:
	InventoryObject();
	InventoryObject(EInventoryKind k);
	virtual ~InventoryObject()		{}

	virtual const bool operator == (const InventoryObject & obj);
	virtual bool SerializeToFile(FILE* f);
	virtual bool DeserializeFromFile(FILE* f);

	virtual void SetKind(EInventoryKind k)		{ kind = k; }
	virtual void SetName(const std::string s)	{ name = s; }
	virtual void SetDesc(const std::string s)	{ desc = s; }
	virtual void SetWeight(const int s)			{ weight = s; }
	virtual void SetCondition(const int s)		{ cond = s; }
	virtual void SetCost(const int s)			{ cost = s; }
	virtual void SetCount(const int s)			{ count = s; }
	virtual void IncCount()						{ ++count; }
	virtual bool DecCount();
	virtual void SetBoost(SPABase nb)			{ boost = nb; }

	virtual EInventoryKind GetKind()			{ return kind; }
	virtual std::string GetName()				{ return name; }
	virtual std::string GetDesc()				{ return desc; }
	virtual int GetWeight()						{ return weight; }
	virtual int GetCondition()					{ return cond; }
	virtual int GetCost()						{ return cost; }
	virtual int GetCount() 						{ return count; }
	virtual SPABase GetBoost()					{ return boost; }
};

/* ******************************************************************** */

/* Storage container for all of the inventory items */
class Inventory : public IGData {
private:
	std::vector<InventoryObject*> items;

public:
	Inventory();
	virtual ~Inventory();

	///Game data interface serialization implementation.
	bool SerializeToFile(FILE* f);
	bool DeserializeFromFile(FILE* f);

	///Adds an object to the inventory list.
	void AddObject(InventoryObject* obj);

	///Returns the number of objects contained in the inventory.
	int GetNumItems()							{ return items.size(); }

	///Returns an object from the inventory.
	InventoryObject * GetInventoryObject(int n);

	///Destroys an object from the inventory.
	bool DestroyObject(InventoryObject* obj);

	///Default inventory sorting.
	void SortDefault();

	///Sorting inventory by name.
	void SortByName();

	///Sorting inventory by weight
	void SortByWeight();

	///Sorting inventory by count
	void SortByCount();

	///Sorting inventory by cost
	void SortByCost();
};

/* ******************************************************************** */

/* Wearable object type */
class WearableObject : public InventoryObject {
protected:
	std::map<voxel,voxel> replacement;		//Original voxel id -> Replacement voxel id
	VModel* oldmod;

	//Game data I/O
	virtual void PushPrivateData(FILE* f);
	virtual bool PullPrivateData(FILE* f);

public:
	WearableObject();
	virtual ~WearableObject();

	void AddReplacement(const voxel old_id, const voxel new_id);
	void AddReplacement(const SVoxelTab* vtab, const char* oldmkr, const char* newmrk);
	void RemoveReplacement(const voxel original_id);

	void ApplyToModel(VModel* mod);
	void RemoveFromModel(VModel* mod);
};

/* Raw voxel storage type */
class VoxelObject : public InventoryObject {
protected:
	voxel stored;

	//Game data I/O
	virtual void PushPrivateData(FILE* f);
	virtual bool PullPrivateData(FILE* f);

public:
	VoxelObject();
	VoxelObject(voxel s);
	virtual ~VoxelObject()								{}

	void SetVoxelId(voxel id)							{ stored = id; }
	voxel GetVoxelId()									{ return stored; }
};

/* AI-relative object */
class AIPartObject : public InventoryObject {
protected:
	AIChip* chip;
	bool controller;

	//Game data I/O
	virtual void PushPrivateData(FILE* f);
	virtual bool PullPrivateData(FILE* f);

public:
	AIPartObject();
	virtual ~AIPartObject();
};

#endif /* INVENTORY_H_ */
