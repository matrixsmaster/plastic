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
	oj->SetName("Scull of enemy");
	oj->SetDesc("Just a bowl.");
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
	oj->SetDesc("The last object. Ever.");
	oj->SetWeight(5);
	oj->SetCondition(99);
	oj->SetCost(7);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Shovel");
	oj->SetDesc("It's just a shovel. It's a multifunctional instrument.");
	oj->SetWeight(7);
	oj->SetCondition(70);
	oj->SetCost(1);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Spoon");
	oj->SetDesc("Slow and not very effective weapon..");
	oj->SetWeight(0);
	oj->SetCondition(15);
	oj->SetCost(7);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Chromatic Tuner");
	oj->SetDesc("It's useful for tune your instrument (e.g. the chainsaw)");
	oj->SetWeight(5);
	oj->SetCondition(99);
	oj->SetCost(7);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Chainsaw \"Friendship\"");
	oj->SetDesc("Musical device.");
	oj->SetWeight(5);
	oj->SetCondition(99);
	oj->SetCost(7);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Wheel");
	oj->SetDesc("Circle.");
	oj->SetWeight(5);
	oj->SetCondition(99);
	oj->SetCost(7);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Pitchfork");
	oj->SetDesc("Good use for lifting persons.");
	oj->SetWeight(5);
	oj->SetCondition(99);
	oj->SetCost(7);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Book \"How to kill everyone\"");
	oj->SetDesc("Wish you a good reading.");
	oj->SetWeight(5);
	oj->SetCondition(99);
	oj->SetCost(7);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Telegraph-post");
	oj->SetDesc("It's hard to move, probably..");
	oj->SetWeight(5);
	oj->SetCondition(99);
	oj->SetCost(7);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Wheel");
	oj->SetDesc("Fuck");
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

InventoryObject* Inventory::GetInventoryObject(int n)
{
	if (items.empty()) return NULL;
	return items.at(n);
}
