#pragma once

#include "IStaticBlock.h"

/**
 * @brief Represents the grass block — a natural surface block found on top of dirt.
 *
 * The GrassBlock is a decorative terrain block that visually represents grass-covered ground.  
 * It does not change its state during gameplay and requires no ticking or updates.  
 * Typically used for surface layers in the world.
 *
 * @note Implements @ref IStaticBlock since its state and properties remain constant.  
 *       Usually, a single shared instance of this block type is used globally.
 */
class GrassBlock : public IStaticBlock {
private:
    /// @brief The block type identifier for Grass.
    static constexpr BlockType blockType = BlockType::Grass;

public:
    /**
     * @brief Returns the string name of the block.
     *
     * Used for internal serialization, debug output, or JSON mappings.
     *
     * @return The lowercase string `"grass"`.
     */
    std::string GetName() const override {
        return ToString(blockType);
    }

    /**
     * @brief Returns the type of this block.
     *
     * @return BlockType::Grass
     */
    BlockType GetType() const override {
        return blockType;
    }
};