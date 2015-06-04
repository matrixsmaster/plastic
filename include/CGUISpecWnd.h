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

/* Special kinds of CurseGUI windows are defined here */

#ifndef INCLUDE_CGUISPECWND_H_
#define INCLUDE_CGUISPECWND_H_

#include <string>
#include <vector>
#include "CurseGUI.h"
#include "CGUIControls.h"

#include <pthread.h>
#include "vecmath.h"
#include "LVRconsts.h"


/* ********************************** Message Box ********************************** */

//percent of maximum coverage
#define MSGBOXSIZEX 50

class CurseGUIMessageBox : public CurseGUIWnd {
private:
	int result;

	void MoveToCenter();

public:
	CurseGUIMessageBox(CurseGUI* scrn, const char* title, const char* text, const char* butns);
	virtual ~CurseGUIMessageBox()			{}

	bool PutEvent(SGUIEvent* e);

	int GetButtonPressed();
};

/* ********************************** Debug Console ********************************** */

class CurseGUIDebugWnd : public CurseGUIWnd {
private:
	std::vector<std::string> log;
	bool hidden;
	int key;
	bool edit;
	std::string edit_line;
	pthread_mutex_t wmutex;
	bool scrlk;
	size_t locked;

	void ResizeWnd();

public:
	CurseGUIDebugWnd(CurseGUI* scrn);
	virtual ~CurseGUIDebugWnd();

	//Debug console is always focused if shown.
	bool GainFocus()					{ return focused; }
	bool LooseFocus()					{ return hidden; }

	void ToggleShow();

	void Update(bool refr);
	bool PutEvent(SGUIEvent* e);

	void PutString(char* str);
	void PutString(std::string str);
	//TODO: add ability to dump log to text file
};

/* ********************************** Inventory window ********************************** */

//percent of coverage:
#define INVENTSIZEX 70
#define INVENTSIZEY 70

class Inventory;

class CurseGUIInventoryWnd : public CurseGUIWnd {
private:
	Inventory* invent;
	CurseGUITable* table;
	CurseGUILabel* description_lbl;
	CurseGUIProgrBar* prgr_bar;

	CurseGUIButton* destroy_btn;
	CurseGUIButton* drop_btn;
	CurseGUIButton* wear_btn;
	CurseGUIButton* use_btn;
	CurseGUIButton* repair_btn;

	CurseGUIEditBox* search_edit;

	int sitem; //selected item in inventory
	int prev;


	void ResizeWnd();

	void FillInventoryTable();

	void SetSelectedItem();

	///Show item description
	void ShowDescription();

	///Convert int to std::string
	std::string IntToString(int v);

public:
	CurseGUIInventoryWnd(CurseGUI* scrn, Inventory* iptr);
	virtual ~CurseGUIInventoryWnd()		{}

	bool PutEvent(SGUIEvent* e);
};

/* ********************************** Map View window ********************************** */

//percent of coverage:
#define MAPVIEWSIZEX 80
#define MAPVIEWSIZEY 75
#define MAPVIEWHIGHM 1.25
#define MAPVIEWLOWM 0.75
#define MAPVIEWRULX 5
#define MAPVIEWRULY 3
#define MAPVIEWRULSTR 16
#define MAPVIEWBOTLINSTR 129

class DataPipe;

class CurseGUIMapViewWnd : public CurseGUIWnd {
private:
	DataPipe* pipe;
	int scale;
	vector2di base;
	vector3di gpos,lpos;
	bool showelev;
	int m_w, m_h;

	void ResizeWnd();
	void DrawMap();

public:
	CurseGUIMapViewWnd(CurseGUI* scrn, DataPipe* pdat);
	virtual ~CurseGUIMapViewWnd()		{}

	void SetPos(const vector3di glob, const vector3di loc);

	bool PutEvent(SGUIEvent* e);
};

/* ********************************** Renderer Config window ********************************** */

class RenderPool;

class CurseGUIRenderConfWnd : public CurseGUIWnd {
private:
	RenderPool* pool;
	float scale;
	vector3d fov;
	int far;
	SLVRPostProcess ppset;
	CurseGUIEditBox* e_scale;
	CurseGUIEditBox* e_fovx,*e_fovy;
	CurseGUIEditBox* e_far;
	CurseGUIEditBox* e_fog;
	CurseGUIEditBox* e_fogr,*e_fogg,*e_fogb;
	CurseGUIEditBox* e_noise;
	CurseGUIButton* b_apply,*b_reset;

	void Fill();
	void Scan();
	void Apply();
	void Reset();

public:
	CurseGUIRenderConfWnd(CurseGUI* scrn, RenderPool* ppool);
	virtual ~CurseGUIRenderConfWnd()	{}

	bool PutEvent(SGUIEvent* e);
};

/* ********************************** Actor View window ********************************** */

//Minimal width of Actor View window
#define ACTRVIEWMINW 50

class PlasticActor;

class CurseGUIActorViewWnd : public CurseGUIWnd {
private:
	PlasticActor* actor;
	CurseGUIPicture* portrait;

	void ResizeWnd();

public:
	CurseGUIActorViewWnd(CurseGUI* scrn, PlasticActor* actr);
	virtual ~CurseGUIActorViewWnd()		{}

	bool PutEvent(SGUIEvent* e);
};

#endif /* INCLUDE_CGUISPECWND_H_ */
