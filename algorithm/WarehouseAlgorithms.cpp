#include "WarehouseAlgorithms.h"
#include "../model/Shelf.h"
#include "../model/Product.h"

#include <random>
#include <algorithm>
#include <chrono>

// -------------------- helpers
static long long nowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}
static long long nowUs() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

// -------------------- DOĞRU ve TEK verimlilik hesabı
// %100 üstü ASLA üretmez
double WarehouseAlgorithms::computeWarehouseEfficiency(const std::vector<Shelf>& shelves) {
    long long used = 0;
    long long capacity = 0;

    for (const auto& s : shelves) {
        used += s.used;
        capacity += s.capacity;
    }

    if (capacity == 0) return 0.0;
    return (double)used * 100.0 / (double)capacity;
}

// -------------------- Data
std::vector<Product> WarehouseAlgorithms::generateProducts(int count) {
    static std::vector<QString> names = {
            "Laptop","Telefon","Tablet","Monitor","Klavye","Mouse","Yazici","SSD",
            "HDD","Router","Switch","Kamera","Hoparlor","Kulaklik","Powerbank",
            "Adaptor","Kablo","Drone","Projeksiyon","Konsol"
    };

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> salesDist(50, 300);
    std::uniform_int_distribution<int> volumeDist(1, 8);
    std::uniform_int_distribution<int> nameDist(0, (int)names.size() - 1);

    std::vector<Product> products;
    products.reserve(count);

    for (int i = 0; i < count; ++i) {
        Product p;
        p.name   = names[nameDist(rng)] + "_" + QString::number(i + 1);
        p.sales  = salesDist(rng);
        p.volume = volumeDist(rng);
        products.push_back(p);
    }
    return products;
}

// -------------------- Placement: Static
PlacementResult WarehouseAlgorithms::staticPlacement(const std::vector<Product>& products, int shelfCount, int shelfCap) {
    long long t0ms = nowMs();
    long long t0us = nowUs();

    std::vector<Shelf> shelves(shelfCount);
    for (auto& s : shelves) { s.capacity = shelfCap; s.used = 0; }

    int idx = 0;
    for (const auto& p : products) {
        int tries = 0;
        while (tries < shelfCount) {
            int s = idx % shelfCount;
            if (shelves[s].used + p.volume <= shelves[s].capacity) {
                shelves[s].products.push_back(p);
                shelves[s].used += p.volume;
                idx++;
                break;
            }
            idx++;
            tries++;
        }
    }

    long long t1ms = nowMs();
    long long t1us = nowUs();

    PlacementResult out;
    out.shelves   = shelves;
    out.elapsedMs = (t1ms - t0ms);
    out.elapsedUs = (t1us - t0us);
    return out;
}

// -------------------- Sorting
void WarehouseAlgorithms::sortBySalesDesc(std::vector<Product>& products) {
    std::sort(products.begin(), products.end(),
              [](const Product& a, const Product& b) {
                  return a.sales > b.sales;
              });
}
void WarehouseAlgorithms::sortByNameAsc(std::vector<Product>& products) {
    std::sort(products.begin(), products.end(),
              [](const Product& a, const Product& b) {
                  return a.name.toLower() < b.name.toLower();
              });
}

// -------------------- Placement: Greedy
PlacementResult WarehouseAlgorithms::greedyPlacement(std::vector<Product> products, int shelfCount, int shelfCap) {
    long long t0ms = nowMs();
    long long t0us = nowUs();

    sortBySalesDesc(products);

    std::vector<Shelf> shelves(shelfCount);
    for (auto& s : shelves) { s.capacity = shelfCap; s.used = 0; }

    for (const auto& p : products) {
        for (int i = 0; i < shelfCount; ++i) {
            if (shelves[i].used + p.volume <= shelves[i].capacity) {
                shelves[i].products.push_back(p);
                shelves[i].used += p.volume;
                break;
            }
        }
    }

    long long t1ms = nowMs();
    long long t1us = nowUs();

    PlacementResult out;
    out.shelves   = shelves;
    out.elapsedMs = (t1ms - t0ms);
    out.elapsedUs = (t1us - t0us);
    return out;
}

// -------------------- DP Knapsack placement
DPKnapsackResult WarehouseAlgorithms::dpPlacementKnapsack(std::vector<Product> products, int shelfCount, int shelfCap) {
    long long t0ms = nowMs();
    long long t0us = nowUs();

    std::vector<Shelf> shelves(shelfCount);
    for (auto& s : shelves) { s.capacity = shelfCap; s.used = 0; }

    std::vector<std::vector<int>> firstDp;
    std::vector<Product> firstChosen;

    for (int shelfIdx = 0; shelfIdx < shelfCount; ++shelfIdx) {
        int n = (int)products.size();
        if (n == 0) break;

        std::vector<std::vector<int>> dp(n + 1, std::vector<int>(shelfCap + 1, 0));

        for (int i = 1; i <= n; ++i) {
            int w = products[i - 1].volume;
            int v = products[i - 1].sales;
            for (int cap = 0; cap <= shelfCap; ++cap) {
                dp[i][cap] = dp[i - 1][cap];
                if (w <= cap) {
                    dp[i][cap] = std::max(dp[i][cap],
                                           dp[i - 1][cap - w] + v);
                }
            }
        }

        int cap = shelfCap;
        std::vector<int> pickedIdx;
        for (int i = n; i >= 1; --i) {
            if (dp[i][cap] != dp[i - 1][cap]) {
                pickedIdx.push_back(i - 1);
                cap -= products[i - 1].volume;
            }
        }

        int used = 0;
        for (int idx : pickedIdx) {
            shelves[shelfIdx].products.push_back(products[idx]);
            used += products[idx].volume;
        }
        shelves[shelfIdx].used = used;

        if (shelfIdx == 0) {
            firstDp = dp;
            for (int idx : pickedIdx)
                firstChosen.push_back(products[idx]);
        }

        std::sort(pickedIdx.begin(), pickedIdx.end());
        for (int i = (int)pickedIdx.size() - 1; i >= 0; --i) {
            products.erase(products.begin() + pickedIdx[i]);
        }
    }

    long long t1ms = nowMs();
    long long t1us = nowUs();

    DPKnapsackResult out;
    out.placement.shelves   = shelves;
    out.placement.elapsedMs = (t1ms - t0ms);
    out.placement.elapsedUs = (t1us - t0us);

    out.dpTable = firstDp;
    out.chosenForFirstShelf = firstChosen;
    return out;
}

// -------------------- Searching
SearchResult WarehouseAlgorithms::linearSearchByName(const std::vector<Product>& products, const QString& name) {
    long long t0 = nowUs();
    for (int i = 0; i < (int)products.size(); ++i) {
        if (products[i].name.compare(name, Qt::CaseInsensitive) == 0) {
            long long t1 = nowUs();
            return { true, i, (t1 - t0) };
        }
    }
    long long t1 = nowUs();
    return { false, -1, (t1 - t0) };
}

SearchResult WarehouseAlgorithms::binarySearchByName(const std::vector<Product>& productsSortedByName, const QString& name) {
    long long t0 = nowUs();

    int l = 0, r = (int)productsSortedByName.size() - 1;
    QString key = name.toLower();

    while (l <= r) {
        int mid = (l + r) / 2;
        QString cur = productsSortedByName[mid].name.toLower();
        if (cur == key) {
            long long t1 = nowUs();
            return { true, mid, (t1 - t0) };
        }
        if (cur < key) l = mid + 1;
        else r = mid - 1;
    }

    long long t1 = nowUs();
    return { false, -1, (t1 - t0) };
}

// -------------------- ASCII Map
QString WarehouseAlgorithms::buildAsciiMap(const std::vector<Shelf>& shelves) {
    QString out;
    out += "=========== ASCII DEPO HARITASI ===========\n";
    for (int i = 0; i < (int)shelves.size(); ++i) {
        const auto& s = shelves[i];
        out += QString("Raf %1 | Kapasite:%2 | Kullanilan:%3 | Bos:%4\n")
                .arg(i + 1).arg(s.capacity).arg(s.used).arg(s.capacity - s.used);

        int barLen = 30;
        int filled = (s.capacity == 0) ? 0 : (int)((double)s.used / s.capacity * barLen);
        out += "[";
        for (int k = 0; k < barLen; ++k) out += (k < filled ? "#" : ".");
        out += "]\n";

        for (const auto& p : s.products) {
            out += QString("  - %1 (Satis:%2, Hacim:%3)\n")
                    .arg(p.name).arg(p.sales).arg(p.volume);
        }
        out += "\n";
    }
    return out;
}
