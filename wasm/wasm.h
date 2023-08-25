
#ifndef __WASM_H__
#define __WASM_H__

#include <stdio.h>
#include <string.h>

#include <emscripten.h>

#include <winpr/wtypes.h>
#include <winpr/pool.h>

#include <freerdp/freerdp.h>

#include "updateThread.h"

struct wasm_context
{
	rdpContext context;
	BYTE *outPtr;
	BYTE *intputPtr;
	UpdateThread *uThread;
};

typedef struct wasm_context wasmContext;

struct update_gfx_param
{
	BYTE *inputPtr;
	wasmContext *wContext;
};
typedef struct update_gfx_param updateGfxParam;

struct draw_args
{
	UINT32 x;
	UINT32 y;
	UINT32 w;
	UINT32 h;
	UINT32 bpp;
	UINT32 size;
	BYTE *buffer;
};
typedef struct draw_args drawArgs;

#endif