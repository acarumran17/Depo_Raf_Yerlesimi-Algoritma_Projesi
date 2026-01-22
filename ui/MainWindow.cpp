#include "MainWindow.h"
#include "../algorithm/WarehouseAlgorithms.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QStatusBar>
#include <QSplitter>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QDialog>
#include <QPainter>
#include <algorithm>

#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>

#include <QMessageBox>

#include <QFormLayout>
#include <QDialogButtonBox>
#include <QGroupBox>


static QString actionBtnStyle() {
    return R"(
        QPushButton {
            background-color: #2b2b2b;
            border: 1px solid #444;
            border-radius: 10px;
            padding: 10px 16px;
            font-size: 14px;
            font-weight: 700;
        }
        QPushButton:hover {
            background-color: #3a3a3a;
            border: 1px solid #00aa66;
        }
        QPushButton:pressed {
            background-color: #00aa66;
            color: black;
        }
    )";
}
static void applyButton(QPushButton* b) {
    b->setStyleSheet(actionBtnStyle());
    b->setCursor(Qt::PointingHandCursor);
    b->setMinimumHeight(40);
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Depo Raf Yerleşimi – Algoritma Laboratuvarı");
    showMaximized();

    statusBar()->setStyleSheet("QStatusBar{font-size:14px;font-weight:bold;padding:6px;}");
    showStatus("Hazır");

    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(10,10,10,10);
    root->setSpacing(10);

    auto* top = new QHBoxLayout;

    searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("Ürün adı ara (örn: Laptop_12)");
    searchEdit->setMinimumHeight(36);

    searchTypeCombo = new QComboBox;
    searchTypeCombo->addItems({"Linear Search O(n)", "Binary Search O(log n)"});
    searchTypeCombo->setMinimumHeight(36);

    btnSearch = new QPushButton("Ara");
    applyButton(btnSearch);

    algoCombo = new QComboBox;
    algoCombo->addItems({"Statik", "Greedy", "DP (Knapsack)"});
    algoCombo->setMinimumHeight(36);

    btnRun = new QPushButton("Çalıştır");
    applyButton(btnRun);

    top->addWidget(searchEdit, 2);
    top->addWidget(searchTypeCombo, 1);
    top->addWidget(btnSearch);
    top->addSpacing(18);
    top->addWidget(new QLabel("Algoritma:"));
    top->addWidget(algoCombo, 1);
    top->addWidget(btnRun);

    auto* param = new QHBoxLayout;

    spProductCount = new QSpinBox; spProductCount->setRange(10, 5000); spProductCount->setValue(40);
    spShelfCount   = new QSpinBox; spShelfCount->setRange(1, 50);      spShelfCount->setValue(5);
    spShelfCap     = new QSpinBox; spShelfCap->setRange(5, 500);       spShelfCap->setValue(20);

    for (auto* s : {spProductCount, spShelfCount, spShelfCap}) s->setMinimumHeight(36);

    btnGenerate = new QPushButton("Yeni Ürün Seti");
    applyButton(btnGenerate);

    param->addWidget(new QLabel("Ürün"));
    param->addWidget(spProductCount);
    param->addSpacing(10);
    param->addWidget(new QLabel("Raf"));
    param->addWidget(spShelfCount);
    param->addSpacing(10);
    param->addWidget(new QLabel("Kapasite"));
    param->addWidget(spShelfCap);
    param->addStretch();
    param->addWidget(btnGenerate);

    table = new QTableWidget;
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setMinimumHeight(380);

    outText = new QTextEdit;
    outText->setReadOnly(true);
    outText->setStyleSheet("font-family: Consolas; font-size: 13px;");
    outText->setMinimumHeight(200);

    auto* split = new QSplitter(Qt::Vertical);
    split->addWidget(table);
    split->addWidget(outText);
    split->setStretchFactor(0, 3);
    split->setStretchFactor(1, 1);

    // ================= ALT BUTONLAR =================
    auto* bottom = new QHBoxLayout;

    btnComplexity  = new QPushButton("Depo Verimliliği");
    btnPerformance = new QPushButton("Algoritmaların Çalışma Süresi Karşılaştırması");
    btnAscii       = new QPushButton("ASCII Depo");

    applyButton(btnComplexity);
    applyButton(btnPerformance);
    applyButton(btnAscii);

    bottom->addWidget(btnComplexity);
    bottom->addWidget(btnPerformance);
    bottom->addWidget(btnAscii);

    root->addLayout(top);
    root->addLayout(param);
    root->addWidget(split, 1);
    root->addLayout(bottom);

    connect(btnGenerate, &QPushButton::clicked, this, &MainWindow::regenerateProducts);
    connect(btnSearch,   &QPushButton::clicked, this, &MainWindow::runSearch);

    connect(btnRun, &QPushButton::clicked, this, [this]{
        int idx = algoCombo->currentIndex();
        if (idx == 0) runStatic();
        else if (idx == 1) runGreedy();
        else runDP();
    });

    connect(btnComplexity,  &QPushButton::clicked, this, &MainWindow::runDepotEfficiency);
    connect(btnPerformance, &QPushButton::clicked, this, &MainWindow::runPerformance);
    connect(btnAscii,       &QPushButton::clicked, this, &MainWindow::runAsciiMap);

    regenerateProducts();
}

void MainWindow::showStatus(const QString& msg) {
    statusBar()->showMessage(msg);
}

void MainWindow::regenerateProducts() {
    lastProducts = WarehouseAlgorithms::generateProducts(spProductCount->value());

    // ÖNEMLİ: yeni ürün setinde önceki sonuçları "geçersiz" say
    hasStatic = false;
    hasGreedy = false;
    hasDP     = false;

    lastShelves.clear();

    outText->setText("Yeni ürün seti üretildi.\nAlgoritma seçip çalıştırabilirsin.");
    showStatus("Yeni ürün seti hazır.");
}

void MainWindow::clearTableHighlights() {
    for (int c = 0; c < table->columnCount(); ++c) {
        for (int r = 0; r < table->rowCount(); ++r) {
            if (auto* it = table->item(r, c)) {
                it->setBackground(QBrush());
                it->setForeground(QBrush());
            }
        }
    }
}

void MainWindow::renderShelves(const std::vector<Shelf>& shelves) {
    lastShelves = shelves;

    table->clear();
    table->setRowCount(0);
    table->setColumnCount(0);

    table->setColumnCount((int)shelves.size());
    QStringList headers;
    for (int i = 0; i < (int)shelves.size(); ++i) {
        headers << QString("Raf %1 (K:%2 U:%3)")
                .arg(i+1)
                .arg(shelves[i].capacity)
                .arg(shelves[i].used);
    }
    table->setHorizontalHeaderLabels(headers);

    int maxRow = 0;
    for (const auto& s : shelves)
        maxRow = std::max(maxRow, (int)s.products.size());
    table->setRowCount(maxRow);

    for (int c = 0; c < (int)shelves.size(); ++c) {
        for (int r = 0; r < (int)shelves[c].products.size(); ++r) {
            const auto& p = shelves[c].products[r];
            table->setItem(r, c, new QTableWidgetItem(
                    QString("%1\nS:%2 V:%3").arg(p.name).arg(p.sales).arg(p.volume)
            ));
        }
    }

    for (int c = 0; c < (int)shelves.size(); ++c) {
        double ratio = (shelves[c].capacity == 0)
                       ? 0.0
                       : (double)shelves[c].used / shelves[c].capacity;

        QColor cellBg;
        QColor headerBg;

        if (ratio < 0.5) {
            cellBg   = QColor(200, 255, 200);
            headerBg = QColor(150, 220, 150);
        }
        else if (ratio < 0.8) {
            cellBg   = QColor(255, 240, 180);
            headerBg = QColor(230, 210, 140);
        }
        else {
            cellBg   = QColor(255, 200, 200);
            headerBg = QColor(230, 160, 160);
        }

        for (int r = 0; r < table->rowCount(); ++r) {
            if (auto* it = table->item(r, c)) {
                it->setBackground(cellBg);
            }
        }

        if (auto* headerItem = table->horizontalHeaderItem(c)) {
            headerItem->setBackground(headerBg);
            headerItem->setForeground(Qt::black);
        }
    }

    clearTableHighlights();
}

void MainWindow::highlightProductInTable(const QString& name) {
    clearTableHighlights();

    bool found = false;
    for (int c = 0; c < table->columnCount(); ++c) {
        for (int r = 0; r < table->rowCount(); ++r) {
            if (auto* it = table->item(r, c)) {
                QString firstLine = it->text().split('\n').value(0).trimmed();
                if (firstLine.compare(name, Qt::CaseInsensitive) == 0) {
                    it->setBackground(QColor(255, 235, 59));
                    it->setForeground(Qt::black);
                    table->scrollToItem(it);
                    found = true;
                }
            }
        }
    }

    if (!found) outText->append("\nUyarı: Bu isimle birebir eşleşen ürün yok.");
}

void MainWindow::runSearch() {
    QString key = searchEdit->text().trimmed();
    if (key.isEmpty()) {
        outText->setText("Arama için ürün adı gir.");
        return;
    }

    outText->clear();
    outText->append("Aranan: " + key);

    if (searchTypeCombo->currentIndex() == 0) {
        auto r = WarehouseAlgorithms::linearSearchByName(lastProducts, key);
        outText->append(QString("Linear Search → found=%1 | %2 µs | O(n)")
                                .arg(r.found ? "true":"false")
                                .arg(r.elapsedUs));
    } else {
        auto sorted = lastProducts;
        WarehouseAlgorithms::sortByNameAsc(sorted);
        auto r = WarehouseAlgorithms::binarySearchByName(sorted, key);
        outText->append("Binary Search için sıralama yapıldı: O(n log n)");
        outText->append(QString("Binary Search → found=%1 | %2 µs | O(log n)")
                                .arg(r.found ? "true":"false")
                                .arg(r.elapsedUs));
    }

    if (table->rowCount() > 0) highlightProductInTable(key);
    else outText->append("\nNot: Sarı işaretleme için önce Statik/Greedy/DP çalıştır.");

    showStatus("Arama tamamlandı.");
}

void MainWindow::runStatic() {
    lastStaticResult =
            WarehouseAlgorithms::staticPlacement(
                    lastProducts,
                    spShelfCount->value(),
                    spShelfCap->value()
            );

    hasStatic = true;
    renderShelves(lastStaticResult.shelves);

    outText->setText(
            QString("Statik Yerleşim\nSüre: %1 ms\nO(n)")
                    .arg(lastStaticResult.elapsedMs)
    );

    showStatus("Statik Yerleşim tamamlandı.");
}

void MainWindow::runGreedy() {
    lastGreedyResult =
            WarehouseAlgorithms::greedyPlacement(
                    lastProducts,
                    spShelfCount->value(),
                    spShelfCap->value()
            );

    hasGreedy = true;
    renderShelves(lastGreedyResult.shelves);

    outText->setText(
            QString("Greedy Yerleşim\nSüre: %1 ms\nO(n log n)")
                    .arg(lastGreedyResult.elapsedMs)
    );

    showStatus("Greedy Yerleşim tamamlandı.");
}
void MainWindow::runDP() {
    lastDPResult =
            WarehouseAlgorithms::dpPlacementKnapsack(
                    lastProducts,
                    spShelfCount->value(),
                    spShelfCap->value()
            );

    hasDP = true;
    renderShelves(lastDPResult.placement.shelves);

    outText->setText(
            QString("DP (Knapsack)\nSüre: %1 ms\nO(n*C)")
                    .arg(lastDPResult.placement.elapsedMs)
    );

    showStatus("DP Yerleşim tamamlandı.");
}

void MainWindow::runDepotEfficiency() {
    if (!hasStatic || !hasGreedy || !hasDP) {
        QMessageBox::warning(this, "Uyarı",
                             "Lütfen önce Statik, Greedy ve DP algoritmalarını çalıştırın.");
        return;
    }

    int totalCapacity = spShelfCount->value() * spShelfCap->value();
    if (totalCapacity <= 0) totalCapacity = 1;

    auto calcUsed = [](const std::vector<Shelf>& shelves) {
        int used = 0;
        for (const auto& s : shelves)
            used += s.used;
        return used;
    };

    double effStatic = (double)calcUsed(lastStaticResult.shelves) / totalCapacity * 100.0;
    double effGreedy = (double)calcUsed(lastGreedyResult.shelves) / totalCapacity * 100.0;
    double effDP     = (double)calcUsed(lastDPResult.placement.shelves) / totalCapacity * 100.0;

    auto* set = new QBarSet("Depo Doluluk (%)");
    *set << effStatic << effGreedy << effDP;

    auto* series = new QBarSeries();
    series->append(set);

    series->setLabelsVisible(true);
    series->setLabelsFormat("@value %");

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Algoritmalara Göre Depo Verimliliği");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << "Statik" << "Greedy" << "DP";

    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setTitleText("Algoritma");

    auto* axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTitleText("Depo Doluluk (%)");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    auto* view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);

    auto* dlg = new QDialog(this);
    dlg->setWindowTitle("Depo Verimliliği Grafiği");
    dlg->resize(700, 500);

    auto* layout = new QVBoxLayout(dlg);
    layout->addWidget(view);
    dlg->exec();

    outText->setText(
            QString(
                    "Depo Verimliliği (%%)\n\n"
                    "Statik  : %1 %%\n"
                    "Greedy  : %2 %%\n"
                    "DP      : %3 %%"
            )
                    .arg(effStatic, 0, 'f', 2)
                    .arg(effGreedy, 0, 'f', 2)
                    .arg(effDP,     0, 'f', 2)
    );
    showStatus("Depo verimliliği grafiği oluşturuldu.");
}

void MainWindow::runAsciiMap() {
    if (lastShelves.empty()) {
        outText->setText("Önce yerleşim çalıştır.");
        return;
    }
    outText->setText(WarehouseAlgorithms::buildAsciiMap(lastShelves));
    showStatus("ASCII depo üretildi.");
}


void MainWindow::runPerformance()
{
    const int maxN       = spProductCount->value(); // ANA EKRANDAN
    const int shelfCount = spShelfCount->value();
    const int shelfCap   = spShelfCap->value();

    if (maxN <= 0 || shelfCount <= 0 || shelfCap <= 0) {
        QMessageBox::warning(this, "Uyarı", "Ürün / Raf / Kapasite değerleri 0'dan büyük olmalıdır.");
        return;
    }

    auto* dlg = new QDialog(this);
    dlg->setWindowTitle("Performans Karşılaştırması (Süre - Ürün Sayısı)");
    dlg->resize(1200, 520);

    auto* root = new QVBoxLayout(dlg);
    auto* row  = new QHBoxLayout();
    root->addLayout(row, 1);

    auto makeChart = [&](const QString& title) {
        auto* chart = new QChart();
        chart->setTitle(title + QString("\n(Raf=%1, Kapasite=%2, MaxN=%3)")
                                .arg(shelfCount).arg(shelfCap).arg(maxN));
        chart->legend()->setVisible(false);
        chart->setAnimationOptions(QChart::NoAnimation);
        return chart;
    };

    auto* chartS = makeChart("Statik Yerleşim");
    auto* chartG = makeChart("Greedy Yerleşim");
    auto* chartD = makeChart("DP (Knapsack) Yerleşim");

    auto* sS = new QLineSeries();
    auto* sG = new QLineSeries();
    auto* sD = new QLineSeries();

    chartS->addSeries(sS);
    chartG->addSeries(sG);
    chartD->addSeries(sD);

    auto* vS = new QChartView(chartS);
    auto* vG = new QChartView(chartG);
    auto* vD = new QChartView(chartD);

    vS->setRenderHint(QPainter::Antialiasing);
    vG->setRenderHint(QPainter::Antialiasing);
    vD->setRenderHint(QPainter::Antialiasing);

    row->addWidget(vS, 1);
    row->addWidget(vG, 1);
    row->addWidget(vD, 1);

    // Benchmark: 1..maxN
    double maxY = 1.0;

    for (int n = 1; n <= maxN; ++n) {
        auto products = WarehouseAlgorithms::generateProducts(n);

        auto rS = WarehouseAlgorithms::staticPlacement(products, shelfCount, shelfCap);
        auto rG = WarehouseAlgorithms::greedyPlacement(products, shelfCount, shelfCap);
        auto rD = WarehouseAlgorithms::dpPlacementKnapsack(products, shelfCount, shelfCap);

        const double tS = (double)rS.elapsedUs;
        const double tG = (double)rG.elapsedUs;
        const double tD = (double)rD.placement.elapsedUs;

        sS->append(n, tS);
        sG->append(n, tG);
        sD->append(n, tD);

        maxY = std::max({maxY, tS, tG, tD});
    }

    auto applyAxes = [&](QChart* c) {
        // temiz başlat
        for (auto* ax : c->axes(Qt::Horizontal)) c->removeAxis(ax);
        for (auto* ay : c->axes(Qt::Vertical))   c->removeAxis(ay);

        auto* axX = new QValueAxis();
        axX->setTitleText("Ürün Sayısı (N)");
        axX->setLabelFormat("%d");
        axX->setRange(1, maxN);

        auto* axY = new QValueAxis();
        axY->setTitleText("Süre (µs)");
        axY->setRange(0, maxY * 1.15);

        c->addAxis(axX, Qt::AlignBottom);
        c->addAxis(axY, Qt::AlignLeft);

        for (auto* s : c->series()) {
            s->attachAxis(axX);
            s->attachAxis(axY);
        }
    };

    applyAxes(chartS);
    applyAxes(chartG);
    applyAxes(chartD);

    // Close
    auto* btnClose = new QPushButton("Close");
    connect(btnClose, &QPushButton::clicked, dlg, &QDialog::accept);
    root->addWidget(btnClose, 0, Qt::AlignRight);

    dlg->exec();
}
