/**
 * PixLab: C++ WebAssembly Image Studio - Frontend Controller
 *
 * Implements full user workflow, memory bridge to WebAssembly (WASM),
 * interactive split-view comparison, and client-side BMP generation/export.
 */

document.addEventListener("DOMContentLoaded", () => {
    // --- Application State ---
    const state = {
        originalBytes: null,
        originalUrl: null,
        processedBytes: null,
        processedUrl: null,
        filename: "image.bmp",
        width: 0,
        height: 0,
        selectedFilterId: 1, // 1: Grayscale (default)
        selectedFilterName: "Grayscale",
        pixelateBlockSize: 10,
        sharpnessAlpha: 1.5,
        activeWorkflow: "presets", // 'presets' | 'enhancement'
        currentViewMode: "split",  // 'split' | 'side' | 'single'
        isDraggingSplit: false
    };

    // --- DOM Elements ---
    const elements = {
        // Drop & File Input
        dropZone: document.getElementById("dropZone"),
        fileInput: document.getElementById("fileInput"),
        btnBrowseFile: document.getElementById("btnBrowseFile"),
        btnGenerateSample: document.getElementById("btnGenerateSample"),
        btnEmptyStateSample: document.getElementById("btnEmptyStateSample"),

        // Tabs
        tabPresetFilters: document.getElementById("tabPresetFilters"),
        tabEnhancement: document.getElementById("tabEnhancement"),
        contentPresets: document.getElementById("contentPresets"),
        contentEnhancement: document.getElementById("contentEnhancement"),

        // Filter Controls
        filterCards: document.querySelectorAll(".filter-card"),
        pixelateParamRow: document.getElementById("pixelateParamRow"),
        pixelateBlockSize: document.getElementById("pixelateBlockSize"),
        pixelateBlockSizeVal: document.getElementById("pixelateBlockSizeVal"),
        sharpnessAlpha: document.getElementById("sharpnessAlpha"),
        sharpnessAlphaVal: document.getElementById("sharpnessAlphaVal"),

        // Actions
        btnProcess: document.getElementById("btnProcess"),
        processBtnText: document.getElementById("processBtnText"),
        processSpinner: document.getElementById("processSpinner"),
        btnDownload: document.getElementById("btnDownload"),

        // Viewer Stage & Views
        viewerStage: document.getElementById("viewerStage"),
        stagePlaceholder: document.getElementById("stagePlaceholder"),
        splitViewer: document.getElementById("splitViewer"),
        sideViewer: document.getElementById("sideViewer"),
        singleViewer: document.getElementById("singleViewer"),

        // Image Tags
        originalImgSplit: document.getElementById("originalImgSplit"),
        processedImgSplit: document.getElementById("processedImgSplit"),
        originalLayer: document.getElementById("originalLayer"),
        splitDivider: document.getElementById("splitDivider"),

        originalImgSide: document.getElementById("originalImgSide"),
        processedImgSide: document.getElementById("processedImgSide"),
        processedImgSingle: document.getElementById("processedImgSingle"),
        singleResultTitle: document.getElementById("singleResultTitle"),

        // View Mode Buttons
        btnViewSplit: document.getElementById("btnViewSplit"),
        btnViewSide: document.getElementById("btnViewSide"),
        btnViewResult: document.getElementById("btnViewResult"),

        // Metrics
        metricDimensions: document.getElementById("metricDimensions"),
        metricBitDepth: document.getElementById("metricBitDepth"),
        metricFileSize: document.getElementById("metricFileSize"),
        metricExecTime: document.getElementById("metricExecTime"),

        // Modal & Status
        btnInfoModal: document.getElementById("btnInfoModal"),
        infoModal: document.getElementById("infoModal"),
        btnCloseModal: document.getElementById("btnCloseModal"),
        wasmStatusBadge: document.getElementById("wasmStatusBadge"),
        wasmStatusText: document.getElementById("wasmStatusText"),
        toastContainer: document.getElementById("toastContainer")
    };

    // --- Helper: Toast Notifications ---
    function showToast(message, type = "info") {
        const toast = document.createElement("div");
        toast.className = `toast toast-${type}`;
        toast.textContent = message;
        elements.toastContainer.appendChild(toast);
        setTimeout(() => {
            toast.style.opacity = "0";
            toast.style.transform = "translateX(100%)";
            toast.style.transition = "all 0.3s ease";
            setTimeout(() => toast.remove(), 300);
        }, 3500);
    }

    // --- Helper: Format Bytes ---
    function formatBytes(bytes) {
        if (!bytes || bytes === 0) return "0 B";
        const k = 1024;
        const sizes = ["B", "KB", "MB"];
        const i = Math.floor(Math.log(bytes) / Math.log(k));
        return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + " " + sizes[i];
    }

    // --- Helper: Parse BMP Dimensions from Header ---
    function parseBmpDimensions(uint8Array) {
        if (uint8Array.length < 54) return null;
        const view = new DataView(uint8Array.buffer, uint8Array.byteOffset, uint8Array.byteLength);
        const magic = view.getUint16(0, true);
        if (magic !== 0x4D42) return null;
        const width = view.getInt32(18, true);
        const height = Math.abs(view.getInt32(22, true));
        const bpp = view.getUint16(28, true);
        return { width, height, bpp };
    }

    // --- Procedural 24-bit Windows BMP Benchmark Generator ---
    function generateSyntheticBmp(width = 512, height = 512) {
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
        view.setInt32(22, height, true); // positive = bottom-up
        view.setUint16(26, 1, true);     // 1 plane
        view.setUint16(28, 24, true);    // 24-bit
        view.setUint32(30, 0, true);     // BI_RGB
        view.setUint32(34, pixelArraySize, true);
        view.setInt32(38, 2835, true);   // 72 DPI
        view.setInt32(42, 2835, true);
        view.setUint32(46, 0, true);
        view.setUint32(50, 0, true);

        const cx = width / 2;
        const cy = height / 2;

        // Generate colorful pattern: radial gradient + geometric circles + color swatches
        for (let y = 0; y < height; ++y) {
            const rowOffset = 54 + y * rowStride;
            const dy = y - cy;

            for (let x = 0; x < width; ++x) {
                const dx = x - cx;
                const dist = Math.sqrt(dx * dx + dy * dy);

                let r, g, b;

                // Circular rings
                if (Math.abs(dist - 140) < 6 || Math.abs(dist - 80) < 4) {
                    r = 255; g = 255; b = 255; // White ring
                } else if (dist < 60) {
                    // Center orb: vibrant cyan-to-violet gradient
                    r = Math.floor(130 + 120 * Math.sin(x * 0.05));
                    g = Math.floor(60 + 50 * Math.cos(y * 0.05));
                    b = 240;
                } else {
                    // Smooth diagonal color spectrum
                    r = Math.floor((x / width) * 230 + 25);
                    g = Math.floor((y / height) * 220 + 20);
                    b = Math.floor(255 - ((x + y) / (width + height)) * 200);
                }

                // Grid checker pattern on the outer border
                if (x < 32 || x > width - 32 || y < 32 || y > height - 32) {
                    const checker = ((Math.floor(x / 8) + Math.floor(y / 8)) % 2 === 0);
                    if (checker) { r = 240; g = 200; b = 40; }
                    else { r = 20; g = 30; b = 50; }
                }

                const pixelIdx = rowOffset + x * 3;
                u8[pixelIdx + 0] = b; // Blue
                u8[pixelIdx + 1] = g; // Green
                u8[pixelIdx + 2] = r; // Red
            }

            for (let p = 0; p < padding; ++p) {
                u8[rowOffset + width * 3 + p] = 0;
            }
        }

        return u8;
    }

    // --- Convert Any Image (PNG, JPG, WebP, 32-bit BMP) to 24-bit uncompressed BMP in RAM ---
    function convertFileTo24BitBmp(file) {
        return new Promise((resolve, reject) => {
            const url = URL.createObjectURL(file);
            const img = new Image();
            img.onload = () => {
                try {
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
                    view.setInt32(22, height, true); // standard bottom-up
                    view.setUint16(26, 1, true);     // 1 plane
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

                    URL.revokeObjectURL(url);
                    resolve(u8);
                } catch (err) {
                    URL.revokeObjectURL(url);
                    reject(err);
                }
            };
            img.onerror = () => {
                URL.revokeObjectURL(url);
                reject(new Error("Unable to decode image. Please select a valid BMP, PNG, or JPG file."));
            };
            img.src = url;
        });
    }

    // --- Master Handler for Uploaded / Dropped Files ---
    async function handleUploadedFile(file) {
        if (!file) return;

        // Check if file is already a valid 24-bit uncompressed BMP
        const isBmp = /\.bmp$/i.test(file.name) || file.type === "image/bmp" || file.type === "image/x-ms-bmp";

        if (isBmp) {
            try {
                const buffer = await file.arrayBuffer();
                const bytes = new Uint8Array(buffer);
                const specs = parseBmpDimensions(bytes);
                if (specs && specs.bpp === 24) {
                    loadBmpData(bytes, file.name);
                    return;
                }
            } catch (e) {
                // If direct read fails, fall through to canvas decoder
            }
        }

        // Automatic in-memory conversion to standard 24-bit BMP
        try {
            showToast(`Loading "${file.name}"...`, "info");
            const bmpBytes = await convertFileTo24BitBmp(file);
            const baseName = file.name.replace(/\.[^/.]+$/, "");
            loadBmpData(bmpBytes, `${baseName}.bmp`);
            showToast(`Converted "${file.name}" to 24-bit BMP for WASM engine.`, "success");
        } catch (err) {
            console.error("[PixLab] File upload error:", err);
            showToast(err.message, "error");
        } finally {
            elements.fileInput.value = "";
        }
    }

    // --- Load BMP Bytes into Studio ---
    function loadBmpData(bytes, filename = "image.bmp") {
        const specs = parseBmpDimensions(bytes);
        if (!specs) {
            showToast("Invalid BMP file. Must be a valid 24-bit uncompressed Windows BMP.", "error");
            return;
        }

        if (specs.bpp !== 24) {
            showToast(`Unsupported bit depth: ${specs.bpp}-bit. PixLab requires 24-bit BMP.`, "error");
            return;
        }

        state.originalBytes = bytes;
        state.processedBytes = null;
        state.filename = filename;
        state.width = specs.width;
        state.height = specs.height;

        // Cleanup old Object URLs
        if (state.originalUrl) URL.revokeObjectURL(state.originalUrl);
        if (state.processedUrl) URL.revokeObjectURL(state.processedUrl);

        const blob = new Blob([bytes], { type: "image/bmp" });
        state.originalUrl = URL.createObjectURL(blob);
        state.processedUrl = state.originalUrl; // Default preview to original until processed

        // Update UI Image Views
        elements.originalImgSplit.src = state.originalUrl;
        elements.processedImgSplit.src = state.processedUrl;
        elements.originalImgSide.src = state.originalUrl;
        elements.processedImgSide.src = state.processedUrl;
        elements.processedImgSingle.src = state.processedUrl;

        // Update Top Metrics
        elements.metricDimensions.textContent = `${specs.width} × ${specs.height} px`;
        elements.metricFileSize.textContent = formatBytes(bytes.length);
        elements.metricExecTime.textContent = "—";

        // Enable Process Button
        elements.btnProcess.disabled = false;
        elements.btnDownload.disabled = true;

        // Reveal Active View
        elements.stagePlaceholder.style.display = "none";
        updateViewModeDisplay();

        showToast(`Loaded "${filename}" (${specs.width}×${specs.height}, ${formatBytes(bytes.length)})`, "success");
    }

    // --- WASM Image Processing Engine Bridge ---
    async function executeImageProcessing() {
        if (!state.originalBytes) {
            showToast("Please load an image first.", "error");
            return;
        }

        const Module = window.Module || window.PixLabModule;
        if (!Module || typeof Module._processImage !== "function") {
            showToast("WebAssembly engine is not ready.", "error");
            return;
        }

        // Set Loading State
        elements.btnProcess.disabled = true;
        elements.processBtnText.style.display = "none";
        elements.processSpinner.style.display = "inline-block";

        try {
            // Determine filter code and parameters
            let filterCode = state.selectedFilterId;
            let param = 0.0;

            if (state.activeWorkflow === "enhancement") {
                filterCode = 7; // UnsharpMaskEnhancer
                param = state.sharpnessAlpha;
            } else {
                if (filterCode === 5) { // Pixelate
                    param = state.pixelateBlockSize;
                }
            }

            // Small timeout to allow UI spinner to render
            await new Promise((resolve) => setTimeout(resolve, 30));

            // 1. Read input bytes
            const inputBuffer = state.originalBytes;
            const inputSize = inputBuffer.length;

            // 2. Allocate WASM memory
            const inPtr = Module._malloc(inputSize);
            if (!inPtr) throw new Error("Failed to allocate memory on WASM heap for input.");

            // 3. Write input bytes into WASM heap
            Module.HEAPU8.set(inputBuffer, inPtr);

            // 4. Call C++ WebAssembly filter execution with high-res timer
            const t0 = performance.now();
            const outPtr = Module._processImage(inPtr, inputSize, filterCode, param);
            const t1 = performance.now();
            const execTimeMs = (t1 - t0).toFixed(2);

            // 5. Query output size and safely clone bytes
            const outSize = Module._getOutputSize();
            if (!outPtr || outSize === 0) {
                Module._freeBuffer(inPtr);
                throw new Error("WASM image processing failed or returned empty buffer.");
            }

            // Immediately slice out safe bytes to avoid stale memory or detachment
            const safeBytes = new Uint8Array(Module.HEAPU8.buffer, outPtr, outSize).slice();

            // 6. Explicitly free both heap allocations
            Module._freeBuffer(inPtr);
            Module._freeBuffer(outPtr);

            // 7. Store processed output and create Blob URL
            state.processedBytes = safeBytes;
            if (state.processedUrl && state.processedUrl !== state.originalUrl) {
                URL.revokeObjectURL(state.processedUrl);
            }
            const blob = new Blob([safeBytes], { type: "image/bmp" });
            state.processedUrl = URL.createObjectURL(blob);

            // 8. Update Viewer Views
            elements.processedImgSplit.src = state.processedUrl;
            elements.processedImgSide.src = state.processedUrl;
            elements.processedImgSingle.src = state.processedUrl;

            const filterLabel = state.activeWorkflow === "enhancement"
                ? `Unsharp Mask (α = ${state.sharpnessAlpha})`
                : state.selectedFilterName;
            elements.singleResultTitle.textContent = `${filterLabel} Output`;

            // 9. Update Metrics Toolbar
            elements.metricExecTime.textContent = `${execTimeMs} ms`;
            elements.btnDownload.disabled = false;

            showToast(`C++ WASM: ${filterLabel} completed in ${execTimeMs} ms!`, "success");

        } catch (err) {
            console.error("[PixLab] Processing Error:", err);
            showToast(`Processing Error: ${err.message}`, "error");
        } finally {
            elements.btnProcess.disabled = false;
            elements.processBtnText.style.display = "flex";
            elements.processSpinner.style.display = "none";
        }
    }

    // --- Interactive Comparison View Mode Management ---
    function updateViewModeDisplay() {
        if (!state.originalBytes) {
            elements.stagePlaceholder.style.display = "flex";
            elements.splitViewer.style.display = "none";
            elements.sideViewer.style.display = "none";
            elements.singleViewer.style.display = "none";
            return;
        }

        elements.stagePlaceholder.style.display = "none";
        elements.splitViewer.style.display = state.currentViewMode === "split" ? "block" : "none";
        elements.sideViewer.style.display = state.currentViewMode === "side" ? "grid" : "none";
        elements.singleViewer.style.display = state.currentViewMode === "single" ? "block" : "none";

        [elements.btnViewSplit, elements.btnViewSide, elements.btnViewResult].forEach(btn => btn.classList.remove("active"));
        if (state.currentViewMode === "split") elements.btnViewSplit.classList.add("active");
        if (state.currentViewMode === "side") elements.btnViewSide.classList.add("active");
        if (state.currentViewMode === "single") elements.btnViewResult.classList.add("active");
    }

    // --- Split-Slider Dragging Logic ---
    function setSplitPosition(clientX) {
        const rect = elements.splitViewer.getBoundingClientRect();
        let offsetX = clientX - rect.left;
        if (offsetX < 0) offsetX = 0;
        if (offsetX > rect.width) offsetX = rect.width;

        const percentage = (offsetX / rect.width) * 100;
        elements.originalLayer.style.width = `${percentage}%`;
        elements.splitDivider.style.left = `${percentage}%`;
    }

    elements.splitDivider.addEventListener("mousedown", (e) => {
        state.isDraggingSplit = true;
        e.preventDefault();
    });

    window.addEventListener("mousemove", (e) => {
        if (!state.isDraggingSplit) return;
        setSplitPosition(e.clientX);
    });

    window.addEventListener("mouseup", () => {
        state.isDraggingSplit = false;
    });

    // Touch support for mobile / tablets
    elements.splitDivider.addEventListener("touchstart", (e) => {
        state.isDraggingSplit = true;
    }, { passive: true });

    window.addEventListener("touchmove", (e) => {
        if (!state.isDraggingSplit || !e.touches[0]) return;
        setSplitPosition(e.touches[0].clientX);
    }, { passive: true });

    window.addEventListener("touchend", () => {
        state.isDraggingSplit = false;
    });

    // --- File Input & Drag and Drop Handlers ---
    // Clicking anywhere on the drop zone card opens the file dialog
    elements.dropZone.addEventListener("click", () => {
        elements.fileInput.click();
    });

    // Keyboard accessibility: Enter or Space triggers file dialog
    elements.dropZone.addEventListener("keydown", (e) => {
        if (e.key === "Enter" || e.key === " ") {
            e.preventDefault();
            elements.fileInput.click();
        }
    });

    elements.btnBrowseFile.addEventListener("click", (e) => {
        e.stopPropagation(); // Avoid triggering dropZone click twice
        elements.fileInput.click();
    });

    elements.fileInput.addEventListener("change", (e) => {
        const file = e.target.files && e.target.files[0];
        if (file) {
            handleUploadedFile(file);
        }
    });

    // Prevent default browser behavior on accidental window drop (navigating to file)
    window.addEventListener("dragover", (e) => e.preventDefault());
    window.addEventListener("drop", (e) => e.preventDefault());

    // Drop zone drag events with dragCounter to prevent child flickering
    let dragCounter = 0;

    elements.dropZone.addEventListener("dragenter", (e) => {
        e.preventDefault();
        dragCounter++;
        elements.dropZone.classList.add("drag-active");
    });

    elements.dropZone.addEventListener("dragover", (e) => {
        e.preventDefault();
        elements.dropZone.classList.add("drag-active");
    });

    elements.dropZone.addEventListener("dragleave", (e) => {
        e.preventDefault();
        dragCounter--;
        if (dragCounter <= 0) {
            dragCounter = 0;
            elements.dropZone.classList.remove("drag-active");
        }
    });

    elements.dropZone.addEventListener("drop", (e) => {
        e.preventDefault();
        dragCounter = 0;
        elements.dropZone.classList.remove("drag-active");
        const files = e.dataTransfer.files;
        if (files && files.length > 0) {
            handleUploadedFile(files[0]);
        }
    });

    // Sample BMP buttons
    const handleGenerateSample = () => {
        const sampleBytes = generateSyntheticBmp(512, 512);
        loadBmpData(sampleBytes, "pixlab_benchmark_sample.bmp");
    };

    elements.btnGenerateSample.addEventListener("click", handleGenerateSample);
    elements.btnEmptyStateSample.addEventListener("click", handleGenerateSample);

    // --- Tab Switching Logic ---
    elements.tabPresetFilters.addEventListener("click", () => {
        state.activeWorkflow = "presets";
        elements.tabPresetFilters.classList.add("active");
        elements.tabEnhancement.classList.remove("active");
        elements.contentPresets.classList.add("active");
        elements.contentEnhancement.classList.remove("active");
    });

    elements.tabEnhancement.addEventListener("click", () => {
        state.activeWorkflow = "enhancement";
        elements.tabEnhancement.classList.add("active");
        elements.tabPresetFilters.classList.remove("active");
        elements.contentEnhancement.classList.add("active");
        elements.contentPresets.classList.remove("active");
    });

    // --- Preset Filter Selection ---
    elements.filterCards.forEach(card => {
        card.addEventListener("click", () => {
            elements.filterCards.forEach(c => c.classList.remove("active"));
            card.classList.add("active");

            state.selectedFilterId = parseInt(card.dataset.filterId, 10);
            state.selectedFilterName = card.dataset.name;

            // Show pixelate parameter row if Pixelate (filter 5) is selected
            if (state.selectedFilterId === 5) {
                elements.pixelateParamRow.style.display = "flex";
            } else {
                elements.pixelateParamRow.style.display = "none";
            }
        });
    });

    // Sliders
    elements.pixelateBlockSize.addEventListener("input", (e) => {
        state.pixelateBlockSize = parseInt(e.target.value, 10);
        elements.pixelateBlockSizeVal.textContent = `${state.pixelateBlockSize} px`;
    });

    elements.sharpnessAlpha.addEventListener("input", (e) => {
        state.sharpnessAlpha = parseFloat(e.target.value);
        elements.sharpnessAlphaVal.textContent = state.sharpnessAlpha.toFixed(2);
    });

    // View Mode Controls
    elements.btnViewSplit.addEventListener("click", () => {
        state.currentViewMode = "split";
        updateViewModeDisplay();
    });

    elements.btnViewSide.addEventListener("click", () => {
        state.currentViewMode = "side";
        updateViewModeDisplay();
    });

    elements.btnViewResult.addEventListener("click", () => {
        state.currentViewMode = "single";
        updateViewModeDisplay();
    });

    // Process and Download Actions
    elements.btnProcess.addEventListener("click", executeImageProcessing);

    elements.btnDownload.addEventListener("click", () => {
        if (!state.processedBytes) {
            showToast("No processed image available for download.", "error");
            return;
        }

        const filterTag = state.activeWorkflow === "enhancement"
            ? `enhanced_alpha${state.sharpnessAlpha}`
            : state.selectedFilterName.toLowerCase().replace(/\s+/g, "_");

        const baseName = state.filename.replace(/\.bmp$/i, "");
        const exportFilename = `${baseName}_pixlab_${filterTag}.bmp`;

        const blob = new Blob([state.processedBytes], { type: "image/bmp" });
        const url = URL.createObjectURL(blob);
        const a = document.createElement("a");
        a.href = url;
        a.download = exportFilename;
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(url);

        showToast(`Saved ${exportFilename}`, "success");
    });

    // Modal Events
    elements.btnInfoModal.addEventListener("click", () => {
        elements.infoModal.style.display = "flex";
    });

    elements.btnCloseModal.addEventListener("click", () => {
        elements.infoModal.style.display = "none";
    });

    elements.infoModal.addEventListener("click", (e) => {
        if (e.target === elements.infoModal) {
            elements.infoModal.style.display = "none";
        }
    });

    // Preload sample image for instant demo experience
    setTimeout(() => {
        handleGenerateSample();
    }, 400);
});
