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


/* Basic inventory item */
class InventoryObject {
private:
	std::string name,desc;
	int weight,cond,cost;

public:
	InventoryObject();
	virtual ~InventoryObject()		{}

	virtual void SetName(const std::string s)	{ name = s; }
	virtual void SetDesc(const std::string s)	{ desc = s; }
	virtual void SetWeight(const int s)			{ weight = s; }
	virtual void SetCondition(const int s)		{ cond = s; }
	virtual void SetCost(const int s)			{ cost = s; }

	virtual std::string GetName()				{ return name; }
	virtual std::string GetDesc()				{ return desc; }
	virtual int GetWeight()						{ return weight; }
	virtual int GetCondition()					{ return cond; }
	virtual int GetCost()						{ return cost; }

};

/* Storage container for all of the inventory items */
class Inventory {
private:
	std::vector<InventoryObject*> items;

public:
	Inventory();
	virtual ~Inventory();

	///Returns the number of objects contained in the inventory
	int GetNumberItems();

	///Returns an object from the inventory
	InventoryObject * GetInventoryObject(int n);

};

#endif /* INVENTORY_H_ */
