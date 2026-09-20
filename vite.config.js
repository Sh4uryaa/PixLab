import { defineConfig } from "vite";

export default defineConfig({
    build: {
        rollupOptions: {
            input: {
                main: "index.html",
                gallery: "gallery.html",
                enhance: "enhance.html",
                about: "about.html"
            }
        }
    }
});
