#pragma once
#include <chrono>

using namespace std::chrono_literals;
using high_res_clock = std::chrono::high_resolution_clock;
using nanoSeconds = std::chrono::nanoseconds;
#define nano_cast std::chrono::duration_cast<nanoSeconds>

// Inline used here to avoid duplicate constants on C++17
// https://www.learncpp.com/cpp-tutorial/sharing-global-constants-across-multiple-files-using-inline-variables/
inline constexpr nanoSeconds singleSecond(1s);