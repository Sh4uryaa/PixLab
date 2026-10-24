/*
Optional browser integration after compiling pixlab_wasm.cpp to pixlab_wasm.js.
This function accepts an ImageData object and filter name; it mutates its RGBA bytes.
*/
export function applyPixLabFilter(Module, imageData, filterName) {
  const ids = { grayscale: 0, sepia: 1, reflect: 2, blur: 3 };
  if (!(filterName in ids)) throw new Error(`Unknown PixLab filter: ${filterName}`);

  const bytes = imageData.data;
  const ptr = Module._malloc(bytes.length);
  if (!ptr) throw new Error("Unable to allocate WebAssembly memory.");

  try {
    Module.HEAPU8.set(bytes, ptr);
    const ok = Module._pixlab_apply_filter(ptr, imageData.width, imageData.height, ids[filterName]);
    if (ok !== 1) throw new Error("PixLab filter failed.");
    imageData.data.set(Module.HEAPU8.subarray(ptr, ptr + bytes.length));
    return imageData;
  } finally {
    Module._free(ptr);
  }
}
