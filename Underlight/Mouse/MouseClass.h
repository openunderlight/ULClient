#pragma once
//mouseclass and mouse event class was found at 
// https://www.youtube.com/watch?v=r0t2yMyGJME&list=PLcacUGyBsOIBlGyQQWzp6D1Xn6ZENx9Y2&index=6
// https://www.youtube.com/watch?v=M61AjhVDFS8&list=PLcacUGyBsOIBlGyQQWzp6D1Xn6ZENx9Y2&index=7
// C++ DirectX 11 Engine Tutorial 6 - Mouse Input by jpres
// C++ DirectX 11 Engine Tutorial 7 - Raw Mouse Input by jpres
//implimented by christy wear on 7/8/2021 to improve mouselook

#include "MouseEvent.h" // included to use mouse events

#include <queue> // for mouse event queue

class MouseClass
{
public:
    //functions will be called for window proc
	void OnLeftPressed(int x, int y); //left click
	void OnLeftReleased(int x, int y); //left click let go
	void OnRightPressed(int x, int y); //right click
	void OnRightReleased(int x, int y); //right click let go
	void OnMiddlePressed(int x, int y); //middle click
	void OnMiddleReleased(int x, int y); //middle click let go
	void OnWheelUp(int x, int y); //mouse scroll wheel up
	void OnWheelDown(int x, int y); //mouse scroll wheel down
	void OnMouseMove(int x, int y); // general mouse movement that returns coords
	void OnMouseMoveRaw(int x, int y); //raw direct mouse shows how much which dir was moved.

	//can call these anytime
	//checks if condition is true
	bool IsLeftDown(); //is left mouse button clicked?
	bool IsMiddleDown(); // is middle mouse button clicked?
	bool IsRightDown(); // is right mouse button clicked?

	//can call these anytime
	int GetPosX(); // what is the mouses X pos?
	int GetPosY(); // what is the mouses y pos?
	MousePoint GetPos(); // what is the mouse point containing both x and y?

	bool EventBufferIsEmpty(); // is our event buffer empty no events?
	MouseEvent ReadEvent(); // read the events please

private:
	std::queue<MouseEvent> eventBuffer; //our que of events
	bool leftIsDown = false; //not in use atm
	bool rightIsDown = false; // not in use atm
	bool mbuttonDown = false; // not in use atm
	static inline int x; // our static var
	static inline int y; // our static var
};