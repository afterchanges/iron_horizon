// TIntPointHash.h
#pragma once

#include <functional>  // for std::hash

namespace std {
template <>
struct hash<UE::Math::TIntPoint<int>> {
  size_t operator()(const UE::Math::TIntPoint<int> &k) const {
    // Compute individual hash values for two elements in pair and combine them
    size_t h1 = std::hash<int>()(k.X);
    size_t h2 = std::hash<int>()(k.Y);

    return h1 ^ (h2 << 1);
  }
};
}  // namespace std