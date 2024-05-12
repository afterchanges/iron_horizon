#pragma once

#include "CoreMinimal.h"
#include "HexGridManager.h"
#include "HexTile.h"

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

TArray<AHexTile *> AHexGridManager::GetNeighbors(
    const FIntPoint &GridPositionIndex
) {
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

int HexDistance(FIntPoint start, FIntPoint end) {
    int dx = std::abs(start.X - end.X);
    int dy = std::abs(start.Y - end.Y);
    int dz = std::abs(-start.X - start.Y - (-end.X - end.Y));

    return std::max(dx, (std::max(dy, dz)));
}

struct FIntPointHash {
    std::size_t operator()(const FIntPoint &point) const {
        return std::hash<int>()(point.X) ^ std::hash<int>()(point.Y);
    }
};

struct HexTileNodeScoreComparator {
    bool operator()(HexTileNode& A, HexTileNode& B) const {
        return A.FScore > B.FScore;
    }
};

TArray<FIntPoint> AHexGridManager::HexGridAStar(AHexTile* StartTile, AHexTile* EndTile) {
    TArray<HexTileNode *> OpenSet;
    TMap<FIntPoint, HexTileNode *> OpenSetMap;
    TSet<FIntPoint> ClosedSet;

    HexTileNodeScoreComparator comparator;

    HexTileNode *StartNode =
        new HexTileNode(StartTile->GridPositionIndex, 0.0f, 0.0f, nullptr);
    OpenSet.Add(StartNode);
    OpenSet.Heapify(comparator);
    OpenSetMap.Add(StartTile->GridPositionIndex, StartNode);


    while (OpenSet.Num() > 0) {

        HexTileNode *CurrentNode = OpenSet[0];
        OpenSet.Heapify(comparator);
        OpenSet.HeapPop(CurrentNode, comparator, false);

        if (CurrentNode->GridPositionIndex == EndTile->GridPositionIndex) {
            TArray<FIntPoint> Path;
            HexTileNode *PathNode = CurrentNode;
            while (PathNode != nullptr) {
                Path.Insert(PathNode->GridPositionIndex, 0);
                PathNode = PathNode->Parent;
            }
            return Path;
        }

        ClosedSet.Add(CurrentNode->GridPositionIndex);
        TArray<AHexTile *> Neighbors = GetNeighbors(CurrentNode->GridPositionIndex);
        for (AHexTile *Neighbor : Neighbors) {
            if (ClosedSet.Contains(Neighbor->GridPositionIndex)) {
                continue;
            }

            float GScore = CurrentNode->GScore + 1; // assuming all edges have a weight of 1
            float HScore = HexDistance(Neighbor->GridPositionIndex, EndTile->GridPositionIndex);
            float FScore = GScore + HScore;

            HexTileNode *NeighborNode = OpenSetMap.FindRef(Neighbor->GridPositionIndex);
            if (NeighborNode == nullptr) {
                NeighborNode = new HexTileNode(
                    Neighbor->GridPositionIndex, GScore, FScore, CurrentNode
                );
                OpenSet.Add(NeighborNode);
                OpenSetMap.Add(Neighbor->GridPositionIndex, NeighborNode);
            } else if (GScore < NeighborNode->GScore) {
                NeighborNode->GScore = GScore;
                NeighborNode->FScore = FScore;
                NeighborNode->Parent = CurrentNode;
            }
        }
    }

    return TArray<FIntPoint>();
}