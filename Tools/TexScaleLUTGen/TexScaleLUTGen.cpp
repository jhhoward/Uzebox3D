#include <stdio.h>
#include <stdint.h>
#include <vector>
#include "Defines.h"

#define OUTPUT_PATH "../../DataHeaders/TextureScaleLUT.h"
#define OUTPUT_PATH_W2Z "../../DataHeaders/W2Z.h"
#define SCALER_LUT_SIZE (MAX_WALL_BUFFER_HEIGHT + 1)

#define MAX_PATCHES_PER_SCANLINE 10

#undef TEXTURE_HEIGHT
#define TEXTURE_HEIGHT 7

using namespace std;

uint8_t scalerLUT[SCALER_LUT_SIZE];

uint8_t generateScaleForSize(int scanline, int geo)
{
	float top = HALF_DISPLAYHEIGHT - geo;

	if((TEXTURE_HEIGHT & 0x1) == 0)
	{
		top = HALF_DISPLAYHEIGHT - geo * (1.0f + 1.0f / TEXTURE_HEIGHT);
	}

	float bottom = top + geo * 2;
	float v = (scanline - top) / (bottom - top);
	if(v < 0.0f)
	{
		return 0;
	}
	else if(v >= 1.0f)
	{
		return TEXTURE_SIZE + 1;
	}
	else
	{
		float sampleSize = TEXTURE_HEIGHT - 1;
		int samplePoint = (int)(v * sampleSize + 0.5f);
		uint8_t texLocation = (samplePoint * (TEXTURE_HEIGHT - 1)) / sampleSize;

		//texLocation &= 0xfe;
		/*if(geo <= 12)
		{
			return 1;
		}*/
	//	texLocation >>= 1;

		//uint8_t texLocation = (uint8_t)(v * (TEXTURE_SIZE));
		return texLocation + 1;
	}
}

struct TexScalerPatch
{
	uint8_t index;
	uint8_t newValue;
	int scanline;
};

void generateWtoZtable()
{
	FILE* fs = NULL;
	
	fopen_s(&fs, OUTPUT_PATH_W2Z, "w");
	
	if(fs)
	{
		fprintf(fs, "const int16_t W2Z[] PROGMEM = {\n");

		for(int n = 0; n < 256; n++)
		{
			fprintf(fs, "\t(int16_t)(Z_SCALE_MULTIPLIER / %d.5f),\n", n);
		}
		fprintf(fs, "};\n");
		fclose(fs);
	}
}

int main()
{
	for(int n = 0; n < SCALER_LUT_SIZE; n++)
	{
		scalerLUT[n] = generateScaleForSize(0, n);
	}

	int maxCount = 0;
	vector<TexScalerPatch> patchQueue;
	vector< vector<TexScalerPatch> > patches;

	for(int y = 1; y < DISPLAYHEIGHT; y++)
	{
		int count = 0;
		int expectedDifferences = 0;

		for(int n = 0; n < SCALER_LUT_SIZE; n++)
		{
			uint8_t updated = generateScaleForSize(y, n);
			if(updated != scalerLUT[n])
			{
				TexScalerPatch patch;
				patch.index = n;
				patch.newValue = updated;
				patch.scanline = y;
				patchQueue.push_back(patch);
				
				//scalerLUT[n] = updated;

				count++;
			}
			if(generateScaleForSize(y, n) != generateScaleForSize(y - 1, n))
			{
				expectedDifferences ++;
			}
		}

		vector<TexScalerPatch> patchesToApply;

		// remove duplicates
		for(int n = 0; n < patchQueue.size(); n++)
		{
			for(int c = n + 1; c < patchQueue.size(); c++)
			{
				if(patchQueue[n].index == patchQueue[c].index)
				{
					if(patchQueue[c].newValue > patchQueue[n].newValue)
					{
						patchQueue[n].newValue = patchQueue[c].newValue;
					}
					patchQueue.erase(patchQueue.begin() + c);
					c--;
				}
			}
		}

		// First find edges
		for(int n = 0; n < patchQueue.size() && patchesToApply.size() < MAX_PATCHES_PER_SCANLINE; n++)
		{
			if((scalerLUT[patchQueue[n].index] == 0 && patchQueue[n].newValue > 0) || patchQueue[n].newValue == TEXTURE_SIZE + 1)
			{
				patchesToApply.push_back(patchQueue[n]);
				patchQueue.erase(patchQueue.begin() + n);
				n--;
			}
		}

		// priority to long waiting patches
		for(int n = 0; n < patchQueue.size() && patchesToApply.size() < MAX_PATCHES_PER_SCANLINE; n++)
		{
			if(patchQueue[n].scanline < y)
			{
				patchesToApply.push_back(patchQueue[n]);
				patchQueue.erase(patchQueue.begin() + n);
				n--;
			}
		}

		// larger stepping patches
		for(int n = 0; n < patchQueue.size() && patchesToApply.size() < MAX_PATCHES_PER_SCANLINE; n++)
		{
			if(patchQueue[n].newValue > scalerLUT[patchQueue[n].index] + 1)
			{
				patchesToApply.push_back(patchQueue[n]);
				patchQueue.erase(patchQueue.begin() + n);
				n--;
			}
		}

		for(int n = 0; n < patchQueue.size() && patchesToApply.size() < MAX_PATCHES_PER_SCANLINE; n++)
		{
			patchesToApply.push_back(patchQueue[n]);
			patchQueue.erase(patchQueue.begin() + n);
			n--;
		}

		for(int n = 0; n < patchesToApply.size(); n++)
		{
			scalerLUT[patchesToApply[n].index] = patchesToApply[n].newValue;
		}

		int errorCount = 0;
		int bigErrorCount = 0;

		for(int n = 0; n < SCALER_LUT_SIZE; n++)
		{
			uint8_t correctScale = generateScaleForSize(y, n);
			if(correctScale != scalerLUT[n])
			{
				errorCount ++;
			}
			if(correctScale > scalerLUT[n] + 1)
			{
				printf("Size: %d expected: %d got: %d\n", n, correctScale, scalerLUT[n]);
				bigErrorCount ++;
			}
		}

		patches.push_back(patchesToApply);

		printf("Scanline: %d Differences: %d Expected: %d Patches applied: %d Errors: %d (%d>1)\n", y, count, expectedDifferences, patchesToApply.size(), errorCount, bigErrorCount);
		if(count > maxCount)
			maxCount = count;
	}

	printf("Max diff: %d\n", maxCount);

	FILE* fs = NULL;
	
	fopen_s(&fs, OUTPUT_PATH, "w");
	
	if(fs)
	{
		fprintf(fs, "#define PATCHES_PER_SCANLINE %d\n\n", MAX_PATCHES_PER_SCANLINE);

		fprintf(fs, "const uint8_t TextureScaleLUT[] PROGMEM = {\n\t");

		for(int n = 0; n < SCALER_LUT_SIZE; n++)
		{
			uint8_t scale = generateScaleForSize(0, n);
			fprintf(fs, "0x%02x, ", scale);
		}
		fprintf(fs, "\n\t");

		for(int y = 0; y < patches.size(); y++)
		{
			for(int n = 0; n < MAX_PATCHES_PER_SCANLINE; n++)
			{
				if(n < patches[y].size())
				{
					fprintf(fs, "0x%02x, 0x%02x, ", patches[y][n].index, patches[y][n].newValue);
				}
				else
				{
					uint8_t scale = generateScaleForSize(y + 1, 0);
					fprintf(fs, "0x%02x, 0x%02x, ", 0, scale);
				}
			}
			fprintf(fs, "\n\t");
		}

		fprintf(fs, "\n};\n");
		fclose(fs);
	}

	generateWtoZtable();

	return 0;
}


