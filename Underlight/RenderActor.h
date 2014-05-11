// Copyright Lyra LLC, 1997. All rights reserved.
// Header file for actor related rendering

// Note : Used internally by the Graphics Engine. 

#ifndef ACTOR_RENDERER_H
#define ACTOR_RENDERER_H

///////////////////////////////////////////////////////////
// Class, data types
class cActor;

enum eClipTestResult 
{
	DISJOINT,
	CONTAINED,
	CONTAINS,
	START_OVERLAP,
	END_OVERLAP
};


class cColumnClipper  // Performs actor based clipping
{
private:

	struct column_span 
	{
		short start; 
		short end;   // rows
	} *spans;


public:

	void init(int view_width);
	~cColumnClipper();
	void frame_reset(void);
	void add_span(int col,int start,int end);
	eClipTestResult test_span(int col,int start,int end);
	eClipTestResult test_span_and_clip(int col,int &start,int &end);
	inline short span_start(int col) { return spans[col].start; };
	inline short span_end(int col)   { return spans[col].end;    };

	bool scanline_hidden(int row, int start, int end);


};


extern cColumnClipper column_clipper;

/////////////////////////////////////////////////////////////////////
// Function prototypes

void InitRenderActor(void);
void DeInitRenderActor(void);

void StartRenderActor(UCHAR *viewBuffer, int pitch, cPlayer *player);
void EndRenderActor(void);

bool render_actor(cActor *actor);
bool point_in_actor(cActor *actor, int row, int col);

bool test_span_and_clip(int row, short &start, short &end);  

#endif