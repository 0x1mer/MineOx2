#pragma once

#include <string>
#include <vector>

/**
 * @brief Enumeration of all available block types in the game.
 *
 * Each value represents a specific kind of block that can exist in the world.
 * The order of declaration can be used for indexing or serialization if needed.
 */
enum class BlockType {
    Air,   /// Empty space, non-solid and invisible.
    Dirt,
    Grass,

    Count  /// Number of defined block types (used for iteration or validation).
};

/**
 * @brief Converts a BlockType to its string representation.
 *
 * @param block The block type to convert.
 * @return A lowercase string name of the block type.
 *
 * @note The function returns `"unknown"` for undefined or unsupported block types.
 */
inline std::string ToString(BlockType block) {
    switch (block) {
        case BlockType::Air:   return "air";
        case BlockType::Dirt:  return "dirt";
        case BlockType::Grass: return "grass";
        default:               return "unknown";
    }
}

inline std::vector<BlockType> AllBlockTypes() {
    std::vector<BlockType> result;
    for (int i = 0; i < static_cast<int>(BlockType::Count); ++i)
        result.push_back(static_cast<BlockType>(i));
    return result;
}