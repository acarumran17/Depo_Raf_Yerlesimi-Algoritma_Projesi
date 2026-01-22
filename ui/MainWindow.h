#pragma once

#include <QMainWindow>
#include <QTableWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>

#include "../model/Shelf.h"
#include "../model/Product.h"
#include "../algorithm/WarehouseAlgorithms.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
    explicit MainWindow(QWidget* parent = nullptr);

    private slots:
        void regenerateProducts();
    void runSearch();
    void runDepotEfficiency();
    void runStatic();
    void runGreedy();
    void runDP();
    void runPerformance();
    void runAsciiMap();

private:
    // ================= UI =================
    QLineEdit*   searchEdit = nullptr;
    QComboBox*   algoCombo = nullptr;
    QComboBox*   searchTypeCombo = nullptr;
    QPushButton* btnRun = nullptr;
    QPushButton* btnSearch = nullptr;

    QSpinBox*    spProductCount = nullptr;
    QSpinBox*    spShelfCount = nullptr;
    QSpinBox*    spShelfCap = nullptr;
    QPushButton* btnGenerate = nullptr;

    QTableWidget* table = nullptr;
    QTextEdit*    outText = nullptr;

    QPushButton* btnComplexity = nullptr;
    QPushButton* btnPerformance = nullptr;
    QPushButton* btnAscii = nullptr;

    // ================= DATA =================
    std::vector<Product> lastProducts;
    std::vector<Shelf>   lastShelves;

    // ================= SON ÇALIŞTIRILAN ALGORİTMA SONUÇLARI =================
    PlacementResult   lastStaticResult;
    PlacementResult   lastGreedyResult;
    DPKnapsackResult  lastDPResult;

    bool hasStatic = false;
    bool hasGreedy = false;
    bool hasDP = false;

    // ================= HELPERS =================
    void renderShelves(const std::vector<Shelf>& shelves);
    void clearTableHighlights();
    void highlightProductInTable(const QString& name);
    void showStatus(const QString& msg);
};