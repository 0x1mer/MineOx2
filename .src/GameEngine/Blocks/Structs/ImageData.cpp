#include "ImageData.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stbImage/stb_image.h>

ImageData::~ImageData() {
    if (data)
        stbi_image_free(data);
}