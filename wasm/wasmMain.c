
#include <unistd.h>

#include <freerdp/client/rdpgfx.h>
#include <freerdp/gdi/gfx.h>
#include <freerdp/freerdp.h>

#include <FreeRDP/channels/rdpgfx/client/rdpgfx_main.h>
#include <FreeRDP/libfreerdp/core/update.h>

#include "wasm.h"

static freerdp *globalInstance = NULL;

VOID Sleep(DWORD dwMilliseconds)
{
	usleep(dwMilliseconds * 1000);
}

static rdpSettings *initRdpSettings(UINT32 width, UINT32 height)
{
	rdpSettings *settings = (rdpSettings *)malloc(sizeof(rdpSettings));
	if (!settings)
		return NULL;

	settings->DesktopWidth = width;
	settings->DesktopHeight = height;
	settings->ColorDepth = 32;

	settings->GlyphCache = calloc(10, sizeof(GLYPH_CACHE_DEFINITION));
	if (!settings->GlyphCache)
	{
		free(settings);
		return NULL;
	}
	settings->GlyphCache[0].cacheEntries = 254;
	settings->GlyphCache[0].cacheMaximumCellSize = 4;
	settings->GlyphCache[1].cacheEntries = 254;
	settings->GlyphCache[1].cacheMaximumCellSize = 4;
	settings->GlyphCache[2].cacheEntries = 254;
	settings->GlyphCache[2].cacheMaximumCellSize = 8;
	settings->GlyphCache[3].cacheEntries = 254;
	settings->GlyphCache[3].cacheMaximumCellSize = 8;
	settings->GlyphCache[4].cacheEntries = 254;
	settings->GlyphCache[4].cacheMaximumCellSize = 16;
	settings->GlyphCache[5].cacheEntries = 254;
	settings->GlyphCache[5].cacheMaximumCellSize = 32;
	settings->GlyphCache[6].cacheEntries = 254;
	settings->GlyphCache[6].cacheMaximumCellSize = 64;
	settings->GlyphCache[7].cacheEntries = 254;
	settings->GlyphCache[7].cacheMaximumCellSize = 128;
	settings->GlyphCache[8].cacheEntries = 254;
	settings->GlyphCache[8].cacheMaximumCellSize = 256;
	settings->GlyphCache[9].cacheEntries = 64;
	settings->GlyphCache[9].cacheMaximumCellSize = 256;

	settings->PointerCacheSize = 20;

	settings->BitmapCacheV2NumCells = 5;
	settings->BitmapCacheV2CellInfo =
			(BITMAP_CACHE_V2_CELL_INFO *)calloc(6, sizeof(BITMAP_CACHE_V2_CELL_INFO));
	if (!settings->BitmapCacheV2CellInfo)
	{
		free(settings->GlyphCache);
		free(settings);
		return NULL;
	}

	settings->BitmapCacheV2CellInfo[0].numEntries = 600;
	settings->BitmapCacheV2CellInfo[0].persistent = FALSE;
	settings->BitmapCacheV2CellInfo[1].numEntries = 600;
	settings->BitmapCacheV2CellInfo[1].persistent = FALSE;
	settings->BitmapCacheV2CellInfo[2].numEntries = 2048;
	settings->BitmapCacheV2CellInfo[2].persistent = FALSE;
	settings->BitmapCacheV2CellInfo[3].numEntries = 4096;
	settings->BitmapCacheV2CellInfo[3].persistent = FALSE;
	settings->BitmapCacheV2CellInfo[4].numEntries = 2048;
	settings->BitmapCacheV2CellInfo[4].persistent = FALSE;

	settings->OffscreenCacheSize = 7680;
	settings->OffscreenCacheEntries = 2000;

	settings->DrawNineGridCacheSize = 2560;
	settings->DrawNineGridCacheEntries = 256;

	settings->GfxThinClient = TRUE;
	settings->GfxSmallCache = FALSE;
	settings->GfxProgressive = FALSE;
	settings->GfxProgressiveV2 = FALSE;
	settings->GfxH264 = FALSE;
	settings->GfxAVC444 = FALSE;
	settings->GfxSendQoeAck = FALSE;
	settings->GfxCapsFilter = FALSE;

	return settings;
}

static void settingsFree(rdpSettings *settings)
{
	if (!settings)
		return;

	free(settings->BitmapCacheV2CellInfo);
	free(settings->GlyphCache);
	free(settings);
}

static freerdp *initFreerdp()
{
	freerdp *instance;
	instance = (freerdp *)calloc(1, sizeof(freerdp));
	if (!instance)
		return NULL;

	instance->ContextSize = sizeof(rdpContext);
	return instance;
}

static BOOL initRdpContext(freerdp *instance)
{
	wasmContext *wcontext = (wasmContext *)malloc(sizeof(wasmContext));
	if (!wcontext)
		return FALSE;
	rdpContext *context = (rdpContext *)wcontext;

	context->settings = instance->settings;
	context->codecs = codecs_new(context);
	if (!context->codecs)
		goto codecsNewFail;

	if (!freerdp_client_codecs_prepare(context->codecs, FREERDP_CODEC_ALL & ~FREERDP_CODEC_REMOTEFX,
																		 context->settings->DesktopWidth, context->settings->DesktopHeight))
		goto codecsPrepareFail;

	context->pubSub = PubSub_New(TRUE);
	if (!context->pubSub)
		goto pubSubNewFail;

	context->instance = instance;
	instance->context = wcontext;
	instance->ContextSize = sizeof(wasmContext);

	return TRUE;
pubSubNewFail:
codecsPrepareFail:
	codecs_free(context->codecs);
codecsNewFail:
	free(wcontext);
	return FALSE;
}

static void rdpContextFree(rdpContext *context)
{
	if (!context)
		return;

	PubSub_Free(context->pubSub);
	codecs_free(context->codecs);
	free(context);
}

static BOOL wasm_begin_paint(rdpContext *context)
{
	return FALSE;
}
static BOOL wasm_end_paint(rdpContext *context)
{
	INT32 x, y;
	UINT32 w, h;
	wasmContext *wContext = (wasmContext *)context;
	rdpGdi *gdi = context->gdi;

	if (gdi->suppressOutput)
		return TRUE;

	if (gdi->primary->hdc->hwnd->invalid->null)
		return TRUE;

	drawArgs *args = (drawArgs *)wContext->outPtr;
	args->bpp = 4;
	args->x = gdi->primary->hdc->hwnd->invalid->x;
	args->y = gdi->primary->hdc->hwnd->invalid->y;
	args->w = gdi->primary->hdc->hwnd->invalid->w;
	args->h = gdi->primary->hdc->hwnd->invalid->h;
	args->size = args->w * args->h * args->bpp;
	args->buffer = wContext->outPtr + sizeof(drawArgs);

	int dest_pos = 0;
	int dest_line_width = args->w * args->bpp;
	for (int i = args->y; i < args->y + args->h; i++)
	{
		int start_pos = (i * gdi->width * args->bpp) + (args->x * args->bpp);
		BYTE *src = &gdi->primary_buffer[start_pos];
		BYTE *dest = &args->buffer[dest_pos];
		memcpy(dest, src, dest_line_width);
		dest_pos += dest_line_width;
	}

	drawArgs *args2 = (drawArgs *)(wContext->outPtr + sizeof(drawArgs) + args->size);
	args2->size = 0;

	MAIN_THREAD_EM_ASM({
		window.rdpVue &&window.rdpVue.drawFromOutputPtr &&window.rdpVue.drawFromOutputPtr();
	});

	gdi->primary->hdc->hwnd->invalid->null = TRUE;
	gdi->primary->hdc->hwnd->ninvalid = 0;
	return TRUE;
}

BOOL gfx_init(UINT32 width, UINT32 height, BYTE *outPtr, BYTE *inputPtr)
{
	globalInstance = initFreerdp();
	if (!globalInstance)
		goto globalInstanceFail;

	rdpSettings *settings = initRdpSettings(width, height);
	if (!settings)
		goto settingsFail;
	globalInstance->settings = settings;
	settings->instance = globalInstance;

	if (!initRdpContext(globalInstance))
		goto contextNewFail;
	wasmContext *wContext = (wasmContext *)globalInstance->context;
	wContext->intputPtr = inputPtr;
	wContext->outPtr = outPtr;

	globalInstance->context->graphics = graphics_new(globalInstance->context);
	if (!globalInstance->context->graphics)
		goto graphicsNewFail;

	globalInstance->update = update_new(NULL);
	if (!globalInstance->update)
		goto updateNewFail;
	globalInstance->update->context = globalInstance->context;
	globalInstance->context->update = globalInstance->update;
	update_register_server_callbacks(globalInstance->update);
	globalInstance->update->EndPaint = wasm_end_paint;

	if (!gdi_init(globalInstance, PIXEL_FORMAT_RGBX32))
		goto gdiInitFail;

	RdpgfxClientContext *gfx = NULL;
	if (wasm_rdpgfx_init(settings, &gfx))
		goto rdpgfxInitFail;
	if (!gdi_graphics_pipeline_init(globalInstance->context->gdi, gfx))
		goto graphicsPipelineInitFail;

	wContext->uThread = createUpdateThread();
	if (!wContext->uThread)
		goto uThreadFail;
	return TRUE;
uThreadFail:
	gdi_graphics_pipeline_uninit(globalInstance->context->gdi, gfx);
graphicsPipelineInitFail:
	wasm_rdpgfx_free(gfx);
rdpgfxInitFail:
	gdi_free(globalInstance);
gdiInitFail:
	update_free(globalInstance->update);
updateNewFail:
	graphics_free(globalInstance->context->graphics);
graphicsNewFail:
	rdpContextFree(globalInstance->context);
contextNewFail:
	settingsFree(globalInstance->settings);
settingsFail:
	free(globalInstance);
globalInstanceFail:
	return FALSE;
}

void gfx_free()
{
	if (!globalInstance)
		return;
	wasmContext *wContext = (wasmContext *)globalInstance->context;
	updateThreadFree(wContext->uThread);
	if (globalInstance->context && globalInstance->context->gdi)
	{
		RdpgfxClientContext *gfx = globalInstance->context->gdi->gfx;
		gdi_graphics_pipeline_uninit(globalInstance->context->gdi, gfx);
		wasm_rdpgfx_free(gfx);
		gdi_free(globalInstance);
	}
	update_free(globalInstance->update);
	if (globalInstance->context)
	{
		graphics_free(globalInstance->context->graphics);
	}
	rdpContextFree(globalInstance->context);
	settingsFree(globalInstance->settings);
	free(globalInstance);
	globalInstance = NULL;
}

struct _wStream_args
{
	UINT32 length;
	UINT32 pointer;
	UINT32 capacity;
	UINT32 count;
	UINT32 isAllocatedStream;
	UINT32 isOwner;
};
typedef struct _wStream_args wStreamArgs;

static void gfx_surface_update_callback(void *arg)
{
	updateGfxParam *ugp = (updateGfxParam *)arg;
	if (!ugp || !ugp->wContext || !ugp->inputPtr)
		goto StartFail;

	wasmContext *wContext = (wasmContext *)ugp->wContext;
	wStreamArgs *wsa = (wStreamArgs *)ugp->inputPtr;

	wStream *s = Stream_New(ugp->inputPtr + sizeof(wStreamArgs), wsa->length);
	if (!s)
		goto StreamNewFail;
	s->capacity = wsa->capacity;
	s->pointer = s->buffer + wsa->pointer;
	s->count = wsa->count;
	s->isAllocatedStream = wsa->isAllocatedStream;
	s->isOwner = wsa->isOwner;

	rdpGdi *gdi = ((rdpContext *)wContext)->gdi;
	RDPGFX_PLUGIN *gfxPlugin = (RDPGFX_PLUGIN *)gdi->gfx->handle;
	RDPGFX_CHANNEL_CALLBACK *channelCallback =
			(RDPGFX_CHANNEL_CALLBACK *)gfxPlugin->listener_callback->channel_callback;
	channelCallback->iface.OnDataReceived(channelCallback, s);

	Stream_Free(s, FALSE);
StreamNewFail:
StartFail:
	free(ugp->inputPtr);
	free(ugp);
}

BOOL gfx_surface_update(UINT32 length)
{
	if (!globalInstance || !globalInstance->context)
		return FALSE;

	wasmContext *wContext = (wasmContext *)globalInstance->context;
	updateGfxParam *ugp = (updateGfxParam *)malloc(sizeof(updateGfxParam));
	if (!ugp)
		return FALSE;

	ugp->wContext = wContext;
	ugp->inputPtr = (BYTE *)malloc(length);
	if (!ugp->inputPtr)
	{
		free(ugp);
		return FALSE;
	}
	memcpy(ugp->inputPtr, wContext->intputPtr, length);
	if (!addUpdateTask(wContext->uThread, gfx_surface_update_callback, ugp))
	{
		free(ugp->inputPtr);
		free(ugp);
		return FALSE;
	}

	return TRUE;
}
