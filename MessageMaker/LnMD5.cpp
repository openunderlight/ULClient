// LnMD5.cpp  -*- C++ -*-
// Copyright 2003 Lyra LLC, All rights reserved.
//
// implementation of Lyra's MD5 class


#include "LyraDefs.h"
#include "md5global.h"
#include "md5.h"
#include "LnMD5.h"
#include <stdio.h>

LnMD5::LnMD5()
{
	ctx_ = new MD5_CTX();
	Clear();
}

LnMD5::~LnMD5()
{
	if(ctx_)
		delete ctx_;
}

void LnMD5::Clear()
{
	MD5Init(ctx_);
}

void LnMD5::Update(void* data, size_t len)
{
	MD5Update(ctx_, (unsigned char*)data, len);
}

void LnMD5::Final(MD5Hash_t& hash)
{
	MD5Final(hash, ctx_);
	Clear();
}

void LnMD5::Dump(TCHAR* buffer)
{
	_stprintf(buffer, "state: %d count: %d buffer len: %d buffer: %s\n",
		ctx_->state, ctx_->count, ctx_->buffer);
	return;
}

