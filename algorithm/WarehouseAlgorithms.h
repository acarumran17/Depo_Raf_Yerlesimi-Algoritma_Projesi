#pragma once
#include <vector>
#include <QString>

#include "../model/Product.h"
#include "../model/Shelf.h"

struct PlacementResult {
    std::vector<Shelf> shelves;
    double efficiency = 0.0;

    long long elapsedMs = 0;
    long long elapsedUs = 0;
};

struct DPKnapsackResult {
    PlacementResult placement;
    std::vector<std::vector<int>> dpTable;      // 1. raf DP tablosu
    std::vector<Product> chosenForFirstShelf;   // 1. raf seçilenler
};

struct SearchResult {
    bool found = false;
    int index = -1;
    long long elapsedUs = 0;
};

class WarehouseAlgorithms {
public:
    static double computeWarehouseEfficiency(const std::vector<Shelf>& shelves);

    // Veri
    static std::vector<Product> generateProducts(int count);

    // Yerleşim
    static PlacementResult staticPlacement(const std::vector<Product>& products, int shelfCount, int shelfCap);
    static PlacementResult greedyPlacement(std::vector<Product> products, int shelfCount, int shelfCap);
    static DPKnapsackResult dpPlacementKnapsack(std::vector<Product> products, int shelfCount, int shelfCap);

    // Sorting
    static void sortBySalesDesc(std::vector<Product>& products);
    static void sortByNameAsc(std::vector<Product>& products);

    // Searching
    static SearchResult linearSearchByName(const std::vector<Product>& products, const QString& name);
    static SearchResult binarySearchByName(const std::vector<Product>& productsSortedByName, const QString& name);

    // ASCII depo haritası
    static QString buildAsciiMap(const std::vector<Shelf>& shelves);
};
