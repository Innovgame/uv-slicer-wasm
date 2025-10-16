// TypeScript bindings for emscripten-generated code.  Automatically generated at compile time.
interface WasmModule {
  _main(_0: number, _1: number): number;
}

interface EmbindModule {
  encodePrzImage(_0: any): any;
}

export type MainModule = WasmModule & EmbindModule;
export default function MainModuleFactory (options?: unknown): Promise<MainModule>;
