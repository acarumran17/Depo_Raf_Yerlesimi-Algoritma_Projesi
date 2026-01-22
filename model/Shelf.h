#pragma once
#include <vector>
#include "Product.h"

struct Shelf {
    int capacity = 0;
    int used = 0;
    std::vector<Product> products;
};
