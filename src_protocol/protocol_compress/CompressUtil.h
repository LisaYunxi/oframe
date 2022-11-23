// ***************************************************************
//  CompressUtil   version:  1.0   ¡¤  date: 06/20/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************
#pragma once

#ifndef _COMPRESSUTIL_H_
#define _COMPRESSUTIL_H_

class CompressUtil
{
public:
	CompressUtil(void);
public:
	virtual ~CompressUtil(void);
public:
	//compress function
	static int Zerocompress(unsigned char const *inputBuffer,unsigned long inputNumBytes,unsigned char *outputBuffer, unsigned long &outputNumBytes);
	//decompress function
	static int Zerodecompress(unsigned char const *inputBuffer,unsigned long inputNumBytes,unsigned char *outputBuffer, unsigned long &outputNumBytes);

};

#endif
