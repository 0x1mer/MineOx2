#pragma once

#include "IStaticBlock.h"

/**
 * @brief Represents the air block — an empty, non-solid block in the world.
 *
 * The AirBlock is used to represent empty space where no physical block exists.  
 * It does not interact with the environment, has no collision, and cannot be modified by players.
 *
 * @note This class implements @ref IStaticBlock since its state never changes.  
 *       Only one static instance of this block type typically exists in the game.  
 *       It is often used as a placeholder for "no block" or transparent space.
 */
class AirBlock : public IStaticBlock {
private:
    /// @brief The block type identifier for Air.
    static constexpr BlockType blockType = BlockType::Air;

public:
    /**
     * @brief Returns the string name of the block.
     *
     * Used for internal serialization, debug output, or JSON mappings.
     *
     * @return The lowercase string `"air"`.
     */
    std::string GetName() const override {
        return ToString(blockType);
    }

    /**
     * @brief Returns the type of this block.
     *
     * @return BlockType::Air
     */
    BlockType GetType() const override {
        return blockType;
    }
};