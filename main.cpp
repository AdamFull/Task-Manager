#include "router.hpp"
#include "data_example_block.hpp"
//#include <unistd.h>
#include <iostream>
#include <random>

void mergeBlock(void* pdata)
{
    merge_data* data = static_cast<merge_data*>(pdata);
    uint8_t* src_rgba = data->pRGBABlock;
    uint8_t* src_alpha = data->pAlphaBlock;

    for (size_t y = 0; y < data->block_height; ++y)
    {
        for (size_t x = 0; x < data->bytes_per_row;)
        {
            uint8_t& rAlpha1 = *(src_rgba + 3);
            uint8_t& rAlpha2 = *src_alpha;

            //usleep(rand()%500);
            rAlpha1 = rAlpha2;

            src_rgba = src_rgba + 4;
            src_alpha = src_alpha + 4;
            x += 4;
        }
    }
}

void mergeAlpha(size_t nWidth, size_t nHeight, size_t nBytesPerRow, size_t nThreads, uint8_t* pRGBABuffer, uint8_t* pAlphaBuffer)
{
	size_t blockLine = nHeight / nThreads;
	size_t lastBlock = nHeight - (blockLine * (nThreads - 1));

	for (size_t i = 0; i < nThreads; i++)
	{
		size_t blockPosition = i * nBytesPerRow * blockLine;
        size_t curBlock = (i == nThreads - 1) ? lastBlock : blockLine;
        merge_data data = {curBlock, nBytesPerRow, pRGBABuffer + blockPosition, pAlphaBuffer + blockPosition};
        router::get_instance()->add_routine(mergeBlock, &data);
	}

    router::get_instance()->wait();
}

int main()
{
    router::get_instance();
    uint8_t* test1 = (uint8_t*)calloc(1500*200, sizeof(uint8_t));
    uint8_t* test2 = (uint8_t*)calloc(1500*200, sizeof(uint8_t));
    
    mergeAlpha(1500, 500, 150, 30, test1, test2);
    //mergeAlpha(1500, 500, 150, 5, test1, test2);
    //mergeAlpha(1500, 500, 150, 5, test1, test2);
    //mergeAlpha(1500, 500, 150, 5, test1, test2);

    router::get_instance()->kill_all();
    std::cout << "All done" << std::endl;
}