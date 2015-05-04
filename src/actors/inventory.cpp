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


Inventory::Inventory()
{
	//DEBUG:
	InventoryObject* oj = new InventoryObject();
	oj->SetName("First");
	oj->SetDesc("Just an object.");
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Second");
	oj->SetDesc("Yet another useless piece of crap. But this piece have a long description.");
	items.push_back(oj);
	oj = new InventoryObject();
	oj->SetName("Last");
	oj->SetDesc("The last object.");
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
