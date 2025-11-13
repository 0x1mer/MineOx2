#pragma once

#include "IBlock.h"

/**
 * @brief Interface for immutable (static) blocks.
 *
 * Represents blocks that do not change their state during gameplay.
 * Such blocks have constant properties (e.g. texture, shape, hardness)
 * and require no ticking or property updates.
 *
 * Static blocks are usually represented by a single shared instance
 * that is created once during engine initialization and reused
 * across the entire world. This means multiple identical block
 * positions all reference the same object in memory, improving
 * performance and reducing memory usage.
 *
 * @note This interface is mutually exclusive with @ref IPropertyBlock.  
 *       A block should implement either @ref IStaticBlock or @ref IPropertyBlock, but not both.
 * 
 */
class IStaticBlock : public IBlock
{
protected:
    IStaticBlock() = default;

public:
    virtual ~IStaticBlock() = default;
};