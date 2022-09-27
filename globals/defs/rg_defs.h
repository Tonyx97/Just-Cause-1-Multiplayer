#pragma once

inline constexpr int16_t RG_WORLD_CHUNK_COUNT = 64;
inline constexpr uint16_t RG_WORLD_CHUNK_SIZE = 512u;
inline constexpr uint8_t RG_WORLD_CHUNK_RADIUS = 3;
inline constexpr float RG_WORLD_MAX_SQUARE_DISTANCE_IN_CHUNK =	(static_cast<float>(RG_WORLD_CHUNK_SIZE) *
																static_cast<float>(RG_WORLD_CHUNK_SIZE) +
																static_cast<float>(RG_WORLD_CHUNK_SIZE) *
																static_cast<float>(RG_WORLD_CHUNK_SIZE)) * static_cast<float>(RG_WORLD_CHUNK_RADIUS * RG_WORLD_CHUNK_RADIUS);