#include "emscripten/bind.h"
#include "emscripten/val.h"
#include "log.h"
#include <algorithm>
#include <chrono>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <wasm_simd128.h>

emscripten::val encodePwsImage(const emscripten::val &param)
{

    using namespace std::chrono;
    auto startTime = high_resolution_clock::now();

    const auto &data = emscripten::convertJSArrayToNumberVector<uint8_t>(param["data"]);
    const auto length = data.size();
    const auto AntiAliasLevel = param["AntiAliasLevel"].as<int>();

    const int RLE1EncodingLimit = 125;
    std::vector<uint8_t> rawData;
    rawData.reserve(length); // 预分配一半长度

    for (int aalevel = 1; aalevel <= AntiAliasLevel; aalevel++)
    {
        bool obit = false;
        int rep = 0;
        uint8_t threshold = static_cast<uint8_t>((255 * aalevel) / (AntiAliasLevel + 1) + 1);

        auto addRep = [&]() {
            if (rep <= 0)
                return;
            uint8_t by = rep & 0xFF;
            if (obit)
                by |= 0x80;
            rawData.push_back(std::min(by, (uint8_t)0xFF));
            rep = 0;
        };

        for (int i = 0; i < length; i++)
        {
            bool nbit = data[i] >= threshold;
            if (nbit == obit)
            {
                rep++;
                if (rep == RLE1EncodingLimit)
                {
                    addRep();
                }
            }
            else
            {
                addRep();
                obit = nbit;
                rep = 1;
            }
        }

        addRep(); // 处理剩余
    }

    auto endTime = high_resolution_clock::now();
    double totalMs = duration<double, std::milli>(endTime - startTime).count();
    LOG_INFO("[encodePwsImage] total time: {} ms, input: {} bytes, output: {} bytes\n", totalMs, length,
             rawData.size());

    return emscripten::val(emscripten::typed_memory_view<uint8_t>(rawData.size(), rawData.data()));
}

emscripten::val encodePw0Image(const emscripten::val &param)
{

    using namespace std::chrono;
    auto startTime = high_resolution_clock::now();

    const auto &data = emscripten::convertJSArrayToNumberVector<uint8_t>(param["data"]);
    const auto length = data.size();

    const int RLE4EncodingLimit = 0xFFF;
    std::vector<uint8_t> raw;
    raw.reserve(length / 2);

    auto putReps = [&](int &reps, int &lastColor) {
        while (reps > 0)
        {
            int done = reps;
            if (lastColor == 0 || lastColor == 0xF)
            {
                done = std::min(done, RLE4EncodingLimit);
                uint16_t more = (lastColor << 12) | done;
                raw.push_back((more >> 8) & 0xFF);
                raw.push_back(more & 0xFF);
            }
            else
            {
                done = std::min(done, 0xF);
                raw.push_back((done | (lastColor << 4)) & 0xFF);
            }
            reps -= done;
        }
    };

    int lastColor = -1;
    int reps = 0;

    int i = 0;
    while (i + 16 <= length)
    {
        v128_t block = wasm_v128_load(&data[i]);
        v128_t shifted = wasm_u8x16_shr(block, 4);
        uint8_t temp[16];
        wasm_v128_store(temp, shifted);

        for (int j = 0; j < 16; j++)
        {
            int color = temp[j];
            if (color == lastColor)
            {
                reps++;
            }
            else
            {
                putReps(reps, lastColor);
                lastColor = color;
                reps = 1;
            }
        }
        i += 16;
    }

    for (; i < length; i++)
    {
        int color = data[i] >> 4;
        if (color == lastColor)
        {
            reps++;
        }
        else
        {
            putReps(reps, lastColor);
            lastColor = color;
            reps = 1;
        }
    }

    putReps(reps, lastColor);

    auto endTime = high_resolution_clock::now();
    double durationMs = duration<double, std::milli>(endTime - startTime).count();
    LOG_INFO("[encodePw0Image] encoded: {} ms, input: {} bytes, output: {} bytes\n", durationMs, length, raw.size());
    return emscripten::val(emscripten::typed_memory_view<uint8_t>(raw.size(), raw.data()));
}

EMSCRIPTEN_BINDINGS(UvSlicerAc)
{
    emscripten::function("encodePwsImage", &encodePwsImage);
    emscripten::function("encodePw0Image", &encodePw0Image);
}