#include "mainwindow.h"
#include <QDialog>
#include <QGridLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QSpinBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QStringConverter>
#include <algorithm>
#include <locale>
#include <iomanip>
#include <QVector>
#include <sstream>
#include <QSet>
#include <stdexcept>
#include <QDate>
#include <QListWidget>
#include <QStatusBar>
void MainWindow::setupUI() {
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    // ÁP DỤNG STYLE ĐẸP (TRONG HÀM, KHÔNG BỊ LỖI)
    setStyleSheet(R"(
    /* Nền trắng tinh */
    QMainWindow { background: #ffffff; }

    /* Tab trắng, viền nhẹ */
    QTabWidget::pane {
        border: 1px solid #e0e0e0;
        background: #ffffff;
        border-radius: 12px;
        margin-top: 8px;
    }

    QTabBar::tab {
        background: #f5f5f5;
        color: #333333;
        padding: 14px 32px;
        margin-right: 6px;
        border-top-left-radius: 12px;
        border-top-right-radius: 12px;
        font-weight: bold;
        min-width: 140px;
        font-size: 11pt;
        border: 1px solid #e0e0e0;
        border-bottom: none;
    }

    QTabBar::tab:selected {
        background: #ffffff;
        color: #1e3a8a;
        border: 1px solid #e0e0e0;
        border-bottom: none;
    }

    QTabBar::tab:hover:!selected {
        background: #f0f0f0;
    }

    /* GroupBox trắng, viền nhẹ */
    QGroupBox {
        font-weight: bold;
        font-size: 13px;
        border: 2px solid #e0e0e0;
        border-radius: 12px;
        margin-top: 14px;
        padding-top: 12px;
        background: #ffffff;
    }

    QGroupBox::title {
        subcontrol-origin: margin;
        left: 16px;
        padding: 0 10px;
        background: #ffffff;
        color: #1e293b;
    }

    /* Nút trắng + xanh nhẹ */
    QPushButton {
        background: #f8f9fa;
        color: #1e3a8a;
        border: 2px solid #1e3a8a;
        padding: 12px 24px;
        border-radius: 10px;
        font-weight: bold;
        font-size: 11pt;
    }

    QPushButton:hover {
        background: #e3f2fd;
        border-color: #1e40af;
    }

    QPushButton#danger {
        background: #fef2f2;
        color: #dc2626;
        border: 2px solid #dc2626;
    }

    QPushButton#danger:hover {
        background: #fee2e2;
    }

    /* Input trắng, viền nhẹ */
    QLineEdit, QComboBox, QDateEdit {
        padding: 10px;
        border: 1px solid #d1d5db;
        border-radius: 10px;
        font-size: 11pt;
        background: #ffffff;
    }

    QLineEdit:focus, QComboBox:focus, QDateEdit:focus {
        border: 2px solid #1e40af;
        background: #f8faff;
    }

    /* Bảng trắng, sạch sẽ */
    QTableWidget {
        gridline-color: #e5e7eb;
        selection-background-color: #dbeafe;
        font-size: 11pt;
        background: #ffffff;
        border: 1px solid #e0e0e0;
        border-radius: 8px;
    }

    QHeaderView::section {
        background: #f9fafb;
        color: #1f2937;
        padding: 12px;
        font-weight: bold;
        border: 1px solid #e0e0e0;
    }

    /* Thanh trạng thái */
    QStatusBar {
        background: #f8f9fa;
        color: #4b5563;
        font-size: 10pt;
    }
)");

    createBenhNhanTab();
    createBacSiTab();
    createPhongTab();
    createDieuTriTab();
    createThongKeTab();

    // SỬA LỖI: statusBar() cần include QStatusBar
    statusBar()->showMessage("  Hệ thống sẵn sàng • " + QDate::currentDate().toString("dd/MM/yyyy"));
}

// ============= ĐỊNH NGHĨA CÁC HÀM CỦA BÁC SĨ =============


void BacSi::themBenhNhan(std::shared_ptr<BenhNhan> bn) {
    bool isExist = false;
    if (bn->getTrangThaiNhapVien()) {
        for (const auto& weak_bn : danhSachBN) {
            if (auto ptr = weak_bn.lock(); ptr && ptr->getMaBN() == bn->getMaBN()) {
                isExist = true;
                break;
            }
        }
        if (!isExist) {
            danhSachBN.push_back(std::weak_ptr<BenhNhan>(bn));
        }
    }
}

void BacSi::xoaBenhNhan(const std::string& maBN) {
    danhSachBN.erase(
        std::remove_if(danhSachBN.begin(), danhSachBN.end(),
                       [&maBN](const std::weak_ptr<BenhNhan>& weak_bn) {
                           if (auto ptr = weak_bn.lock(); ptr) {
                               return ptr->getMaBN() == maBN;
                           }
                           return true; // Xóa luôn các weak_ptr đã hết hạn
                       }),
        danhSachBN.end()
        );
}

// ============= ĐỊNH NGHĨA CÁC HÀM CỦA QUANLYBENHVIEN =============

void QuanLyBenhVien::xoaBN(const std::string& maBN) {
    auto bn = timBN(maBN);
    if (!bn) throw std::runtime_error("Benh nhan khong ton tai.");

    // Xử lý phân công
    if (dsPhanCong.count(maBN)) {
        raVien(maBN); // Ra viện trước khi xóa hẳn
    }

    dsBN.erase(maBN);
}

void QuanLyBenhVien::xoaBS(const std::string& maBS) {
    auto bs = timBS(maBS);
    if (!bs) throw std::runtime_error("Bac si khong ton tai.");

    // Hủy phân công cho các BN đang nằm viện do BS này phụ trách
    std::vector<std::string> bnPhuTrach;
    for (const auto& pair : dsPhanCong) {
        if (pair.second.maBS == maBS) {
            bnPhuTrach.push_back(pair.first);
        }
    }

    if (!bnPhuTrach.empty()) {
        throw std::runtime_error("Khong the xoa. Bac si con phu trach " + std::to_string(bnPhuTrach.size()) + " benh nhan.");
    }

    dsBS.erase(maBS);
}

void QuanLyBenhVien::xoaPhong(const std::string& maPhong) {
    auto phong = timPhong(maPhong);
    if (!phong) throw std::runtime_error("Phong benh khong ton tai.");

    if (phong->getSoBNDangNam() > 0) {
        throw std::runtime_error("Khong the xoa. Phong con benh nhan dang nam.");
    }

    dsPhong.erase(maPhong);
}

void QuanLyBenhVien::phanCong(const std::string& maBN, const std::string& maBS, const std::string& maPhong) {
    auto bn = timBN(maBN);
    auto bs = timBS(maBS);
    auto phong = timPhong(maPhong);

    if (!bn || !bs || !phong) throw std::runtime_error("Ma BN, BS hoac Phong khong hop le.");
    if (!bn->getTrangThaiNhapVien()) throw std::runtime_error("Benh nhan khong o trong trang thai nhap vien.");
    if (phong->getTrangThai() == "Sửa chữa") throw std::runtime_error("Phong dang sua chua.");
    if (phong->getSoBNDangNam() >= phong->getSoGiuong()) throw std::runtime_error("Phong da day.");

    // Cập nhật phòng cũ
    if (!bn->getMaPhongHienTai().empty() && dsPhong.count(bn->getMaPhongHienTai())) {
        dsPhong.at(bn->getMaPhongHienTai())->giamBN();
    }

    // Cập nhật thông tin mới
    phong->tangBN();
    bn->setMaPhongHienTai(maPhong);
    bs->themBenhNhan(bn);

    // Lưu phân công
    dsPhanCong[maBN] = {maBN, maBS, maPhong};
}

void QuanLyBenhVien::raVien(const std::string& maBN) {
    if (!dsPhanCong.count(maBN)) throw std::runtime_error("Benh nhan chua duoc phan cong hoac da ra vien.");

    auto bn = timBN(maBN);
    if (!bn) {
        dsPhanCong.erase(maBN);
        throw std::runtime_error("Benh nhan khong ton tai.");
    }

    PhanCong pc = dsPhanCong.at(maBN);
    auto bs = timBS(pc.maBS);
    auto phong = timPhong(pc.maPhong);

    // Cập nhật trạng thái
    bn->setTrangThaiNhapVien(false);
    bn->setMaPhongHienTai("");
    if (phong) phong->giamBN();
    if (bs) bs->xoaBenhNhan(maBN);

    // Xóa phân công
    dsPhanCong.erase(maBN);
}


// ============= ĐỊNH NGHĨA CÁC HÀM CỦA MAINWINDOW =============

// >> HÀM HỖ TRỢ TẠO MÃ TỰ ĐỘNG <<
std::string MainWindow::generateNextMaBN() {
    int maxNum = 0;
    for (const auto& pair : qlbv.getDsBN()) {
        const std::string& maBN = pair.first;
        if (maBN.size() > 2 && maBN.substr(0, 2) == "BN") {
            try {
                int num = std::stoi(maBN.substr(2));
                if (num > maxNum) {
                    maxNum = num;
                }
            } catch (const std::exception&) {}
        }
    }

    int nextNum = maxNum + 1;
    std::stringstream ss;
    ss << "BN" << std::setw(3) << std::setfill('0') << nextNum;
    return ss.str();
}

std::string MainWindow::generateNextMaBS() {
    int maxNum = 0;
    for (const auto& pair : qlbv.getDsBS()) {
        const std::string& maBS = pair.first;
        if (maBS.size() > 2 && maBS.substr(0, 2) == "BS") {
            try {
                int num = std::stoi(maBS.substr(2));
                if (num > maxNum) {
                    maxNum = num;
                }
            } catch (const std::exception&) {}
        }
    }

    int nextNum = maxNum + 1;
    std::stringstream ss;
    ss << "BS" << std::setw(3) << std::setfill('0') << nextNum;
    return ss.str();
}

// >> HÀM HỖ TRỢ LỌC CHUYÊN KHOA <<
std::string findChuyenKhoaForBenhLy(const std::string& benhLy) {
    for (const auto& pair : chuyenKhoaToBenhLy) {
        const std::string& chuyenKhoa = pair.first;
        const std::vector<std::string>& benhLyList = pair.second;
        if (std::find(benhLyList.begin(), benhLyList.end(), benhLy) != benhLyList.end()) {
            return chuyenKhoa;
        }
    }
    return ""; // Không tìm thấy
}
// << KẾT THÚC HÀM HỖ TRỢ LỌC CHUYÊN KHOA >>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Thêm dữ liệu mẫu (Sample data)
    qlbv.themBS(std::make_shared<BacSi>("BS001", "Lê Nhựt Trường", "Nam", "0901234567", "Hà Nội", "Tim mạch"));
    qlbv.themBS(std::make_shared<BacSi>("BS002", "Trần Thị B", "Nữ", "0901234568", "TP.HCM", "Hô hấp"));
    qlbv.themBS(std::make_shared<BacSi>("BS003", "Lê Văn E", "Nam", "0901234569", "Hà Nội", "Tiêu hóa"));
    // BN001: Nhồi máu cơ tim (Tim mạch), BN002: Viêm phổi (Hô hấp), BN003: Viêm loét dạ dày (Tiêu hóa)
    qlbv.themBN(std::make_shared<BenhNhan>("BN001", "Phạm Quốc Khánh", "Nam", "0911111111", "Đà Nẵng", "1990-01-01", "2025-11-15", "Nhồi máu cơ tim", false));
    qlbv.themBN(std::make_shared<BenhNhan>("BN002", "Phạm Thị D", "Nữ", "0922222222", "Hà Nội", "1980-05-10", "2025-11-15", "Viêm phổi", true));
    qlbv.themBN(std::make_shared<BenhNhan>("BN003", "Trần Văn F", "Nam", "0933333333", "TP.HCM", "2000-10-20", "2025-11-16", "Viêm loét dạ dày", true));
    qlbv.themPhong(std::make_shared<PhongBenh>("P101", "Thường", 5));
    qlbv.themPhong(std::make_shared<PhongBenh>("P102", "VIP", 2));
    qlbv.themPhong(std::make_shared<PhongBenh>("P201", "Thường", 4));
    try {
        qlbv.phanCong("BN001", "BS001", "P101");
    } catch (...) {} // Ignore if sample data setup fails

    setupUI();
    updateBenhNhanTable();
    updateBacSiTable();
    updatePhongTable();
    updatePhanCongTable();
}

MainWindow::~MainWindow()
{

}

// ============= THIẾT LẬP GIAO DIỆN =============


void MainWindow::createBenhNhanTab() {
    benhNhanTab = new QWidget();
    tabWidget->addTab(benhNhanTab, "Quản lý Bệnh nhân");
    QVBoxLayout* layout = new QVBoxLayout(benhNhanTab);

    // Filter & Search Group
    QGroupBox* filterGroup = new QGroupBox("Lọc & Tìm kiếm");
    QHBoxLayout* filterLayout = new QHBoxLayout(filterGroup);

    QLabel* searchLabel = new QLabel("Tìm kiếm (Mã/Tên):");
    filterBNInput = new QLineEdit();
    filterBNInput->setPlaceholderText("Nhập mã hoặc tên BN...");

    QLabel* comboLabel = new QLabel("Lọc theo Bệnh lý:");
    filterBNCombo = new QComboBox();
    filterBNCombo->addItem("Tất cả Bệnh lý");

    // Thêm các bệnh lý từ hàm tiện ích vào combo box
    std::vector<std::string> uniqueDiseases = getAllUniqueBenhLy();
    for (const auto& disease : uniqueDiseases) {
        filterBNCombo->addItem(QString::fromStdString(disease));
    }

    filterLayout->addWidget(searchLabel);
    filterLayout->addWidget(filterBNInput);
    filterLayout->addWidget(comboLabel);
    filterLayout->addWidget(filterBNCombo);
    layout->addWidget(filterGroup);

    // Table
    benhNhanTable = new QTableWidget();
    benhNhanTable->setColumnCount(9);
    benhNhanTable->setHorizontalHeaderLabels({"Mã bệnh nhân", "Họ tên", "Giới tính", "SĐT", "Địa chỉ", "Ngày sinh", "Ngày nhập viện", "Bệnh lý", "BHYT"});
    benhNhanTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    layout->addWidget(benhNhanTable);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnThemBN = new QPushButton("Thêm Bệnh nhân");
    btnSuaBN = new QPushButton("Sửa Bệnh nhân");
    btnXoaBN = new QPushButton("Xóa Bệnh nhân");
    btnXuatBN = new QPushButton("Xuất CSV");

    btnLayout->addWidget(btnThemBN);
    btnLayout->addWidget(btnSuaBN);
    btnLayout->addWidget(btnXoaBN);
    btnLayout->addStretch();
    btnLayout->addWidget(btnXuatBN);
    layout->addLayout(btnLayout);

    // Connections
    connect(btnThemBN, &QPushButton::clicked, this, &MainWindow::onThemBenhNhan);
    connect(btnSuaBN, &QPushButton::clicked, this, &MainWindow::onSuaBenhNhan);
    connect(btnXoaBN, &QPushButton::clicked, this, &MainWindow::onXoaBenhNhan);
    connect(btnXuatBN, &QPushButton::clicked, this, &MainWindow::onXuatBenhNhan);
    connect(filterBNInput, &QLineEdit::textChanged, this, &MainWindow::onFilterBenhNhan);
    connect(filterBNCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onFilterBenhNhan);
}

void MainWindow::createBacSiTab() {
    bacSiTab = new QWidget();
    tabWidget->addTab(bacSiTab, "Quản lý Bác sĩ");
    QVBoxLayout* layout = new QVBoxLayout(bacSiTab);

    // Filter & Search Group
    QGroupBox* filterGroup = new QGroupBox("Lọc & Tìm kiếm");
    QHBoxLayout* filterLayout = new QHBoxLayout(filterGroup);

    QLabel* searchLabel = new QLabel("Tìm kiếm (Mã/Tên):");
    searchBacSiLine = new QLineEdit();
    searchBacSiLine->setPlaceholderText("Nhập mã hoặc tên BS...");

    QLabel* comboLabel = new QLabel("Lọc theo Chuyên khoa:");
    filterChuyenKhoaBacSiCombo = new QComboBox();
    filterChuyenKhoaBacSiCombo->addItem("Tất cả Chuyên khoa");
    // Thêm các chuyên khoa từ map vào combo box
    for (const auto& pair : chuyenKhoaToBenhLy) {
        filterChuyenKhoaBacSiCombo->addItem(QString::fromStdString(pair.first));
    }

    filterLayout->addWidget(searchLabel);
    filterLayout->addWidget(searchBacSiLine);
    filterLayout->addWidget(comboLabel);
    filterLayout->addWidget(filterChuyenKhoaBacSiCombo);
    filterLayout->setStretch(1, 2);
    filterLayout->setStretch(3, 1);
    layout->addWidget(filterGroup);

    // Table
    bacSiTable = new QTableWidget();
    bacSiTable->setColumnCount(6);
    bacSiTable->setHorizontalHeaderLabels({"Mã BS", "Họ tên", "GT", "SĐT", "Địa chỉ", "Chuyên khoa"});
    bacSiTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    layout->addWidget(bacSiTable);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnThemBS = new QPushButton("Thêm Bác sĩ");
    btnSuaBS = new QPushButton("Sửa Bác sĩ");
    btnXoaBS = new QPushButton("Xóa Bác sĩ");
    btnXuatBS = new QPushButton("Xuất CSV");

    btnLayout->addWidget(btnThemBS);
    btnLayout->addWidget(btnSuaBS);
    btnLayout->addWidget(btnXoaBS);
    btnLayout->addStretch();
    btnLayout->addWidget(btnXuatBS);
    layout->addLayout(btnLayout);

    // Connections
    connect(btnThemBS, &QPushButton::clicked, this, &MainWindow::onThemBacSi);
    connect(btnSuaBS, &QPushButton::clicked, this, &MainWindow::onSuaBacSi);
    connect(btnXoaBS, &QPushButton::clicked, this, &MainWindow::onXoaBacSi);
    connect(btnXuatBS, &QPushButton::clicked, this, &MainWindow::onXuatBacSi);
    connect(searchBacSiLine, &QLineEdit::textChanged, this, &MainWindow::onFilterBacSi);
    connect(filterChuyenKhoaBacSiCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onFilterBacSi);
}

void MainWindow::createPhongTab() {
    phongTab = new QWidget();
    tabWidget->addTab(phongTab, "Quản lý Phòng bệnh");
    QVBoxLayout* layout = new QVBoxLayout(phongTab);

    // Table
    phongTable = new QTableWidget();
    phongTable->setColumnCount(5);
    phongTable->setHorizontalHeaderLabels({"Mã Phòng", "Loại Phòng", "Số giường", "BN đang nằm", "Trạng thái"});
    phongTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    layout->addWidget(phongTable);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnThemPhong = new QPushButton("Thêm Phòng");
    btnSuaPhong = new QPushButton("Sửa Phòng");
    btnXoaPhong = new QPushButton("Xóa Phòng");
    btnXuatPhong = new QPushButton("Xuất CSV");

    btnLayout->addWidget(btnThemPhong);
    btnLayout->addWidget(btnSuaPhong);
    btnLayout->addWidget(btnXoaPhong);
    btnLayout->addStretch();
    btnLayout->addWidget(btnXuatPhong);
    layout->addLayout(btnLayout);

    // Connections
    connect(btnThemPhong, &QPushButton::clicked, this, &MainWindow::onThemPhong);
    connect(btnSuaPhong, &QPushButton::clicked, this, &MainWindow::onSuaPhong);
    connect(btnXoaPhong, &QPushButton::clicked, this, &MainWindow::onXoaPhong);
    connect(btnXuatPhong, &QPushButton::clicked, this, &MainWindow::onXuatPhong);
}

void MainWindow::createDieuTriTab() {
    dieuTriTab = new QWidget();
    tabWidget->addTab(dieuTriTab, "Điều trị & Ra viện");
    QVBoxLayout* layout = new QVBoxLayout(dieuTriTab);

    // Table
    phanCongTable = new QTableWidget();
    phanCongTable->setColumnCount(6);
    phanCongTable->setHorizontalHeaderLabels({"Mã BN", "Tên BN", "Mã BS", "Tên BS", "Mã Phòng", "Ngày NV"});
    phanCongTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    layout->addWidget(phanCongTable);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnPhanCong = new QPushButton("Phân công / Chuyển phòng");
    btnRaVien = new QPushButton("Thủ tục Ra viện");

    btnLayout->addWidget(btnPhanCong);
    btnLayout->addStretch();
    btnLayout->addWidget(btnRaVien);
    layout->addLayout(btnLayout);

    // Connections
    connect(btnPhanCong, &QPushButton::clicked, this, &MainWindow::onPhanCong);
    connect(btnRaVien, &QPushButton::clicked, this, &MainWindow::onRaVien);
}

void MainWindow::createThongKeTab() {
    thongKeTab = new QWidget();
    tabWidget->addTab(thongKeTab, "Thống kê");
    QVBoxLayout* layout = new QVBoxLayout(thongKeTab);

    thongKeOutput = new QTextEdit();
    thongKeOutput->setReadOnly(true);
    layout->addWidget(thongKeOutput);

    btnThongKe = new QPushButton("Tạo Báo cáo Thống kê");
    layout->addWidget(btnThongKe);

    connect(btnThongKe, &QPushButton::clicked, this, &MainWindow::onThongKe);
}

// ============= CẬP NHẬT BẢNG (TABLES) =============
void MainWindow::updateBenhNhanTable(const std::string& keyword, const std::string& filterDisease) {
    benhNhanTable->setRowCount(0);

    std::vector<std::shared_ptr<BenhNhan>> filteredList;
    QString lowerKeyword = QString::fromStdString(keyword).toLower();

    for (const auto& pair : qlbv.getDsBN()) {
        const auto& bn = pair.second;

        if (!bn->getTrangThaiNhapVien()) continue; // Chỉ hiển thị BN đang nằm viện

        // 1. Lọc theo Bệnh lý
        bool diseaseMatch = true;
        if (filterDisease != "Tất cả Bệnh lý" && !filterDisease.empty()) {
            if (bn->getBenhLy() != filterDisease) {
                diseaseMatch = false;
            }
        }

        // 2. Lọc theo từ khóa tìm kiếm
        bool keywordMatch = true;
        if (!keyword.empty()) {
            QString maBn = QString::fromStdString(bn->getMaBN()).toLower();
            QString tenBn = QString::fromStdString(bn->getHoTen()).toLower();

            if (!maBn.contains(lowerKeyword) && !tenBn.contains(lowerKeyword)) {
                keywordMatch = false;
            }
        }

        if (diseaseMatch && keywordMatch) {
            filteredList.push_back(bn);
        }
    }

    int row = 0;
    for (const auto& bn : filteredList) {
        benhNhanTable->insertRow(row);
        benhNhanTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(bn->getMaBN())));
        benhNhanTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(bn->getHoTen())));
        benhNhanTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(bn->getGioiTinh())));
        benhNhanTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(bn->getSdt())));
        benhNhanTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(bn->getDiaChi())));
        benhNhanTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(bn->getNgaySinh())));
        benhNhanTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(bn->getNgayNhapVien())));
        benhNhanTable->setItem(row, 7, new QTableWidgetItem(QString::fromStdString(bn->getBenhLy())));
        benhNhanTable->setItem(row, 8, new QTableWidgetItem(bn->getLaHoNgheo() ? "Có" : "Không"));
        row++;
    }
    benhNhanTable->resizeColumnsToContents();
}

void MainWindow::updateBacSiTable(const std::string& keyword, const std::string& filterChuyenKhoa) {
    bacSiTable->setRowCount(0);

    std::vector<std::shared_ptr<BacSi>> filteredList;
    QString lowerKeyword = QString::fromStdString(keyword).toLower();

    // Lọc danh sách bác sĩ
    for (const auto& pair : qlbv.getDsBS()) {
        const auto& bs = pair.second;

        // 1. Lọc theo chuyên khoa
        bool chuyenKhoaMatch = true;
        if (filterChuyenKhoa != "Tất cả Chuyên khoa" && !filterChuyenKhoa.empty()) {
            if (bs->getChuyenKhoa() != filterChuyenKhoa) {
                chuyenKhoaMatch = false;
            }
        }

        // 2. Lọc theo từ khóa tìm kiếm (Mã BS hoặc Họ Tên)
        bool keywordMatch = true;
        if (!keyword.empty()) {
            QString maBs = QString::fromStdString(bs->getMaBS()).toLower();
            QString tenBs = QString::fromStdString(bs->getHoTen()).toLower();

            if (!maBs.contains(lowerKeyword) && !tenBs.contains(lowerKeyword)) {
                keywordMatch = false;
            }
        }

        if (chuyenKhoaMatch && keywordMatch) {
            filteredList.push_back(bs);
        }
    }

    int row = 0;
    for (const auto& bs : filteredList) {
        bacSiTable->insertRow(row);
        bacSiTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(bs->getMaBS())));
        bacSiTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(bs->getHoTen())));
        bacSiTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(bs->getGioiTinh())));
        bacSiTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(bs->getSdt())));
        bacSiTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(bs->getDiaChi())));
        bacSiTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(bs->getChuyenKhoa())));
        row++;
    }
    bacSiTable->resizeColumnsToContents();
}

void MainWindow::updatePhongTable() {
    phongTable->setRowCount(0);
    int row = 0;
    for (const auto& pair : qlbv.getDsPhong()) {
        const auto& phong = pair.second;
        phongTable->insertRow(row);
        phongTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(phong->getMaPhong())));
        phongTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(phong->getLoaiPhong())));
        phongTable->setItem(row, 2, new QTableWidgetItem(QString::number(phong->getSoGiuong())));
        phongTable->setItem(row, 3, new QTableWidgetItem(QString::number(phong->getSoBNDangNam())));
        phongTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(phong->getTrangThai())));
        row++;
    }
    phongTable->resizeColumnsToContents();
}

void MainWindow::updatePhanCongTable() {
    phanCongTable->setRowCount(0);
    int row = 0;
    for (const auto& pair : qlbv.getDsPhanCong()) {
        const auto& pc = pair.second;
        auto bn = qlbv.timBN(pc.maBN);
        auto bs = qlbv.timBS(pc.maBS);
        if (!bn || !bs) continue; // Bỏ qua nếu dữ liệu không khớp

        phanCongTable->insertRow(row);
        phanCongTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(bn->getMaBN())));
        phanCongTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(bn->getHoTen())));
        phanCongTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(bs->getMaBS())));
        phanCongTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(bs->getHoTen())));
        phanCongTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(pc.maPhong)));
        phanCongTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(bn->getNgayNhapVien())));
        row++;
    }
    phanCongTable->resizeColumnsToContents();
}


// ============= HỘP THOẠI & LOGIC CRUD =============
void MainWindow::showBenhNhanInputDialog(std::shared_ptr<BenhNhan> bn) {
    QDialog dialog(this);
    dialog.setWindowTitle(bn ? "Sửa thông tin Bệnh nhân" : "Thêm Bệnh nhân mới");
    QGridLayout* layout = new QGridLayout(&dialog);

    // Inputs
    QLineEdit* maInput = new QLineEdit();
    QLineEdit* tenInput = new QLineEdit();
    QComboBox* gioiTinhCombo = new QComboBox();
    gioiTinhCombo->addItems({"Nam", "Nữ"});
    QLineEdit* sdtInput = new QLineEdit();
    QLineEdit* diaChiInput = new QLineEdit();
    QDateEdit* ngaySinhDate = new QDateEdit(QDate::currentDate());
    QDateEdit* ngayNhapDate = new QDateEdit(QDate::currentDate());

    // Thay đổi Bệnh lý từ QLineEdit thành QComboBox
    QComboBox* benhLyCombo = new QComboBox();
    std::vector<std::string> uniqueDiseases = getAllUniqueBenhLy();
    for (const auto& disease : uniqueDiseases) {
        benhLyCombo->addItem(QString::fromStdString(disease));
    }

    QCheckBox* hoNgheoCheck = new QCheckBox("Có BHYT");

    // Set giá trị mặc định/tự động tạo mã
    if (bn) {
        maInput->setText(QString::fromStdString(bn->getMaBN()));
        tenInput->setText(QString::fromStdString(bn->getHoTen()));
        if (bn->getGioiTinh() == "Nữ") gioiTinhCombo->setCurrentIndex(1);
        sdtInput->setText(QString::fromStdString(bn->getSdt()));
        diaChiInput->setText(QString::fromStdString(bn->getDiaChi()));
        ngaySinhDate->setDate(QDate::fromString(QString::fromStdString(bn->getNgaySinh()), "yyyy-MM-dd"));
        ngayNhapDate->setDate(QDate::fromString(QString::fromStdString(bn->getNgayNhapVien()), "yyyy-MM-dd"));

        // Cập nhật giá trị mặc định cho Bệnh lý Combo
        benhLyCombo->setCurrentText(QString::fromStdString(bn->getBenhLy()));

        hoNgheoCheck->setChecked(bn->getLaHoNgheo());
    } else {
        std::string nextMaBN = generateNextMaBN();
        maInput->setText(QString::fromStdString(nextMaBN));
    }
    maInput->setReadOnly(true); // Tự động tạo và không cho sửa

    // Layout
    layout->addWidget(new QLabel("Mã BN:"), 0, 0); layout->addWidget(maInput, 0, 1);
    layout->addWidget(new QLabel("Họ tên:"), 1, 0); layout->addWidget(tenInput, 1, 1);
    layout->addWidget(new QLabel("Giới tính:"), 2, 0); layout->addWidget(gioiTinhCombo, 2, 1);
    layout->addWidget(new QLabel("SĐT:"), 3, 0); layout->addWidget(sdtInput, 3, 1);
    layout->addWidget(new QLabel("Địa chỉ:"), 4, 0); layout->addWidget(diaChiInput, 4, 1);
    layout->addWidget(new QLabel("Ngày sinh (yyyy-MM-dd):"), 5, 0); layout->addWidget(ngaySinhDate, 5, 1);
    layout->addWidget(new QLabel("Ngày nhập viện (yyyy-MM-dd):"), 6, 0); layout->addWidget(ngayNhapDate, 6, 1);

    // Thay thế benhLyInput bằng benhLyCombo
    layout->addWidget(new QLabel("Bệnh lý:"), 7, 0); layout->addWidget(benhLyCombo, 7, 1);

    layout->addWidget(hoNgheoCheck, 8, 0, 1, 2);

    QPushButton* okButton = new QPushButton("Lưu");
    QPushButton* cancelButton = new QPushButton("Hủy");
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(okButton);
    btnLayout->addWidget(cancelButton);
    layout->addLayout(btnLayout, 9, 1);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        try {
            std::string ma = maInput->text().trimmed().toStdString();
            std::string ten = tenInput->text().trimmed().toStdString();
            std::string gt = gioiTinhCombo->currentText().toStdString();
            std::string sdt = sdtInput->text().trimmed().toStdString();
            std::string dc = diaChiInput->text().trimmed().toStdString();
            std::string ns = ngaySinhDate->date().toString("yyyy-MM-dd").toStdString();
            std::string nnv = ngayNhapDate->date().toString("yyyy-MM-dd").toStdString();

            // Lấy Bệnh lý từ ComboBox
            std::string bl = benhLyCombo->currentText().toStdString();

            bool lhn = hoNgheoCheck->isChecked();

            if (ten.empty() || bl.empty()) {
                QMessageBox::critical(this, "Lỗi", "Vui lòng điền đầy đủ Họ tên và chọn Bệnh lý.");
                return;
            }

            if (bn) {
                // Sửa
                bn->setHoTen(ten);
                bn->setGioiTinh(gt);
                bn->setSdt(sdt);
                bn->setDiaChi(dc);
                bn->setNgaySinh(ns);
                // NgayNhapVien không nên sửa khi đang nằm viện
                bn->setBenhLy(bl);
                bn->setLaHoNgheo(lhn);
                qlbv.suaBN(bn);
                QMessageBox::information(this, "Thành công", "Cập nhật Bệnh nhân thành công.");
            } else {
                // Thêm
                auto newBn = std::make_shared<BenhNhan>(ma, ten, gt, sdt, dc, ns, nnv, bl, lhn);
                qlbv.themBN(newBn);
                QMessageBox::information(this, "Thành công", "Thêm Bệnh nhân mới thành công. Vui lòng phân công trong tab 'Điều trị'.");
            }
            updateBenhNhanTable(filterBNInput->text().toStdString(), filterBNCombo->currentText().toStdString());
            updatePhanCongTable();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", QString("Đã xảy ra lỗi: ") + e.what());
        }
    }
}

void MainWindow::showBacSiInputDialog(std::shared_ptr<BacSi> bs) {
    QDialog dialog(this);
    dialog.setWindowTitle(bs ? "Sửa thông tin Bác sĩ" : "Thêm Bác sĩ mới");
    QGridLayout* layout = new QGridLayout(&dialog);

    // Inputs
    QLineEdit* maInput = new QLineEdit();
    QLineEdit* tenInput = new QLineEdit();
    QComboBox* gioiTinhCombo = new QComboBox();
    gioiTinhCombo->addItems({"Nam", "Nữ"});
    QLineEdit* sdtInput = new QLineEdit();
    QLineEdit* diaChiInput = new QLineEdit();
    QComboBox* chuyenKhoaCombo = new QComboBox();

    // Thêm các chuyên khoa
    for (const auto& pair : chuyenKhoaToBenhLy) {
        chuyenKhoaCombo->addItem(QString::fromStdString(pair.first));
    }

    // Set giá trị mặc định/tự động tạo mã
    if (bs) {
        maInput->setText(QString::fromStdString(bs->getMaBS()));
        tenInput->setText(QString::fromStdString(bs->getHoTen()));
        if (bs->getGioiTinh() == "Nữ") gioiTinhCombo->setCurrentIndex(1);
        sdtInput->setText(QString::fromStdString(bs->getSdt()));
        diaChiInput->setText(QString::fromStdString(bs->getDiaChi()));
        chuyenKhoaCombo->setCurrentText(QString::fromStdString(bs->getChuyenKhoa()));
    } else {
        std::string nextMaBS = generateNextMaBS();
        maInput->setText(QString::fromStdString(nextMaBS));
    }
    maInput->setReadOnly(true); // Tự động tạo và không cho sửa

    // Layout
    layout->addWidget(new QLabel("Mã BS:"), 0, 0); layout->addWidget(maInput, 0, 1);
    layout->addWidget(new QLabel("Họ tên:"), 1, 0); layout->addWidget(tenInput, 1, 1);
    layout->addWidget(new QLabel("Giới tính:"), 2, 0); layout->addWidget(gioiTinhCombo, 2, 1);
    layout->addWidget(new QLabel("SĐT:"), 3, 0); layout->addWidget(sdtInput, 3, 1);
    layout->addWidget(new QLabel("Địa chỉ:"), 4, 0); layout->addWidget(diaChiInput, 4, 1);
    layout->addWidget(new QLabel("Chuyên khoa:"), 5, 0); layout->addWidget(chuyenKhoaCombo, 5, 1);

    QPushButton* okButton = new QPushButton("Lưu");
    QPushButton* cancelButton = new QPushButton("Hủy");
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(okButton);
    btnLayout->addWidget(cancelButton);
    layout->addLayout(btnLayout, 6, 1);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        try {
            std::string ma = maInput->text().trimmed().toStdString();
            std::string ten = tenInput->text().trimmed().toStdString();
            std::string gt = gioiTinhCombo->currentText().toStdString();
            std::string sdt = sdtInput->text().trimmed().toStdString();
            std::string dc = diaChiInput->text().trimmed().toStdString();
            std::string ck = chuyenKhoaCombo->currentText().toStdString();

            if (ten.empty() || ck.empty()) {
                QMessageBox::critical(this, "Lỗi", "Vui lòng điền đầy đủ Họ tên và Chuyên khoa.");
                return;
            }

            if (bs) {
                // Sửa
                bs->setHoTen(ten);
                bs->setGioiTinh(gt);
                bs->setSdt(sdt);
                bs->setDiaChi(dc);
                bs->setChuyenKhoa(ck);
                qlbv.suaBS(bs);
                QMessageBox::information(this, "Thành công", "Cập nhật Bác sĩ thành công.");
            } else {
                // Thêm
                auto newBs = std::make_shared<BacSi>(ma, ten, gt, sdt, dc, ck);
                qlbv.themBS(newBs);
                QMessageBox::information(this, "Thành công", "Thêm Bác sĩ mới thành công.");
            }
            updateBacSiTable(searchBacSiLine->text().toStdString(), filterChuyenKhoaBacSiCombo->currentText().toStdString());
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", QString("Đã xảy ra lỗi: ") + e.what());
        }
    }
}

void MainWindow::showPhongInputDialog(std::shared_ptr<PhongBenh> phong) {
    QDialog dialog(this);
    dialog.setWindowTitle(phong ? "Sửa thông tin Phòng bệnh" : "Thêm Phòng bệnh mới");
    QGridLayout* layout = new QGridLayout(&dialog);

    // Inputs
    QLineEdit* maInput = new QLineEdit(phong ? QString::fromStdString(phong->getMaPhong()) : "");
    QComboBox* loaiCombo = new QComboBox();
    loaiCombo->addItems({"Thường", "VIP"});
    QSpinBox* soGiuongSpin = new QSpinBox();
    soGiuongSpin->setRange(1, 20);
    QComboBox* trangThaiCombo = new QComboBox();
    trangThaiCombo->addItems({"Trống", "Còn chỗ", "Đầy", "Sửa chữa"});

    // Set giá trị mặc định
    if (phong) {
        maInput->setReadOnly(true);
        loaiCombo->setCurrentText(QString::fromStdString(phong->getLoaiPhong()));
        soGiuongSpin->setValue(phong->getSoGiuong());
        trangThaiCombo->setCurrentText(QString::fromStdString(phong->getTrangThai()));
    }

    // Layout
    layout->addWidget(new QLabel("Mã Phòng:"), 0, 0); layout->addWidget(maInput, 0, 1);
    layout->addWidget(new QLabel("Loại Phòng:"), 1, 0); layout->addWidget(loaiCombo, 1, 1);
    layout->addWidget(new QLabel("Số giường:"), 2, 0); layout->addWidget(soGiuongSpin, 2, 1);
    layout->addWidget(new QLabel("Trạng thái:"), 3, 0); layout->addWidget(trangThaiCombo, 3, 1);

    QPushButton* okButton = new QPushButton("Lưu");
    QPushButton* cancelButton = new QPushButton("Hủy");
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(okButton);
    btnLayout->addWidget(cancelButton);
    layout->addLayout(btnLayout, 4, 1);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        try {
            std::string ma = maInput->text().trimmed().toStdString();
            std::string loai = loaiCombo->currentText().toStdString();
            int soGiuong = soGiuongSpin->value();
            std::string trangThai = trangThaiCombo->currentText().toStdString();

            if (phong) {
                // Sửa
                phong->setLoaiPhong(loai);
                phong->setSoGiuong(soGiuong);
                phong->setTrangThai(trangThai);
                qlbv.suaPhong(phong);
                QMessageBox::information(this, "Thành công", "Cập nhật Phòng bệnh thành công.");
            } else {
                // Thêm
                auto newPhong = std::make_shared<PhongBenh>(ma, loai, soGiuong);
                newPhong->setTrangThai(trangThai);
                qlbv.themPhong(newPhong);
                QMessageBox::information(this, "Thành công", "Thêm Phòng bệnh mới thành công.");
            }
            updatePhongTable();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", QString("Đã xảy ra lỗi: ") + e.what());
        }
    }
}

// ============= QUẢN LÝ ĐIỀU TRỊ =============
void MainWindow::showPhanCongDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Phân công Bác sĩ & Phòng bệnh");
    QGridLayout* layout = new QGridLayout(&dialog);

    // Dữ liệu
    QComboBox* bnCombo = new QComboBox();
    QComboBox* bsCombo = new QComboBox();
    QComboBox* phongCombo = new QComboBox();

    // 1. Dựng danh sách Bệnh nhân (kèm Bệnh lý)
    for (const auto& pair : qlbv.getDsBN()) {
        if (pair.second->getTrangThaiNhapVien()) {
            bnCombo->addItem(QString::fromStdString(pair.first + " - " + pair.second->getHoTen() + " (Bệnh: " + pair.second->getBenhLy() + ")"));
        }
    }

    // 2. Dựng danh sách Phòng bệnh (kèm số giường trống)
    for (const auto& pair : qlbv.getDsPhong()) {
        const auto& phong = pair.second;
        QString itemText = QString::fromStdString(phong->getMaPhong() + " - " + phong->getLoaiPhong() + " (" + std::to_string(phong->getSoGiuong() - phong->getSoBNDangNam()) + " trống)");
        if (phong->getTrangThai() != "Sửa chữa" && phong->getSoGiuong() > phong->getSoBNDangNam()) {
            phongCombo->addItem(itemText);
        }
    }

    if (bnCombo->count() == 0 || qlbv.getDsBS().empty() || phongCombo->count() == 0) {
        QMessageBox::warning(this, "Cảnh báo", "Không đủ Bệnh nhân đang nằm, Bác sĩ hoặc Phòng trống để phân công.");
        return;
    }

    // --- LOGIC LỌC BÁC SĨ (HÀM CON) ---
    auto updateBsCombo = [&]() {
        bsCombo->clear();
        if (bnCombo->currentIndex() < 0) return;

        std::string selectedText = bnCombo->currentText().toStdString();
        std::string selectedMaBN = selectedText.substr(0, 5); // BNxxx
        auto bn = qlbv.timBN(selectedMaBN);
        if (!bn) return;

        // 1. Tìm chuyên khoa yêu cầu
        std::string benhLy = bn->getBenhLy();
        std::string chuyenKhoaYeuCau = findChuyenKhoaForBenhLy(benhLy);

        // 2. Lọc danh sách Bác sĩ
        int defaultIndex = -1;
        int currentIndex = 0;

        for (const auto& pair : qlbv.getDsBS()) {
            const auto& bs = pair.second;
            std::string itemText = bs->getMaBS() + " - " + bs->getHoTen() + " (CK: " + bs->getChuyenKhoa() + ")";

            // Nếu BS có chuyên khoa phù hợp (có thể là chuyên khoa rỗng nếu bệnh lý chưa được ánh xạ)
            if (chuyenKhoaYeuCau.empty() || bs->getChuyenKhoa() == chuyenKhoaYeuCau) {
                // Thêm BS vào ComboBox
                bsCombo->addItem(QString::fromStdString(itemText));

                // Nếu đây là BS đang phụ trách BN, đặt làm mặc định
                if (qlbv.getDsPhanCong().count(selectedMaBN) && qlbv.getDsPhanCong().at(selectedMaBN).maBS == bs->getMaBS()) {
                    defaultIndex = currentIndex;
                }
                currentIndex++;
            }
        }

        // Đặt mục mặc định (nếu có)
        if (defaultIndex != -1) {
            bsCombo->setCurrentIndex(defaultIndex);
        } else {
            // Nếu không có BS nào được phân công, đặt BS đầu tiên phù hợp làm mặc định
            if (bsCombo->count() > 0) {
                bsCombo->setCurrentIndex(0);
            }
        }
    };
    // --- KẾT THÚC LOGIC LỌC BÁC SĨ ---


    // Layout
    layout->addWidget(new QLabel("Bệnh nhân (Mã - Tên - Bệnh lý):"), 0, 0); layout->addWidget(bnCombo, 0, 1);
    layout->addWidget(new QLabel("Bác sĩ (Mã - Tên - Chuyên khoa):"), 1, 0); layout->addWidget(bsCombo, 1, 1);
    layout->addWidget(new QLabel("Phòng bệnh:"), 2, 0); layout->addWidget(phongCombo, 2, 1);

    QPushButton* okButton = new QPushButton("Phân công");
    QPushButton* cancelButton = new QPushButton("Hủy");
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(okButton);
    btnLayout->addWidget(cancelButton);
    layout->addLayout(btnLayout, 3, 1);

    // Connections
    QObject::connect(bnCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [updateBsCombo](int) {
        updateBsCombo();
    });
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    // Khởi tạo ban đầu cho bsCombo
    updateBsCombo();


    if (dialog.exec() == QDialog::Accepted) {
        try {
            if (bsCombo->currentIndex() < 0) {
                throw std::runtime_error("Vui lòng chọn Bac si.");
            }
            std::string maBN = bnCombo->currentText().toStdString().substr(0, 5);
            std::string maBS = bsCombo->currentText().toStdString().substr(0, 5);
            std::string maPhong = phongCombo->currentText().toStdString().substr(0, 4);

            qlbv.phanCong(maBN, maBS, maPhong);
            QMessageBox::information(this, "Thành công", "Phân công thành công.");
            updateBenhNhanTable(filterBNInput->text().toStdString(), filterBNCombo->currentText().toStdString());
            updateBacSiTable(searchBacSiLine->text().toStdString(), filterChuyenKhoaBacSiCombo->currentText().toStdString());
            updatePhongTable();
            updatePhanCongTable();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", QString("Đã xảy ra lỗi: ") + e.what());
        }
    }
}

void MainWindow::showRaVienDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Thủ tục Ra viện & Thanh toán");
    QGridLayout* layout = new QGridLayout(&dialog);

    QComboBox* bnCombo = new QComboBox();
    for (const auto& pair : qlbv.getDsPhanCong()) {
        auto bn = qlbv.timBN(pair.first);
        if (bn) bnCombo->addItem(QString::fromStdString(pair.first + " - " + bn->getHoTen()));
    }

    if (bnCombo->count() == 0) {
        QMessageBox::warning(this, "Cảnh báo", "Không có Bệnh nhân nào đang nằm viện.");
        return;
    }

    QDateEdit* ngayRaDate = new QDateEdit(QDate::currentDate());

    // Layout
    layout->addWidget(new QLabel("Bệnh nhân:"), 0, 0); layout->addWidget(bnCombo, 0, 1);
    layout->addWidget(new QLabel("Ngày ra viện:"), 1, 0); layout->addWidget(ngayRaDate, 1, 1);

    QPushButton* okButton = new QPushButton("Thanh toán & Ra viện");
    QPushButton* cancelButton = new QPushButton("Hủy");
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(okButton);
    btnLayout->addWidget(cancelButton);
    layout->addLayout(btnLayout, 2, 1);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        try {
            std::string maBN = bnCombo->currentText().toStdString().substr(0, 5);
            auto bn = qlbv.timBN(maBN);
            if (!bn) throw std::runtime_error("Benh nhan khong ton tai.");
            const auto& pc = qlbv.getDsPhanCong().at(maBN);
            auto phong = qlbv.timPhong(pc.maPhong);
            auto bs = qlbv.timBS(pc.maBS);

            if (!phong || !bs) throw std::runtime_error("Loi du lieu phan cong.");

            QDate ngayNhap = QDate::fromString(QString::fromStdString(bn->getNgayNhapVien()), "yyyy-MM-dd");
            QDate ngayRa = ngayRaDate->date();
            if (ngayRa < ngayNhap) throw std::runtime_error("Ngay ra vien khong the truoc ngay nhap vien.");

            int soNgayNam = ngayNhap.daysTo(ngayRa) + 1; // Tính cả ngày nhập

            double giaPhong = (phong->getLoaiPhong() == "VIP") ? CauHinhGia::getInstance().giaPhongVIP : CauHinhGia::getInstance().giaPhongThuong;
            double tongTienChuaGiam = giaPhong * soNgayNam;
            double giamGia = bn->getLaHoNgheo() ? (tongTienChuaGiam * CauHinhGia::getInstance().giamGiaHoNgheo) : 0.0;
            double chiPhiSauGiam = tongTienChuaGiam - giamGia;

            exportHoaDon(bn, soNgayNam, tongTienChuaGiam, giamGia, chiPhiSauGiam, phong, bs);

            qlbv.raVien(maBN);
            QMessageBox::information(this, "Thành công", "Thủ tục Ra viện hoàn tất. Đã xuất hóa đơn.");

            updateBenhNhanTable(filterBNInput->text().toStdString(), filterBNCombo->currentText().toStdString());
            updateBacSiTable(searchBacSiLine->text().toStdString(), filterChuyenKhoaBacSiCombo->currentText().toStdString());
            updatePhongTable();
            updatePhanCongTable();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", QString("Đã xảy ra lỗi: ") + e.what());
        }
    }
}

void MainWindow::exportHoaDon(std::shared_ptr<BenhNhan> bn, int soNgayNam, double tongTienChuaGiam, double giamGia, double chiPhiSauGiam, std::shared_ptr<PhongBenh> phong, std::shared_ptr<BacSi> bs) {
    QString fileName = QFileDialog::getSaveFileName(this, "Xuất Hóa đơn", "hoa_don_" + QString::fromStdString(bn->getMaBN()) + ".txt", "Text Files (*.txt)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);

        out << "========================================\n";
        out << "         HÓA ĐƠN THANH TOÁN VIỆN PHÍ     \n";
        out << "========================================\n";
        out << "Bệnh viện ABC - Ngày: " << QDate::currentDate().toString("yyyy-MM-dd") << "\n";
        out << "----------------------------------------\n";
        out << "THÔNG TIN BỆNH NHÂN:\n";
        out << "Mã BN: " << QString::fromStdString(bn->getMaBN()) << "\n";
        out << "Họ tên: " << QString::fromStdString(bn->getHoTen()) << "\n";
        out << "Ngày nhập viện: " << QString::fromStdString(bn->getNgayNhapVien()) << "\n";
        out << "Ngày ra viện: " << QDate::currentDate().toString("yyyy-MM-dd") << "\n";
        out << "Số ngày nằm viện: " << soNgayNam << "\n";
        out << "Bệnh lý: " << QString::fromStdString(bn->getBenhLy()) << "\n";
        out << "----------------------------------------\n";
        out << "CHI TIẾT PHÍ DỊCH VỤ:\n";
        out << "- Phòng: " << QString::fromStdString(phong->getMaPhong()) << " (" << QString::fromStdString(phong->getLoaiPhong()) << ")\n";
        out << "- Chi phí phòng/ngày: " << QString::number(tongTienChuaGiam / soNgayNam, 'f', 0) << " VND\n";
        out << "----------------------------------------\n";
        out << "TỔNG KẾT:\n";
        out << "Tổng chi phí (chưa giảm): " << QString::number(tongTienChuaGiam, 'f', 0) << " VND\n";
        out << "Giảm giá (BHYT " << (bn->getLaHoNgheo() ? "CÓ" : "KHÔNG") << "): " << QString::number(giamGia, 'f', 0) << " VND\n";
        out << "----------------------------------------\n";
        out << "TỔNG THANH TOÁN: " << QString::number(chiPhiSauGiam, 'f', 0) << " VND\n";
        out << "========================================\n";
        out << "Bác sĩ phụ trách: " << QString::fromStdString(bs->getHoTen()) << " (" << QString::fromStdString(bs->getMaBS()) << ")\n";

        file.close();
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể mở file để xuất hóa đơn.");
    }
}

void MainWindow::exportTableToCsv(QTableWidget* table, const QString& defaultFileName, const QString& title) {
    QString fileName = QFileDialog::getSaveFileName(this, title, defaultFileName, "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);

        // Headers
        QStringList headers;
        for (int i = 0; i < table->columnCount(); ++i) {
            headers << table->horizontalHeaderItem(i)->text();
        }
        out << headers.join(";") << "\n";

        // Rows
        for (int i = 0; i < table->rowCount(); ++i) {
            QStringList rowData;
            for (int j = 0; j < table->columnCount(); ++j) {
                QTableWidgetItem* item = table->item(i, j);
                rowData << (item ? item->text().replace(";", ",") : "");
            }
            out << rowData.join(";") << "\n";
        }

        file.close();
        QMessageBox::information(this, "Thành công", "Đã xuất dữ liệu ra file CSV thành công.");
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể mở file để xuất CSV.");
    }
}


// ============= CÁC SLOTS CỦA BỆNH NHÂN =============
void MainWindow::onThemBenhNhan() {
    showBenhNhanInputDialog();
}

void MainWindow::onSuaBenhNhan() {
    int row = benhNhanTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn một Bệnh nhân để sửa.");
        return;
    }
    std::string maBN = benhNhanTable->item(row, 0)->text().toStdString();
    auto bn = qlbv.timBN(maBN);
    if (bn) {
        showBenhNhanInputDialog(bn);
    }
}

void MainWindow::onXoaBenhNhan() {
    int row = benhNhanTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn một Bệnh nhân để xóa.");
        return;
    }
    std::string maBN = benhNhanTable->item(row, 0)->text().toStdString();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận xóa", QString("Bạn có chắc chắn muốn xóa Bệnh nhân ") + QString::fromStdString(maBN) + "?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        try {
            qlbv.xoaBN(maBN);
            QMessageBox::information(this, "Thành công", "Đã xóa Bệnh nhân thành công.");
            updateBenhNhanTable(filterBNInput->text().toStdString(), filterBNCombo->currentText().toStdString());
            updatePhanCongTable();
            updatePhongTable();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", QString("Đã xảy ra lỗi: ") + e.what());
        }
    }
}

void MainWindow::onXuatBenhNhan() {
    exportTableToCsv(benhNhanTable, "benh_nhan.csv", "Xuất danh sách Bệnh nhân");
}

void MainWindow::onFilterBenhNhan() {
    std::string keyword = filterBNInput->text().toStdString();
    std::string filterDisease = filterBNCombo->currentText().toStdString();

    if (filterDisease == "Tất cả Bệnh lý") {
        filterDisease = "";
    }

    updateBenhNhanTable(keyword, filterDisease);
}


// ============= CÁC SLOTS CỦA BÁC SĨ =============
void MainWindow::onThemBacSi() {
    showBacSiInputDialog();
}

void MainWindow::onSuaBacSi() {
    int row = bacSiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn một Bác sĩ để sửa.");
        return;
    }
    std::string maBS = bacSiTable->item(row, 0)->text().toStdString();
    auto bs = qlbv.timBS(maBS);
    if (bs) {
        showBacSiInputDialog(bs);
    }
}

void MainWindow::onXoaBacSi() {
    int row = bacSiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn một Bác sĩ để xóa.");
        return;
    }
    std::string maBS = bacSiTable->item(row, 0)->text().toStdString();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận xóa", QString("Bạn có chắc chắn muốn xóa Bác sĩ ") + QString::fromStdString(maBS) + "?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        try {
            qlbv.xoaBS(maBS);
            QMessageBox::information(this, "Thành công", "Đã xóa Bác sĩ thành công.");
            updateBacSiTable(searchBacSiLine->text().toStdString(), filterChuyenKhoaBacSiCombo->currentText().toStdString());
            updatePhanCongTable();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", QString("Đã xảy ra lỗi: ") + e.what());
        }
    }
}

void MainWindow::onXuatBacSi() {
    exportTableToCsv(bacSiTable, "bac_si.csv", "Xuất danh sách Bác sĩ");
}

void MainWindow::onFilterBacSi() {
    std::string keyword = searchBacSiLine->text().toStdString();
    std::string filterChuyenKhoa = filterChuyenKhoaBacSiCombo->currentText().toStdString();

    if (filterChuyenKhoa == "Tất cả Chuyên khoa") {
        filterChuyenKhoa = "";
    }

    updateBacSiTable(keyword, filterChuyenKhoa);
}


// ============= CÁC SLOTS CỦA PHÒNG =============
void MainWindow::onThemPhong() {
    showPhongInputDialog();
}

void MainWindow::onSuaPhong() {
    int row = phongTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn một Phòng bệnh để sửa.");
        return;
    }
    std::string maPhong = phongTable->item(row, 0)->text().toStdString();
    auto phong = qlbv.timPhong(maPhong);
    if (phong) {
        showPhongInputDialog(phong);
    }
}

void MainWindow::onXoaPhong() {
    int row = phongTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn một Phòng bệnh để xóa.");
        return;
    }
    std::string maPhong = phongTable->item(row, 0)->text().toStdString();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận xóa", QString("Bạn có chắc chắn muốn xóa Phòng bệnh ") + QString::fromStdString(maPhong) + "?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        try {
            qlbv.xoaPhong(maPhong);
            QMessageBox::information(this, "Thành công", "Đã xóa Phòng bệnh thành công.");
            updatePhongTable();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", QString("Đã xảy ra lỗi: ") + e.what());
        }
    }
}

void MainWindow::onXuatPhong() {
    exportTableToCsv(phongTable, "phong_benh.csv", "Xuất danh sách Phòng bệnh");
}

// ============= CÁC SLOTS ĐIỀU TRỊ =============
void MainWindow::onPhanCong() {
    showPhanCongDialog();
}

void MainWindow::onRaVien() {
    showRaVienDialog();
}

// ============= CÁC SLOTS THỐNG KÊ =============
void MainWindow::onThongKe() {
    std::stringstream ss;

    ss << "=============== BÁO CÁO THỐNG KÊ ===============\n";
    ss << "Ngày tạo: " << QDate::currentDate().toString("yyyy-MM-dd").toStdString() << "\n";
    ss << "------------------------------------------------\n";

    // 1. Tình trạng Bệnh nhân
    ss << "1. Tình trạng Bệnh nhân:\n";
    int tongBn = qlbv.getDsBN().size();
    int bnDangNam = 0;
    int bnHoNgheo = 0;
    for (const auto& pair : qlbv.getDsBN()) {
        if (pair.second->getTrangThaiNhapVien()) {
            bnDangNam++;
            if (pair.second->getLaHoNgheo()) bnHoNgheo++;
        }
    }
    ss << "- Tổng số BN (trong hệ thống): " << tongBn << "\n";
    ss << "- Số BN đang nằm viện: " << bnDangNam << "\n";
    ss << "- Số BN có BHYT: " << bnHoNgheo << "\n";
    ss << "\n";

    // 2. Tình trạng Bác sĩ
    ss << "2. Tình trạng Bác sĩ:\n";
    ss << "- Tổng số Bác sĩ: " << qlbv.getDsBS().size() << "\n";

    std::shared_ptr<BacSi> bsMaxLoad = nullptr;
    int maxBn = 0;
    for (const auto& pair : qlbv.getDsBS()) {
        const auto& bs = pair.second;
        if (bs->getSoBNPhuTrach() > maxBn) {
            maxBn = bs->getSoBNPhuTrach();
            bsMaxLoad = bs;
        }
    }
    ss << "- Bác sĩ phụ trách nhiều BN nhất:\n";
    if (bsMaxLoad) {
        ss << "  + " << bsMaxLoad->getHoTen() << " (" << bsMaxLoad->getMaBS() << "): " << maxBn << " bệnh nhân\n";
    } else {
        ss << "  + N/A\n";
    }
    ss << "\n";

    // 3. Tình trạng Phòng
    ss << "3. Tình trạng Phòng:\n";
    int tongGiuong = 0;
    int giuongTrong = 0;
    int phongDay = 0;
    std::shared_ptr<PhongBenh> phongMaxLoad = nullptr;
    int maxBnPhong = 0;

    for(const auto& pair : qlbv.getDsPhong()) {
        const auto& phong = pair.second;
        tongGiuong += phong->getSoGiuong();
        giuongTrong += (phong->getSoGiuong() - phong->getSoBNDangNam());
        if (phong->getSoBNDangNam() == phong->getSoGiuong()) phongDay++;

        if (phong->getSoBNDangNam() > maxBnPhong) {
            maxBnPhong = phong->getSoBNDangNam();
            phongMaxLoad = phong;
        }
    }
    ss << "- Tổng số giường: " << tongGiuong << "\n";
    ss << "- Số giường trống: " << giuongTrong << "\n";
    ss << "- Số phòng đầy: " << phongDay << "\n";
    ss << "- Phòng đang có nhiều BN nhất: \n";
    if (phongMaxLoad) {
        ss << "  + " << phongMaxLoad->getMaPhong() << " (" << phongMaxLoad->getLoaiPhong() << "): " << maxBnPhong << "/" << phongMaxLoad->getSoGiuong() << " BN\n";
    } else {
        ss << "  + N/A\n";
    }

    thongKeOutput->setText(QString::fromStdString(ss.str()));
}
