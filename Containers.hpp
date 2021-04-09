#pragma once
#include <cstdint>

struct merge_data
{
public:
    merge_data(size_t bh, size_t bpr, uint8_t* rgba, uint8_t* alpha) :
    block_height(bh), bytes_per_row(bpr), pRGBABlock(rgba), pAlphaBlock(alpha) 
    {}

    size_t block_height;
    size_t bytes_per_row;
    uint8_t* pRGBABlock;
    uint8_t* pAlphaBlock;
};