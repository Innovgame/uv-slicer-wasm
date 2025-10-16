# UV Slicer WASM

## Overview

This WebAssembly library converts bitmap image data into various resin 3D printer slice formats, optimized for browser-based applications. Similar in functionality to UVTools' converter, this library brings format conversion capabilities directly to web environments.

## Key Features

- **Browser-first Design**: Runs entirely in the browser via WebAssembly
- **Multi-format Support**: Convert to/from common resin printing formats
- **SIMD Accelerated**: Optimized performance using WebAssembly SIMD
- **Parallel Processing**: Web Worker support for multi-threaded conversions
- **Zero-dependency**: Pure C++ compiled to WASM with clean JavaScript interface

## Supported Formats

### Input Formats
- Raw bitmap data (Uint8Array)

### Output Formats
- `.prz` (Phrozen)
- ...

## Installation

### NPM
```bash
npm install slice-format-converter
```

## Usage

### Basic Conversion
```typescript
import { init } from "uv-slicer-wasm";
import wasmUrl from "uv-slicer-wasm/lib/wasm/uv-slicer.wasm?url";

const wasm = await init(wasmUrl);
const result = wasm.encodePrzImage({ data: data, useColorDiffCompression: useColorDifferenceCompression });
```

## Performance

Typical conversion times on modern browsers:

| Resolution     | Format | Single-thread | 4 Workers | SIMD |
| -------------- | ------ | ------------- | --------- | ---- |
| 8K (7680x4320) | .prz   | 300ms         | 80ms      | 30ms |

## API Reference

### SliceConverter Class

```typescript
declare interface EmbindModule {
    encodePrzImage(_0: any): any;
}

export declare function init(wasmURL: string): Promise<MainModule>;

declare type MainModule = WasmModule & EmbindModule;

declare interface WasmModule {
    _main(_0: number, _1: number): number;
}

export { }

```

## Building from Source

### Build Steps
```bash
git clone https://github.com/your-repo/uv-slicer-wasm.git
cd uv-slicer-wasm

# Build WASM
mkdir build && cd build
emcmake ..

# Build JS wrapper
npm install
npm run build
```

## Browser Compatibility

- Chrome 87+ (with WebAssembly SIMD support recommended)
- Firefox 83+
- Edge 89+
- Safari 14.1+

For best performance, enable WebAssembly SIMD support in your browser flags.

## License

MIT License

## Contributing

Contributions are welcome! Please see CONTRIBUTING.md for guidelines.

## Support

For issues or feature requests, please file an issue on our GitHub repository.