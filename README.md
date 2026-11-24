# PixLab

**A web-based image editor powered by C++ and WebAssembly.**

**Live Website:** [https://pixlab-a564f.web.app/](https://pixlab-a564f.web.app/)

PixLab is an image processing application that lets users upload images, apply creative filters, compare before-and-after results, and enhance their images through a simple web interface.

The project combines a modern frontend with an object-oriented C++ image-processing engine, allowing image transformations to run directly in the browser using WebAssembly.

## Features

- **Grayscale** – Convert images to grayscale.
- **Sepia** – Give images a classic vintage look.
- **Blur** – Apply a blur effect.
- **Reflection** – Flip images horizontally.
- **Pixelate** – Create a pixelated effect.
- **Vignette** – Add a vignette effect to images.
- **Image Enhancement** – Improve image quality using a dedicated enhancement algorithm.
- **Before & After Comparison** – Compare original and processed images using an interactive slider.
- **Image Upload & Download** – Work with your own images.
- **User Authentication** – Sign in using email/password or Google.
- **Gallery** – Browse images through a dedicated gallery interface.

## Tech Stack

| Technology | Purpose |
|---|---|
| C++17 | Image-processing engine |
| WebAssembly (WASM) | Run compiled C++ code in the browser |
| Emscripten | Compile C++ into WebAssembly |
| HTML, CSS, JavaScript | Frontend and interactive UI |
| Vite | Frontend development server |
| Firebase Authentication | User sign-in and authentication |
| Firebase Hosting | Website deployment |

## Planned Improvements

PixLab is an ongoing project, with plans to expand its functionality and improve its accessibility and user experience.

- **Database Integration** – Explore persistent storage for user data and saved image information.
- **Responsive Design** – Further refine and test the interface across mobile, tablet, and desktop screen sizes.
- **Gallery Enhancements** – Explore additional ways to organize and manage images.

These improvements are planned for future updates and are not yet fully implemented.

## License

Released under the [MIT License](LICENSE).