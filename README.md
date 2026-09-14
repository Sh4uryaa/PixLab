# PixLab

PixLab is an image filtering application built around a C++ OOP core, an abstract `Filter` interface with concrete implementations for grayscale, sepia, box blur, reflection, and image enhancement, compiled to WebAssembly and run directly in the browser. No server round-trip for processing: images are read, filtered, and re-encoded entirely client-side using Emscripten's memory bridge between JS and C++. A Node.js/Express backend with PostgreSQL adds optional accounts, letting users save processed images to a personal gallery and revisit their history.

## Features

- Grayscale, sepia, box blur, and reflection filters
- Image enhancement via unsharp masking
- Filtering runs entirely client-side using WebAssembly, no server needed for processing
- Optional accounts to save processed images and view history

## Tech Stack

**Core Engine:** C++ (OOP: encapsulation, abstraction, inheritance, polymorphism)
**Compilation:** Emscripten (C++ to WebAssembly)
**Frontend:** HTML, CSS, JavaScript
**Backend:** Node.js, Express
**Database:** PostgreSQL

## Getting Started

```bash
git clone https://github.com/your-username/pixlab.git
cd pixlab/backend
npm install
npm start
```

## License

This project is open source and available under the MIT License.
