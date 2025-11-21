#pragma once
#include <d3d12.h>
#include "stb_truetype.h"

namespace UiKit {

    struct BitmapFont {
        void* textureId = nullptr;
        unsigned char* atlasPixels = nullptr;
        stbtt_bakedchar charData[96];
        int atlasWidth = 512;
        int atlasHeight = 512;
        float fontSize = 18.0f;
        
        inline void LoadFromTTF(const unsigned char* ttfData, int dataSize, float size, ID3D12Device* device) {
            fontSize = size;
            
            if (atlasPixels) delete[] atlasPixels;
            atlasPixels = new unsigned char[atlasWidth * atlasHeight];
            
            stbtt_BakeFontBitmap(ttfData, 0, fontSize, atlasPixels, atlasWidth, atlasHeight, 32, 96, charData);
            
            textureId = (void*)1;
        }
        
        inline bool GetPixel(int x, int y) {
            if (x < 0 || y < 0 || x >= atlasWidth || y >= atlasHeight) return false;
            return atlasPixels[y * atlasWidth + x] > 128;
        }
        
        inline unsigned char GetPixelAlpha(int x, int y) {
            if (x < 0 || y < 0 || x >= atlasWidth || y >= atlasHeight) return 0;
            return atlasPixels[y * atlasWidth + x];
        }
        
        ~BitmapFont() {
            if (atlasPixels) delete[] atlasPixels;
        }
    };

}