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
#include <QMessageBox>
#include <QScrollArea>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QCloseEvent>

// =======================================================
// QUẢN LÝ BỆNH VIỆN IMPLEMENTATION
// =======================================================

void QuanLyBenhVien::luuFile(const std::string& filename, const std::string& content) const {
    QFile file(QString::fromStdString(filename));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        out << QString::fromStdString(content);
        file.close();
    }
}

std::string QuanLyBenhVien::docFile(const std::string& filename) const {
    QFile file(QString::fromStdString(filename));
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);
        QString content = in.readAll();
        file.close();
        return content.toStdString();
    }
    return "";
}

void QuanLyBenhVien::luuDuLieu() {
    std::stringstream bn_ss, bs_ss, phong_ss, counter_ss;

    // BN: MaBN|Ten|GT|NgaySinh|SDT|DiaChi|BenhLy|HoNgheo|MaBS|MaPhong|NgayNV|PTTT|NgayRV|ChiPhi|DaXuatVien
    for (const auto& pair : dsBenhNhan) {
        const auto& bn = pair.second;
        bn_ss << bn->getMaBN() << "|" << bn->getHoTen() << "|" << bn->getGioiTinh()
              << "|" << bn->getNgaySinh().toString("dd/MM/yyyy").toStdString()
              << "|" << bn->getSoDienThoai() << "|" << bn->getDiaChi()
              << "|" << bn->getBenhLy() << "|" << bn->isHoNgheo()
              << "|" << bn->getMaBSPhuTrach() << "|" << bn->getMaPhongDieuTri()
              << "|" << bn->getNgayNhapVien().toString("dd/MM/yyyy").toStdString()
              << "|" << bn->getPhuongThucThanhToan()
              << "|" << (bn->getNgayRaVien().isValid() ? bn->getNgayRaVien().toString("dd/MM/yyyy").toStdString() : "")
              << "|" << bn->getTongChiPhi()
              << "|" << bn->isDaXuatVien() << "\n";
    }

    // BS: MaBS|Ten|GT|NgaySinh|SDT|DiaChi|ChuyenKhoa
    for (const auto& pair : dsBacSi) {
        const auto& bs = pair.second;
        bs_ss << bs->getMaBS() << "|" << bs->getHoTen() << "|" << bs->getGioiTinh()
              << "|" << bs->getNgaySinh().toString("dd/MM/yyyy").toStdString()
              << "|" << bs->getSoDienThoai() << "|" << bs->getDiaChi()
              << "|" << bs->getChuyenKhoa() << "\n";
    }

    // Phong: MaPhong|LoaiPhong|SoGiuong|SoBNDangNam
    for (const auto& pair : dsPhong) {
        const auto& phong = pair.second;
        phong_ss << phong->getMaPhong() << "|" << phong->getLoaiPhong()
                 << "|" << phong->getSoGiuong() << "|" << phong->getSoBNDangNam() << "\n";
    }

    counter_ss << maBNCounter << "|" << maBSCounter << "\n";

    luuFile("benhnhan.txt", bn_ss.str());
    luuFile("bacsi.txt", bs_ss.str());
    luuFile("phongbenh.txt", phong_ss.str());
    luuFile("counter.txt", counter_ss.str());
}

void QuanLyBenhVien::docDuLieu() {
    // Đọc Counter
    std::string counter_content = docFile("counter.txt");
    if (!counter_content.empty()) {
        std::stringstream counter_ss(counter_content);
        std::string line;
        if (std::getline(counter_ss, line)) {
            std::stringstream line_ss(line);
            std::string segment;
            std::vector<std::string> segments;
            while (std::getline(line_ss, segment, '|')) {
                segments.push_back(segment);
            }
            if (segments.size() >= 2) {
                try {
                    maBNCounter = std::stoi(segments[0]);
                    maBSCounter = std::stoi(segments[1]);
                } catch (...) {
                    maBNCounter = 1;
                    maBSCounter = 1;
                }
            }
        }
    }

    // Đọc BN
    std::string bn_content = docFile("benhnhan.txt");
    std::stringstream bn_ss(bn_content);
    std::string line;
    while (std::getline(bn_ss, line)) {
        if (line.empty()) continue;
        std::stringstream line_ss(line);
        std::string segment;
        std::vector<std::string> segments;
        while (std::getline(line_ss, segment, '|')) {
            segments.push_back(segment);
        }

        if (segments.size() >= 12) {
            try {
                std::string maBN = segments[0];
                std::string hoTen = segments[1];
                std::string gioiTinh = segments[2];
                QDate ngaySinh = QDate::fromString(QString::fromStdString(segments[3]), "dd/MM/yyyy");
                std::string sdt = segments[4];
                std::string diaChi = segments[5];
                std::string benhLy = segments[6];
                bool hoNgheo = (segments[7] == "1" || segments[7] == "true");
                std::string maBSPT = segments[8];
                std::string maPhongDT = segments[9];
                QDate ngayNV = QDate::fromString(QString::fromStdString(segments[10]), "dd/MM/yyyy");
                std::string pttt = segments[11];

                auto bn = std::make_shared<BenhNhan>(maBN, hoTen, gioiTinh, ngaySinh, sdt, diaChi, benhLy, hoNgheo, ngayNV);
                bn->setMaBSPhuTrach(maBSPT);
                bn->setMaPhongDieuTri(maPhongDT);
                bn->setPhuongThucThanhToan(pttt);

                // Đọc thông tin xuất viện nếu có
                if (segments.size() >= 15) {
                    if (!segments[12].empty()) {
                        QDate ngayRV = QDate::fromString(QString::fromStdString(segments[12]), "dd/MM/yyyy");
                        bn->setNgayRaVien(ngayRV);
                    }
                    if (!segments[13].empty()) {
                        bn->setTongChiPhi(std::stod(segments[13]));
                    }
                    bool daXuatVien = (segments[14] == "1" || segments[14] == "true");
                    bn->setDaXuatVien(daXuatVien);
                }

                dsBenhNhan[maBN] = bn;

            } catch (...) { }
        }
    }

    // Đọc BS
    std::string bs_content = docFile("bacsi.txt");
    std::stringstream bs_ss(bs_content);
    while (std::getline(bs_ss, line)) {
        if (line.empty()) continue;
        std::stringstream line_ss(line);
        std::string segment;
        std::vector<std::string> segments;
        while (std::getline(line_ss, segment, '|')) {
            segments.push_back(segment);
        }

        if (segments.size() >= 7) {
            try {
                std::string maBS = segments[0];
                std::string hoTen = segments[1];
                std::string gioiTinh = segments[2];
                QDate ngaySinh = QDate::fromString(QString::fromStdString(segments[3]), "dd/MM/yyyy");
                std::string sdt = segments[4];
                std::string diaChi = segments[5];
                std::string chuyenKhoa = segments[6];

                auto bs = std::make_shared<BacSi>(maBS, hoTen, gioiTinh, ngaySinh, sdt, diaChi, chuyenKhoa);
                dsBacSi[maBS] = bs;
            } catch (...) { }
        }
    }

    // Đọc Phòng
    std::string phong_content = docFile("phongbenh.txt");
    std::stringstream phong_ss(phong_content);
    while (std::getline(phong_ss, line)) {
        if (line.empty()) continue;
        std::stringstream line_ss(line);
        std::string segment;
        std::vector<std::string> segments;
        while (std::getline(line_ss, segment, '|')) {
            segments.push_back(segment);
        }

        if (segments.size() >= 4) {
            try {
                std::string maPhong = segments[0];
                std::string loaiPhong = segments[1];
                int soGiuong = std::stoi(segments[2]);
                auto phong = std::make_shared<PhongBenh>(maPhong, loaiPhong, soGiuong);
                dsPhong[maPhong] = phong;
            } catch (...) { }
        }
    }

    // Cập nhật lại số BN trong phòng (chỉ tính BN chưa xuất viện)
    for (const auto& pair : dsBenhNhan) {
        const auto& bn = pair.second;
        if (!bn->isDaXuatVien() && !bn->getMaPhongDieuTri().empty()) {
            if (auto phong = getPhong(bn->getMaPhongDieuTri())) {
                try {
                    if (phong->getSoBNDangNam() < phong->getSoGiuong()) {
                        phong->tangBN();
                    }
                } catch (...) { }
            }
        }
    }
}

QuanLyBenhVien::QuanLyBenhVien() {
    docDuLieu();

    if (dsBenhNhan.empty()) maBNCounter = 1;
    if (dsBacSi.empty()) maBSCounter = 1;
    if (maBNCounter < 1) maBNCounter = 1;
    if (maBSCounter < 1) maBSCounter = 1;
}

std::string QuanLyBenhVien::taoMaBNTuDong() const {
    std::stringstream ss;
    ss << "BN" << std::setfill('0') << std::setw(4) << maBNCounter;
    return ss.str();
}

std::string QuanLyBenhVien::taoMaBSTuDong() const {
    std::stringstream ss;
    ss << "BS" << std::setfill('0') << std::setw(4) << maBSCounter;
    return ss.str();
}

void QuanLyBenhVien::themBenhNhan(std::shared_ptr<BenhNhan> bn) {
    if (!isMaBNUnique(bn->getMaBN())) throw std::runtime_error("Mã bệnh nhân đã tồn tại.");
    dsBenhNhan[bn->getMaBN()] = bn;
}

void QuanLyBenhVien::themBacSi(std::shared_ptr<BacSi> bs) {
    if (!isMaBSUnique(bs->getMaBS())) throw std::runtime_error("Mã bác sĩ đã tồn tại.");
    dsBacSi[bs->getMaBS()] = bs;
}

void QuanLyBenhVien::themPhong(std::shared_ptr<PhongBenh> phong) {
    if (!isMaPhongUnique(phong->getMaPhong())) throw std::runtime_error("Mã phòng đã tồn tại.");
    dsPhong[phong->getMaPhong()] = phong;
}

void QuanLyBenhVien::xoaBenhNhan(const std::string& maBN) {
    auto bn = getBenhNhan(maBN);
    if (!bn) throw std::runtime_error("Mã bệnh nhân không tồn tại.");

    if (!bn->isDaXuatVien() && !bn->getMaPhongDieuTri().empty()) {
        if (auto phong = getPhong(bn->getMaPhongDieuTri())) {
            phong->giamBN();
        }
    }

    dsBenhNhan.erase(maBN);
}

void QuanLyBenhVien::xoaBacSi(const std::string& maBS) {
    if (!getBacSi(maBS)) throw std::runtime_error("Mã bác sĩ không tồn tại.");

    for (const auto& pair : dsBenhNhan) {
        if (pair.second->getMaBSPhuTrach() == maBS) {
            pair.second->setMaBSPhuTrach("");
        }
    }

    dsBacSi.erase(maBS);
}

void QuanLyBenhVien::xoaPhong(const std::string& maPhong) {
    if (!getPhong(maPhong)) throw std::runtime_error("Mã phòng không tồn tại.");

    for (const auto& pair : dsBenhNhan) {
        if (pair.second->getMaPhongDieuTri() == maPhong) {
            pair.second->setMaPhongDieuTri("");
        }
    }

    dsPhong.erase(maPhong);
}

void QuanLyBenhVien::phanCongDieuTri(const std::string& maBN, const std::string& maBS, const std::string& maPhong) {
    auto bn = getBenhNhan(maBN);
    auto bs = getBacSi(maBS);
    auto phong = getPhong(maPhong);

    if (!bn) throw std::runtime_error("Mã bệnh nhân không tồn tại.");
    if (!bs) throw std::runtime_error("Mã bác sĩ không tồn tại.");
    if (!phong) throw std::runtime_error("Mã phòng không tồn tại.");

    if (!bn->getMaPhongDieuTri().empty() && bn->getMaPhongDieuTri() != maPhong) {
        if (auto oldPhong = getPhong(bn->getMaPhongDieuTri())) {
            oldPhong->giamBN();
        }
    }

    if (bn->getMaPhongDieuTri() != maPhong) {
        phong->tangBN();
        bn->setMaPhongDieuTri(maPhong);
    }

    bn->setMaBSPhuTrach(maBS);
}

double QuanLyBenhVien::raVien(const std::string& maBN, const QDate& ngayRaVien) {
    auto bn = getBenhNhan(maBN);
    if (!bn) throw std::runtime_error("Mã bệnh nhân không tồn tại.");
    if (bn->getMaPhongDieuTri().empty()) throw std::runtime_error("Bệnh nhân chưa được phân phòng, không thể ra viện.");
    if (ngayRaVien < bn->getNgayNhapVien()) throw std::runtime_error("Ngày ra viện không hợp lệ.");

    auto phong = getPhong(bn->getMaPhongDieuTri());
    if (!phong) throw std::runtime_error("Phòng bệnh của BN không tồn tại. (Lỗi đồng bộ)");

    int soNgay = bn->getNgayNhapVien().daysTo(ngayRaVien);
    if (soNgay == 0) soNgay = 1;

    double giaPhong = (phong->getLoaiPhong() == "VIP") ? CauHinhGia::getInstance().giaPhongVIP : CauHinhGia::getInstance().giaPhongThuong;
    double chiPhi = soNgay * giaPhong;

    if (bn->isHoNgheo()) {
        chiPhi *= (1.0 - CauHinhGia::getInstance().giamGiaHoNgheo);
    }

    // Cập nhật trạng thái xuất viện
    bn->setNgayRaVien(ngayRaVien);
    bn->setTongChiPhi(chiPhi);
    bn->setDaXuatVien(true);

    // Giảm số BN trong phòng
    phong->giamBN();

    return chiPhi;
}

std::shared_ptr<BenhNhan> QuanLyBenhVien::getBenhNhan(const std::string& maBN) const {
    auto it = dsBenhNhan.find(maBN);
    return (it != dsBenhNhan.end()) ? it->second : nullptr;
}

std::shared_ptr<BacSi> QuanLyBenhVien::getBacSi(const std::string& maBS) const {
    auto it = dsBacSi.find(maBS);
    return (it != dsBacSi.end()) ? it->second : nullptr;
}

std::shared_ptr<PhongBenh> QuanLyBenhVien::getPhong(const std::string& maPhong) const {
    auto it = dsPhong.find(maPhong);
    return (it != dsPhong.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<BenhNhan>> QuanLyBenhVien::getBenhNhanPhuTrach(const std::string& maBS) const {
    std::vector<std::shared_ptr<BenhNhan>> result;
    for (const auto& pair : dsBenhNhan) {
        if (!pair.second->isDaXuatVien() && pair.second->getMaBSPhuTrach() == maBS) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::string QuanLyBenhVien::thongKeTongHop() const {
    std::stringstream ss;
    ss << "=======================================\n";
    ss << "       BÁO CÁO THỐNG KÊ TỔNG HỢP\n";
    ss << "=======================================\n";
    ss << "1. Tình trạng Bệnh nhân:\n";

    int bnDangDieuTri = 0;
    int bnDaXuatVien = 0;
    int bnCoBS = 0;
    int bnCoPhong = 0;
    int bnHoNgheo = 0;

    for (const auto& pair : dsBenhNhan) {
        const auto& bn = pair.second;
        if (bn->isDaXuatVien()) {
            bnDaXuatVien++;
        } else {
            bnDangDieuTri++;
            if (!bn->getMaBSPhuTrach().empty()) bnCoBS++;
            if (!bn->getMaPhongDieuTri().empty()) bnCoPhong++;
        }
        if (bn->isHoNgheo()) bnHoNgheo++;
    }

    ss << "  + Tổng số BN đang điều trị: " << bnDangDieuTri << "\n";
    ss << "  + BN đã phân công BS: " << bnCoBS << "\n";
    ss << "  + BN đã phân phòng: " << bnCoPhong << "\n";
    ss << "  + BN hộ nghèo (đang điều trị): " << bnHoNgheo << "\n";
    ss << "  + Tổng số BN đã xuất viện: " << bnDaXuatVien << "\n";
    ss << "\n";

    ss << "2. Tình trạng Bác sĩ:\n";
    ss << "  + Tổng số Bác sĩ: " << dsBacSi.size() << "\n";
    int bsCoBN = 0;
    std::shared_ptr<BacSi> bsMaxLoad = nullptr;
    int maxBn = 0;
    for (const auto& pair : dsBacSi) {
        const auto& bs = pair.second;
        int soBN = getBenhNhanPhuTrach(bs->getMaBS()).size();
        if (soBN > 0) bsCoBN++;
        if (soBN > maxBn) {
            maxBn = soBN;
            bsMaxLoad = bs;
        }
    }
    ss << "  + Bác sĩ đang phụ trách BN: " << bsCoBN << "\n";
    ss << "  + Bác sĩ phụ trách nhiều BN nhất:\n";
    if (bsMaxLoad) {
        ss << "    " << bsMaxLoad->getHoTen() << " (" << bsMaxLoad->getMaBS() << "): " << maxBn << " bệnh nhân\n";
    } else {
        ss << "    N/A\n";
    }
    ss << "\n";

    ss << "3. Tình trạng Phòng:\n";
    int tongGiuong = 0;
    int giuongTrong = 0;
    int phongDay = 0;
    std::shared_ptr<PhongBenh> phongMaxLoad = nullptr;
    int maxBnPhong = 0;

    for(const auto& pair : dsPhong) {
        const auto& phong = pair.second;
        tongGiuong += phong->getSoGiuong();
        giuongTrong += (phong->getSoGiuong() - phong->getSoBNDangNam());
        if (phong->getSoBNDangNam() == phong->getSoGiuong()) phongDay++;

        if (phong->getSoBNDangNam() > maxBnPhong) {
            maxBnPhong = phong->getSoBNDangNam();
            phongMaxLoad = phong;
        }
    }
    ss << "  + Tổng số phòng: " << dsPhong.size() << "\n";
    ss << "  + Tổng số giường: " << tongGiuong << "\n";
    ss << "  + Số giường trống: " << giuongTrong << "\n";
    ss << "  + Số phòng đã đầy: " << phongDay << "\n";
    ss << "  + Phòng có nhiều BN nhất:\n";
    if (phongMaxLoad) {
        ss << "    " << phongMaxLoad->getMaPhong() << " (" << phongMaxLoad->getLoaiPhong() << "): " << maxBnPhong << "/" << phongMaxLoad->getSoGiuong() << " bệnh nhân\n";
    } else {
        ss << "    N/A\n";
    }

    ss << "=======================================\n";
    return ss.str();
}

// =======================================================
// MAIN WINDOW IMPLEMENTATION
// =======================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    hienThiBenhNhan();
    hienThiBacSi();
    hienThiPhong();
    hienThiDieuTri();
    hienThiXuatVien();
    hienThiThongKe();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI() {
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    setStyleSheet(R"(
        QMainWindow { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f8f9fc, stop:1 #e0e7ff); }
        QTabWidget::pane { border: 1px solid #c0c4cc; background: white; border-radius: 12px; margin-top: 8px; }
        QTabBar::tab {
            background: #e0e7ff; padding: 14px 32px; margin-right: 6px;
            border-top-left-radius: 12px; border-top-right-radius: 12px;
            font-weight: bold; color: #333333;
        }
        QTabBar::tab:selected {
            background: white; border-bottom: none;
            color: #1976d2;
        }
        QPushButton {
            background-color: #1976d2; color: white; border: none;
            padding: 10px 20px; border-radius: 8px;
            font-weight: bold;
        }
        QPushButton:hover { background-color: #1565c0; }
        QTableWidget {
            border: 1px solid #c0c4cc; border-radius: 8px;
            gridline-color: #dddddd;
            selection-background-color: #bbdefb;
        }
        QHeaderView::section {
            background-color: #e3f2fd; color: #1a237e;
            padding: 8px; border: 1px solid #c0c4cc;
            font-weight: bold;
        }
        QLineEdit, QComboBox, QSpinBox, QDateEdit, QTextEdit, QListWidget {
            border: 1px solid #c0c4cc; padding: 8px; border-radius: 6px;
        }
    )");

    setupBenhNhanTab();
    setupBacSiTab();
    setupPhongBenhTab();
    setupDieuTriTab();
    setupXuatVienTab();
    setupThongKeTab();

    tabWidget->addTab(benhNhanTab, "Bệnh Nhân");
    tabWidget->addTab(bacSiTab, "Bác Sĩ");
    tabWidget->addTab(phongBenhTab, "Phòng Bệnh");
    tabWidget->addTab(dieuTriTab, "Phân Công/Ra Viện");
    tabWidget->addTab(xuatVienTab, "Đã Xuất Viện");
    tabWidget->addTab(thongKeTab, "Thống Kê");

    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        QString tabText = tabWidget->tabText(index);
        if (tabText == "Bệnh Nhân") hienThiBenhNhan();
        else if (tabText == "Bác Sĩ") hienThiBacSi();
        else if (tabText == "Phòng Bệnh") hienThiPhong();
        else if (tabText == "Phân Công/Ra Viện") hienThiDieuTri();
        else if (tabText == "Đã Xuất Viện") hienThiXuatVien();
        else if (tabText == "Thống Kê") hienThiThongKe();
    });
}

void MainWindow::setupBenhNhanTab() {
    benhNhanTab = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(benhNhanTab);

    QHBoxLayout* filterLayout = new QHBoxLayout;
    QLabel* filterLabel = new QLabel("Tìm kiếm:");
    filterBNInput = new QLineEdit;
    filterBNInput->setPlaceholderText("Nhập Mã BN/Họ Tên");
    QLabel* benhLyLabel = new QLabel("Bệnh Lý:");
    filterBenhLyCombo = new QComboBox;
    filterBenhLyCombo->addItem("Tất cả bệnh lý");  // mục 0
    // Lấy danh sách bệnh lý từ chuyenKhoaMapping (bệnh lý chính là key)
    for (const auto& pair : chuyenKhoaMapping) {
        filterBenhLyCombo->addItem(QString::fromStdString(pair.first));
    }

    filterBNCombo = new QComboBox;
    filterBNCombo->addItem("Tất cả BN");
    filterBNCombo->addItem("BN đã phân công");
    filterBNCombo->addItem("BN chưa phân công");
    filterBNCombo->addItem("BN hộ nghèo");

    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(filterBNInput);
    filterLayout->addWidget(filterBNCombo);
    mainLayout->addLayout(filterLayout);
    filterLayout->addWidget(filterBenhLyCombo);

    benhNhanTable = new QTableWidget(0, 10);
    benhNhanTable->setHorizontalHeaderLabels(
        QStringList() << "STT" << "Mã BN" << "Họ Tên" << "Giới Tính"
                      << "Ngày Sinh" << "Tuổi" << "SĐT" << "Địa Chỉ"
                      << "Bệnh Lý" << "Hộ Nghèo"
        );

    benhNhanTable->setSortingEnabled(true);
    benhNhanTable->sortByColumn(-1, Qt::AscendingOrder);

    QHeaderView* header = benhNhanTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    benhNhanTable->setColumnWidth(0, 50);

    mainLayout->addWidget(benhNhanTable);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    btnThemBN = new QPushButton("Thêm BN");
    btnSuaBN = new QPushButton("Sửa BN");
    btnXoaBN = new QPushButton("Xóa BN");
    btnXuatBN = new QPushButton("Xuất File");

    buttonLayout->addWidget(btnThemBN);
    buttonLayout->addWidget(btnSuaBN);
    buttonLayout->addWidget(btnXoaBN);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(btnXuatBN);
    mainLayout->addLayout(buttonLayout);

    connect(btnThemBN, &QPushButton::clicked, this, &MainWindow::onThemBenhNhan);
    connect(btnSuaBN, &QPushButton::clicked, this, &MainWindow::onSuaBenhNhan);
    connect(btnXoaBN, &QPushButton::clicked, this, &MainWindow::onXoaBenhNhan);
    connect(btnXuatBN, &QPushButton::clicked, this, &MainWindow::onXuatBenhNhan);
    connect(filterBNInput, &QLineEdit::textChanged, this, &MainWindow::onFilterBenhNhan);
    connect(filterBNCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onFilterBenhNhan);
    connect(filterBenhLyCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onFilterBenhNhan);
}

void MainWindow::setupBacSiTab() {
    bacSiTab = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(bacSiTab);

    QHBoxLayout* filterLayout = new QHBoxLayout;
    QLabel* searchLabel = new QLabel("Tìm kiếm (Mã/Tên):");
    searchBacSiLine = new QLineEdit;
    searchBacSiLine->setPlaceholderText("Nhập Mã BS/Họ Tên");
    QLabel* filterCKLabel = new QLabel("Chuyên Khoa:");
    filterChuyenKhoaBacSiCombo = new QComboBox;
    filterChuyenKhoaBacSiCombo->addItem("Tất cả");
    QSet<QString> chuyenKhoaSet;
    for (const auto& pair : chuyenKhoaMapping) {
        for (const auto& ck : pair.second) {
            chuyenKhoaSet.insert(QString::fromStdString(ck));
        }
    }
    for (const QString& ck : chuyenKhoaSet) {
        filterChuyenKhoaBacSiCombo->addItem(ck);
    }

    filterLayout->addWidget(searchLabel);
    filterLayout->addWidget(searchBacSiLine);
    filterLayout->addWidget(filterCKLabel);
    filterLayout->addWidget(filterChuyenKhoaBacSiCombo);
    mainLayout->addLayout(filterLayout);

    bacSiTable = new QTableWidget(0, 8);
    bacSiTable->setHorizontalHeaderLabels(
        QStringList() << "STT" << "Mã BS" << "Họ Tên" << "Ngày Sinh"
                      << "SĐT" << "Địa Chỉ" << "Chuyên Khoa" << "Số BN Phụ Trách"
        );

    bacSiTable->setSortingEnabled(true);
    bacSiTable->sortByColumn(-1, Qt::AscendingOrder);

    QHeaderView* header = bacSiTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    bacSiTable->setColumnWidth(0, 50);

    mainLayout->addWidget(bacSiTable);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    btnThemBS = new QPushButton("Thêm BS");
    btnSuaBS = new QPushButton("Sửa BS");
    btnXoaBS = new QPushButton("Xóa BS");
    btnXuatBS = new QPushButton("Xuất File");

    buttonLayout->addWidget(btnThemBS);
    buttonLayout->addWidget(btnSuaBS);
    buttonLayout->addWidget(btnXoaBS);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(btnXuatBS);
    mainLayout->addLayout(buttonLayout);

    connect(btnThemBS, &QPushButton::clicked, this, &MainWindow::onThemBacSi);
    connect(btnSuaBS, &QPushButton::clicked, this, &MainWindow::onSuaBacSi);
    connect(btnXoaBS, &QPushButton::clicked, this, &MainWindow::onXoaBacSi);
    connect(btnXuatBS, &QPushButton::clicked, this, &MainWindow::onXuatBacSi);
    connect(searchBacSiLine, &QLineEdit::textChanged, this, &MainWindow::onFilterBacSi);
    connect(filterChuyenKhoaBacSiCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onFilterBacSi);
}

void MainWindow::setupPhongBenhTab() {
    phongBenhTab = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(phongBenhTab);

    phongBenhTable = new QTableWidget(0, 5);
    phongBenhTable->setHorizontalHeaderLabels(
        QStringList() << "STT" << "Mã Phòng" << "Loại Phòng" << "Số Giường" << "Số BN Đang Nằm"
        );

    phongBenhTable->setSortingEnabled(true);
    phongBenhTable->sortByColumn(-1, Qt::AscendingOrder);

    QHeaderView* header = phongBenhTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    phongBenhTable->setColumnWidth(0, 50);

    mainLayout->addWidget(phongBenhTable);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    btnThemPhong = new QPushButton("Thêm Phòng");
    btnSuaPhong = new QPushButton("Sửa Phòng");
    btnXoaPhong = new QPushButton("Xóa Phòng");
    btnXuatPhong = new QPushButton("Xuất File");

    buttonLayout->addWidget(btnThemPhong);
    buttonLayout->addWidget(btnSuaPhong);
    buttonLayout->addWidget(btnXoaPhong);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(btnXuatPhong);
    mainLayout->addLayout(buttonLayout);

    connect(btnThemPhong, &QPushButton::clicked, this, &MainWindow::onThemPhong);
    connect(btnSuaPhong, &QPushButton::clicked, this, &MainWindow::onSuaPhong);
    connect(btnXoaPhong, &QPushButton::clicked, this, &MainWindow::onXoaPhong);
    connect(btnXuatPhong, &QPushButton::clicked, this, &MainWindow::onXuatPhong);
}

void MainWindow::setupDieuTriTab() {
    dieuTriTab = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(dieuTriTab);

    QHBoxLayout* searchLayout = new QHBoxLayout;
    QLabel* searchLabel = new QLabel("Tìm kiếm:");
    searchDieuTriInput = new QLineEdit;
    searchDieuTriInput->setPlaceholderText("Nhập Mã BN hoặc Họ Tên để tìm kiếm...");
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchDieuTriInput);
    mainLayout->addLayout(searchLayout);

    QHBoxLayout* contentLayout = new QHBoxLayout;

    // Bảng BN chưa phân công
    QVBoxLayout* leftLayout = new QVBoxLayout;
    QLabel* labelChuaPC = new QLabel("📋 Bệnh Nhân Chưa Phân Công BS/Phòng:");
    labelChuaPC->setStyleSheet("font-weight: bold; font-size: 14px; color: #d32f2f;");
    tableBenhNhanChuaPhanCong = new QTableWidget(0, 4);
    tableBenhNhanChuaPhanCong->setHorizontalHeaderLabels(
        QStringList() << "Mã BN" << "Họ Tên" << "Bệnh Lý" << "Ngày Nhập Viện"
        );
    tableBenhNhanChuaPhanCong->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableBenhNhanChuaPhanCong->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableBenhNhanChuaPhanCong->setEditTriggers(QAbstractItemView::NoEditTriggers);
    leftLayout->addWidget(labelChuaPC);
    leftLayout->addWidget(tableBenhNhanChuaPhanCong);

    // Nút phân công và ra viện
    QVBoxLayout* centerLayout = new QVBoxLayout;
    centerLayout->addStretch(1);
    btnPhanCong = new QPushButton("➡️ Phân Công\nĐiều Trị");
    btnPhanCong->setMinimumHeight(80);
    btnPhanCong->setStyleSheet("font-size: 14px; font-weight: bold;");
    btnRaVien = new QPushButton("✅ Ra Viện &\nThanh Toán");
    btnRaVien->setMinimumHeight(80);
    btnRaVien->setStyleSheet("font-size: 14px; font-weight: bold; background-color: #388e3c;");
    centerLayout->addWidget(btnPhanCong);
    centerLayout->addWidget(btnRaVien);
    centerLayout->addStretch(1);

    // Bảng BN đã phân công
    QVBoxLayout* rightLayout = new QVBoxLayout;
    QLabel* labelDaPC = new QLabel("✅ Bệnh Nhân Đã Phân Công BS/Phòng:");
    labelDaPC->setStyleSheet("font-weight: bold; font-size: 14px; color: #388e3c;");
    tableBenhNhanDaPhanCong = new QTableWidget(0, 6);
    tableBenhNhanDaPhanCong->setHorizontalHeaderLabels(
        QStringList() << "Mã BN" << "Họ Tên" << "Bệnh Lý" << "Bác Sĩ" << "Phòng" << "Ngày NV"
        );
    tableBenhNhanDaPhanCong->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableBenhNhanDaPhanCong->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableBenhNhanDaPhanCong->setEditTriggers(QAbstractItemView::NoEditTriggers);
    rightLayout->addWidget(labelDaPC);
    rightLayout->addWidget(tableBenhNhanDaPhanCong);

    contentLayout->addLayout(leftLayout, 5);
    contentLayout->addLayout(centerLayout, 1);
    contentLayout->addLayout(rightLayout, 5);
    mainLayout->addLayout(contentLayout);

    connect(btnPhanCong, &QPushButton::clicked, this, &MainWindow::onPhanCong);
    connect(btnRaVien, &QPushButton::clicked, this, &MainWindow::onRaVien);
    connect(searchDieuTriInput, &QLineEdit::textChanged, this, &MainWindow::hienThiDieuTri);
}

void MainWindow::setupXuatVienTab() {
    xuatVienTab = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(xuatVienTab);

    // Thanh tìm kiếm và lọc
    QHBoxLayout* filterLayout = new QHBoxLayout;
    QLabel* searchLabel = new QLabel("Tìm kiếm:");
    searchXuatVienInput = new QLineEdit;
    searchXuatVienInput->setPlaceholderText("Nhập Mã BN/Họ Tên/Bệnh Lý");

    QLabel* filterLabel = new QLabel("Lọc theo:");
    filterXuatVienCombo = new QComboBox;
    filterXuatVienCombo->addItem("Tất cả");
    filterXuatVienCombo->addItem("Hộ nghèo");
    filterXuatVienCombo->addItem("Xuất viện tuần này");
    filterXuatVienCombo->addItem("Xuất viện tháng này");

    filterLayout->addWidget(searchLabel);
    filterLayout->addWidget(searchXuatVienInput);
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(filterXuatVienCombo);
    mainLayout->addLayout(filterLayout);

    // Bảng danh sách đã xuất viện
    xuatVienTable = new QTableWidget(0, 11);
    xuatVienTable->setHorizontalHeaderLabels(
        QStringList() << "STT" << "Mã BN" << "Họ Tên" << "Bệnh Lý"
                      << "Ngày NV" << "Ngày RV" << "Số Ngày" << "Tổng Chi Phí"
                      << "Hộ Nghèo" << "PTTT" << "Bác Sĩ"
        );

    xuatVienTable->setSortingEnabled(true);
    xuatVienTable->sortByColumn(-1, Qt::AscendingOrder);

    QHeaderView* header = xuatVienTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    xuatVienTable->setColumnWidth(0, 50);

    mainLayout->addWidget(xuatVienTable);

    // Nút chức năng
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    btnXemChiTietXV = new QPushButton("Xem Chi Tiết");
    btnXuatDSXuatVien = new QPushButton("Xuất Danh Sách");

    buttonLayout->addWidget(btnXemChiTietXV);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(btnXuatDSXuatVien);
    mainLayout->addLayout(buttonLayout);

    connect(searchXuatVienInput, &QLineEdit::textChanged, this, &MainWindow::onFilterXuatVien);
    connect(filterXuatVienCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onFilterXuatVien);
    connect(btnXemChiTietXV, &QPushButton::clicked, this, &MainWindow::onXemChiTietXuatVien);
    connect(btnXuatDSXuatVien, &QPushButton::clicked, this, &MainWindow::onXuatDanhSachXuatVien);
}

void MainWindow::setupThongKeTab() {
    thongKeTab = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(thongKeTab);

    thongKeOutput = new QTextEdit;
    thongKeOutput->setReadOnly(true);
    thongKeOutput->setFontWeight(QFont::Bold);

    btnThongKe = new QPushButton("Cập Nhật Thống Kê");

    mainLayout->addWidget(thongKeOutput);
    mainLayout->addWidget(btnThongKe);

    connect(btnThongKe, &QPushButton::clicked, this, &MainWindow::onThongKe);
}

// =======================================================
// HIỂN THỊ DỮ LIỆU
// =======================================================

void MainWindow::hienThiBenhNhan() {
    benhNhanTable->setSortingEnabled(false);
    benhNhanTable->setRowCount(0);
    int row = 0;
    int stt = 1;

    std::vector<std::shared_ptr<BenhNhan>> dsLoc;
    for (const auto& pair : qlbv.getDsBenhNhan()) {
        const auto& bn = pair.second;
        if (!bn->isDaXuatVien() && checkFilterBenhNhan(bn)) {
            dsLoc.push_back(bn);
        }
    }

    benhNhanTable->setRowCount(dsLoc.size());

    for (const auto& bn : dsLoc) {
        QTableWidgetItem* sttItem = new QTableWidgetItem(QString::number(stt++));
        sttItem->setTextAlignment(Qt::AlignCenter);
        sttItem->setData(Qt::EditRole, stt - 1);
        benhNhanTable->setItem(row, 0, sttItem);

        benhNhanTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(bn->getMaBN())));
        benhNhanTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(bn->getHoTen())));
        benhNhanTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(bn->getGioiTinh())));
        benhNhanTable->setItem(row, 4, new QTableWidgetItem(bn->getNgaySinh().toString("dd/MM/yyyy")));

        QTableWidgetItem* tuoiItem = new QTableWidgetItem(QString::number(bn->getTuoi()));
        tuoiItem->setData(Qt::EditRole, bn->getTuoi());
        tuoiItem->setTextAlignment(Qt::AlignCenter);
        benhNhanTable->setItem(row, 5, tuoiItem);

        benhNhanTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(bn->getSoDienThoai())));
        benhNhanTable->setItem(row, 7, new QTableWidgetItem(QString::fromStdString(bn->getDiaChi())));
        benhNhanTable->setItem(row, 8, new QTableWidgetItem(QString::fromStdString(bn->getBenhLy())));
        benhNhanTable->setItem(row, 9, new QTableWidgetItem(bn->isHoNgheo() ? "Có" : "Không"));

        row++;
    }

    benhNhanTable->setSortingEnabled(true);
}

void MainWindow::hienThiBacSi() {
    bacSiTable->setSortingEnabled(false);
    bacSiTable->setRowCount(0);
    int row = 0;
    int stt = 1;

    std::vector<std::shared_ptr<BacSi>> dsLoc;
    for (const auto& pair : qlbv.getDsBacSi()) {
        const auto& bs = pair.second;
        if (checkFilterBacSi(bs)) {
            dsLoc.push_back(bs);
        }
    }

    bacSiTable->setRowCount(dsLoc.size());

    for (const auto& bs : dsLoc) {
        QTableWidgetItem* sttItem = new QTableWidgetItem(QString::number(stt++));
        sttItem->setTextAlignment(Qt::AlignCenter);
        sttItem->setData(Qt::EditRole, stt - 1);
        bacSiTable->setItem(row, 0, sttItem);

        bacSiTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(bs->getMaBS())));
        bacSiTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(bs->getHoTen())));
        bacSiTable->setItem(row, 3, new QTableWidgetItem(bs->getNgaySinh().toString("dd/MM/yyyy")));
        bacSiTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(bs->getSoDienThoai())));
        bacSiTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(bs->getDiaChi())));
        bacSiTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(bs->getChuyenKhoa())));

        int soBN = qlbv.getBenhNhanPhuTrach(bs->getMaBS()).size();
        QTableWidgetItem* soBNItem = new QTableWidgetItem(QString::number(soBN));
        soBNItem->setData(Qt::EditRole, soBN);
        soBNItem->setTextAlignment(Qt::AlignCenter);
        bacSiTable->setItem(row, 7, soBNItem);

        row++;
    }

    bacSiTable->setSortingEnabled(true);
}

void MainWindow::hienThiPhong() {
    phongBenhTable->setSortingEnabled(false);
    phongBenhTable->setRowCount(0);
    int row = 0;
    int stt = 1;

    std::vector<std::shared_ptr<PhongBenh>> dsLoc;
    for (const auto& pair : qlbv.getDsPhong()) {
        dsLoc.push_back(pair.second);
    }
    std::sort(dsLoc.begin(), dsLoc.end(), [](const auto& a, const auto& b) {
        return a->getMaPhong() < b->getMaPhong();
    });

    phongBenhTable->setRowCount(dsLoc.size());

    for (const auto& phong : dsLoc) {
        QTableWidgetItem* sttItem = new QTableWidgetItem(QString::number(stt++));
        sttItem->setTextAlignment(Qt::AlignCenter);
        sttItem->setData(Qt::EditRole, stt - 1);
        phongBenhTable->setItem(row, 0, sttItem);

        phongBenhTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(phong->getMaPhong())));
        phongBenhTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(phong->getLoaiPhong())));

        QTableWidgetItem* giuongItem = new QTableWidgetItem(QString::number(phong->getSoGiuong()));
        giuongItem->setData(Qt::EditRole, phong->getSoGiuong());
        giuongItem->setTextAlignment(Qt::AlignCenter);
        phongBenhTable->setItem(row, 3, giuongItem);

        QTableWidgetItem* bnItem = new QTableWidgetItem(QString::number(phong->getSoBNDangNam()));
        bnItem->setData(Qt::EditRole, phong->getSoBNDangNam());
        bnItem->setTextAlignment(Qt::AlignCenter);
        phongBenhTable->setItem(row, 4, bnItem);

        row++;
    }

    phongBenhTable->setSortingEnabled(true);
}

void MainWindow::hienThiDieuTri() {
    tableBenhNhanChuaPhanCong->setRowCount(0);
    tableBenhNhanDaPhanCong->setRowCount(0);

    QString searchText = searchDieuTriInput->text().trimmed().toLower();

    int rowChua = 0, rowDa = 0;
    for (const auto& pair : qlbv.getDsBenhNhan()) {
        const auto& bn = pair.second;

        // Chỉ hiển thị BN chưa xuất viện
        if (bn->isDaXuatVien()) continue;

        // Lọc theo tìm kiếm
        if (!searchText.isEmpty()) {
            QString bnText = QString::fromStdString(bn->getMaBN() + " " + bn->getHoTen()).toLower();
            if (!bnText.contains(searchText)) continue;
        }

        if (bn->getMaBSPhuTrach().empty() || bn->getMaPhongDieuTri().empty()) {
            // BN chưa phân công
            tableBenhNhanChuaPhanCong->insertRow(rowChua);
            tableBenhNhanChuaPhanCong->setItem(rowChua, 0, new QTableWidgetItem(QString::fromStdString(bn->getMaBN())));
            tableBenhNhanChuaPhanCong->setItem(rowChua, 1, new QTableWidgetItem(QString::fromStdString(bn->getHoTen())));
            tableBenhNhanChuaPhanCong->setItem(rowChua, 2, new QTableWidgetItem(QString::fromStdString(bn->getBenhLy())));
            tableBenhNhanChuaPhanCong->setItem(rowChua, 3, new QTableWidgetItem(bn->getNgayNhapVien().toString("dd/MM/yyyy")));
            rowChua++;
        } else {
            // BN đã phân công
            tableBenhNhanDaPhanCong->insertRow(rowDa);
            tableBenhNhanDaPhanCong->setItem(rowDa, 0, new QTableWidgetItem(QString::fromStdString(bn->getMaBN())));
            tableBenhNhanDaPhanCong->setItem(rowDa, 1, new QTableWidgetItem(QString::fromStdString(bn->getHoTen())));
            tableBenhNhanDaPhanCong->setItem(rowDa, 2, new QTableWidgetItem(QString::fromStdString(bn->getBenhLy())));

            auto bs = qlbv.getBacSi(bn->getMaBSPhuTrach());
            QString bsName = bs ? QString::fromStdString(bs->getHoTen()) : "N/A";
            tableBenhNhanDaPhanCong->setItem(rowDa, 3, new QTableWidgetItem(bsName));
            tableBenhNhanDaPhanCong->setItem(rowDa, 4, new QTableWidgetItem(QString::fromStdString(bn->getMaPhongDieuTri())));
            tableBenhNhanDaPhanCong->setItem(rowDa, 5, new QTableWidgetItem(bn->getNgayNhapVien().toString("dd/MM/yyyy")));
            rowDa++;
        }
    }
}

void MainWindow::hienThiXuatVien() {
    xuatVienTable->setSortingEnabled(false);
    xuatVienTable->setRowCount(0);
    int row = 0;
    int stt = 1;

    std::vector<std::shared_ptr<BenhNhan>> dsLoc;
    for (const auto& pair : qlbv.getDsBenhNhan()) {
        const auto& bn = pair.second;
        if (bn->isDaXuatVien() && checkFilterXuatVien(bn)) {
            dsLoc.push_back(bn);
        }
    }

    // Sắp xếp theo ngày ra viện (mới nhất trước)
    std::sort(dsLoc.begin(), dsLoc.end(), [](const auto& a, const auto& b) {
        return a->getNgayRaVien() > b->getNgayRaVien();
    });

    xuatVienTable->setRowCount(dsLoc.size());

    for (const auto& bn : dsLoc) {
        QTableWidgetItem* sttItem = new QTableWidgetItem(QString::number(stt++));
        sttItem->setTextAlignment(Qt::AlignCenter);
        xuatVienTable->setItem(row, 0, sttItem);

        xuatVienTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(bn->getMaBN())));
        xuatVienTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(bn->getHoTen())));
        xuatVienTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(bn->getBenhLy())));
        xuatVienTable->setItem(row, 4, new QTableWidgetItem(bn->getNgayNhapVien().toString("dd/MM/yyyy")));
        xuatVienTable->setItem(row, 5, new QTableWidgetItem(bn->getNgayRaVien().toString("dd/MM/yyyy")));

        int soNgay = bn->getNgayNhapVien().daysTo(bn->getNgayRaVien());
        if (soNgay == 0) soNgay = 1;
        QTableWidgetItem* soNgayItem = new QTableWidgetItem(QString::number(soNgay));
        soNgayItem->setTextAlignment(Qt::AlignCenter);
        xuatVienTable->setItem(row, 6, soNgayItem);

        QTableWidgetItem* chiPhiItem = new QTableWidgetItem(QString::number(bn->getTongChiPhi(), 'f', 0) + " VNĐ");
        chiPhiItem->setTextAlignment(Qt::AlignRight);
        xuatVienTable->setItem(row, 7, chiPhiItem);

        xuatVienTable->setItem(row, 8, new QTableWidgetItem(bn->isHoNgheo() ? "Có" : "Không"));
        xuatVienTable->setItem(row, 9, new QTableWidgetItem(QString::fromStdString(bn->getPhuongThucThanhToan())));

        auto bs = qlbv.getBacSi(bn->getMaBSPhuTrach());
        QString bsName = bs ? QString::fromStdString(bs->getHoTen()) : "N/A";
        xuatVienTable->setItem(row, 10, new QTableWidgetItem(bsName));

        row++;
    }

    xuatVienTable->setSortingEnabled(true);
}

void MainWindow::hienThiThongKe() {
    thongKeOutput->setText(QString::fromStdString(qlbv.thongKeTongHop()));
}

// =======================================================
// DIALOGS
// =======================================================

void MainWindow::themSuaBenhNhanDialog(bool isThem, const std::string& maBN) {
    QDialog dialog(this);
    dialog.setWindowTitle(isThem ? "Thêm Bệnh Nhân" : "Sửa Bệnh Nhân");
    dialog.setMinimumWidth(500);

    QGridLayout* layout = new QGridLayout(&dialog);

    std::shared_ptr<BenhNhan> bn_data = isThem ? nullptr : qlbv.getBenhNhan(maBN);

    QLineEdit* maBNInput = new QLineEdit;
    if (isThem) {
        maBNInput->setText(QString::fromStdString(qlbv.taoMaBNTuDong()));
        maBNInput->setReadOnly(true);
        maBNInput->setStyleSheet("background-color: #e0e0e0;");
    } else {
        maBNInput->setText(QString::fromStdString(maBN));
        maBNInput->setReadOnly(true);
    }

    QLineEdit* hoTenInput = new QLineEdit;
    if (!isThem) hoTenInput->setText(QString::fromStdString(bn_data->getHoTen()));

    QComboBox* gioiTinhCombo = new QComboBox;
    gioiTinhCombo->addItem("Nam");
    gioiTinhCombo->addItem("Nữ");
    if (!isThem) {
        gioiTinhCombo->setCurrentText(QString::fromStdString(bn_data->getGioiTinh()));
    }

    QDateEdit* ngaySinhDateEdit = new QDateEdit(QDate::currentDate().addYears(-30));
    ngaySinhDateEdit->setCalendarPopup(true);
    ngaySinhDateEdit->setMaximumDate(QDate::currentDate());
    if (!isThem) ngaySinhDateEdit->setDate(bn_data->getNgaySinh());

    QLineEdit* sdtInput = new QLineEdit;
    if (!isThem) sdtInput->setText(QString::fromStdString(bn_data->getSoDienThoai()));

    QLineEdit* diaChiInput = new QLineEdit;
    if (!isThem) diaChiInput->setText(QString::fromStdString(bn_data->getDiaChi()));

    QComboBox* benhLyCombo = new QComboBox;
    benhLyCombo->addItem("");
    for (const auto& pair : chuyenKhoaMapping) {
        benhLyCombo->addItem(QString::fromStdString(pair.first));
    }
    if (!isThem) benhLyCombo->setCurrentText(QString::fromStdString(bn_data->getBenhLy()));

    QCheckBox* hoNgheoCheck = new QCheckBox("Thuộc hộ nghèo");
    if (!isThem) hoNgheoCheck->setChecked(bn_data->isHoNgheo());

    QDateEdit* ngayNVDateEdit = new QDateEdit(QDate::currentDate());
    ngayNVDateEdit->setCalendarPopup(true);
    if (!isThem) ngayNVDateEdit->setDate(bn_data->getNgayNhapVien());

    QPushButton* okButton = new QPushButton(isThem ? "Thêm" : "Lưu");
    QPushButton* cancelButton = new QPushButton("Hủy");

    int row = 0;
    layout->addWidget(new QLabel("Mã BN (Tự động):"), row, 0);
    layout->addWidget(maBNInput, row++, 1);
    layout->addWidget(new QLabel("Họ Tên:"), row, 0);
    layout->addWidget(hoTenInput, row++, 1);
    layout->addWidget(new QLabel("Giới Tính:"), row, 0);
    layout->addWidget(gioiTinhCombo, row++, 1);
    layout->addWidget(new QLabel("Ngày Sinh:"), row, 0);
    layout->addWidget(ngaySinhDateEdit, row++, 1);
    layout->addWidget(new QLabel("Số Điện Thoại:"), row, 0);
    layout->addWidget(sdtInput, row++, 1);
    layout->addWidget(new QLabel("Địa Chỉ:"), row, 0);
    layout->addWidget(diaChiInput, row++, 1);
    layout->addWidget(new QLabel("Bệnh Lý:"), row, 0);
    layout->addWidget(benhLyCombo, row++, 1);
    layout->addWidget(hoNgheoCheck, row++, 1);
    layout->addWidget(new QLabel("Ngày Nhập Viện:"), row, 0);
    layout->addWidget(ngayNVDateEdit, row++, 1);
    layout->addWidget(okButton, row, 0);
    layout->addWidget(cancelButton, row++, 1);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        try {
            std::string ma = maBNInput->text().toStdString();
            std::string hoTen = hoTenInput->text().toStdString();
            if (hoTen.empty()) throw std::runtime_error("Họ tên không được để trống.");

            std::string gioiTinh = gioiTinhCombo->currentText().toStdString();
            QDate ngaySinh = ngaySinhDateEdit->date();
            std::string sdt = sdtInput->text().toStdString();
            std::string diaChi = diaChiInput->text().toStdString();
            std::string benhLy = benhLyCombo->currentText().toStdString();
            if (benhLy.empty()) throw std::runtime_error("Phải chọn Bệnh Lý.");

            bool hoNgheo = hoNgheoCheck->isChecked();
            QDate ngayNV = ngayNVDateEdit->date();

            if (isThem) {
                auto new_bn = std::make_shared<BenhNhan>(ma, hoTen, gioiTinh, ngaySinh, sdt, diaChi, benhLy, hoNgheo, ngayNV);
                qlbv.themBenhNhan(new_bn);
                qlbv.tangMaBNCounter();
                QMessageBox::information(this, "Thành công", "Đã thêm Bệnh nhân mới với mã: " + QString::fromStdString(ma));
            } else {
                bn_data->setHoTen(hoTen);
                bn_data->setGioiTinh(gioiTinh);
                bn_data->setNgaySinh(ngaySinh);
                bn_data->setSoDienThoai(sdt);
                bn_data->setDiaChi(diaChi);
                bn_data->setBenhLy(benhLy);
                bn_data->setHoNgheo(hoNgheo);
                QMessageBox::information(this, "Thành công", "Đã sửa thông tin Bệnh nhân.");
            }
            hienThiBenhNhan();
            hienThiDieuTri();
            hienThiThongKe();
        } catch (const std::runtime_error& e) {
            QMessageBox::critical(this, "Lỗi", e.what());
        }
    }
}

void MainWindow::themSuaBacSiDialog(bool isThem, const std::string& maBS) {
    QDialog dialog(this);
    dialog.setWindowTitle(isThem ? "Thêm Bác Sĩ" : "Sửa Bác Sĩ");
    dialog.setMinimumWidth(500);

    QGridLayout* layout = new QGridLayout(&dialog);
    std::shared_ptr<BacSi> bs_data = isThem ? nullptr : qlbv.getBacSi(maBS);

    QLineEdit* maBSInput = new QLineEdit;
    if (isThem) {
        maBSInput->setText(QString::fromStdString(qlbv.taoMaBSTuDong()));
        maBSInput->setReadOnly(true);
        maBSInput->setStyleSheet("background-color: #e0e0e0;");
    } else {
        maBSInput->setText(QString::fromStdString(maBS));
        maBSInput->setReadOnly(true);
    }

    QLineEdit* hoTenInput = new QLineEdit;
    if (!isThem) hoTenInput->setText(QString::fromStdString(bs_data->getHoTen()));

    QComboBox* gioiTinhCombo = new QComboBox;
    gioiTinhCombo->addItems({"Nam", "Nữ"});
    if (!isThem) gioiTinhCombo->setCurrentText(QString::fromStdString(bs_data->getGioiTinh()));

    QDateEdit* ngaySinhEdit = new QDateEdit(QDate::currentDate().addYears(-60));
    ngaySinhEdit->setCalendarPopup(true);
    ngaySinhEdit->setMaximumDate(QDate::currentDate().addYears(-22));
    if (!isThem) ngaySinhEdit->setDate(bs_data->getNgaySinh());

    QLineEdit* sdtInput = new QLineEdit;
    if (!isThem) sdtInput->setText(QString::fromStdString(bs_data->getSoDienThoai()));

    QLineEdit* diaChiInput = new QLineEdit;
    if (!isThem) diaChiInput->setText(QString::fromStdString(bs_data->getDiaChi()));

    QComboBox* chuyenKhoaCombo = new QComboBox;
    QSet<QString> ckSet;
    for (const auto& p : chuyenKhoaMapping) {
        for (const auto& ck : p.second) ckSet << QString::fromStdString(ck);
    }
    chuyenKhoaCombo->addItems(ckSet.values());
    if (!isThem) chuyenKhoaCombo->setCurrentText(QString::fromStdString(bs_data->getChuyenKhoa()));

    QPushButton* okBtn = new QPushButton(isThem ? "Thêm" : "Lưu");
    QPushButton* cancelBtn = new QPushButton("Hủy");

    int r = 0;
    layout->addWidget(new QLabel("Mã BS:"), r, 0);    layout->addWidget(maBSInput, r++, 1);
    layout->addWidget(new QLabel("Họ tên:"), r, 0);   layout->addWidget(hoTenInput, r++, 1);
    layout->addWidget(new QLabel("Giới tính:"), r, 0); layout->addWidget(gioiTinhCombo, r++, 1);
    layout->addWidget(new QLabel("Ngày sinh:"), r, 0); layout->addWidget(ngaySinhEdit, r++, 1);
    layout->addWidget(new QLabel("SĐT:"), r, 0);      layout->addWidget(sdtInput, r++, 1);
    layout->addWidget(new QLabel("Địa chỉ:"), r, 0);  layout->addWidget(diaChiInput, r++, 1);
    layout->addWidget(new QLabel("Chuyên khoa:"), r, 0); layout->addWidget(chuyenKhoaCombo, r++, 1);
    layout->addWidget(okBtn, r, 0); layout->addWidget(cancelBtn, r++, 1);

    connect(okBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        try {
            std::string ma = maBSInput->text().toStdString();
            std::string ten = hoTenInput->text().toStdString();
            if (ten.empty()) throw std::runtime_error("Họ tên không được để trống");

            if (isThem) {
                auto bs = std::make_shared<BacSi>(ma, ten,
                                                  gioiTinhCombo->currentText().toStdString(),
                                                  ngaySinhEdit->date(),
                                                  sdtInput->text().toStdString(),
                                                  diaChiInput->text().toStdString(),
                                                  chuyenKhoaCombo->currentText().toStdString());
                qlbv.themBacSi(bs);
                qlbv.tangMaBSCounter();
                QMessageBox::information(this, "Thành công", "Thêm bác sĩ thành công!");
            } else {
                bs_data->setHoTen(ten);
                bs_data->setGioiTinh(gioiTinhCombo->currentText().toStdString());
                bs_data->setNgaySinh(ngaySinhEdit->date());
                bs_data->setSoDienThoai(sdtInput->text().toStdString());
                bs_data->setDiaChi(diaChiInput->text().toStdString());
                bs_data->setChuyenKhoa(chuyenKhoaCombo->currentText().toStdString());
                QMessageBox::information(this, "Thành công", "Cập nhật bác sĩ thành công!");
            }
            hienThiBacSi();
            hienThiThongKe();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", e.what());
        }
    }
}

void MainWindow::themSuaPhongDialog(bool isThem, const std::string& maPhong) {
    QDialog d(this);
    d.setWindowTitle(isThem ? "Thêm Phòng" : "Sửa Phòng");
    QGridLayout* l = new QGridLayout(&d);

    QLineEdit* ma = new QLineEdit;
    QComboBox* loai = new QComboBox; loai->addItems({"Thường", "VIP"});
    QSpinBox* giuong = new QSpinBox; giuong->setRange(1, 50); giuong->setValue(10);

    if (!isThem) {
        auto p = qlbv.getPhong(maPhong);
        ma->setText(QString::fromStdString(p->getMaPhong()));
        loai->setCurrentText(QString::fromStdString(p->getLoaiPhong()));
        giuong->setValue(p->getSoGiuong());
    } else {
        ma->setText("P" + QString::number(qlbv.getDsPhong().size() + 1));
    }
    ma->setReadOnly(!isThem);

    l->addWidget(new QLabel("Mã phòng:"), 0, 0); l->addWidget(ma, 0, 1);
    l->addWidget(new QLabel("Loại phòng:"), 1, 0); l->addWidget(loai, 1, 1);
    l->addWidget(new QLabel("Số giường:"), 2, 0); l->addWidget(giuong, 2, 1);

    QPushButton* ok = new QPushButton(isThem ? "Thêm" : "Lưu");
    QPushButton* cancel = new QPushButton("Hủy");
    l->addWidget(ok, 3, 0); l->addWidget(cancel, 3, 1);

    connect(ok, &QPushButton::clicked, &d, &QDialog::accept);
    connect(cancel, &QPushButton::clicked, &d, &QDialog::reject);

    if (d.exec() == QDialog::Accepted) {
        try {
            std::string m = ma->text().toStdString();
            std::string lp = loai->currentText().toStdString();
            int sg = giuong->value();

            if (isThem) {
                auto p = std::make_shared<PhongBenh>(m, lp, sg);
                qlbv.themPhong(p);
            } else {
                auto p = qlbv.getPhong(maPhong);
                p->setLoaiPhong(lp);
                p->setSoGiuong(sg);
            }
            hienThiPhong();
            hienThiThongKe();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", e.what());
        }
    }
}

void MainWindow::phanCongDialog() {
    auto rows = tableBenhNhanChuaPhanCong->selectionModel()->selectedRows();
    if (rows.isEmpty()) {
        QMessageBox::warning(this, "Chọn BN", "Vui lòng chọn bệnh nhân cần phân công!");
        return;
    }
    std::string maBN = tableBenhNhanChuaPhanCong->item(rows[0].row(), 0)->text().toStdString();
    auto bn = qlbv.getBenhNhan(maBN);

    QDialog d(this);
    d.setWindowTitle("Phân Công Điều Trị - " + QString::fromStdString(maBN));
    d.setMinimumWidth(500);
    QVBoxLayout* main = new QVBoxLayout(&d);

    QLabel* lblBS = new QLabel("Chọn Bác Sĩ:");
    QComboBox* cbBS = new QComboBox;
    std::string benhLy = bn->getBenhLy();
    std::vector<std::string> ckPhuHop;
    auto it = chuyenKhoaMapping.find(benhLy);
    if (it != chuyenKhoaMapping.end()) ckPhuHop = it->second;

    for (const auto& p : qlbv.getDsBacSi()) {
        std::string ck = p.second->getChuyenKhoa();
        if (std::find(ckPhuHop.begin(), ckPhuHop.end(), ck) != ckPhuHop.end() || ckPhuHop.empty()) {
            cbBS->addItem(QString::fromStdString(p.second->getMaBS() + " - " + p.second->getHoTen() + " (" + ck + ")"),
                          QString::fromStdString(p.second->getMaBS()));
        }
    }

    QLabel* lblPhong = new QLabel("Chọn Phòng:");
    QComboBox* cbPhong = new QComboBox;
    for (const auto& p : qlbv.getDsPhong()) {
        auto phong = p.second;
        if (phong->getSoBNDangNam() < phong->getSoGiuong()) {
            cbPhong->addItem(QString::fromStdString(phong->getMaPhong() + " - " + phong->getLoaiPhong() +
                                                    " (Còn " + std::to_string(phong->getSoGiuong() - phong->getSoBNDangNam()) + " giường)"),
                             QString::fromStdString(phong->getMaPhong()));
        }
    }

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton* ok = new QPushButton("Phân Công");
    QPushButton* cancel = new QPushButton("Hủy");
    btns->addWidget(ok); btns->addWidget(cancel);

    main->addWidget(lblBS); main->addWidget(cbBS);
    main->addWidget(lblPhong); main->addWidget(cbPhong);
    main->addLayout(btns);

    connect(ok, &QPushButton::clicked, &d, &QDialog::accept);
    connect(cancel, &QPushButton::clicked, &d, &QDialog::reject);

    if (d.exec() == QDialog::Accepted && cbBS->count() > 0 && cbPhong->count() > 0) {
        try {
            std::string maBS = cbBS->currentData().toString().toStdString();
            std::string maP = cbPhong->currentData().toString().toStdString();
            qlbv.phanCongDieuTri(maBN, maBS, maP);
            QMessageBox::information(this, "Thành công", "Phân công thành công!");
            hienThiDieuTri();
            hienThiThongKe();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", e.what());
        }
    }
}

void MainWindow::raVienDialog() {
    auto rows = tableBenhNhanDaPhanCong->selectionModel()->selectedRows();
    if (rows.isEmpty()) {
        QMessageBox::warning(this, "Chọn BN", "Vui lòng chọn bệnh nhân cần ra viện!");
        return;
    }
    std::string maBN = tableBenhNhanDaPhanCong->item(rows[0].row(), 0)->text().toStdString();
    auto bn = qlbv.getBenhNhan(maBN);

    QDialog d(this);
    d.setWindowTitle("Ra Viện & Thanh Toán - " + QString::fromStdString(maBN));
    QGridLayout* l = new QGridLayout(&d);

    QDateEdit* ngayRa = new QDateEdit(QDate::currentDate());
    ngayRa->setCalendarPopup(true);
    ngayRa->setMinimumDate(bn->getNgayNhapVien());

    QComboBox* pttt = new QComboBox;
    pttt->addItems({"Tiền Mặt", "Chuyển Khoản", "Thẻ"});

    l->addWidget(new QLabel("Ngày ra viện:"), 0, 0);
    l->addWidget(ngayRa, 0, 1);
    l->addWidget(new QLabel("Phương thức thanh toán:"), 1, 0);
    l->addWidget(pttt, 1, 1);

    QPushButton* ok = new QPushButton("Xuất Hóa Đơn & Ra Viện");
    QPushButton* cancel = new QPushButton("Hủy");
    l->addWidget(ok, 2, 0); l->addWidget(cancel, 2, 1);

    connect(ok, &QPushButton::clicked, &d, &QDialog::accept);
    connect(cancel, &QPushButton::clicked, &d, &QDialog::reject);

    if (d.exec() == QDialog::Accepted) {
        try {
            QDate ngayRV = ngayRa->date();
            bn->setPhuongThucThanhToan(pttt->currentText().toStdString());

            double chiPhi = qlbv.raVien(maBN, ngayRV);
            int soNgay = bn->getNgayNhapVien().daysTo(ngayRV);
            if (soNgay == 0) soNgay = 1;

            QMessageBox::information(this, "Thành công",
                                     QString("Ra viện thành công!\nTổng chi phí: %1 VNĐ\nSố ngày nằm viện: %2")
                                         .arg(chiPhi, 0, 'f', 0).arg(soNgay));

            xuatHoaDonDialog(maBN, chiPhi, soNgay);

            hienThiDieuTri();
            hienThiBenhNhan();
            hienThiXuatVien();
            hienThiThongKe();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", e.what());
        }
    }
}

void MainWindow::xuatHoaDonDialog(const std::string& maBN, double chiPhi, int soNgay)
{
    auto bn = qlbv.getBenhNhan(maBN);
    if (!bn) return;

    QString fileName = QFileDialog::getSaveFileName(this, "Xuất hóa đơn",
                                                    "HoaDon_" + QString::fromStdString(maBN) + ".pdf", "PDF (*.pdf)");
    if (fileName.isEmpty()) return;

    QString noiDung =
        "MÃ BỆNH NHÂN: " + QString::fromStdString(bn->getMaBN()) + "\n"
                                                                   "HỌ TÊN: " + QString::fromStdString(bn->getHoTen()) + "\n"
                                                   "PHÒNG: " + QString::fromStdString(bn->getMaPhongDieuTri()) + "\n"
                                                            "SỐ NGÀY NẰM VIỆN: " + QString::number(soNgay) + " ngày\n"
                                    "TỔNG VIỆN PHÍ: " + QString::number(chiPhi, 'f', 0) + " VNĐ\n";

    if (bn->isHoNgheo()) {
        noiDung += "→ Đã giảm 50% (hộ nghèo)\n";
    }

    noiDung += "PHƯƠNG THỨC THANH TOÁN: " + QString::fromStdString(bn->getPhuongThucThanhToan()) + "\n"
                                                                                                   "NGÀY XUẤT VIỆN: " + QDate::currentDate().toString("dd/MM/yyyy") + "\n\n"
                                                               "Cảm ơn quý bệnh nhân đã tin tưởng bệnh viện!";

    QTextDocument doc;
    doc.setPlainText(noiDung);

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(20, 20, 20, 20), QPageLayout::Millimeter);

    doc.print(&printer);

    QMessageBox::information(this, "Thành công", "Đã xuất hóa đơn thành công!");
}

// =======================================================
// SLOTS
// =======================================================

void MainWindow::onThemBenhNhan() {
    themSuaBenhNhanDialog(true);
}

void MainWindow::onSuaBenhNhan() {
    int row = benhNhanTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Chọn bệnh nhân", "Vui lòng chọn một bệnh nhân để sửa!");
        return;
    }
    QString maBN = benhNhanTable->item(row, 1)->text();
    themSuaBenhNhanDialog(false, maBN.toStdString());
}

void MainWindow::onXoaBenhNhan() {
    auto row = benhNhanTable->currentRow();
    if (row < 0) return;
    std::string ma = benhNhanTable->item(row, 1)->text().toStdString();
    if (QMessageBox::question(this, "Xác nhận", "Xóa bệnh nhân " + QString::fromStdString(ma) + "?") == QMessageBox::Yes) {
        try {
            qlbv.xoaBenhNhan(ma);
            hienThiBenhNhan();
            hienThiDieuTri();
            hienThiThongKe();
        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", e.what());
        }
    }
}

void MainWindow::onXuatBenhNhan() {
    QString fileName = QFileDialog::getSaveFileName(this, "Xuất danh sách bệnh nhân",
                                                    "DanhSach_BenhNhan_" + QDate::currentDate().toString("dd-MM-yyyy") + ".csv",
                                                    "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Lỗi", "Không thể tạo file!");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "\xEF\xBB\xBF";

    out << "Mã BN,Họ tên,Giới tính,Ngày sinh,SĐT,Địa chỉ,Bệnh lý,Hộ nghèo,Ngày NV,BS phụ trách,Phòng\n";

    for (const auto& p : qlbv.getDsBenhNhan()) {
        auto bn = p.second;
        if (bn->isDaXuatVien()) continue;

        QString hoNgheo = bn->isHoNgheo() ? "Có" : "Không";
        out << QString::fromStdString(bn->getMaBN()) << ","
            << QString::fromStdString(bn->getHoTen()) << ","
            << QString::fromStdString(bn->getGioiTinh()) << ","
            << bn->getNgaySinh().toString("dd/MM/yyyy") << ","
            << QString::fromStdString(bn->getSoDienThoai()) << ","
            << QString::fromStdString(bn->getDiaChi()) << ","
            << QString::fromStdString(bn->getBenhLy()) << ","
            << hoNgheo << ","
            << bn->getNgayNhapVien().toString("dd/MM/yyyy") << ","
            << QString::fromStdString(bn->getMaBSPhuTrach().empty() ? "Chưa phân công" : bn->getMaBSPhuTrach()) << ","
            << QString::fromStdString(bn->getMaPhongDieuTri().empty() ? "Chưa phân phòng" : bn->getMaPhongDieuTri()) << "\n";
    }
    file.close();
    QMessageBox::information(this, "Thành công", "Đã xuất danh sách bệnh nhân đang điều trị!");
}

void MainWindow::onThemBacSi() {
    themSuaBacSiDialog(true);
}

void MainWindow::onSuaBacSi() {
    int row = bacSiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Chọn bác sĩ", "Vui lòng chọn một bác sĩ để sửa!");
        return;
    }
    QString maBS = bacSiTable->item(row, 1)->text();
    themSuaBacSiDialog(false, maBS.toStdString());
}

void MainWindow::onXoaBacSi() {
    int row = bacSiTable->currentRow();
    if (row < 0) return;
    std::string ma = bacSiTable->item(row, 1)->text().toStdString();
    if (QMessageBox::question(this, "Xác nhận", "Xóa bác sĩ " + QString::fromStdString(ma) + "?\nCác bệnh nhân đang được bác sĩ này phụ trách sẽ bị bỏ phụ trách.") == QMessageBox::Yes) {
        try {
            qlbv.xoaBacSi(ma);
            hienThiBacSi();
            hienThiDieuTri();
            hienThiThongKe();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", e.what());
        }
    }
}

void MainWindow::onXuatBacSi() {
    QString fileName = QFileDialog::getSaveFileName(this, "Xuất danh sách bác sĩ",
                                                    "DanhSach_BacSi_" + QDate::currentDate().toString("dd-MM-yyyy") + ".csv",
                                                    "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Lỗi", "Không thể tạo file!");
        return;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "\xEF\xBB\xBF"; // BOM cho Excel hiển thị tiếng Việt đúng
    out << "Mã BS,Họ tên,Giới tính,Ngày sinh,SĐT,Địa chỉ,Chuyên khoa,Số BN phụ trách\n";
    for (const auto& p : qlbv.getDsBacSi()) {
        auto bs = p.second;
        int soBN = qlbv.getBenhNhanPhuTrach(bs->getMaBS()).size();
        out << QString::fromStdString(bs->getMaBS()) << ","
            << QString::fromStdString(bs->getHoTen()) << ","
            << QString::fromStdString(bs->getGioiTinh()) << ","
            << bs->getNgaySinh().toString("dd/MM/yyyy") << ","
            << QString::fromStdString(bs->getSoDienThoai()) << ","
            << QString::fromStdString(bs->getDiaChi()) << ","
            << QString::fromStdString(bs->getChuyenKhoa()) << ","
            << soBN << "\n";
    }
    file.close();
    QMessageBox::information(this, "Thành công", "Đã xuất danh sách bác sĩ!");
}

void MainWindow::onThemPhong() {
    themSuaPhongDialog(true);
}

void MainWindow::onSuaPhong() {
    int row = phongBenhTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Chọn phòng", "Vui lòng chọn một phòng để sửa!");
        return;
    }
    QString maPhong = phongBenhTable->item(row, 1)->text();
    themSuaPhongDialog(false, maPhong.toStdString());
}

void MainWindow::onXoaPhong() {
    int row = phongBenhTable->currentRow();
    if (row < 0) return;
    std::string ma = phongBenhTable->item(row, 1)->text().toStdString();
    if (QMessageBox::question(this, "Xác nhận", "Xóa phòng " + QString::fromStdString(ma) + "?\nCác bệnh nhân đang nằm phòng này sẽ bị bỏ phân phòng.") == QMessageBox::Yes) {
        try {
            qlbv.xoaPhong(ma);
            hienThiPhong();
            hienThiDieuTri();
            hienThiThongKe();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", e.what());
        }
    }
}

void MainWindow::onXuatPhong() {
    QString fileName = QFileDialog::getSaveFileName(this, "Xuất danh sách phòng bệnh",
                                                    "DanhSach_PhongBenh.csv", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "\xEF\xBB\xBF";
    out << "Mã phòng,Loại phòng,Số giường,Số BN đang nằm,Giường trống\n";
    for (const auto& p : qlbv.getDsPhong()) {
        auto phong = p.second;
        out << QString::fromStdString(phong->getMaPhong()) << ","
            << QString::fromStdString(phong->getLoaiPhong()) << ","
            << phong->getSoGiuong() << ","
            << phong->getSoBNDangNam() << ","
            << (phong->getSoGiuong() - phong->getSoBNDangNam()) << "\n";
    }
    file.close();
    QMessageBox::information(this, "Thành công", "Đã xuất danh sách phòng bệnh!");
}

void MainWindow::onPhanCong() {
    phanCongDialog();
}

void MainWindow::onRaVien() {
    raVienDialog();
}

// Filter bệnh nhân
bool MainWindow::checkFilterBenhNhan(const std::shared_ptr<BenhNhan>& bn) const {
    // 1. Lọc theo ô tìm kiếm (Mã/Tên)
    QString text = filterBNInput->text().trimmed().toLower();
    if (!text.isEmpty()) {
        QString search = QString::fromStdString(bn->getMaBN() + " " + bn->getHoTen()).toLower();
        if (!search.contains(text)) return false;
    }

    // 2. Lọc theo combobox Bệnh Lý
    if (filterBenhLyCombo->currentIndex() != 0) {  // không phải "Tất cả bệnh lý"
        QString selectedBenhLy = filterBenhLyCombo->currentText();
        if (QString::fromStdString(bn->getBenhLy()) != selectedBenhLy)
            return false;
    }

    // 3. Lọc theo combobox cũ (đã phân công, hộ nghèo, v.v.)
    int idx = filterBNCombo->currentIndex();
    if (idx == 1) { // BN đã phân công
        return !bn->getMaBSPhuTrach().empty() && !bn->getMaPhongDieuTri().empty();
    } else if (idx == 2) { // BN chưa phân công
        return bn->getMaBSPhuTrach().empty() || bn->getMaPhongDieuTri().empty();
    } else if (idx == 3) { // Hộ nghèo
        return bn->isHoNgheo();
    }
    return true; // Tất cả BN
}

void MainWindow::onFilterBenhNhan() {
    hienThiBenhNhan();
}

// Filter bác sĩ
bool MainWindow::checkFilterBacSi(const std::shared_ptr<BacSi>& bs) const {
    QString text = searchBacSiLine->text().trimmed().toLower();
    if (!text.isEmpty()) {
        QString search = QString::fromStdString(bs->getMaBS() + " " + bs->getHoTen()).toLower();
        if (!search.contains(text)) return false;
    }
    QString ck = filterChuyenKhoaBacSiCombo->currentText();
    if (ck != "Tất cả" && ck != QString::fromStdString(bs->getChuyenKhoa())) return false;
    return true;
}

void MainWindow::onFilterBacSi() {
    hienThiBacSi();
}

// Filter xuất viện
bool MainWindow::checkFilterXuatVien(const std::shared_ptr<BenhNhan>& bn) const {
    QString text = searchXuatVienInput->text().trimmed().toLower();
    if (!text.isEmpty()) {
        QString search = QString::fromStdString(bn->getMaBN() + " " + bn->getHoTen() + " " + bn->getBenhLy()).toLower();
        if (!search.contains(text)) return false;
    }
    int idx = filterXuatVienCombo->currentIndex();
    QDate today = QDate::currentDate();
    if (idx == 1) return bn->isHoNgheo();                                 // Hộ nghèo
    if (idx == 2) return bn->getNgayRaVien().daysTo(today) <= 7;          // Tuần này
    if (idx == 3) return bn->getNgayRaVien().daysTo(today) <= 30;         // Tháng này
    return true;
}

void MainWindow::onFilterXuatVien() {
    hienThiXuatVien();
}

void MainWindow::onXemChiTietXuatVien() {
    int row = xuatVienTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Chọn BN", "Vui lòng chọn một bệnh nhân đã xuất viện!");
        return;
    }
    std::string maBN = xuatVienTable->item(row, 1)->text().toStdString();
    auto bn = qlbv.getBenhNhan(maBN);
    int soNgay = bn->getNgayNhapVien().daysTo(bn->getNgayRaVien());
    if (soNgay == 0) soNgay = 1;
    xuatHoaDonDialog(maBN, bn->getTongChiPhi(), soNgay);
}

void MainWindow::onXuatDanhSachXuatVien() {
    QString fileName = QFileDialog::getSaveFileName(this, "Xuất danh sách đã xuất viện",
                                                    "DanhSach_XuatVien_" + QDate::currentDate().toString("dd-MM-yyyy") + ".csv",
                                                    "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "\xEF\xBB\xBF";
    out << "Mã BN,Họ tên,Bệnh lý,Ngày NV,Ngày RV,Số ngày,Tổng chi phí,Hộ nghèo,PTTT,Bác sĩ\n";
    for (const auto& p : qlbv.getDsBenhNhan()) {
        auto bn = p.second;
        if (!bn->isDaXuatVien()) continue;
        int soNgay = bn->getNgayNhapVien().daysTo(bn->getNgayRaVien());
        if (soNgay == 0) soNgay = 1;
        auto bs = qlbv.getBacSi(bn->getMaBSPhuTrach());
        QString tenBS = bs ? QString::fromStdString(bs->getHoTen()) : "";
        out << QString::fromStdString(bn->getMaBN()) << ","
            << QString::fromStdString(bn->getHoTen()) << ","
            << QString::fromStdString(bn->getBenhLy()) << ","
            << bn->getNgayNhapVien().toString("dd/MM/yyyy") << ","
            << bn->getNgayRaVien().toString("dd/MM/yyyy") << ","
            << soNgay << ","
            << QString::number(bn->getTongChiPhi(), 'f', 0) << ","
            << (bn->isHoNgheo() ? "Có" : "Không") << ","
            << QString::fromStdString(bn->getPhuongThucThanhToan()) << ","
            << tenBS << "\n";
    }
    file.close();
    QMessageBox::information(this, "Thành công", "Đã xuất danh sách bệnh nhân đã xuất viện!");
}

void MainWindow::onThongKe() {
    hienThiThongKe();
}

// Destructor lưu dữ liệu khi đóng chương trình
void MainWindow::closeEvent(QCloseEvent *event) {
    qlbv.luuDuLieu();
    event->accept();
}
