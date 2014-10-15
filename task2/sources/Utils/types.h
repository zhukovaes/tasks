/**
 * @file: types.h 
 * Definition of simple types used in implementation
 */
/*
 * Utils library
 * Copyright (C) 2012  Boris Shurygin
 */
#pragma once

#ifndef TYPES_H
#define TYPES_H

/*
  Size-dependent types (architechture-dependent byte order)
 */
typedef signed char Int8;         /* 8 bit signed */
typedef unsigned char UInt8;      /* 8 bit unsigned */
typedef short Int16;              /* 16 bit signed */
typedef unsigned short UInt16;    /* 16 bit unsigned */
typedef int Int32;                /* 32 bit signed */
typedef unsigned int UInt32;      /* 32 bit unsigned */
typedef float Float;              /* Single-precision floating point value */
typedef double Double;            /* Double-precision floating point value */
#if defined(OS_WIN32) && !defined(CC_GNU)
typedef __int64 Int64;            /* 64 bit signed */
typedef unsigned __int64 UInt64;  /* 64 bit unsigned */
#else
typedef long long Int64;           /* 64 bit signed */
typedef unsigned long long UInt64; /* 64 bit unsigned */
#endif


#endif /* TYPES_H */
