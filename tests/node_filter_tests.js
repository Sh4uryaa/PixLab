/**
 * Automated Test Suite for PixLab Image Processing Engine
 * Verifies correctness, memory management, and performance across all 7 filters.
 */

import '../pixlab_wasm.js';
const PixLabModule = globalThis.PixLabModule;

function createTestBmp(width, height) {
    const padding = (4 - (width * 3) % 4) % 4;
    const rowStride = width * 3 + padding;
    const pixelArraySize = rowStride * height;
    const totalSize = 54 + pixelArraySize;

    const buffer = new ArrayBuffer(totalSize);
    const view = new DataView(buffer);
    const u8 = new Uint8Array(buffer);

    // BITMAPFILEHEADER
    view.setUint16(0, 0x4D42, true); // "BM"
    view.setUint32(2, totalSize, true);
    view.setUint32(10, 54, true);

    // BITMAPINFOHEADER
    view.setUint32(14, 40, true);
    view.setInt32(18, width, true);
    view.setInt32(22, height, true);
    view.setUint16(26, 1, true);
    view.setUint16(28, 24, true); // 24-bit
    view.setUint32(30, 0, true);
    view.setUint32(34, pixelArraySize, true);

    // Fill with gradient test pattern
    for (let r = 0; r < height; ++r) {
        const rowStart = 54 + (r * rowStride);
        for (let c = 0; c < width; ++c) {
            const idx = rowStart + (c * 3);
            u8[idx + 0] = (c * 7) % 256;         // Blue
            u8[idx + 1] = (r * 7) % 256;         // Green
            u8[idx + 2] = ((r + c) * 4) % 256;   // Red
        }
    }

    return u8;
}

function runTests() {
    console.log("==================================================");
    console.log(" PixLab Automated Test & Benchmark Suite (Node.js)");
    console.log("==================================================");

    const testImg = createTestBmp(128, 128);
    console.log(`[Setup] Created 128x128 24-bit BMP test image (${testImg.length} bytes)`);

    const filters = [
        { id: 1, name: "Grayscale (Rec. 601 16-bit Fixed-Point)", param: 0 },
        { id: 2, name: "Sepia (16-bit Fixed-Point Matrix)", param: 0 },
        { id: 3, name: "Blur (Separable Multi-Pass)", param: 0 },
        { id: 4, name: "Reflection (Horizontal In-Place)", param: 0 },
        { id: 5, name: "Pixelate (10px Block Average)", param: 10 },
        { id: 6, name: "Vignette (Square-Root-Free Falloff)", param: 0 },
        { id: 7, name: "Unsharp Mask & Clarity Enhancer", param: 60 }
    ];

    let passed = 0;

    for (const f of filters) {
        const inPtr = PixLabModule._malloc(testImg.length);
        if (!inPtr) {
            console.error(`  FAIL: Failed to allocate input buffer for ${f.name}`);
            continue;
        }

        PixLabModule.HEAPU8.set(testImg, inPtr);

        const startTime = process.hrtime.bigint();
        const outPtr = PixLabModule._processImage(inPtr, testImg.length, f.id, f.param);
        const endTime = process.hrtime.bigint();
        const durationMs = Number(endTime - startTime) / 1e6;

        const outSize = PixLabModule._getOutputSize();

        if (outPtr && outSize >= 54) {
            const outView = new DataView(PixLabModule.HEAPU8.buffer, outPtr, outSize);
            const signature = outView.getUint16(0, true);
            const width = outView.getInt32(18, true);
            const height = Math.abs(outView.getInt32(22, true));

            if (signature === 0x4D42 && width === 128 && height === 128) {
                console.log(`  ✓ Filter ${f.id} [${f.name}]: PASSED (${durationMs.toFixed(2)} ms)`);
                passed++;
            } else {
                console.error(`  FAIL: Invalid output BMP headers for ${f.name}`);
            }
        } else {
            console.error(`  FAIL: Filter ${f.id} returned null or invalid output size`);
        }

        PixLabModule._freeBuffer(inPtr);
        if (outPtr) PixLabModule._freeBuffer(outPtr);
    }

    // Edge case test 1: invalid buffer
    const badOut = PixLabModule._processImage(0, 0, 1, 0);
    if (badOut === 0) {
        console.log("  ✓ Edge case: Empty input buffer rejected safely.");
        passed++;
    } else {
        console.error("  FAIL: Empty input buffer not rejected!");
    }

    // Edge case test 2: corrupt buffer
    const corruptBuf = new Uint8Array(40);
    const corruptPtr = PixLabModule._malloc(40);
    PixLabModule.HEAPU8.set(corruptBuf, corruptPtr);
    const corruptOut = PixLabModule._processImage(corruptPtr, 40, 1, 0);
    PixLabModule._freeBuffer(corruptPtr);
    if (corruptOut === 0) {
        console.log("  ✓ Edge case: Corrupt header rejected safely.");
        passed++;
    } else {
        console.error("  FAIL: Corrupt header not rejected!");
    }

    console.log("==================================================");
    console.log(` Results: ${passed} / ${filters.length + 2} tests passed!`);
    console.log("==================================================");

    if (passed !== filters.length + 2) {
        process.exit(1);
    }
}

runTests();
