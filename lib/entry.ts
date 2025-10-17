import MainModuleFactory from "./wasm/uv-slicer";

export async function init(wasmBinary: ArrayBuffer) {
    const module = await MainModuleFactory({
        wasmBinary: wasmBinary,
    });
    return module;
}

export { type MainModule as UVSlicerModule } from "./wasm/uv-slicer";
