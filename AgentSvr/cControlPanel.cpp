// cControlPanel: The control panel class.

// This file is NOT shared with the main project!

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT

#include "cDDraw.h"
#include "cControlPanel.h"

extern cDDraw *cDD;

/////////////////////////////////////////////////////////////////
// Class Defintion

cControlPanel::cControlPanel(void) 
{
	hwnd_cp = cDD->Hwnd_Main();
}

int cControlPanel::AddItem(cItem* item) { return -1; };
void cControlPanel::DeleteItem(cItem* item, int crap) {};
int cControlPanel::AddNeighbor(cNeighbor* n) { return -1; };
void cControlPanel::DeleteNeighbor(cNeighbor* n, int crap) {};
void cControlPanel::SetSelectedNeighbor(cNeighbor *n) {};
void cControlPanel::SetSelectedItem(cItem *item) {};
void cControlPanel::SetSelectedArt(lyra_id_t art_id) {};
void cControlPanel::DeselectSelected(void) {};
int cControlPanel::AddArt(realmid_t art) { return -1; };
void cControlPanel::DeleteArt(realmid_t art, int crap) {};
void cControlPanel::UpdateStats(void) {};
void cControlPanel::AddAvatar() {};
void cControlPanel::UpdateArt(lyra_id_t art) {};
void cControlPanel::SelectNew(bool crap) {};
void cControlPanel::SetMode(int new_mode, bool art_capture, bool force) {};
int  cControlPanel::Mode(void) {return NO_TAB;};
bool cControlPanel::InventoryFull(void) { return false; };
bool cControlPanel::InventoryEmpty(void) { return false; };
void cControlPanel::StartDrag(cItem *item, int xoffset, int yoffset) {};
void cControlPanel::EndDrag(void) {};
bool cControlPanel::UndrawDrag(unsigned char *buffer) { return true;};
bool cControlPanel::DrawDrag(bool viewport, unsigned char *buffer) {return true;};
void cControlPanel::TurnAvatar(int delta) {};
int  cControlPanel::NumMonsters(void) { return 0;};
cNeighbor* cControlPanel::SelectedNeighbor(void) { return NO_ACTOR;}
cItem* cControlPanel::SelectedItem(void) {return NO_ACTOR;}
lyra_id_t cControlPanel::SelectedArt(void) { return 0; }
void  cControlPanel::ShowNextItem(int count) {}
void  cControlPanel::ShowPrevItem(int count) {}
void  cControlPanel::ShowNextNeighbor(int count) {}
void  cControlPanel::ShowPrevNeighbor(int count) {}
void  cControlPanel::ShowNextArt(int count) {}
void  cControlPanel::ShowPrevArt(int count) {}
void  cControlPanel::DumpInventory(void) {}
void cControlPanel::CheckDragScroll(void) {}



cControlPanel::~cControlPanel(void) {}

#ifdef DEBUG

void cControlPanel::Debug_CheckInvariants(int caller) {}

#endif