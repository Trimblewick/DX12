#pragma once

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

#ifdef NDEBUG
#ifdef _WIN32
#include <Windows.h>
#include <tchar.h>
#define DxAssert(x, y)		{ if (x != y) { MessageBox(NULL, _T("DxAssert"), _T("ERROR"), MB_OK); } }
#else
#define DxAssert(x, y) { x }
#endif
#else
#include <assert.h>
#ifdef _WIN32
#include <Windows.h>
#include <tchar.h>
#define DxAssert(x, y)		{ if (x != y) { MessageBox(NULL, _T("DxAssert"), _T("ERROR"), MB_OK); assert(x == y); } }
#else
#define DxAssert(x, y)		{ assert(x == y) }
#endif
#endif