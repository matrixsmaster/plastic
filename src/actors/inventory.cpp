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

#include <string.h>
#include "inventory.h"

using namespace std;

InventoryObject::InventoryObject() : IGData()
{
	name = "";
	desc = "";
	weight = cond = cost = count = 0;
	memset(&boost,0,sizeof(boost));
}

const bool InventoryObject::operator == (const InventoryObject & obj)
{
	return (name == obj.name) && (desc == obj.desc) && (weight == obj.weight)
			&& (cond == obj.cond) && (cost == obj.cost);
}

bool InventoryObject::SerializeToFile(FILE* f)
{
	//TODO
	return false;
}

bool InventoryObject::DeserializeFromFile(FILE* f)
{
	//TODO
	return false;
}

bool InventoryObject::DecCount()
{
	count = (count > 0)? (count-1):0;
	return (count > 0);
}

/* ******************************************************************** */

Inventory::Inventory() : IGData()
{
	//DEBUG:
	InventoryObject* oj = new InventoryObject();
	oj->SetName("Scull of enemy");
	oj->SetDesc("Just a bowl.");
	oj->SetCount(15);
	oj->SetWeight(1);
	oj->SetCondition(5);
	oj->SetCost(15);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Piece of crap");
	oj->SetDesc("Yet another useless piece of crap. But this piece have a long description.");
	oj->SetCount(1);
	oj->SetWeight(12);
	oj->SetCondition(7);
	oj->SetCost(150);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Last");
	oj->SetDesc("The last object. Ever.");
	oj->SetCount(1);
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
	oj->SetCount(1);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Spoon");
	oj->SetDesc("Slow and not very effective weapon..");
	oj->SetCount(1);
	oj->SetWeight(0);
	oj->SetCondition(15);
	oj->SetCost(7);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Chromatic Tuner");
	oj->SetDesc("It's useful for tune your instrument (e.g. the chainsaw)");
	oj->SetCount(1);
	oj->SetWeight(5);
	oj->SetCondition(96);
	oj->SetCost(7);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Chainsaw \"Friendship\"");
	oj->SetDesc("Musical device.");
	oj->SetCount(1);
	oj->SetWeight(5);
	oj->SetCondition(99);
	oj->SetCost(78);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Wheel");
	oj->SetDesc("Circle.");
	oj->SetCount(3);
	oj->SetWeight(5);
	oj->SetCondition(99);
	oj->SetCost(27);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Pitchfork");
	oj->SetDesc("Good use for lifting persons.");
	oj->SetCount(4);
	oj->SetWeight(5);
	oj->SetCondition(99);
	oj->SetCost(45);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Book \"How to kill everyone\"");
	oj->SetDesc("Wish you a good reading.");
	oj->SetCount(7);
	oj->SetWeight(5);
	oj->SetCondition(99);
	oj->SetCost(12);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Telegraph-post");
	oj->SetDesc("It's hard to move, probably..");
	oj->SetCount(1);
	oj->SetWeight(5);
	oj->SetCondition(99);
	oj->SetCost(170);
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Wheel");
	oj->SetDesc("It's just a fucking wheel");
	oj->SetCount(5);
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

bool Inventory::SerializeToFile(FILE* f)
{
	//TODO
	return false;
}

bool Inventory::DeserializeFromFile(FILE* f)
{
	//TODO
	return false;
}

void Inventory::AddObject(InventoryObject* obj)
{
	if (obj) items.push_back(obj);
}

InventoryObject* Inventory::GetInventoryObject(int n)
{
	if (items.empty() || ((int)items.size() <= n)) return NULL;
	return items.at(n);
}

bool Inventory::DestroyObject(InventoryObject* obj)
{
	vector<InventoryObject*>::iterator it;

	for (it = items.begin(); it != items.end(); ++it)
		if ((*it) == obj) {
			if (!(*it)->DecCount())
				items.erase(it);
			return true;
		}

	return false;
}

static int compdef(const void* a, const void* b)
{
	InventoryObject** one = (InventoryObject**)a;
	InventoryObject** two = (InventoryObject**)b;

	if ( ((*one)) < ((*two)) ) return -1;
	if ( ((*one)) > ((*two)) ) return 1;
	return 0;
}

static int compname(const void* a, const void* b)
{
	InventoryObject** one = (InventoryObject**)a;
	InventoryObject** two = (InventoryObject**)b;

	if ( ((*one)->GetName()) < ((*two)->GetName()) ) return -1;
	if ( ((*one)->GetName()) > ((*two)->GetName()) ) return 1;
	return 0;
}

static int compwght(const void* a, const void* b)
{
	InventoryObject** one = (InventoryObject**)a;
	InventoryObject** two = (InventoryObject**)b;

	if ( ((*one)->GetWeight()) < ((*two)->GetWeight()) ) return -1;
	if ( ((*one)->GetWeight()) > ((*two)->GetWeight()) ) return 1;
	return 0;
}

static int compcnt(const void* a, const void* b)
{
	InventoryObject** one = (InventoryObject**)a;
	InventoryObject** two = (InventoryObject**)b;

	if ( ((*one)->GetCount()) < ((*two)->GetCount()) ) return -1;
	if ( ((*one)->GetCount()) > ((*two)->GetCount()) ) return 1;
	return 0;
}

static int compcst(const void* a, const void* b)
{
	InventoryObject** one = (InventoryObject**)a;
	InventoryObject** two = (InventoryObject**)b;

	if ( ((*one)->GetCost()) < ((*two)->GetCost()) ) return -1;
	if ( ((*one)->GetCost()) > ((*two)->GetCost()) ) return 1;
	return 0;
}


void Inventory::SortDefault()
{
	if (items.empty()) return;
	qsort(&items[0], items.size(), sizeof(InventoryObject*), compdef);
}

void Inventory::SortByName()
{
	if (items.empty()) return;
	qsort(&items[0], items.size(), sizeof(InventoryObject*), compname);
}

void Inventory::SortByWeight()
{
	if (items.empty()) return;
	qsort(&items[0], items.size(), sizeof(InventoryObject*), compwght);
}

void Inventory::SortByCount()
{
	if (items.empty()) return;
	qsort(&items[0], items.size(), sizeof(InventoryObject*), compcnt);
}

void Inventory::SortByCost()
{
	if (items.empty()) return;
	qsort(&items[0], items.size(), sizeof(InventoryObject*), compcst);
}
