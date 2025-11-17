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

// =======================================================
// LỚP QUẢN LÝ BENH VIEN IMPLEMENTATION
// (Không thay đổi, giữ lại logic ban đầu)
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
    std::stringstream bn_ss, bs_ss, phong_ss;

    // BN: MaBN|Ten|GT|Tuoi|BenhLy|HoNgheo|MaBS|MaPhong|NgayNV
    for (const auto& pair : dsBenhNhan) {
        const auto& bn = pair.second;
        bn_ss << bn->getMaBN() << "|" << bn->getHoTen() << "|" << bn->getGioiTinh()
              << "|" << bn->getTuoi() << "|" << bn->getBenhLy() << "|" << bn->isHoNgheo()
              << "|" << bn->getMaBSPhuTrach() << "|" << bn->getMaPhongDieuTri()
              << "|" << bn->getNgayNhapVien().toString("dd/MM/yyyy").toStdString() << "\n";
    }

    // BS: MaBS|Ten|GT|Tuoi|ChuyenKhoa
    for (const auto& pair : dsBacSi) {
        const auto& bs = pair.second;
        bs_ss << bs->getMaBS() << "|" << bs->getHoTen() << "|" << bs->getGioiTinh()
              << "|" << bs->getTuoi() << "|" << bs->getChuyenKhoa() << "\n";
    }

    // Phong: MaPhong|LoaiPhong|SoGiuong|SoBNDangNam
    for (const auto& pair : dsPhong) {
        const auto& phong = pair.second;
        phong_ss << phong->getMaPhong() << "|" << phong->getLoaiPhong()
                 << "|" << phong->getSoGiuong() << "|" << phong->getSoBNDangNam() << "\n";
    }

    luuFile("benhnhan.txt", bn_ss.str());
    luuFile("bacsi.txt", bs_ss.str());
    luuFile("phongbenh.txt", phong_ss.str());
}

void QuanLyBenhVien::docDuLieu() {
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

        if (segments.size() >= 9) {
            try {
                std::string maBN = segments[0];
                std::string hoTen = segments[1];
                std::string gioiTinh = segments[2];
                int tuoi = std::stoi(segments[3]);
                std::string benhLy = segments[4];
                bool hoNgheo = (segments[5] == "1" || segments[5] == "true");
                std::string maBSPT = segments[6];
                std::string maPhongDT = segments[7];
                QDate ngayNV = QDate::fromString(QString::fromStdString(segments[8]), "dd/MM/yyyy");

                auto bn = std::make_shared<BenhNhan>(maBN, hoTen, gioiTinh, tuoi, benhLy, hoNgheo, ngayNV);
                bn->setMaBSPhuTrach(maBSPT);
                bn->setMaPhongDieuTri(maPhongDT);
                dsBenhNhan[maBN] = bn;

            } catch (...) { /* Bỏ qua dòng lỗi */ }
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

        if (segments.size() >= 5) {
            try {
                std::string maBS = segments[0];
                std::string hoTen = segments[1];
                std::string gioiTinh = segments[2];
                int tuoi = std::stoi(segments[3]);
                std::string chuyenKhoa = segments[4];

                auto bs = std::make_shared<BacSi>(maBS, hoTen, gioiTinh, tuoi, chuyenKhoa);
                dsBacSi[maBS] = bs;
            } catch (...) { /* Bỏ qua dòng lỗi */ }
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
                int soBNDangNam = std::stoi(segments[3]);

                auto phong = std::make_shared<PhongBenh>(maPhong, loaiPhong, soGiuong);
                // Cập nhật số BN Đang Nằm mà không kích hoạt exception (vì đã được lưu)
                // Đếm lại từ danh sách BN để đảm bảo đồng bộ
                int bn_count = 0;
                for (const auto& pair : dsBenhNhan) {
                    if (pair.second->getMaPhongDieuTri() == maPhong) {
                        bn_count++;
                    }
                }
                // Điều chỉnh lại số BN nếu cần (lỗi đồng bộ file)
                if (bn_count != soBNDangNam) {
                    // Cập nhật lại số BN Đang Nằm theo dữ liệu BN thực tế (ưu tiên dữ liệu BN)
                    // Dữ liệu BN: Cần một setter private/friend hoặc bỏ qua soBNDangNam trong file
                    // Để đơn giản, ta sẽ chỉ lấy số Giường từ file, và để BN tự động tăng/giảm BN.
                }
                // Vì không có setter cho soBNDangNam trong PhongBenh, ta chỉ khởi tạo rồi để nó bằng 0.
                // Sau đó, cần chạy qua danh sách BN và gán lại cho Phòng.
                // BỎ QUA segments[3] VÀ ĐỂ BN TỰ CẬP NHẬT PHÒNG
                dsPhong[maPhong] = phong;
            } catch (...) { /* Bỏ qua dòng lỗi */ }
        }
    }
    // Sau khi đọc xong BN và Phòng, ta cập nhật lại số BN Đang Nằm cho Phòng.
    for (const auto& pair : dsBenhNhan) {
        const auto& bn = pair.second;
        if (!bn->getMaPhongDieuTri().empty()) {
            if (auto phong = getPhong(bn->getMaPhongDieuTri())) {
                try {
                    // Tăng BN bằng cách bypass kiểm tra phòng đầy (vì dữ liệu đã được lưu)
                    // Đây là một vấn đề nhỏ trong thiết kế lớp, nhưng ta sẽ chấp nhận rủi ro khi đọc file
                    if (phong->getSoBNDangNam() < phong->getSoGiuong()) {
                        phong->tangBN();
                    }
                } catch (...) { /* Bỏ qua nếu phòng đầy, coi như lỗi dữ liệu */ }
            }
        }
    }
}

QuanLyBenhVien::QuanLyBenhVien() {
    docDuLieu();
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

    // Giảm BN trong phòng nếu BN đang nằm viện
    if (!bn->getMaPhongDieuTri().empty()) {
        if (auto phong = getPhong(bn->getMaPhongDieuTri())) {
            phong->giamBN();
        }
    }

    dsBenhNhan.erase(maBN);
}

void QuanLyBenhVien::xoaBacSi(const std::string& maBS) {
    if (!getBacSi(maBS)) throw std::runtime_error("Mã bác sĩ không tồn tại.");

    // Cần phải gỡ phân công cho tất cả BN mà BS này đang phụ trách
    for (const auto& pair : dsBenhNhan) {
        if (pair.second->getMaBSPhuTrach() == maBS) {
            pair.second->setMaBSPhuTrach("");
        }
    }

    dsBacSi.erase(maBS);
}

void QuanLyBenhVien::xoaPhong(const std::string& maPhong) {
    if (!getPhong(maPhong)) throw std::runtime_error("Mã phòng không tồn tại.");

    // Cần phải gỡ phòng cho tất cả BN đang nằm phòng này
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

    // 1. Gỡ khỏi phòng cũ (nếu có)
    if (!bn->getMaPhongDieuTri().empty() && bn->getMaPhongDieuTri() != maPhong) {
        if (auto oldPhong = getPhong(bn->getMaPhongDieuTri())) {
            oldPhong->giamBN();
        }
    }

    // 2. Phân công vào phòng mới
    if (bn->getMaPhongDieuTri() != maPhong) {
        phong->tangBN();
        bn->setMaPhongDieuTri(maPhong);
    }

    // 3. Phân công bác sĩ
    bn->setMaBSPhuTrach(maBS);
}

double QuanLyBenhVien::raVien(const std::string& maBN, const QDate& ngayRaVien) {
    auto bn = getBenhNhan(maBN);
    if (!bn) throw std::runtime_error("Mã bệnh nhân không tồn tại.");
    if (bn->getMaPhongDieuTri().empty()) throw std::runtime_error("Bệnh nhân chưa được phân phòng, không thể ra viện.");
    if (ngayRaVien < bn->getNgayNhapVien()) throw std::runtime_error("Ngày ra viện không hợp lệ.");

    auto phong = getPhong(bn->getMaPhongDieuTri());
    if (!phong) throw std::runtime_error("Phòng bệnh của BN không tồn tại. (Lỗi đồng bộ)");

    // 1. Tính chi phí
    int soNgay = bn->getNgayNhapVien().daysTo(ngayRaVien);
    if (soNgay == 0) soNgay = 1;

    double giaPhong = (phong->getLoaiPhong() == "VIP") ? CauHinhGia::getInstance().giaPhongVIP : CauHinhGia::getInstance().giaPhongThuong;
    double chiPhi = soNgay * giaPhong;

    if (bn->isHoNgheo()) {
        chiPhi *= (1.0 - CauHinhGia::getInstance().giamGiaHoNgheo);
    }

    // 2. Cập nhật trạng thái
    phong->giamBN(); // Giảm số BN trong phòng
    dsBenhNhan.erase(maBN); // Xóa bệnh nhân khỏi danh sách

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
        if (pair.second->getMaBSPhuTrach() == maBS) {
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
    ss << "  + Tổng số BN đang điều trị: " << dsBenhNhan.size() << "\n";

    int bnCoBS = 0;
    int bnCoPhong = 0;
    int bnHoNgheo = 0;
    for (const auto& pair : dsBenhNhan) {
        const auto& bn = pair.second;
        if (!bn->getMaBSPhuTrach().empty()) bnCoBS++;
        if (!bn->getMaPhongDieuTri().empty()) bnCoPhong++;
        if (bn->isHoNgheo()) bnHoNgheo++;
    }
    ss << "  + BN đã phân công BS: " << bnCoBS << "\n";
    ss << "  + BN đã phân phòng: " << bnCoPhong << "\n";
    ss << "  + BN hộ nghèo: " << bnHoNgheo << "\n";
    ss << "\n";

    // 2. Tình trạng Bác sĩ
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
            maxBn = getBenhNhanPhuTrach(bs->getMaBS()).size();
            bsMaxLoad = bs;
        }
    }
    ss << "  + Bác sĩ đang phụ trách BN: " << bsCoBN << "\n";
    ss << "  + Bác sĩ phụ trách nhiều BN nhất:\n";
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
        ss << "  + " << phongMaxLoad->getMaPhong() << " (" << phongMaxLoad->getLoaiPhong() << "): " << maxBnPhong << "/" << phongMaxLoad->getSoGiuong() << " bệnh nhân\n";
    } else {
        ss << "  + N/A\n";
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
    hienThiThongKe();
}

MainWindow::~MainWindow()
{
    // Destructor tự động gọi destructor của qlbv (đã lưu file)
}

void MainWindow::setupUI() {
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    // ÁP DỤNG STYLE ĐẸP (TRONG HÀM, KHÔNG BỊ LỖI)
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
    setupThongKeTab();

    tabWidget->addTab(benhNhanTab, "Bệnh Nhân");
    tabWidget->addTab(bacSiTab, "Bác Sĩ");
    tabWidget->addTab(phongBenhTab, "Phòng Bệnh");
    tabWidget->addTab(dieuTriTab, "Phân Công/Ra Viện");
    tabWidget->addTab(thongKeTab, "Thống Kê");

    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (tabWidget->tabText(index) == "Bệnh Nhân") hienThiBenhNhan();
        else if (tabWidget->tabText(index) == "Bác Sĩ") hienThiBacSi();
        else if (tabWidget->tabText(index) == "Phòng Bệnh") hienThiPhong();
        else if (tabWidget->tabText(index) == "Phân Công/Ra Viện") hienThiDieuTri();
        else if (tabWidget->tabText(index) == "Thống Kê") hienThiThongKe();
    });
}

// =======================================================
// SETUP TABS (Đã cập nhật cột STT và Sắp xếp)
// =======================================================

void MainWindow::setupBenhNhanTab() {
    benhNhanTab = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(benhNhanTab);

    // 1. Phần tìm kiếm và lọc
    QHBoxLayout* filterLayout = new QHBoxLayout;
    QLabel* filterLabel = new QLabel("Tìm kiếm:");
    filterBNInput = new QLineEdit;
    filterBNInput->setPlaceholderText("Nhập Mã BN/Họ Tên/Bệnh Lý");
    filterBNCombo = new QComboBox;
    filterBNCombo->addItem("Tất cả BN");
    filterBNCombo->addItem("BN đã phân công");
    filterBNCombo->addItem("BN chưa phân công");
    filterBNCombo->addItem("BN hộ nghèo");

    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(filterBNInput);
    filterLayout->addWidget(filterBNCombo);
    mainLayout->addLayout(filterLayout);

    // 2. Bảng hiển thị
    // Cập nhật: 7 cột (thêm STT)
    benhNhanTable = new QTableWidget(0, 7);

    benhNhanTable->setHorizontalHeaderLabels(
        QStringList() << "STT" << "Mã BN" << "Họ Tên" << "Giới Tính"
                      << "Tuổi" << "Bệnh Lý" << "Hộ Nghèo"
        );

    // Bật sắp xếp khi click header
    benhNhanTable->setSortingEnabled(true);
    benhNhanTable->sortByColumn(-1, Qt::AscendingOrder);// Không sắp xếp mặc định

    // Thiết lập chế độ kéo dãn tiêu đề cột
    QHeaderView* header = benhNhanTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::Stretch); // Cột Họ Tên tự dãn
    header->setSectionResizeMode(0, QHeaderView::Fixed); // Cột STT không dãn
    benhNhanTable->setColumnWidth(0, 50); // Chiều rộng cố định cho STT

    mainLayout->addWidget(benhNhanTable);

    // 3. Nút chức năng
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

    // Connections
    connect(btnThemBN, &QPushButton::clicked, this, &MainWindow::onThemBenhNhan);
    connect(btnSuaBN, &QPushButton::clicked, this, &MainWindow::onSuaBenhNhan);
    connect(btnXoaBN, &QPushButton::clicked, this, &MainWindow::onXoaBenhNhan);
    connect(btnXuatBN, &QPushButton::clicked, this, &MainWindow::onXuatBenhNhan);
    connect(filterBNInput, &QLineEdit::textChanged, this, &MainWindow::onFilterBenhNhan);
    connect(filterBNCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onFilterBenhNhan);
}

void MainWindow::setupBacSiTab() {
    bacSiTab = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(bacSiTab);

    // 1. Phần tìm kiếm và lọc
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

    // 2. Bảng hiển thị
    // Cập nhật: 5 cột (thêm STT)
    bacSiTable = new QTableWidget(0, 5);
    bacSiTable->setHorizontalHeaderLabels(
        QStringList() << "STT" << "Mã BS" << "Họ Tên" << "Chuyên Khoa" << "Số BN Phụ Trách"
        );

    // Bật sắp xếp khi click header
    bacSiTable->setSortingEnabled(true);
    bacSiTable->sortByColumn(-1, Qt::AscendingOrder); // Không sắp xếp mặc định

    // Thiết lập chế độ kéo dãn tiêu đề cột
    QHeaderView* header = bacSiTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::Stretch); // Cột Họ Tên tự dãn
    header->setSectionResizeMode(0, QHeaderView::Fixed); // Cột STT không dãn
    bacSiTable->setColumnWidth(0, 50); // Chiều rộng cố định cho STT

    mainLayout->addWidget(bacSiTable);

    // 3. Nút chức năng
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

    // Connections
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

    // 1. Bảng hiển thị
    // Cập nhật: 5 cột (thêm STT)
    phongBenhTable = new QTableWidget(0, 5);
    phongBenhTable->setHorizontalHeaderLabels(
        QStringList() << "STT" << "Mã Phòng" << "Loại Phòng" << "Số Giường" << "Số BN Đang Nằm"
        );

    // Bật sắp xếp khi click header
    phongBenhTable->setSortingEnabled(true);
    phongBenhTable->sortByColumn(-1, Qt::AscendingOrder);// Không sắp xếp mặc định

    // Thiết lập chế độ kéo dãn tiêu đề cột
    QHeaderView* header = phongBenhTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::Stretch); // Cột Loại Phòng tự dãn
    header->setSectionResizeMode(0, QHeaderView::Fixed); // Cột STT không dãn
    phongBenhTable->setColumnWidth(0, 50); // Chiều rộng cố định cho STT

    mainLayout->addWidget(phongBenhTable);

    // 2. Nút chức năng
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

    // Connections
    connect(btnThemPhong, &QPushButton::clicked, this, &MainWindow::onThemPhong);
    connect(btnSuaPhong, &QPushButton::clicked, this, &MainWindow::onSuaPhong);
    connect(btnXoaPhong, &QPushButton::clicked, this, &MainWindow::onXoaPhong);
    connect(btnXuatPhong, &QPushButton::clicked, this, &MainWindow::onXuatPhong);
}

void MainWindow::setupDieuTriTab() {
    dieuTriTab = new QWidget;
    QHBoxLayout* mainLayout = new QHBoxLayout(dieuTriTab);

    // Cột trái: BN chưa phân công
    QVBoxLayout* leftLayout = new QVBoxLayout;
    QLabel* labelChuaPC = new QLabel("BN Chưa Phân Công BS/Phòng:");
    listBenhNhanChuaPhanCong = new QListWidget;
    leftLayout->addWidget(labelChuaPC);
    leftLayout->addWidget(listBenhNhanChuaPhanCong);

    // Cột giữa: Nút chức năng
    QVBoxLayout* centerLayout = new QVBoxLayout;
    centerLayout->addStretch(1);
    btnPhanCong = new QPushButton("<< Phân Công >>");
    btnPhanCong->setMinimumWidth(150);
    btnRaVien = new QPushButton("Ra Viện >>");
    btnRaVien->setMinimumWidth(150);
    centerLayout->addWidget(btnPhanCong);
    centerLayout->addWidget(btnRaVien);
    centerLayout->addStretch(1);

    // Cột phải: BN đã phân công
    QVBoxLayout* rightLayout = new QVBoxLayout;
    QLabel* labelDaPC = new QLabel("BN Đã Phân Công BS/Phòng:");
    listBenhNhanDaPhanCong = new QListWidget;
    rightLayout->addWidget(labelDaPC);
    rightLayout->addWidget(listBenhNhanDaPhanCong);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(centerLayout);
    mainLayout->addLayout(rightLayout);

    // Connections
    connect(btnPhanCong, &QPushButton::clicked, this, &MainWindow::onPhanCong);
    connect(btnRaVien, &QPushButton::clicked, this, &MainWindow::onRaVien);
}

void MainWindow::setupThongKeTab() {
    thongKeTab = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(thongKeTab);

    // Output TextEdit
    thongKeOutput = new QTextEdit;
    thongKeOutput->setReadOnly(true);
    thongKeOutput->setFontWeight(QFont::Bold);

    // Button
    btnThongKe = new QPushButton("Cập Nhật Thống Kê");

    mainLayout->addWidget(thongKeOutput);
    mainLayout->addWidget(btnThongKe);

    // Connection
    connect(btnThongKe, &QPushButton::clicked, this, &MainWindow::onThongKe);
}

// =======================================================
// HIỂN THỊ DỮ LIỆU (Đã cập nhật cột STT và Sắp xếp số)
// =======================================================

void MainWindow::hienThiBenhNhan() {
    // Tắt sắp xếp tạm thời để load dữ liệu
    benhNhanTable->setSortingEnabled(false);

    benhNhanTable->setRowCount(0);
    int row = 0;
    int stt = 1; // Khởi tạo số thứ tự

    // Lấy danh sách bệnh nhân đã được lọc (hoặc tất cả)
    std::vector<std::shared_ptr<BenhNhan>> dsLoc;
    for (const auto& pair : qlbv.getDsBenhNhan()) {
        const auto& bn = pair.second;
        if (checkFilterBenhNhan(bn)) { // Kiểm tra điều kiện lọc
            dsLoc.push_back(bn);
        }
    }

    benhNhanTable->setRowCount(dsLoc.size());

    for (const auto& bn : dsLoc) {
        // Cột 0: Số Thứ Tự (STT)
        QTableWidgetItem* sttItem = new QTableWidgetItem(QString::number(stt++));
        sttItem->setTextAlignment(Qt::AlignCenter);
        // Đặt data edit role là số để sắp xếp đúng
        sttItem->setData(Qt::EditRole, stt - 1);
        benhNhanTable->setItem(row, 0, sttItem);

        // Cột 1: Mã BN
        QTableWidgetItem* maItem = new QTableWidgetItem(QString::fromStdString(bn->getMaBN()));
        benhNhanTable->setItem(row, 1, maItem);

        // Cột 2: Họ Tên
        QTableWidgetItem* tenItem = new QTableWidgetItem(QString::fromStdString(bn->getHoTen()));
        benhNhanTable->setItem(row, 2, tenItem);

        // Cột 3: Giới Tính
        QTableWidgetItem* gtItem = new QTableWidgetItem(QString::fromStdString(bn->getGioiTinh()));
        benhNhanTable->setItem(row, 3, gtItem);

        // Cột 4: Tuổi - QUAN TRỌNG: Thiết lập DATA EDIT ROLE để sắp xếp theo SỐ
        QTableWidgetItem* tuoiItem = new QTableWidgetItem(QString::number(bn->getTuoi()));
        tuoiItem->setData(Qt::EditRole, bn->getTuoi());
        tuoiItem->setTextAlignment(Qt::AlignCenter);
        benhNhanTable->setItem(row, 4, tuoiItem);

        // Cột 5: Bệnh Lý
        QTableWidgetItem* benhItem = new QTableWidgetItem(QString::fromStdString(bn->getBenhLy()));
        benhNhanTable->setItem(row, 5, benhItem);

        // Cột 6: Hộ Nghèo
        QTableWidgetItem* ngheoItem = new QTableWidgetItem(bn->isHoNgheo() ? "Có" : "Không");
        benhNhanTable->setItem(row, 6, ngheoItem);

        row++;
    }

    // Sau khi load dữ liệu, bật lại sắp xếp
    benhNhanTable->setSortingEnabled(true);
}

void MainWindow::hienThiBacSi() {
    // Tắt sắp xếp tạm thời
    bacSiTable->setSortingEnabled(false);

    bacSiTable->setRowCount(0);
    int row = 0;
    int stt = 1; // Khởi tạo số thứ tự

    // Lấy danh sách bác sĩ đã được lọc
    std::vector<std::shared_ptr<BacSi>> dsLoc;
    for (const auto& pair : qlbv.getDsBacSi()) {
        const auto& bs = pair.second;
        if (checkFilterBacSi(bs)) {
            dsLoc.push_back(bs);
        }
    }

    bacSiTable->setRowCount(dsLoc.size());

    for (const auto& bs : dsLoc) {
        // Cột 0: Số Thứ Tự (STT)
        QTableWidgetItem* sttItem = new QTableWidgetItem(QString::number(stt++));
        sttItem->setTextAlignment(Qt::AlignCenter);
        sttItem->setData(Qt::EditRole, stt - 1);
        bacSiTable->setItem(row, 0, sttItem);

        // Cột 1: Mã BS
        QTableWidgetItem* maItem = new QTableWidgetItem(QString::fromStdString(bs->getMaBS()));
        bacSiTable->setItem(row, 1, maItem);

        // Cột 2: Họ Tên
        QTableWidgetItem* tenItem = new QTableWidgetItem(QString::fromStdString(bs->getHoTen()));
        bacSiTable->setItem(row, 2, tenItem);

        // Cột 3: Chuyên Khoa
        QTableWidgetItem* ckItem = new QTableWidgetItem(QString::fromStdString(bs->getChuyenKhoa()));
        bacSiTable->setItem(row, 3, ckItem);

        // Cột 4: Số BN Phụ Trách - QUAN TRỌNG: Thiết lập DATA EDIT ROLE để sắp xếp theo SỐ
        int soBN = qlbv.getBenhNhanPhuTrach(bs->getMaBS()).size();
        QTableWidgetItem* soBNItem = new QTableWidgetItem(QString::number(soBN));
        soBNItem->setData(Qt::EditRole, soBN);
        soBNItem->setTextAlignment(Qt::AlignCenter);
        bacSiTable->setItem(row, 4, soBNItem);

        row++;
    }

    // Bật lại sắp xếp
    bacSiTable->setSortingEnabled(true);
}

void MainWindow::hienThiPhong() {
    // Tắt sắp xếp tạm thời
    phongBenhTable->setSortingEnabled(false);

    phongBenhTable->setRowCount(0);
    int row = 0;
    int stt = 1; // Khởi tạo số thứ tự

    // Lấy danh sách phòng (không có lọc phức tạp)
    std::vector<std::shared_ptr<PhongBenh>> dsLoc;
    for (const auto& pair : qlbv.getDsPhong()) {
        dsLoc.push_back(pair.second);
    }
    std::sort(dsLoc.begin(), dsLoc.end(), [](const auto& a, const auto& b) {
        return a->getMaPhong() < b->getMaPhong();
    });

    phongBenhTable->setRowCount(dsLoc.size());

    for (const auto& phong : dsLoc) {
        // Cột 0: Số Thứ Tự (STT)
        QTableWidgetItem* sttItem = new QTableWidgetItem(QString::number(stt++));
        sttItem->setTextAlignment(Qt::AlignCenter);
        sttItem->setData(Qt::EditRole, stt - 1);
        phongBenhTable->setItem(row, 0, sttItem);

        // Cột 1: Mã Phòng
        QTableWidgetItem* maItem = new QTableWidgetItem(QString::fromStdString(phong->getMaPhong()));
        phongBenhTable->setItem(row, 1, maItem);

        // Cột 2: Loại Phòng
        QTableWidgetItem* loaiItem = new QTableWidgetItem(QString::fromStdString(phong->getLoaiPhong()));
        phongBenhTable->setItem(row, 2, loaiItem);

        // Cột 3: Số Giường - QUAN TRỌNG: Thiết lập DATA EDIT ROLE để sắp xếp theo SỐ
        QTableWidgetItem* giuongItem = new QTableWidgetItem(QString::number(phong->getSoGiuong()));
        giuongItem->setData(Qt::EditRole, phong->getSoGiuong());
        giuongItem->setTextAlignment(Qt::AlignCenter);
        phongBenhTable->setItem(row, 3, giuongItem);

        // Cột 4: Số BN Đang Nằm - QUAN TRỌNG: Thiết lập DATA EDIT ROLE để sắp xếp theo SỐ
        QTableWidgetItem* bnItem = new QTableWidgetItem(QString::number(phong->getSoBNDangNam()));
        bnItem->setData(Qt::EditRole, phong->getSoBNDangNam());
        bnItem->setTextAlignment(Qt::AlignCenter);
        phongBenhTable->setItem(row, 4, bnItem);

        row++;
    }

    // Bật lại sắp xếp
    phongBenhTable->setSortingEnabled(true);
}

void MainWindow::hienThiDieuTri() {
    listBenhNhanChuaPhanCong->clear();
    listBenhNhanDaPhanCong->clear();

    for (const auto& pair : qlbv.getDsBenhNhan()) {
        const auto& bn = pair.second;
        QString text = QString::fromStdString(bn->getMaBN() + " - " + bn->getHoTen() + " (" + bn->getBenhLy() + ")");
        if (bn->getMaBSPhuTrach().empty() || bn->getMaPhongDieuTri().empty()) {
            listBenhNhanChuaPhanCong->addItem(text);
        } else {
            QString detail = " | BS: " + QString::fromStdString(bn->getMaBSPhuTrach()) +
                             " | Phòng: " + QString::fromStdString(bn->getMaPhongDieuTri());
            listBenhNhanDaPhanCong->addItem(text + detail);
        }
    }
}

void MainWindow::hienThiThongKe() {
    thongKeOutput->setText(QString::fromStdString(qlbv.thongKeTongHop()));
}

// =======================================================
// DIALOGS & SLOTS (Không thay đổi, giữ lại logic ban đầu)
// =======================================================

// ... (Giữ nguyên các hàm `themSuaBenhNhanDialog`, `themSuaBacSiDialog`, `themSuaPhongDialog`, `phanCongDialog`, `raVienDialog` của bạn)

void MainWindow::themSuaBenhNhanDialog(bool isThem, const std::string& maBN) {
    QDialog dialog(this);
    dialog.setWindowTitle(isThem ? "Thêm Bệnh Nhân" : "Sửa Bệnh Nhân");
    dialog.setMinimumWidth(400);

    QGridLayout* layout = new QGridLayout(&dialog);

    // Dữ liệu ban đầu
    std::shared_ptr<BenhNhan> bn_data = isThem ? nullptr : qlbv.getBenhNhan(maBN);

    // Ma BN
    QLineEdit* maBNInput = new QLineEdit;
    if (!isThem) {
        maBNInput->setText(QString::fromStdString(maBN));
        maBNInput->setReadOnly(true);
    }

    // Ho Ten
    QLineEdit* hoTenInput = new QLineEdit;
    if (!isThem) hoTenInput->setText(QString::fromStdString(bn_data->getHoTen()));

    // Gioi Tinh
    QComboBox* gioiTinhCombo = new QComboBox;
    gioiTinhCombo->addItem("Nam");
    gioiTinhCombo->addItem("Nữ");
    if (!isThem) {
        gioiTinhCombo->setCurrentText(QString::fromStdString(bn_data->getGioiTinh()));
    }

    // Tuoi
    QSpinBox* tuoiSpinBox = new QSpinBox;
    tuoiSpinBox->setRange(1, 120);
    if (!isThem) tuoiSpinBox->setValue(bn_data->getTuoi());
    else tuoiSpinBox->setValue(30);

    // Benh Ly
    QComboBox* benhLyCombo = new QComboBox;
    benhLyCombo->addItem(""); // Lựa chọn trống
    for (const auto& pair : chuyenKhoaMapping) {
        benhLyCombo->addItem(QString::fromStdString(pair.first));
    }
    if (!isThem) benhLyCombo->setCurrentText(QString::fromStdString(bn_data->getBenhLy()));

    // Ho Ngheo
    QCheckBox* hoNgheoCheck = new QCheckBox("Thuộc hộ nghèo");
    if (!isThem) hoNgheoCheck->setChecked(bn_data->isHoNgheo());

    // Ngay Nhap Vien
    QDateEdit* ngayNVDateEdit = new QDateEdit(QDate::currentDate());
    ngayNVDateEdit->setCalendarPopup(true);
    if (!isThem) ngayNVDateEdit->setDate(bn_data->getNgayNhapVien());

    // Nút OK/Cancel
    QPushButton* okButton = new QPushButton(isThem ? "Thêm" : "Lưu");
    QPushButton* cancelButton = new QPushButton("Hủy");

    // Layout
    layout->addWidget(new QLabel("Mã BN:"), 0, 0);
    layout->addWidget(maBNInput, 0, 1);
    layout->addWidget(new QLabel("Họ Tên:"), 1, 0);
    layout->addWidget(hoTenInput, 1, 1);
    layout->addWidget(new QLabel("Giới Tính:"), 2, 0);
    layout->addWidget(gioiTinhCombo, 2, 1);
    layout->addWidget(new QLabel("Tuổi:"), 3, 0);
    layout->addWidget(tuoiSpinBox, 3, 1);
    layout->addWidget(new QLabel("Bệnh Lý:"), 4, 0);
    layout->addWidget(benhLyCombo, 4, 1);
    layout->addWidget(hoNgheoCheck, 5, 1);
    layout->addWidget(new QLabel("Ngày Nhập Viện:"), 6, 0);
    layout->addWidget(ngayNVDateEdit, 6, 1);
    layout->addWidget(okButton, 7, 0);
    layout->addWidget(cancelButton, 7, 1);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        try {
            std::string ma = maBNInput->text().toStdString();
            if (ma.empty()) throw std::runtime_error("Mã BN không được để trống.");

            std::string hoTen = hoTenInput->text().toStdString();
            if (hoTen.empty()) throw std::runtime_error("Họ tên không được để trống.");

            std::string gioiTinh = gioiTinhCombo->currentText().toStdString();
            int tuoi = tuoiSpinBox->value();
            std::string benhLy = benhLyCombo->currentText().toStdString();
            if (benhLy.empty()) throw std::runtime_error("Phải chọn Bệnh Lý.");

            bool hoNgheo = hoNgheoCheck->isChecked();
            QDate ngayNV = ngayNVDateEdit->date();

            if (isThem) {
                auto new_bn = std::make_shared<BenhNhan>(ma, hoTen, gioiTinh, tuoi, benhLy, hoNgheo, ngayNV);
                qlbv.themBenhNhan(new_bn);
                QMessageBox::information(this, "Thành công", "Đã thêm Bệnh nhân mới.");
            } else {
                bn_data->setHoTen(hoTen);
                bn_data->setGioiTinh(gioiTinh);
                bn_data->setTuoi(tuoi);
                bn_data->setBenhLy(benhLy);
                bn_data->setHoNgheo(hoNgheo);
                // Ngày NV không được sửa sau khi nhập viện
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
    dialog.setMinimumWidth(400);

    QGridLayout* layout = new QGridLayout(&dialog);

    // Dữ liệu ban đầu
    std::shared_ptr<BacSi> bs_data = isThem ? nullptr : qlbv.getBacSi(maBS);

    // Ma BS
    QLineEdit* maBSInput = new QLineEdit;
    if (!isThem) {
        maBSInput->setText(QString::fromStdString(maBS));
        maBSInput->setReadOnly(true);
    }

    // Ho Ten
    QLineEdit* hoTenInput = new QLineEdit;
    if (!isThem) hoTenInput->setText(QString::fromStdString(bs_data->getHoTen()));

    // Gioi Tinh
    QComboBox* gioiTinhCombo = new QComboBox;
    gioiTinhCombo->addItem("Nam");
    gioiTinhCombo->addItem("Nữ");
    if (!isThem) {
        gioiTinhCombo->setCurrentText(QString::fromStdString(bs_data->getGioiTinh()));
    }

    // Tuoi
    QSpinBox* tuoiSpinBox = new QSpinBox;
    tuoiSpinBox->setRange(25, 100);
    if (!isThem) tuoiSpinBox->setValue(bs_data->getTuoi());
    else tuoiSpinBox->setValue(35);

    // Chuyen Khoa
    QComboBox* chuyenKhoaCombo = new QComboBox;
    QSet<QString> chuyenKhoaSet;
    for (const auto& pair : chuyenKhoaMapping) {
        for (const auto& ck : pair.second) {
            chuyenKhoaSet.insert(QString::fromStdString(ck));
        }
    }
    for (const QString& ck : chuyenKhoaSet) {
        chuyenKhoaCombo->addItem(ck);
    }
    if (!isThem) chuyenKhoaCombo->setCurrentText(QString::fromStdString(bs_data->getChuyenKhoa()));

    // Nút OK/Cancel
    QPushButton* okButton = new QPushButton(isThem ? "Thêm" : "Lưu");
    QPushButton* cancelButton = new QPushButton("Hủy");

    // Layout
    layout->addWidget(new QLabel("Mã BS:"), 0, 0);
    layout->addWidget(maBSInput, 0, 1);
    layout->addWidget(new QLabel("Họ Tên:"), 1, 0);
    layout->addWidget(hoTenInput, 1, 1);
    layout->addWidget(new QLabel("Giới Tính:"), 2, 0);
    layout->addWidget(gioiTinhCombo, 2, 1);
    layout->addWidget(new QLabel("Tuổi:"), 3, 0);
    layout->addWidget(tuoiSpinBox, 3, 1);
    layout->addWidget(new QLabel("Chuyên Khoa:"), 4, 0);
    layout->addWidget(chuyenKhoaCombo, 4, 1);
    layout->addWidget(okButton, 5, 0);
    layout->addWidget(cancelButton, 5, 1);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        try {
            std::string ma = maBSInput->text().toStdString();
            if (ma.empty()) throw std::runtime_error("Mã BS không được để trống.");

            std::string hoTen = hoTenInput->text().toStdString();
            if (hoTen.empty()) throw std::runtime_error("Họ tên không được để trống.");

            std::string gioiTinh = gioiTinhCombo->currentText().toStdString();
            int tuoi = tuoiSpinBox->value();
            std::string chuyenKhoa = chuyenKhoaCombo->currentText().toStdString();

            if (isThem) {
                auto new_bs = std::make_shared<BacSi>(ma, hoTen, gioiTinh, tuoi, chuyenKhoa);
                qlbv.themBacSi(new_bs);
                QMessageBox::information(this, "Thành công", "Đã thêm Bác sĩ mới.");
            } else {
                bs_data->setHoTen(hoTen);
                bs_data->setGioiTinh(gioiTinh);
                bs_data->setTuoi(tuoi);
                bs_data->setChuyenKhoa(chuyenKhoa);
                QMessageBox::information(this, "Thành công", "Đã sửa thông tin Bác sĩ.");
            }
            hienThiBacSi();
            hienThiThongKe();
        } catch (const std::runtime_error& e) {
            QMessageBox::critical(this, "Lỗi", e.what());
        }
    }
}

void MainWindow::themSuaPhongDialog(bool isThem, const std::string& maPhong) {
    QDialog dialog(this);
    dialog.setWindowTitle(isThem ? "Thêm Phòng Bệnh" : "Sửa Phòng Bệnh");
    dialog.setMinimumWidth(400);

    QGridLayout* layout = new QGridLayout(&dialog);

    // Dữ liệu ban đầu
    std::shared_ptr<PhongBenh> phong_data = isThem ? nullptr : qlbv.getPhong(maPhong);

    // Ma Phong
    QLineEdit* maPhongInput = new QLineEdit;
    if (!isThem) {
        maPhongInput->setText(QString::fromStdString(maPhong));
        maPhongInput->setReadOnly(true);
    }

    // Loai Phong
    QComboBox* loaiPhongCombo = new QComboBox;
    loaiPhongCombo->addItem("Thường");
    loaiPhongCombo->addItem("VIP");
    if (!isThem) {
        loaiPhongCombo->setCurrentText(QString::fromStdString(phong_data->getLoaiPhong()));
    }

    // So Giuong
    QSpinBox* soGiuongSpinBox = new QSpinBox;
    soGiuongSpinBox->setRange(1, 50);
    if (!isThem) {
        soGiuongSpinBox->setValue(phong_data->getSoGiuong());
        // Không cho phép giảm giường nếu số BN đang nằm lớn hơn
        if (phong_data->getSoBNDangNam() > 0) {
            soGiuongSpinBox->setMinimum(phong_data->getSoBNDangNam());
            soGiuongSpinBox->setToolTip("Không được giảm số giường dưới số BN đang nằm.");
        }
    }
    else soGiuongSpinBox->setValue(10);

    // Nút OK/Cancel
    QPushButton* okButton = new QPushButton(isThem ? "Thêm" : "Lưu");
    QPushButton* cancelButton = new QPushButton("Hủy");

    // Layout
    layout->addWidget(new QLabel("Mã Phòng:"), 0, 0);
    layout->addWidget(maPhongInput, 0, 1);
    layout->addWidget(new QLabel("Loại Phòng:"), 1, 0);
    layout->addWidget(loaiPhongCombo, 1, 1);
    layout->addWidget(new QLabel("Số Giường:"), 2, 0);
    layout->addWidget(soGiuongSpinBox, 2, 1);
    layout->addWidget(okButton, 3, 0);
    layout->addWidget(cancelButton, 3, 1);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        try {
            std::string ma = maPhongInput->text().toStdString();
            if (ma.empty()) throw std::runtime_error("Mã Phòng không được để trống.");

            std::string loaiPhong = loaiPhongCombo->currentText().toStdString();
            int soGiuong = soGiuongSpinBox->value();

            if (isThem) {
                auto new_phong = std::make_shared<PhongBenh>(ma, loaiPhong, soGiuong);
                qlbv.themPhong(new_phong);
                QMessageBox::information(this, "Thành công", "Đã thêm Phòng bệnh mới.");
            } else {
                // Kiểm tra logic giảm giường
                if (soGiuong < phong_data->getSoBNDangNam()) {
                    throw std::runtime_error("Số giường không hợp lệ, phải lớn hơn hoặc bằng số BN đang nằm.");
                }
                phong_data->setLoaiPhong(loaiPhong);
                phong_data->setSoGiuong(soGiuong);
                QMessageBox::information(this, "Thành công", "Đã sửa thông tin Phòng bệnh.");
            }
            hienThiPhong();
            hienThiThongKe();
        } catch (const std::runtime_error& e) {
            QMessageBox::critical(this, "Lỗi", e.what());
        }
    }
}

void MainWindow::phanCongDialog() {
    QString selectedText = listBenhNhanChuaPhanCong->currentItem() ? listBenhNhanChuaPhanCong->currentItem()->text() : "";
    if (selectedText.isEmpty()) {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn một Bệnh nhân chưa phân công.");
        return;
    }

    std::string maBN = selectedText.section(" - ", 0, 0).toStdString();
    auto bn = qlbv.getBenhNhan(maBN);
    if (!bn) return; // Should not happen

    QDialog dialog(this);
    dialog.setWindowTitle("Phân Công Điều Trị: " + QString::fromStdString(bn->getHoTen()));
    dialog.setMinimumWidth(400);

    QGridLayout* layout = new QGridLayout(&dialog);

    // 1. Chọn Bác sĩ
    QLabel* bsLabel = new QLabel("Chọn Bác sĩ:");
    QComboBox* bsCombo = new QComboBox;
    bsCombo->addItem(""); // Cho phép bỏ trống
    std::string benhLy = bn->getBenhLy();
    QSet<QString> availableBS;

    // Lọc BS theo chuyên khoa phù hợp với bệnh lý
    auto it = chuyenKhoaMapping.find(benhLy);
    std::vector<std::string> suitableCKs;
    if (it != chuyenKhoaMapping.end()) {
        suitableCKs = it->second;
    }

    for (const auto& pair : qlbv.getDsBacSi()) {
        const auto& bs = pair.second;
        QString bsText = QString::fromStdString(bs->getMaBS() + " - " + bs->getHoTen() + " (" + bs->getChuyenKhoa() + ")");
        if (std::find(suitableCKs.begin(), suitableCKs.end(), bs->getChuyenKhoa()) != suitableCKs.end()) {
            bsCombo->addItem(bsText);
        } else {
            // Cho phép chọn BS không phù hợp CK nhưng cảnh báo
            availableBS.insert(bsText);
        }
    }

    // Thêm các BS không phù hợp CK vào sau (nếu chưa có)
    for (const auto& pair : qlbv.getDsBacSi()) {
        const auto& bs = pair.second;
        QString bsText = QString::fromStdString(bs->getMaBS() + " - " + bs->getHoTen() + " (" + bs->getChuyenKhoa() + ")");
        if (!availableBS.contains(bsText)) {
            bsCombo->addItem(bsText);
        }
    }


    // 2. Chọn Phòng
    QLabel* phongLabel = new QLabel("Chọn Phòng Bệnh:");
    QComboBox* phongCombo = new QComboBox;
    phongCombo->addItem(""); // Cho phép bỏ trống
    for (const auto& pair : qlbv.getDsPhong()) {
        const auto& phong = pair.second;
        QString phongText = QString::fromStdString(phong->getMaPhong() + " - " + phong->getLoaiPhong() + " (" + std::to_string(phong->getSoBNDangNam()) + "/" + std::to_string(phong->getSoGiuong()) + ")");
        if (phong->getSoBNDangNam() < phong->getSoGiuong()) {
            phongCombo->addItem(phongText); // Ưu tiên phòng còn trống
        }
    }

    // Thêm phòng đầy (có thể chuyển BN từ phòng đầy sang phòng khác)
    for (const auto& pair : qlbv.getDsPhong()) {
        const auto& phong = pair.second;
        if (phong->getSoBNDangNam() == phong->getSoGiuong()) {
            QString phongText = QString::fromStdString(phong->getMaPhong() + " - " + phong->getLoaiPhong() + " (Đã đầy: " + std::to_string(phong->getSoBNDangNam()) + "/" + std::to_string(phong->getSoGiuong()) + ")");
            phongCombo->addItem(phongText);
        }
    }


    // Thiết lập giá trị mặc định nếu BN đã được phân công trước
    if (!bn->getMaBSPhuTrach().empty()) {
        QString currentBSText = QString::fromStdString(bn->getMaBSPhuTrach());
        int index = bsCombo->findText(currentBSText, Qt::MatchStartsWith);
        if (index != -1) bsCombo->setCurrentIndex(index);
    }
    if (!bn->getMaPhongDieuTri().empty()) {
        QString currentPhongText = QString::fromStdString(bn->getMaPhongDieuTri());
        int index = phongCombo->findText(currentPhongText, Qt::MatchStartsWith);
        if (index != -1) phongCombo->setCurrentIndex(index);
    }


    // Nút OK/Cancel
    QPushButton* okButton = new QPushButton("Phân Công");
    QPushButton* cancelButton = new QPushButton("Hủy");

    // Layout
    layout->addWidget(bsLabel, 0, 0);
    layout->addWidget(bsCombo, 0, 1);
    layout->addWidget(phongLabel, 1, 0);
    layout->addWidget(phongCombo, 1, 1);
    layout->addWidget(okButton, 2, 0);
    layout->addWidget(cancelButton, 2, 1);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        try {
            std::string maBS = bsCombo->currentText().section(" - ", 0, 0).toStdString();
            std::string maPhong = phongCombo->currentText().section(" - ", 0, 0).toStdString();

            if (maBS.empty()) throw std::runtime_error("Phải chọn Bác sĩ phụ trách.");
            if (maPhong.empty()) throw std::runtime_error("Phải chọn Phòng điều trị.");

            qlbv.phanCongDieuTri(maBN, maBS, maPhong);

            QMessageBox::information(this, "Thành công", "Đã phân công Bác sĩ và Phòng cho BN: " + QString::fromStdString(bn->getHoTen()));
            hienThiBenhNhan();
            hienThiBacSi();
            hienThiPhong();
            hienThiDieuTri();
            hienThiThongKe();
        } catch (const std::runtime_error& e) {
            QMessageBox::critical(this, "Lỗi", e.what());
        }
    }
}

void MainWindow::raVienDialog() {
    QString selectedText = listBenhNhanDaPhanCong->currentItem() ? listBenhNhanDaPhanCong->currentItem()->text() : "";
    if (selectedText.isEmpty()) {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn một Bệnh nhân đã phân công.");
        return;
    }

    std::string maBN = selectedText.section(" - ", 0, 0).toStdString();
    auto bn = qlbv.getBenhNhan(maBN);
    if (!bn) return; // Should not happen

    QDialog dialog(this);
    dialog.setWindowTitle("Xác nhận Ra Viện: " + QString::fromStdString(bn->getHoTen()));
    dialog.setMinimumWidth(400);

    QGridLayout* layout = new QGridLayout(&dialog);

    QLabel* infoLabel = new QLabel("Thông tin BN:\n" + QString::fromStdString(bn->toString()));
    QDateEdit* ngayRVDateEdit = new QDateEdit(QDate::currentDate());
    ngayRVDateEdit->setCalendarPopup(true);
    ngayRVDateEdit->setMinimumDate(bn->getNgayNhapVien());

    QPushButton* okButton = new QPushButton("Xác nhận Ra Viện");
    QPushButton* cancelButton = new QPushButton("Hủy");

    layout->addWidget(infoLabel, 0, 0, 1, 2);
    layout->addWidget(new QLabel("Ngày Ra Viện:"), 1, 0);
    layout->addWidget(ngayRVDateEdit, 1, 1);
    layout->addWidget(okButton, 2, 0);
    layout->addWidget(cancelButton, 2, 1);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        try {
            QDate ngayRaVien = ngayRVDateEdit->date();
            double chiPhi = qlbv.raVien(maBN, ngayRaVien);

            QString message = QString("BN **%1** đã ra viện thành công!\n\n")
                                  .arg(QString::fromStdString(bn->getHoTen()));
            message += QString("Tổng Chi Phí: **%1 VND**\n")
                           .arg(chiPhi, 0, 'f', 0); // Định dạng số không có thập phân

            if (bn->isHoNgheo()) {
                message += "(*Đã được giảm 50% chi phí do thuộc hộ nghèo)";
            }

            QMessageBox::information(this, "Ra Viện Thành Công", message);
            hienThiBenhNhan();
            hienThiBacSi();
            hienThiPhong();
            hienThiDieuTri();
            hienThiThongKe();
        } catch (const std::runtime_error& e) {
            QMessageBox::critical(this, "Lỗi", e.what());
        }
    }
}

// =======================================================
// HELPER & SLOTS CHỨC NĂNG
// =======================================================

bool MainWindow::checkFilterBenhNhan(std::shared_ptr<BenhNhan> bn) {
    QString filterText = filterBNInput->text().trimmed();
    QString filterCombo = filterBNCombo->currentText();
    QString bnText = QString::fromStdString(bn->getMaBN() + " " + bn->getHoTen() + " " + bn->getBenhLy()).toLower();

    // Lọc theo Text
    if (!filterText.isEmpty() && !bnText.contains(filterText.toLower())) {
        return false;
    }

    // Lọc theo ComboBox
    if (filterCombo == "BN đã phân công" && (bn->getMaBSPhuTrach().empty() || bn->getMaPhongDieuTri().empty())) {
        return false;
    }
    if (filterCombo == "BN chưa phân công" && (!bn->getMaBSPhuTrach().empty() && !bn->getMaPhongDieuTri().empty())) {
        return false;
    }
    if (filterCombo == "BN hộ nghèo" && !bn->isHoNgheo()) {
        return false;
    }

    return true;
}

bool MainWindow::checkFilterBacSi(std::shared_ptr<BacSi> bs) {
    QString searchText = searchBacSiLine->text().trimmed();
    QString filterCK = filterChuyenKhoaBacSiCombo->currentText();
    QString bsText = QString::fromStdString(bs->getMaBS() + " " + bs->getHoTen()).toLower();

    // Lọc theo Text
    if (!searchText.isEmpty() && !bsText.contains(searchText.toLower())) {
        return false;
    }

    // Lọc theo Chuyên Khoa
    if (filterCK != "Tất cả" && QString::fromStdString(bs->getChuyenKhoa()) != filterCK) {
        return false;
    }

    return true;
}

void MainWindow::onThemBenhNhan() { themSuaBenhNhanDialog(true); }
void MainWindow::onSuaBenhNhan() {
    int row = benhNhanTable->currentRow();
    if (row >= 0) {
        std::string maBN = benhNhanTable->item(row, 1)->text().toStdString();
        themSuaBenhNhanDialog(false, maBN);
    } else {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn một Bệnh nhân để sửa.");
    }
}
void MainWindow::onXoaBenhNhan() {
    int row = benhNhanTable->currentRow();
    if (row >= 0) {
        std::string maBN = benhNhanTable->item(row, 1)->text().toStdString();
        if (QMessageBox::question(this, "Xác nhận xóa", "Bạn có chắc chắn muốn xóa Bệnh nhân **" + QString::fromStdString(maBN) + "** không?") == QMessageBox::Yes) {
            try {
                qlbv.xoaBenhNhan(maBN);
                QMessageBox::information(this, "Thành công", "Đã xóa Bệnh nhân.");
                hienThiBenhNhan();
                hienThiDieuTri();
                hienThiThongKe();
            } catch (const std::runtime_error& e) {
                QMessageBox::critical(this, "Lỗi", e.what());
            }
        }
    } else {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn một Bệnh nhân để xóa.");
    }
}
void MainWindow::onXuatBenhNhan() {
    QString fileName = QFileDialog::getSaveFileName(this, "Xuất danh sách Bệnh nhân", "danhsach_benhnhan.txt", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QString content;
        content += "STT\tMã BN\tHọ Tên\tGiới Tính\tTuổi\tBệnh Lý\tHộ Nghèo\tBS PT\tPhòng DT\tNgày NV\n";
        int stt = 1;
        for (const auto& pair : qlbv.getDsBenhNhan()) {
            const auto& bn = pair.second;
            content += QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\t%9\t%10\n")
                           .arg(stt++)
                           .arg(QString::fromStdString(bn->getMaBN()))
                           .arg(QString::fromStdString(bn->getHoTen()))
                           .arg(QString::fromStdString(bn->getGioiTinh()))
                           .arg(bn->getTuoi())
                           .arg(QString::fromStdString(bn->getBenhLy()))
                           .arg(bn->isHoNgheo() ? "Có" : "Không")
                           .arg(QString::fromStdString(bn->getMaBSPhuTrach()))
                           .arg(QString::fromStdString(bn->getMaPhongDieuTri()))
                           .arg(bn->getNgayNhapVien().toString("dd/MM/yyyy"));
        }
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream out(&file);
            out.setEncoding(QStringConverter::Utf8);
            out << content;
            file.close();
            QMessageBox::information(this, "Thành công", "Đã xuất file thành công.");
        } else {
            QMessageBox::critical(this, "Lỗi", "Không thể ghi file.");
        }
    }
}
void MainWindow::onFilterBenhNhan() { hienThiBenhNhan(); }

void MainWindow::onThemBacSi() { themSuaBacSiDialog(true); }
void MainWindow::onSuaBacSi() {
    int row = bacSiTable->currentRow();
    if (row >= 0) {
        std::string maBS = bacSiTable->item(row, 1)->text().toStdString();
        themSuaBacSiDialog(false, maBS);
    } else {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn một Bác sĩ để sửa.");
    }
}
void MainWindow::onXoaBacSi() {
    int row = bacSiTable->currentRow();
    if (row >= 0) {
        std::string maBS = bacSiTable->item(row, 1)->text().toStdString();
        if (QMessageBox::question(this, "Xác nhận xóa", "Bạn có chắc chắn muốn xóa Bác sĩ **" + QString::fromStdString(maBS) + "** không? (Các BN sẽ bị gỡ phân công)") == QMessageBox::Yes) {
            try {
                qlbv.xoaBacSi(maBS);
                QMessageBox::information(this, "Thành công", "Đã xóa Bác sĩ.");
                hienThiBacSi();
                hienThiBenhNhan();
                hienThiDieuTri();
                hienThiThongKe();
            } catch (const std::runtime_error& e) {
                QMessageBox::critical(this, "Lỗi", e.what());
            }
        }
    } else {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn một Bác sĩ để xóa.");
    }
}
void MainWindow::onXuatBacSi() {
    QString fileName = QFileDialog::getSaveFileName(this, "Xuất danh sách Bác sĩ", "danhsach_bacsi.txt", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QString content;
        content += "STT\tMã BS\tHọ Tên\tGiới Tính\tTuổi\tChuyên Khoa\tSố BN PT\n";
        int stt = 1;
        for (const auto& pair : qlbv.getDsBacSi()) {
            const auto& bs = pair.second;
            int soBN = qlbv.getBenhNhanPhuTrach(bs->getMaBS()).size();
            content += QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\n")
                           .arg(stt++)
                           .arg(QString::fromStdString(bs->getMaBS()))
                           .arg(QString::fromStdString(bs->getHoTen()))
                           .arg(QString::fromStdString(bs->getGioiTinh()))
                           .arg(bs->getTuoi())
                           .arg(QString::fromStdString(bs->getChuyenKhoa()))
                           .arg(soBN);
        }
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream out(&file);
            out.setEncoding(QStringConverter::Utf8);
            out << content;
            file.close();
            QMessageBox::information(this, "Thành công", "Đã xuất file thành công.");
        } else {
            QMessageBox::critical(this, "Lỗi", "Không thể ghi file.");
        }
    }
}
void MainWindow::onFilterBacSi() { hienThiBacSi(); }


void MainWindow::onThemPhong() { themSuaPhongDialog(true); }
void MainWindow::onSuaPhong() {
    int row = phongBenhTable->currentRow();
    if (row >= 0) {
        std::string maPhong = phongBenhTable->item(row, 1)->text().toStdString();
        themSuaPhongDialog(false, maPhong);
    } else {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn một Phòng bệnh để sửa.");
    }
}
void MainWindow::onXoaPhong() {
    int row = phongBenhTable->currentRow();
    if (row >= 0) {
        std::string maPhong = phongBenhTable->item(row, 1)->text().toStdString();
        auto phong = qlbv.getPhong(maPhong);
        if (phong && phong->getSoBNDangNam() > 0) {
            QMessageBox::critical(this, "Lỗi", "Không thể xóa phòng đang có bệnh nhân.");
            return;
        }

        if (QMessageBox::question(this, "Xác nhận xóa", "Bạn có chắc chắn muốn xóa Phòng **" + QString::fromStdString(maPhong) + "** không?") == QMessageBox::Yes) {
            try {
                qlbv.xoaPhong(maPhong);
                QMessageBox::information(this, "Thành công", "Đã xóa Phòng bệnh.");
                hienThiPhong();
                hienThiBenhNhan();
                hienThiDieuTri();
                hienThiThongKe();
            } catch (const std::runtime_error& e) {
                QMessageBox::critical(this, "Lỗi", e.what());
            }
        }
    } else {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn một Phòng bệnh để xóa.");
    }
}
void MainWindow::onXuatPhong() {
    QString fileName = QFileDialog::getSaveFileName(this, "Xuất danh sách Phòng bệnh", "danhsach_phongbenh.txt", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QString content;
        content += "STT\tMã Phòng\tLoại Phòng\tSố Giường\tSố BN Đang Nằm\n";
        int stt = 1;
        for (const auto& pair : qlbv.getDsPhong()) {
            const auto& phong = pair.second;
            content += QString("%1\t%2\t%3\t%4\t%5\n")
                           .arg(stt++)
                           .arg(QString::fromStdString(phong->getMaPhong()))
                           .arg(QString::fromStdString(phong->getLoaiPhong()))
                           .arg(phong->getSoGiuong())
                           .arg(phong->getSoBNDangNam());
        }
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream out(&file);
            out.setEncoding(QStringConverter::Utf8);
            out << content;
            file.close();
            QMessageBox::information(this, "Thành công", "Đã xuất file thành công.");
        } else {
            QMessageBox::critical(this, "Lỗi", "Không thể ghi file.");
        }
    }
}

void MainWindow::onPhanCong() { phanCongDialog(); }
void MainWindow::onRaVien() { raVienDialog(); }

void MainWindow::onThongKe() { hienThiThongKe(); }
