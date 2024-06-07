#pragma once

#include "CoreMinimal.h"
#include "HexGridManager.h"
#include "HexTile.h"

struct HexTileNode {
    FIntVector CubeCoordinates;
    float GScore;
    float FScore;
    HexTileNode* Parent;

    HexTileNode(const FIntVector& InCubeCoordinates, float InGScore, float InFScore, HexTileNode* InParent)
        : CubeCoordinates(InCubeCoordinates), GScore(InGScore), FScore(InFScore), Parent(InParent) {}
};

struct HexTileNodeComparator {
    bool operator()(const HexTileNode& A, const HexTileNode& B) const {
        return A.FScore > B.FScore;
    }
};

TArray<AHexTile*> AHexGridManager::GetPassableNeighbors(const FIntVector& CubeCoordinates) {
    TArray<AHexTile*> Neighbors;
    TArray<FIntVector> NeighborOffsets = {
        FIntVector(1, -1, 0), FIntVector(1, 0, -1), FIntVector(0, 1, -1),
        FIntVector(-1, 1, 0), FIntVector(-1, 0, 1), FIntVector(0, -1, 1)
    };

    UE_LOG(LogTemp, Warning, TEXT("Cube coordinates are %d %d %d"), CubeCoordinates.X, CubeCoordinates.Y, CubeCoordinates.Z);
    UE_LOG(LogTemp, Warning, TEXT("Grid coordinates are %d %d"), GetTileAtCubeCoordinates(CubeCoordinates)->GridPositionIndex.X, GetTileAtCubeCoordinates(CubeCoordinates)->GridPositionIndex.Y);
    UE_LOG(LogTemp, Warning, TEXT("Tile type is %d"), GetTileAtCubeCoordinates(CubeCoordinates)->GetTileType());
    for (const FIntVector& Offset : NeighborOffsets) {
        FIntVector NeighborPosition = CubeCoordinates + Offset;
        AHexTile* NeighborTile = GetTileAtCubeCoordinates(NeighborPosition);
        if (NeighborTile != nullptr) {
            UE_LOG(LogTemp, Warning, TEXT("Neighbor position is %d %d %d"), NeighborPosition.X, NeighborPosition.Y, NeighborPosition.Z);
            UE_LOG(LogTemp, Warning, TEXT("Neighbor grid position is %d %d"), GetTileAtCubeCoordinates(NeighborPosition)->GridPositionIndex.X, GetTileAtCubeCoordinates(NeighborPosition)->GridPositionIndex.Y);
            UE_LOG(LogTemp, Warning, TEXT("Neighbor type is %d"), NeighborTile->GetTileType());
        }
        if (NeighborTile != nullptr && (NeighborTile->GetTileType() == HexTileType::RAILWAY || NeighborTile->GetTileType() == HexTileType::CITY)) {
            Neighbors.Add(NeighborTile);
        }
    }

    return Neighbors;
}

int HexDistance(FIntVector start, FIntVector end) {
    return (FMath::Abs(start.X - end.X) + FMath::Abs(start.Y - end.Y) + FMath::Abs(start.Z - end.Z)) / 2;
}

TArray<AHexTile*> AHexGridManager::HexGridAStar(AHexTile* StartTile, AHexTile* EndTile) {
    TArray<HexTileNode*> OpenSet;
    TMap<FIntVector, HexTileNode*> OpenSetMap;
    TSet<FIntVector> ClosedSet;

    HexTileNodeComparator Comparator;
    HexTileNode* StartNode = new HexTileNode(StartTile->CubeCoordinates, 0.0f, HexDistance(StartTile->CubeCoordinates, EndTile->CubeCoordinates), nullptr);
    OpenSet.HeapPush(StartNode, Comparator);
    OpenSetMap.Add(StartTile->CubeCoordinates, StartNode);

    while (OpenSet.Num() > 0) {
        HexTileNode* CurrentNode = nullptr;
        OpenSet.HeapPop(CurrentNode, Comparator);

        if (CurrentNode->CubeCoordinates == EndTile->CubeCoordinates) {
            TArray<FIntVector> Path;
            HexTileNode* PathNode = CurrentNode;
            while (PathNode != nullptr) {
                Path.Insert(PathNode->CubeCoordinates, 0);
                PathNode = PathNode->Parent;
            }
            TArray<AHexTile*> PathTiles;
            for (const FIntVector& Node : Path) {
                AHexTile* Tile = GetTileAtCubeCoordinates(Node);
                if (Tile) {
                    PathTiles.Add(Tile);
                }
            }
            return PathTiles;
        }

        ClosedSet.Add(CurrentNode->CubeCoordinates);
        TArray<AHexTile*> Neighbors = GetPassableNeighbors(CurrentNode->CubeCoordinates);

        for (AHexTile* Neighbor : Neighbors) {
            if (ClosedSet.Contains(Neighbor->CubeCoordinates)) {
                continue;
            }

            float NewGScore = CurrentNode->GScore + 1;
            float NewHScore = HexDistance(Neighbor->CubeCoordinates, EndTile->CubeCoordinates);
            float NewFScore = NewGScore + NewHScore;
            UE_LOG(LogTemp, Warning, TEXT("Current Node: %s GScore: %f, FScore: %f"), *CurrentNode->CubeCoordinates.ToString(), CurrentNode->GScore, CurrentNode->FScore);


            HexTileNode** NeighborNodePtr = OpenSetMap.Find(Neighbor->CubeCoordinates);
            if (NeighborNodePtr == nullptr) {
                HexTileNode* NeighborNode = new HexTileNode(Neighbor->CubeCoordinates, NewGScore, NewFScore, CurrentNode);
                OpenSet.HeapPush(NeighborNode, Comparator);
                OpenSetMap.Add(Neighbor->CubeCoordinates, NeighborNode);
            } else {
                HexTileNode* NeighborNode = *NeighborNodePtr;
                if (NewGScore < NeighborNode->GScore) {
                    NeighborNode->GScore = NewGScore;
                    NeighborNode->FScore = NewFScore;
                    NeighborNode->Parent = CurrentNode;
                    OpenSet.Heapify(Comparator);
                }
            }
        }
        UE_LOG(LogTemp, Warning, TEXT("Open set size: %d"), OpenSet.Num());
        UE_LOG(LogTemp, Warning, TEXT("next astar step"));
    }

    return TArray<AHexTile*>();
}
