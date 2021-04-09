#include "TaskManager.hpp"

TaskManager tmngr;

void mergeBlock(merge_data* data)
{
    uint8_t* src_rgba = data->pRGBABlock;
    uint8_t* src_alpha = data->pAlphaBlock;

    for (size_t y = 0; y < data->block_height; ++y)
    {
        for (size_t x = 0; x < data->bytes_per_row;)
        {
            uint8_t& rAlpha1 = *(src_rgba + 3);
            uint8_t& rAlpha2 = *src_alpha;

            //rAlpha1 = rAlpha2;

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
        tmngr.add_task(mergeBlock, &data);
	}

    while(!tmngr.is_all_tasks_done());
}

int main()
{
    uint8_t* test1 = (uint8_t*)calloc(150*200, sizeof(uint8_t));
    uint8_t* test2 = (uint8_t*)calloc(150*200, sizeof(uint8_t));

    mergeAlpha(150, 500, 150, 6, test1, test2);
    mergeAlpha(150, 500, 150, 3, test1, test2);
}