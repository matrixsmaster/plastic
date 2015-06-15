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

#ifndef GAMEMSG_H_
#define GAMEMSG_H_

#include <string>
#include "datapipe.h"

class GameMessages {
private:
	/* core */
	DataPipe* pipe;

	/* current values */
	std::string pcname;

	///Resolves format token.
	std::string GetToken(std::string tok);

	///Parses format string and returns compiled result.
	std::string Translate(std::string fmt);

public:
	GameMessages(DataPipe* pipeptr);
	virtual ~GameMessages()						{}

	void SetPCName(const char* name)			{ pcname = std::string(name); }
	//TODO: append more

	std::string GetMessage(const char* descr);
	std::string GetMessage(std::string descr);
};

#endif /* GAMEMSG_H_ */
