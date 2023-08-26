cmake_minimum_required (VERSION 2.8)
 
project (freerdp_gfx)

set(CMAKE_C_COMPILER emcc)
# set(CMAKE_CXX_COMPILER em++)

add_definitions(
    -DHAVE_UNISTD_H
    # -D_GNU_SOURCE
    # -DEMCC_DEBUG=1
    -DWITH_POSIX_TIMER
    -DHAVE_PTHREAD_MUTEX_TIMEDLOCK 
    -sUSE_PTHREADS=1
    # -fsanitize=undefined  -gsource-map
    -msimd128 -D__SSE3__ -D__SSSE3__ -D__SSE2__ -D__SSE__ -DWITH_SSE2
    -DFORCE_PRIMITIVES_ONLY_CPU)

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")
link_libraries(m)

include_directories(./)
include_directories(wasm/Freerdp/include)
include_directories(wasm/Freerdp/libfreerdp/core)
include_directories(wasm/Freerdp/winpr/include/)
include_directories(FreeRDP/channels/rdpgfx)
include_directories(FreeRDP/winpr/include)
include_directories(FreeRDP/include)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/FreeRDP/winpr/include/winpr/wtypes.h.in ${CMAKE_CURRENT_BINARY_DIR}/FreeRDP/winpr/include/winpr/wtypes.h)
include_directories(${CMAKE_BINARY_DIR}/FreeRDP/winpr/include)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/FreeRDP/include/freerdp/build-config.h.in ${CMAKE_CURRENT_BINARY_DIR}/FreeRDP/include/freerdp/build-config.h)
include_directories(${CMAKE_BINARY_DIR}/FreeRDP/include)

set(SRC_FILE 
    wasm/updateThread.c
    wasm/wlog.c
    FreeRDP/libfreerdp/core/update.c
    FreeRDP/channels/rdpgfx/client/rdpgfx_main.c
    FreeRDP/libfreerdp/codec/progressive.c
    FreeRDP/libfreerdp/codec/rfx.c
    FreeRDP/libfreerdp/primitives/primitives.c
    FreeRDP/winpr/libwinpr/pool/pool.c
    FreeRDP/winpr/libwinpr/pool/work.c
    FreeRDP/winpr/libwinpr/sysinfo/sysinfo.c
    
    FreeRDP/libfreerdp/core/codecs.c
    
    FreeRDP/libfreerdp/codec/rfx_quantization.c
    FreeRDP/libfreerdp/codec/rfx_dwt.c
    FreeRDP/libfreerdp/codec/rfx_rlgr.c
    FreeRDP/libfreerdp/codec/rfx_decode.c
    FreeRDP/libfreerdp/codec/rfx_encode.c

    FreeRDP/libfreerdp/codec/interleaved.c
    FreeRDP/libfreerdp/codec/bitmap.c

    FreeRDP/libfreerdp/codec/clear.c


    FreeRDP/libfreerdp/codec/planar.c

    FreeRDP/libfreerdp/codec/nsc.c
    FreeRDP/libfreerdp/codec/nsc_encode.c

    FreeRDP/winpr/libwinpr/registry/registry.c
    FreeRDP/winpr/libwinpr/registry/registry_reg.c

    FreeRDP/libfreerdp/primitives/prim_add.c
    FreeRDP/libfreerdp/primitives/prim_andor.c
    FreeRDP/libfreerdp/primitives/prim_alphaComp.c
    FreeRDP/libfreerdp/primitives/prim_copy.c
    FreeRDP/libfreerdp/primitives/prim_set.c
    FreeRDP/libfreerdp/primitives/prim_shift.c
    FreeRDP/libfreerdp/primitives/prim_sign.c
    FreeRDP/libfreerdp/primitives/prim_colors.c
    FreeRDP/libfreerdp/primitives/prim_YCoCg.c
    FreeRDP/libfreerdp/primitives/prim_YUV.c

    
    FreeRDP/libfreerdp/codec/region.c
    FreeRDP/libfreerdp/gdi/region.c

    FreeRDP/winpr/libwinpr/crt/alignment.c
    FreeRDP/winpr/libwinpr/utils/stream.c
    
    
    FreeRDP/winpr/libwinpr/crt/unicode.c
    FreeRDP/winpr/libwinpr/crt/string.c
    FreeRDP/winpr/libwinpr/crt/utf.c
    
    FreeRDP/winpr/libwinpr/error/error.c
    FreeRDP/winpr/libwinpr/nt/nt.c
    
    FreeRDP/winpr/libwinpr/utils/collections/ObjectPool.c
    FreeRDP/winpr/libwinpr/utils/collections/BufferPool.c
    FreeRDP/winpr/libwinpr/utils/collections/HashTable.c
    FreeRDP/winpr/libwinpr/utils/collections/PubSub.c
    FreeRDP/winpr/libwinpr/utils/collections/StreamPool.c
    FreeRDP/winpr/libwinpr/utils/collections/ListDictionary.c
    FreeRDP/winpr/libwinpr/utils/collections/Queue.c
    FreeRDP/winpr/libwinpr/utils/collections/CountdownEvent.c
    FreeRDP/winpr/libwinpr/utils/collections/ArrayList.c
    FreeRDP/winpr/libwinpr/utils/collections/CountdownEvent.c
    
    FreeRDP/winpr/libwinpr/crypto/rand.c
    
    FreeRDP/winpr/libwinpr/file/file.c
    
    FreeRDP/winpr/libwinpr/synch/init.c
    FreeRDP/winpr/libwinpr/interlocked/interlocked.c
    
    FreeRDP/winpr/libwinpr/utils/collections/BitStream.c

    FreeRDP/winpr/libwinpr/utils/trio/trio.c
    FreeRDP/winpr/libwinpr/utils/trio/trionan.c
    FreeRDP/winpr/libwinpr/utils/trio/triostr.c
    
    FreeRDP/libfreerdp/codec/color.c
    
    FreeRDP/libfreerdp/gdi/gfx.c
    FreeRDP/channels/rdpgfx/rdpgfx_common.c
    FreeRDP/channels/rdpgfx/client/rdpgfx_codec.c
    FreeRDP/libfreerdp/codec/zgfx.c

    FreeRDP/libfreerdp/gdi/gdi.c
    FreeRDP/libfreerdp/gdi/dc.c
    FreeRDP/libfreerdp/gdi/bitmap.c
    FreeRDP/libfreerdp/gdi/graphics.c
    FreeRDP/libfreerdp/core/graphics.c
    FreeRDP/libfreerdp/gdi/clipping.c
    FreeRDP/libfreerdp/gdi/drawing.c
    FreeRDP/libfreerdp/gdi/brush.c
    FreeRDP/libfreerdp/gdi/shape.c
    FreeRDP/libfreerdp/gdi/pen.c
    FreeRDP/libfreerdp/gdi/line.c
    FreeRDP/libfreerdp/cache/cache.c
    FreeRDP/libfreerdp/cache/glyph.c
    FreeRDP/libfreerdp/cache/brush.c
    FreeRDP/libfreerdp/cache/bitmap.c
    FreeRDP/libfreerdp/cache/offscreen.c
    FreeRDP/libfreerdp/cache/palette.c
    FreeRDP/libfreerdp/cache/pointer.c
    FreeRDP/libfreerdp/cache/nine_grid.c

    FreeRDP/libfreerdp/core/orders.c
    FreeRDP/libfreerdp/core/window.c

    FreeRDP/libfreerdp/primitives/prim_add_opt.c
    FreeRDP/libfreerdp/primitives/prim_andor_opt.c
    FreeRDP/libfreerdp/primitives/prim_alphaComp_opt.c
    FreeRDP/libfreerdp/primitives/prim_set_opt.c
    FreeRDP/libfreerdp/primitives/prim_shift_opt.c
    FreeRDP/libfreerdp/primitives/prim_sign_opt.c
    FreeRDP/libfreerdp/primitives/prim_colors_opt.c
    FreeRDP/libfreerdp/primitives/prim_YCoCg_opt.c
    FreeRDP/libfreerdp/primitives/prim_YUV_ssse3.c
    FreeRDP/libfreerdp/codec/nsc_sse2.c
    FreeRDP/libfreerdp/codec/rfx_sse2.c

    FreeRDP/winpr/libwinpr/synch/critical.c
    FreeRDP/winpr/libwinpr/synch/event.c
    FreeRDP/winpr/libwinpr/synch/wait.c
    FreeRDP/winpr/libwinpr/synch/pollset.c
    FreeRDP/winpr/libwinpr/utils/debug.c

    FreeRDP/winpr/libwinpr/thread/thread.c
    FreeRDP/winpr/libwinpr/thread/apc.c
    FreeRDP/winpr/libwinpr/thread/tls.c

    FreeRDP/winpr/libwinpr/handle/handle.c)

add_executable(freerdp_gfx.js wasm/wasmMain.c ${SRC_FILE})

target_link_options(freerdp_gfx.js PRIVATE
-pthread
# -fsanitize=undefined -gsource-map
# -sSSERTIONS
-sPTHREAD_POOL_SIZE=5
-sINITIAL_MEMORY=2gb
-sEXPORTED_FUNCTIONS=['_malloc','_free','_gfx_init','_gfx_surface_update','_gfx_free']
-sEXPORTED_RUNTIME_METHODS=ccall)
