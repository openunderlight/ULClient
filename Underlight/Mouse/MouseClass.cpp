#include "MouseClass.h"


//mouseclass and mouse event class was found at 
// https://www.youtube.com/watch?v=r0t2yMyGJME&list=PLcacUGyBsOIBlGyQQWzp6D1Xn6ZENx9Y2&index=6
// https://www.youtube.com/watch?v=M61AjhVDFS8&list=PLcacUGyBsOIBlGyQQWzp6D1Xn6ZENx9Y2&index=7
// C++ DirectX 11 Engine Tutorial 6 - Mouse Input by jpres
// C++ DirectX 11 Engine Tutorial 7 - Raw Mouse Input by jpres
//implimented by christy wear on 7/8/2021 to improve mouselook

//these functions will be called for window proc

//left click
void MouseClass::OnLeftPressed(int x, int y)
{
	this->leftIsDown = true; //sets private class variable bool leftIsDown to true
	MouseEvent me(MouseEvent::EventType::LPress, x, y); //creates a new MouseEvent called me, to LPress, and passes x y pos of mouse
	this->eventBuffer.push(me); // pushes me MouseEvent onto the eventbuffer queue
}

//left click let go
void MouseClass::OnLeftReleased(int x, int y)
{
	this->leftIsDown = false; // sets private class variable bool leftIsDown to false
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::LRelease, x, y)); //push a MouseEvent onto the eventbuffer saying left mouse released
}

//right click
void MouseClass::OnRightPressed(int x, int y)
{
	this->rightIsDown = true; // sets private class variable rightIsDown to true
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::RPress, x, y)); //push event to buffer, Right click, x, y pos
}

//right click let go
void MouseClass::OnRightReleased(int x, int y)
{
	this->rightIsDown = false; // sets private class variable rightIsDown to false
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::RRelease, x, y)); // adds event to buffer, right mouse button released + xy pos
}

//middle click
void MouseClass::OnMiddlePressed(int x, int y)
{
	this->mbuttonDown = true; // sets private class variable mbuttonDown to true
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::MPress, x, y)); //sends event that middle mouse button pressed at x y 
}

//middle click let go
void MouseClass::OnMiddleReleased(int x, int y)
{
	this->mbuttonDown = false; //sets private class variable mbuttonDown to false
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::MRelease, x, y)); //sends event that middle m button released at x and y
}

//mouse scroll wheel up
void MouseClass::OnWheelUp(int x, int y)
{
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelUp, x, y)); // sends event mouse wheel moved up at x y
}

//mouse scroll wheel down
void MouseClass::OnWheelDown(int x, int y)
{
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelDown, x, y)); // event mouse wheel moved down at x y
}

// general mouse movement that returns coords
void MouseClass::OnMouseMove(int x, int y)
{
	this->x = x; // sets private variable x, to incoming x value
	this->y = y; // sets private variable y, to incoming y value
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::Move, x, y)); // event mouse moved, at x y
}

//raw direct mouse shows how much which dir was moved. //pushes event onto queue to save x y
void MouseClass::OnMouseMoveRaw(int x, int y)
{
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::RAW_MOVE, x, y));
}

//can call these anytime
//checks if condition is true

//is left mouse button clicked?
bool MouseClass::IsLeftDown()
{
	return this->leftIsDown; // returns copy of private bool variable leftIsDown
}

// is middle mouse button clicked?
bool MouseClass::IsMiddleDown()
{
	return this->mbuttonDown; // returns copy of private bool variable mbuttonDown
}

// is right mouse button clicked?
bool MouseClass::IsRightDown()
{
	return this->rightIsDown; // returns copy private bool variable rightIsDown
}

// what is the mouses X pos?
int MouseClass::GetPosX()
{
	return this->x; //returns copy private variable int x pos of mouse
}

// what is the mouses y pos?
int MouseClass::GetPosY()
{
	return this->y; // returns copy private variable int y pos of mouse
}

// what is the mouse point containing both x and y?
MousePoint MouseClass::GetPos()
{
	MousePoint a;
	a.x = GetPosX();
	a.y = GetPosY();
	return a;
	 // returns point struct using copy of private variables x and y
}

// is our event buffer empty no events?
bool MouseClass::EventBufferIsEmpty()
{
	return this->eventBuffer.empty(); // uses function of queue to tell if it's empty no events.
}

// read the events please
MouseEvent MouseClass::ReadEvent()
{
	if (this->eventBuffer.empty())  // if queue is empty
	{
		return MouseEvent(); // return event. We are done here
	}
	else
	{
		MouseEvent e = this->eventBuffer.front(); // get first even from buffer
		this->eventBuffer.pop(); //remove first event from buffer
		return e; // returns this event.
	}
}
