#pragma once

//mouseclass and mouse event class was found at 
// https://www.youtube.com/watch?v=r0t2yMyGJME&list=PLcacUGyBsOIBlGyQQWzp6D1Xn6ZENx9Y2&index=6
// https://www.youtube.com/watch?v=M61AjhVDFS8&list=PLcacUGyBsOIBlGyQQWzp6D1Xn6ZENx9Y2&index=7
// C++ DirectX 11 Engine Tutorial 6 - Mouse Input by jpres
// C++ DirectX 11 Engine Tutorial 7 - Raw Mouse Input by jpres
//implimented by christy wear on 7/8/2021 to improve mouselook


struct MousePoint //combined cords to make mousepoint
{
	int x;
	int y;
};

class MouseEvent 
{
	public:
	enum class EventType //our event class for mouse
	{
		LPress =0,
		LRelease =1,
		RPress =2,
		RRelease = 3,
		MPress = 4,
		MRelease = 5,
		WheelUp = 6,
		WheelDown = 7,
		Move = 8,
		RAW_MOVE = 9,
		Invalid = 10
	};
private:
	EventType type;
	static inline int x; //static so available anywhere, even store near you // inline so its useful
	static inline int y;//static so available anywhere, even store near you // inline so its useful
	static inline bool XorYReady;//static so available anywhere, even store near you // inline so its useful
public:
	
	MouseEvent(); // default constructor
	MouseEvent(const EventType type, const int x, const int y); //constructor
	bool IsValid(); // check if event is valid
	EventType GetType(); // gets event type
	MousePoint GetPos(); // gets pos struct that contains x and y
	void SetPosX(int x); // sets x
	void SetPosY(int y); // setx y
	int GetPosX(); // gets x
	int GetPosY(); //gets y
	bool MouseXorYOverThree(); //checks for equal or greater/lesser than 3 then returns answer
	void MouseXorYBoolSet(bool val); // manually resets bool
};