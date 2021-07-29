#include "MouseEvent.h"


//mouseclass and mouse event class was found at 
// https://www.youtube.com/watch?v=r0t2yMyGJME&list=PLcacUGyBsOIBlGyQQWzp6D1Xn6ZENx9Y2&index=6
// https://www.youtube.com/watch?v=M61AjhVDFS8&list=PLcacUGyBsOIBlGyQQWzp6D1Xn6ZENx9Y2&index=7
// C++ DirectX 11 Engine Tutorial 6 - Mouse Input by jpres
// C++ DirectX 11 Engine Tutorial 7 - Raw Mouse Input by jpres
//implimented by christy wear on 7/8/2021 to improve mouselook



	//default constructor invalid event type
MouseEvent::MouseEvent():type(MouseEvent::EventType::Invalid) 
{
	//event is invalid, setting cords to zero.
	SetPosX(0);
	SetPosY(0);
}

	//constructor takes EventType, x,y then sets them in private variables
MouseEvent::MouseEvent(EventType type, int x, int y):type(type)
{
	//valid cords, assigning them.
	SetPosX(x);
	SetPosY(y);
}

	//returns bool if it's not invalid
	bool MouseEvent::IsValid()
	{
		return this->type != EventType::Invalid;
	}

	// returns the Type of event
	MouseEvent::EventType MouseEvent::GetType()
	{
		return this->type;
	}

	//returns the mouses x and y in a point
	MousePoint MouseEvent::GetPos()
	{
		//make an actual mousepoint so c++ doesnt complain
		MousePoint a;
		//assign each cord of mousepoint
		a.x = GetPosX();
		a.y = GetPosY();
		//return it.
		return a;
	}

	//specifically returns only int value for x
	int MouseEvent::GetPosX()
	{
		return this->x;
	}

	void MouseEvent::SetPosX(int x)
	{
		this->x = x;
	}

	//specifically returns only int value for y
	int MouseEvent::GetPosY()
	{
		return this->y;
	}

	//allows safely(hah hah) setting local var y
	void MouseEvent::SetPosY(int y)
	{
		this->y = y;
	}

	// detects if cords are above or equal to 3 for mouse look
	bool MouseEvent::MouseXorYOverThree()
	{
		// is cords equal,above or below 3?
		if (GetPosX() >= 3 || GetPosX() <= 3 || GetPosY() >= 3 || GetPosY() <= 3)
		{
			//yep , set bool to true
			XorYReady = true;
		}
		else
			//nope set bool to false
			XorYReady = false;
		//throw it back
		return XorYReady;
	}

	//allow manual reset since for whatever reason mouse relative doesnt reset to zero on no input.
	void MouseEvent::MouseXorYBoolSet(bool val)
	{
		XorYReady = val;
	}

	
