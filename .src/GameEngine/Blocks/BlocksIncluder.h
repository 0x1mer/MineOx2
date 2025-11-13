#pragma once

#include "BlockRegistrationMacros.h"

/**
 * @file BlockRegistration.h
 * @brief Centralized block registration unit.
 *
 * This file is **auto-generated** and must be included **exactly once**
 * (typically in main initialization module) to register all available blocks
 * in the @ref BlockFactory.
 *
 * Each new block class you add should:
 * 1. Have its own header included here.
 * 2. Be registered with the @ref REGISTER_BLOCK macro.
 *
 * @note Do **not** include this file multiple times — it contains static registration
 * code that must run only once.
 */

#include "AirBlock.h"
#include "DirtBlock.h"
#include "GrassBlock.h"

/**
 * @brief Performs automatic registration of all available block types.
 *
 * These macros create static registrar objects that register the blocks
 * with the global @ref BlockFactory during program startup.
 */
REGISTER_BLOCK(AirBlock, BlockType::Air);
REGISTER_BLOCK(DirtBlock, BlockType::Dirt);
REGISTER_BLOCK(GrassBlock, BlockType::Grass);

// -----------------------------------------------------------------------------
// Safety check: detect multiple inclusion at compile-time
// -----------------------------------------------------------------------------
#ifdef BLOCK_REGISTRATION_INCLUDED
    static_assert(false,
        "❌ BlockRegistration.h included multiple times! "
        "This file must be included only once in the entire program.");
#else
    #define BLOCK_REGISTRATION_INCLUDED
#endif
