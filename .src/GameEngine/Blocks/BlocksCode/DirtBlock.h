#pragma once

#include "IPropertyBlock.h"

/**
 * @brief Represents the dirt block — a basic terrain block that can hold mutable properties.
 *
 * The DirtBlock is a simple ground block that may have properties modified at runtime
 * (for example, moisture level or composition in future mechanics).
 * It derives from @ref IPropertyBlock since its state can change during gameplay.
 *
 * @note Unlike @ref IStaticBlock types, each DirtBlock instance can store its own unique properties.
 *       This allows multiple dirt blocks to exist with different configurations.
 */
class DirtBlock : public IPropertyBlock
{
private:
    /// @brief The block type identifier for Dirt.
    static constexpr BlockType blockType = BlockType::Dirt;

    /// @brief String representing the block's current properties (e.g. `"{moisture}:{0.8}"`).
    std::string properties_;

public:
    DirtBlock() {}
    /**
     * @brief Constructs a dirt block with given properties.
     *
     * @param[in] properties  – A string containing block properties in the format
     *                       `"{property}:{value},..."`, separated by commas.
     */
    explicit DirtBlock(const std::string &properties)
        : properties_(properties) {}

    /**
     * @brief Returns the string name of the block.
     *
     * Used for internal serialization, debug output, or JSON mappings.
     *
     * @return The lowercase string `"dirt"`.
     */
    std::string GetName() const override
    {
        return ToString(blockType);
    }

    /**
     * @brief Returns the block type.
     *
     * @return BlockType::Dirt
     */
    BlockType GetType() const override
    {
        return blockType;
    }

    /**
     * @brief Retrieves the current block properties.
     *
     * @return String containing all properties (e.g. `"{moisture}:{0.8},{color}:{brown}"`).
     */
    std::string GetBlockProperties() const override
    {
        return properties_;
    }

    /**
     * @brief Updates the block properties.
     *
     * Replaces the current property string with a new one.
     * The input must follow the same format as @ref GetBlockProperties(), for example:
     * ```
     * {hardness}:{5},{flammable}:{true}
     * ```
     *
     * @param[in] properties New property string in the same format as @ref GetBlockProperties().
     * @return `true` if the new properties were applied successfully.
     */
    bool SetBlockProperties(const std::string &properties) override
    {
        properties_ = properties;
        return true;
    }
};