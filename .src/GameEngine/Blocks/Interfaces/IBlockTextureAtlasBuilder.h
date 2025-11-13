#pragma once

#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include <utility> // std::pair

struct TextureRegion {
    glm::vec2 uvMin;
    glm::vec2 uvMax;
};

class IBlockTextureAtlasBuilder {
public:
    virtual ~IBlockTextureAtlasBuilder() = default;

    virtual void AddTexture(const std::string& textureName,
                            const std::filesystem::path& texturePath) = 0;

    virtual void BuildAtlas() = 0;

    virtual void SaveAtlas(const std::filesystem::path& outputPath) const = 0;

    virtual TextureRegion GetTextureRegion(const std::string& blockName) const = 0;

    virtual std::pair<int, int> GetAtlasSize() const = 0;
};