import { defineConfig } from "vite";
import dts from "vite-plugin-dts";

export default defineConfig({
    plugins: [dts({ rollupTypes: true, outDir: "./dist" })],
    build: {
        lib: {
            entry: "./lib/entry.ts",
            name: "UVSlicerWasm",
            fileName: "uv-slicer-wasm",
        },
    },
});
