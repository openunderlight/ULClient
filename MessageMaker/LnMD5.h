#ifndef _LnMD5_H_
#define _LnMD5_H_

#include "md5global.h"
#include "md5.h"

#define MD5HASHLEN	16
typedef unsigned char MD5Hash_t[MD5HASHLEN];
struct MD5_CTX_s;

class LnMD5
{
public:
	LnMD5();
	~LnMD5();

	void Clear();
	void Update(void* data, size_t len);
	void Final(MD5Hash_t& hash);
	void Dump(TCHAR* buffer);

protected:
	MD5_CTX_s*	ctx_;
};

#endif // _LnMD5_H
