#pragma once
#include <QString>

struct Product {
    QString name;
    int sales = 0;   // popülerlik
    int volume = 0;  // hacim / yer kaplama
};
