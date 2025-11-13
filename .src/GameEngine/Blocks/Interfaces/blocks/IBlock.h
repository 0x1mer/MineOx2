#pragma once

#include "BlockTypes.h"

/**
 * @brief Base interface for all block types.
 *
 * Defines the common behavior shared by every block in the game world.
 *
 * @note This class must not be instantiated directly.  
 *       Use one of its derived interfaces (e.g. @ref IStaticBlock.h, @ref IPropertyBlock.h) instead.
 */
class IBlock
{
protected:
    IBlock() = default;

    friend class IPropertyBlock;
    friend class IStaticBlock;
    
public:
    virtual ~IBlock() = default;

    /**
     * @brief Returns the unique name of the block.
     *
     * The name is used across multiple systems (e.g. JSON linking, registration, or serialization)
     * to identify the block.
     *
     * @return A string representing the block's name.
     */
    virtual std::string GetName() const = 0;

    /**
     * @brief Returns the logical type of the block.
     *
     * This corresponds to the block ID in the game (see @ref BlockType enum),
     * and is not related to physical properties such as transparency or solidity.
     *
     * @return The block's type identifier.
     */
    virtual BlockType GetType() const = 0;
};