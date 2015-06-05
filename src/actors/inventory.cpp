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

#include "inventory.h"

using namespace std;

InventoryObject::InventoryObject()
{
	name = "";
	desc = "";
	weight = cond = cost = 0;
}


Inventory::Inventory()
{
	//DEBUG:
	InventoryObject* oj = new InventoryObject();
	oj->SetName("First object");
	oj->SetDesc("Just an object.");
	oj->SetWeight(1);
	oj->SetCondition(5);
	oj->SetCost(15);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Second object");
	oj->SetDesc("Yet another useless piece of crap. But this piece have a long description.");
	oj->SetWeight(12);
	oj->SetCondition(7);
	oj->SetCost(150);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Last");
	oj->SetDesc("The last object.");
	oj->SetWeight(5);
	oj->SetCondition(99);
	oj->SetCost(7);
	items.push_back(oj);
}

Inventory::~Inventory()
{
	vector<InventoryObject*>::iterator it;

	if (!items.empty()) {
		for (it = items.begin(); it != items.end(); ++it)
			delete (*it);
		items.clear();
	}
}

int Inventory::GetNumberItems()
{
	return items.size();
}

/*
 * FIXME: that's truly horrible. I'm fucking scared to bones.
 * What is this stuff for? Are you goin' to provide access to every field
 * of an inventory object?! Why not just return the object's pointer?
 * Besides, all of the conditions are not met by signedness.
 */

string Inventory::GetName(int n)
{
	if (items.size() > n) return items.at(n)->GetName();
	return "Unknown";
}

string Inventory::GetDesc(int n)
{
	if (int(items.size()) > n) return items.at(n)->GetDesc();
	return "Unknown";
}

int Inventory::GetWeight(int n)
{
	if (items.size() > n) return items.at(n)->GetWeight();
	return 0;
}
int Inventory::GetCondition(int n)
{
	if (items.size() > n) return items.at(n)->GetConditions();
	return 0;
}
int Inventory::GetCost(int n)
{
	if (items.size() > n) return items.at(n)->GetCost();
	return 0;
}
