// TypeScript bindings for emscripten-generated code.  Automatically generated at compile time.
interface WasmModule {
  _main(_0: number, _1: number): number;
}

interface EmbindModule {
  encodePwsImage(_0: any): any;
  encodePw0Image(_0: any): any;
  encodePrzImage(_0: any): any;
}

export type MainModule = WasmModule & EmbindModule;
export default function MainModuleFactory (options?: unknown): Promise<MainModule>;
