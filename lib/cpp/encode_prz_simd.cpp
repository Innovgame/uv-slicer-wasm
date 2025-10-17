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

emscripten::val encodePrzImage(const emscripten::val &param)
{

    using namespace std::chrono;
    auto startTime = high_resolution_clock::now();
    const auto &data = emscripten::convertJSArrayToNumberVector<uint8_t>(param["data"]);
    const auto length = data.size();
    const auto useColorDiffCompression = param["useColorDiffCompression"].as<bool>();

    const uint8_t LayerMagic = 0x55;
    std::vector<uint8_t> rle;
    // rle.reserve(length + 1024); // 预分配
    rle.push_back(LayerMagic);

    int previousColor = 0;
    int currentColor = 0;
    int stride = 0;
    uint32_t checkSum = 0;

    auto addRep = [&](int strideVal, int prevColor, int currColor) {
        if (strideVal == 0)
            return;

        size_t firstByteIndex = rle.size();
        rle.push_back(0); // placeholder

        int colorDiff = currColor - prevColor;
        if (colorDiff < 0)
            colorDiff = -colorDiff;

        if (useColorDiffCompression && colorDiff <= 0xf && strideVal <= 0xff && currColor > 0 && currColor < 0xff)
        {
            rle[firstByteIndex] = (0b10 << 6) | (colorDiff & 0xf);
            if (strideVal > 1)
            {
                rle[firstByteIndex] |= 0x1 << 4;
                rle.push_back(strideVal);
            }
            if (currColor < prevColor)
                rle[firstByteIndex] |= 0x1 << 5;
        }
        else
        {
            if (currColor == 0xff)
                rle[firstByteIndex] |= 0b11 << 6;
            else if (currColor > 0)
            {
                rle[firstByteIndex] |= 0b01 << 6;
                rle.push_back(currColor);
            }

            rle[firstByteIndex] |= strideVal & 0xf;
            if (strideVal <= 0xf)
                return;
            if (strideVal <= 0xfff)
            {
                rle[firstByteIndex] |= 0b01 << 4;
                rle.push_back(strideVal >> 4);
                return;
            }
            if (strideVal <= 0xfffff)
            {
                rle[firstByteIndex] |= 0b10 << 4;
                rle.push_back(strideVal >> 12);
                rle.push_back(strideVal >> 4);
                return;
            }
            if (strideVal <= 0xfffffff)
            {
                rle[firstByteIndex] |= 0b11 << 4;
                rle.push_back(strideVal >> 20);
                rle.push_back(strideVal >> 12);
                rle.push_back(strideVal >> 4);
                return;
            }
        }
    };

    int i = 0;
    while (i < length)
    {
        currentColor = data[i];
        stride = 0;

        // SIMD 比较连续16字节
        v128_t val = wasm_i8x16_splat((uint8_t)currentColor);

        while (i + 16 <= length)
        {
            v128_t block = wasm_v128_load(&data[i]);
            v128_t cmp = wasm_i8x16_eq(block, val);
            int mask = wasm_i8x16_bitmask(cmp);

            if (mask == 0xFFFF)
            { // 全部相等
                stride += 16;
                i += 16;
            }
            else
            {
                int sameCount = __builtin_ctz(~mask); // 前面连续相等数量
                stride += sameCount;
                i += sameCount;
                break;
            }
        }

        // 处理剩余未对齐部分
        while (i < length && data[i] == currentColor)
        {
            stride++;
            i++;
        }

        addRep(stride, previousColor, currentColor);
        previousColor = currentColor;
    }

    // 计算校验和
    for (size_t k = 1; k < rle.size(); k++)
        checkSum += rle[k];
    rle.push_back(~checkSum & 0xFF);

    auto endTime = high_resolution_clock::now();
    double durationMs = duration<double, std::milli>(endTime - startTime).count();

    // 输出耗时到控制台
    LOG_INFO("encodePrzImage layer encoded: {} ms, input size: {} bytes, output size: {} bytes", durationMs, length,
             rle.size());

    return emscripten::val(emscripten::typed_memory_view<uint8_t>(rle.size(), rle.data()));
}

EMSCRIPTEN_BINDINGS(UvSlicerWrapper)
{
    emscripten::function("encodePrzImage", &encodePrzImage);
}
