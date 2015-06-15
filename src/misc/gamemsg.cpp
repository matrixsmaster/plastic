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

#include "gamemsg.h"
#include "debug.h"

using namespace std;


GameMessages::GameMessages(DataPipe* pipeptr)
{
	pipe = pipeptr;
}

string GameMessages::GetToken(string tok)
{
	if (tok == "PCName") {
		return pcname;
	} else {
		dbg_print("Unknown token '%s'",tok.c_str());
		return "";
	}
}

string GameMessages::Translate(string fmt)
{
	unsigned i,fsm = 0;
	string r,tok;
	char c;

	for (i = 0; i < fmt.size(); i++) {
		c = fmt.at(i);
		switch (fsm) {
		case 0:
			if (c == '%') fsm = 1;
			else r += c;
			break;
		case 1:
			if ((!isalpha(c)) || (c == ' ')) {
				if (tok.empty())
					dbg_print("Empty token in string '%s'\n",fmt.c_str());
				else
					r += GetToken(tok);
				tok.clear();
				fsm = 0;
				i--; //unget current symbol
			} else
				tok += c;
			break;
		default: fsm = 0;
		}
	}

	if (!tok.empty()) r += GetToken(tok);

	return r;
}

string GameMessages::GetMessage(const char* descr)
{
	return GetMessage(string(descr));
}

string GameMessages::GetMessage(string descr)
{
	string fmt = pipe->GetIniDataS(MSGSDBFNAME,descr);
//	dbg_print("Fmt string = '%s'",fmt.c_str());
	fmt = Translate(fmt);
//	dbg_print("Res string = '%s'",fmt.c_str());
	return fmt;
}
