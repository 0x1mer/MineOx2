#pragma once

#include "IBlockTextureAtlasBuilder.h"
#include <unordered_map>

class BlockTextureAtlasBuilder : public IBlockTextureAtlasBuilder {
public:
    BlockTextureAtlasBuilder() = default;
    ~BlockTextureAtlasBuilder() override = default;

    void AddTexture(const std::string& textureName,
                    const std::filesystem::path& texturePath) override {
                        
                    }

    void BuildAtlas() override;

    void SaveAtlas(const std::filesystem::path& outputPath) const override;

    TextureRegion GetTextureRegion(const std::string& blockName) const override;

    std::pair<int, int> GetAtlasSize() const override;

private:
    std::unordered_map<std::string, TextureRegion> regions_; // UV-координаты текстур
    int atlasWidth_ = 0;
    int atlasHeight_ = 0;
};