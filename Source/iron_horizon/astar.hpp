#pragma once

#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "CoreMinimal.h"

struct HexTileNode {
    FIntPoint GridPositionIndex;
    float GScore;
    float FScore;
    HexTileNode *Parent;

    HexTileNode(
        const FIntPoint &GridPositionIndex,
        float GScore,
        float FScore,
        HexTileNode *Parent
    )
        : GridPositionIndex(GridPositionIndex),
          GScore(GScore),
          FScore(FScore),
          Parent(Parent) {
    }
};

TArray<AHexTile *> GetNeighbors(const FIntPoint &GridPositionIndex) {
        TArray<AHexTile *> Neighbors;
        FIntPoint Neighbor1(GridPositionIndex.X + 1, GridPositionIndex.Y);
        AHexTile *NeighborTile1 = GetTileAtPosition(Neighbor1);
        if (NeighborTile1 != nullptr) {
            Neighbors.Add(NeighborTile1);
        }
        FIntPoint Neighbor2(GridPositionIndex.X - 1, GridPositionIndex.Y);
        AHexTile *NeighborTile2 = GetTileAtPosition(Neighbor2);
        if (NeighborTile2 != nullptr) {
            Neighbors.Add(NeighborTile2);
        }
        FIntPoint Neighbor3(GridPositionIndex.X, GridPositionIndex.Y + 1);
        AHexTile *NeighborTile3 = GetTileAtPosition(Neighbor3);
        if (NeighborTile3 != nullptr) {
            Neighbors.Add(NeighborTile3);
        }
        FIntPoint Neighbor4(GridPositionIndex.X, GridPositionIndex.Y - 1);
        AHexTile *NeighborTile4 = GetTileAtPosition(Neighbor4);
        if (NeighborTile4 != nullptr) {
            Neighbors.Add(NeighborTile4);
        }
        FIntPoint Neighbor5(GridPositionIndex.X + 1, GridPositionIndex.Y - 1);
        AHexTile *NeighborTile5 = GetTileAtPosition(Neighbor5);
        if (NeighborTile5 != nullptr) {
            Neighbors.Add(NeighborTile5);
        }
        FIntPoint Neighbor6(GridPositionIndex.X - 1, GridPositionIndex.Y + 1);
        AHexTile *NeighborTile6 = GetTileAtPosition(Neighbor6);
        if (NeighborTile6 != nullptr) {
            Neighbors.Add(NeighborTile6);
        }

        return Neighbors;
    }
    return Neighbors;
}

int HexDistance(FIntPoint start, FIntPoint end) {
    int isStartRowEven = start.X % 2;
    int isEndRowEven = end.X % 2;

    int dx = std::abs(start.X - end.X);
    int dy = std::abs(start.Y - end.Y);

    if (isStartRowEven) {
        if (isEndRowEven) {
            return std::max(dx, std::max(dy, dx + dy / 2));
        } else {
            return std::max(dx, std::max(dy, dx + (dy + 1) / 2));
        }
    } else {
        if (isEndRowEven) {
            return std::max(dx, std::max(dy, dx + (dy + 1) / 2));
        } else {
            return std::max(dx, std::max(dy, dx + dy / 2));
        }
    }
}

struct HexTileNodeScoreComparator {
    bool operator()(const HexTileNode *A, const HexTileNode *B) const {
        return A->FScore > B->FScore;
    }
};

TArray<FIntPoint> HexGridAStar(AHexTile *StartTile, AHexTile *GoalTile) {
    std::priority_queue<
        HexTileNode *, std::vector<HexTileNode *>, HexTileNodeScoreComparator>
        OpenSet;
    std::unordered_map<FIntPoint, HexTileNode *> OpenSetMap;
    std::unordered_set<FIntPoint> ClosedSet;

    HexTileNode *StartNode =
        new HexTileNode(StartTile->GridPositionIndex, 0.0f, 0.0f, nullptr);
    OpenSet.push(StartNode);
    OpenSetMap[StartTile->GridPositionIndex] = StartNode;

    while (!OpenSet.empty()) {
        HexTileNode *CurrentNode = OpenSet.top();
        OpenSet.pop();

        if (CurrentNode->GridPositionIndex == GoalTile->GridPositionIndex) {
            TArray<FIntPoint> Path;
            while (CurrentNode != nullptr) {
                Path.Insert(CurrentNode->GridPositionIndex, 0);
                CurrentNode = CurrentNode->Parent;
            }
            return Path;
        }

        ClosedSet.insert(CurrentNode->GridPositionIndex);
        TArray<AHexTile *> Neighbors =
            GetNeighbors(CurrentNode->GridPositionIndex);
        for (AHexTile *Neighbor : Neighbors) {
            if (ClosedSet.find(Neighbor->GridPositionIndex) !=
                ClosedSet.end()) {
                continue;
            }

            float GScore =
                CurrentNode->GScore +
                HexDistance(
                    CurrentNode->GridPositionIndex, Neighbor->GridPositionIndex
                );
            float HScore = HexDistance(
                Neighbor->GridPositionIndex, GoalTile->GridPositionIndex
            );
            float FScore = GScore + HScore;

            HexTileNode *NeighborNode = OpenSetMap[Neighbor->GridPositionIndex];
            if (NeighborNode == nullptr) {
                NeighborNode = new HexTileNode(
                    Neighbor->GridPositionIndex, GScore, FScore, CurrentNode
                );
                OpenSet.push(NeighborNode);
                OpenSetMap[Neighbor->GridPositionIndex] = NeighborNode;
            } else if (GScore < NeighborNode->GScore) {
                NeighborNode->GScore = GScore;
                NeighborNode->FScore = FScore;
                NeighborNode->Parent = CurrentNode;
            }
        }
    }

    return TArray<FIntPoint>();
}
