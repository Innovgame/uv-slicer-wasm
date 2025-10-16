import MainModuleFactory from "./wasm/uv-slicer";

export async function init(wasmURL: string) {
    const res = await fetch(wasmURL);
    const arrayBuffer = await res.arrayBuffer();
    const module = await MainModuleFactory({
        wasmBinary: arrayBuffer,
    });
    return module;
}
