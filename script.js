/**
 * PixLab - Core Frontend Script
 *
 * Connects the PixLab UI to the C++ WebAssembly (WASM) image processing engine:
 * - Interactive before/after split slider
 * - Drag-and-drop & file upload with universal format support (PNG, JPG, BMP, WebP)
 * - Container aspect ratio adaptation so images contain at their original natural size
 * - Multi-pass C++17 filters (Grayscale, Sepia, Blur, Reflection, Pixelate, Vignette)
 * - C++ Unsharp Mask Image Enhancement (High-pass spatial sharpening)
 * - Memory-recycled in-RAM BMP export and download
 */

document.addEventListener("DOMContentLoaded", () => {
    // -------------------------------------------------------------
    // 1. Comparison Sliders Setup (Used on Filters & Enhance pages)
    // -------------------------------------------------------------
    document.querySelectorAll(".comparison-container").forEach((container) => {
        const images = container.querySelectorAll("img");
        const slider = container.querySelector('input[type="range"]');
        const line = container.querySelector(".slider-line");

        if (images.length < 2 || !slider || !line) return;

        const resultImage = images[1];

        function updateSlider() {
            const position = Number(slider.value);
            resultImage.style.clipPath = `inset(0 0 0 ${position}%)`;
            line.style.left = `${position}%`;
        }

        slider.addEventListener("input", updateSlider);
        updateSlider();
    });

    // -------------------------------------------------------------
    // 2. Elements & Application State
    // -------------------------------------------------------------
    const fileInput = document.getElementById("fileInput");
    const uploadBox = document.querySelector(".upload-box");
    const filterOptions = document.getElementById("filterOptions");
    const applyFilterBtn = document.getElementById("applyFilterBtn");
    const applyEnhancementBtn = document.getElementById("applyEnhancementBtn");
    const downloadBtn = document.getElementById("downloadBtn");

    // Image references (index.html uses filterExampleImg, enhance.html uses beforeImage)
    const originalImg = document.getElementById("filterExampleImg") || document.getElementById("beforeImage");
    const resultImg = document.getElementById("filterExampleResultImg") || document.getElementById("afterImage");

    let currentImageElement = null; // Stored HTMLImageElement of the uploaded image
    let currentFileName = "image.png";
    let processedBlobUrl = null;

    // Filter type mappings matching C++ FilterFactory::FilterType
    const FILTER_MAP = {
        grayscale: { id: 1, param: 0.0, name: "grayscale" },
        sepia: { id: 2, param: 0.0, name: "sepia" },
        blur: { id: 3, param: 0.0, name: "blur" },
        reflection: { id: 4, param: 0.0, name: "reflection" },
        pixelate: { id: 5, param: 10.0, name: "pixelate" },
        vignette: { id: 6, param: 0.0, name: "vignette" }
    };

    // Adapt container aspect ratio to original image so it contains at natural size
    function updateContainerAspectRatio(img) {
        if (!img) return;
        const container = img.closest(".comparison-container");
        if (container && img.naturalWidth > 0 && img.naturalHeight > 0) {
            container.style.aspectRatio = `${img.naturalWidth} / ${img.naturalHeight}`;
        }
    }

    if (originalImg) {
        if (originalImg.complete && originalImg.naturalWidth > 0) {
            updateContainerAspectRatio(originalImg);
            currentImageElement = originalImg;
        } else {
            originalImg.addEventListener("load", () => {
                updateContainerAspectRatio(originalImg);
                if (!currentImageElement) currentImageElement = originalImg;
            });
        }
    }

    // -------------------------------------------------------------
    // 3. Helper: Convert Any Image to 24-bit Uncompressed Windows BMP
    // -------------------------------------------------------------
    function imageTo24BitBmp(img) {
        const width = img.naturalWidth || img.width;
        const height = img.naturalHeight || img.height;

        if (!width || !height || width <= 0 || height <= 0) {
            throw new Error("Invalid image dimensions.");
        }

        const canvas = document.createElement("canvas");
        canvas.width = width;
        canvas.height = height;
        const ctx = canvas.getContext("2d", { willReadFrequently: true });
        ctx.drawImage(img, 0, 0, width, height);

        const imgData = ctx.getImageData(0, 0, width, height);
        const rgba = imgData.data;

        const padding = (4 - (width * 3) % 4) % 4;
        const rowStride = width * 3 + padding;
        const pixelArraySize = rowStride * height;
        const totalSize = 54 + pixelArraySize;

        const buffer = new ArrayBuffer(totalSize);
        const view = new DataView(buffer);
        const u8 = new Uint8Array(buffer);

        // BITMAPFILEHEADER (14 bytes)
        view.setUint16(0, 0x4D42, true); // "BM"
        view.setUint32(2, totalSize, true);
        view.setUint16(6, 0, true);
        view.setUint16(8, 0, true);
        view.setUint32(10, 54, true);

        // BITMAPINFOHEADER (40 bytes)
        view.setUint32(14, 40, true);
        view.setInt32(18, width, true);
        view.setInt32(22, height, true); // Positive = standard bottom-up
        view.setUint16(26, 1, true);
        view.setUint16(28, 24, true);    // 24-bit uncompressed RGB
        view.setUint32(30, 0, true);     // BI_RGB
        view.setUint32(34, pixelArraySize, true);
        view.setInt32(38, 2835, true);   // 72 DPI
        view.setInt32(42, 2835, true);
        view.setUint32(46, 0, true);
        view.setUint32(50, 0, true);

        // Write scanlines bottom-to-top (row = height - 1 down to 0) in BGR order
        for (let row = 0; row < height; ++row) {
            const sourceRow = height - 1 - row;
            const rowStart = 54 + (row * rowStride);

            for (let col = 0; col < width; ++col) {
                const srcIdx = (sourceRow * width + col) * 4;
                const destIdx = rowStart + (col * 3);
                u8[destIdx + 0] = rgba[srcIdx + 2]; // Blue
                u8[destIdx + 1] = rgba[srcIdx + 1]; // Green
                u8[destIdx + 2] = rgba[srcIdx + 0]; // Red
            }

            for (let p = 0; p < padding; ++p) {
                u8[rowStart + width * 3 + p] = 0x00;
            }
        }

        return u8;
    }

    // -------------------------------------------------------------
    // 4. File Upload & Drag-and-Drop Handling
    // -------------------------------------------------------------
    function handleSelectedFile(file) {
        if (!file || !file.type.startsWith("image/")) {
            alert("Please select a valid image file (PNG, JPG, JPEG, BMP, WebP).");
            return;
        }

        currentFileName = file.name;
        const reader = new FileReader();

        reader.onload = (e) => {
            const img = new Image();
            img.onload = () => {
                currentImageElement = img;

                // Update original and result image views
                if (originalImg) originalImg.src = img.src;
                if (resultImg) resultImg.src = img.src;

                // Ensure container adapts to natural aspect ratio without enlarging/cropping
                updateContainerAspectRatio(img);

                // Reset split slider to 50%
                const slider = document.getElementById("comparisonSlider");
                if (slider) {
                    slider.value = 50;
                    slider.dispatchEvent(new Event("input"));
                }

                // Update upload box UI feedback
                if (uploadBox) {
                    const desc = uploadBox.querySelector("p");
                    if (desc) desc.textContent = `Loaded: ${file.name}`;
                    const btn = uploadBox.querySelector(".upload-button");
                    if (btn) btn.textContent = "✓ Image Ready (Click or drop to replace)";
                }

                // Clear previous download state
                if (downloadBtn) {
                    downloadBtn.removeAttribute("href");
                }
            };
            img.src = e.target.result;
        };

        reader.readAsDataURL(file);
    }

    if (fileInput) {
        fileInput.addEventListener("change", (e) => {
            if (e.target.files && e.target.files[0]) {
                handleSelectedFile(e.target.files[0]);
                fileInput.value = "";
            }
        });
    }

    if (uploadBox) {
        uploadBox.addEventListener("dragover", (e) => {
            e.preventDefault();
            uploadBox.classList.add("drag-over");
        });

        uploadBox.addEventListener("dragleave", (e) => {
            e.preventDefault();
            uploadBox.classList.remove("drag-over");
        });

        uploadBox.addEventListener("drop", (e) => {
            e.preventDefault();
            uploadBox.classList.remove("drag-over");
            if (e.dataTransfer.files && e.dataTransfer.files[0]) {
                handleSelectedFile(e.dataTransfer.files[0]);
                if (fileInput) fileInput.value = "";
            }
        });
    }

    // -------------------------------------------------------------
    // 5. WASM Processing Execution Bridge
    // -------------------------------------------------------------
    async function processWithWasm(filterId, param, filterName) {
        const Module = window.Module || window.PixLabModule;
        if (!Module || typeof Module._processImage !== "function") {
            alert("WebAssembly engine is still initializing. Please try again in a moment.");
            return;
        }

        // Re-synchronize memory view in case WebAssembly heap grew
        if (!Module.HEAPU8 || Module.HEAPU8.byteLength === 0) {
            if (window.PixLabModule && window.PixLabModule.HEAPU8) {
                Module.HEAPU8 = window.PixLabModule.HEAPU8;
            }
        }

        // Determine source image: user uploaded image or default preview
        let srcImg = currentImageElement;
        if (!srcImg && originalImg) {
            srcImg = originalImg;
        }

        if (!srcImg || !srcImg.complete || srcImg.naturalWidth === 0) {
            alert("Please wait for the image to load completely.");
            return;
        }

        try {
            // 1. Convert image to 24-bit uncompressed BMP binary in RAM
            const bmpBytes = imageTo24BitBmp(srcImg);
            const inputSize = bmpBytes.length;

            // 2. Allocate WASM heap buffer
            const inPtr = Module._malloc(inputSize);
            if (!inPtr) throw new Error("WASM out of memory.");

            // 3. Write BMP into WASM heap
            Module.HEAPU8.set(bmpBytes, inPtr);

            // 4. Call C++ processing engine
            const outPtr = Module._processImage(inPtr, inputSize, filterId, param);
            const outSize = Module._getOutputSize();

            if (!outPtr || outSize === 0) {
                Module._freeBuffer(inPtr);
                throw new Error("C++ filter processing failed.");
            }

            // 5. Clone output buffer safely into JavaScript typed array
            const safeOutput = new Uint8Array(Module.HEAPU8.buffer, outPtr, outSize).slice();

            // 6. Free both WASM allocations explicitly (resets allocator back to base)
            Module._freeBuffer(inPtr);
            Module._freeBuffer(outPtr);

            // 7. Create Object URL from BMP Blob
            if (processedBlobUrl) {
                URL.revokeObjectURL(processedBlobUrl);
            }
            const blob = new Blob([safeOutput], { type: "image/bmp" });
            processedBlobUrl = URL.createObjectURL(blob);

            // 8. Update result image in comparison slider
            if (resultImg) {
                resultImg.src = processedBlobUrl;
            }

            // 9. Enable and configure Download button
            if (downloadBtn) {
                const baseName = currentFileName.replace(/\.[^/.]+$/, "");
                downloadBtn.href = processedBlobUrl;
                downloadBtn.download = `pixlab_${baseName}_${filterName}.bmp`;
            }

        } catch (err) {
            console.error("[PixLab] Filter execution error:", err);
            alert(`Processing failed: ${err.message}`);
        }
    }

    // -------------------------------------------------------------
    // 6. Action Button Listeners
    // -------------------------------------------------------------

    // "Apply Filter" button on index.html
    if (applyFilterBtn && filterOptions) {
        applyFilterBtn.addEventListener("click", async () => {
            const selectedKey = filterOptions.value;
            const filterConfig = FILTER_MAP[selectedKey] || FILTER_MAP.grayscale;

            const originalText = applyFilterBtn.textContent;
            applyFilterBtn.textContent = "Processing...";
            applyFilterBtn.disabled = true;

            await new Promise((resolve) => setTimeout(resolve, 30));
            await processWithWasm(filterConfig.id, filterConfig.param, filterConfig.name);

            applyFilterBtn.textContent = originalText;
            applyFilterBtn.disabled = false;
        });
    }

    // "Enhance Image" button on enhance.html
    if (applyEnhancementBtn) {
        applyEnhancementBtn.addEventListener("click", async () => {
            const originalText = applyEnhancementBtn.textContent;
            applyEnhancementBtn.textContent = "Enhancing...";
            applyEnhancementBtn.disabled = true;

            await new Promise((resolve) => setTimeout(resolve, 30));
            // Filter 7: UnsharpMaskEnhancer with alpha = 1.8
            await processWithWasm(7, 1.8, "enhanced");

            applyEnhancementBtn.textContent = originalText;
            applyEnhancementBtn.disabled = false;
        });
    }
});
