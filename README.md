# freerdp2-wasm-gfx

从freerdp的generator_emit_gfx_OnDataReceived拷贝参数wStream发送到网页，网页调用wasm解码。

copy freerdp generator_emit_gfx_OnDataReceived param wStream send to web, web use wasm decode.
```shell
git clone --recurse-submodules https://github.com/wowuyv/freerdp2-wasm-gfx.git
```

#### compiler
<https://emscripten.org/index.html>

#### api
```c
BOOL gfx_init(UINT32 width, UINT32 height, BYTE *outPtr, BYTE *inputPtr);
void gfx_free();
BOOL gfx_surface_update(UINT32 length);
```

#### web draw
```c
MAIN_THREAD_EM_ASM({
  window.rdpVue &&window.rdpVue.drawFromOutputPtr &&window.rdpVue.drawFromOutputPtr();
});
```
#### javascript invocation
```javascript
initGfx(width, height) {
  const Module = window.Module

  this.ptrLength = width * height * 4 + 28 * 20
  this.inputPtr = Module._malloc(this.ptrLength)
  this.inputHeap = new Uint8Array(Module.HEAPU8.buffer, this.inputPtr, this.ptrLength)
  this.outputPtr = Module._malloc(this.ptrLength)
  this.outputHeap = new Uint8Array(Module.HEAPU8.buffer, this.outputPtr, this.ptrLength)

  Module.ccall('gfx_init',
    'number',
    ['number', 'number', 'number', 'number'],
    [width, height, this.outputHeap.byteOffset, this.inputHeap.byteOffset]
  )
},
updateGfx(gfxData) {
  const Module = window.Module
  this.inputHeap.set(new Uint8Array(gfxData))
  Module.ccall('gfx_surface_update',
    'number',
    ['number'],
    [gfxData.byteLength]
  )
}
drawFromOutputPtr() {
  const buf = new Uint32Array(this.outputHeap.buffer, this.outputHeap.byteOffset, 28)
  const drawArgs = {
    x: buf[0],
    y: buf[1],
    w: buf[2],
    h: buf[3],
    bpp: buf[4],
    size: buf[5],
    data: null
  }
  if (drawArgs.size > 0) {
    drawArgs.data = new Uint8ClampedArray(this.outputHeap.buffer, this.outputHeap.byteOffset + 28, drawArgs.size)
    const imageData = this.ctx.createImageData(drawArgs.w, drawArgs.h)
    imageData.data.set(drawArgs.data)
    this.ctx.putImageData(imageData, drawArgs.x, drawArgs.y)
  }
}
```
