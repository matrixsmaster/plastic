/*
 * plastic.h
 *
 *  Created on: Apr 19, 2015
 *      Author: matrixsmaster
 */

#ifndef PLASTIC_H_
#define PLASTIC_H_

#ifndef BUILDNO
#define BUILDNO 1
#endif

#define VERMAJOR 0
#define VERMINOR 0

#define PRODNAME "Plastic Inquisitor"

#define HELLOSTR  "%s ver.%d.%d build %d\n \
Copyright (C) 2015, The Plastic Team\n \
%s comes with ABSOLUTELY NO WARRANTY.\n \
This is free software, and you are welcome to redistribute it \
under conditions of GNU GPL v2\n"


struct SGameSettings {
	//TODO
	bool use_shell;
};

#define DEFAULT_SETTINGS { \
	true \
}

#endif /* PLASTIC_H_ */
