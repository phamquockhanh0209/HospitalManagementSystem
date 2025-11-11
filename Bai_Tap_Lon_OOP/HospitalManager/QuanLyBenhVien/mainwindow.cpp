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

// ============= ĐỊNH NGHĨA CÁC HÀM CỦA LỚP BACSI =============

void BacSi::themBenhNhan(std::shared_ptr<BenhNhan> bn) {
    danhSachBN.push_back(std::weak_ptr<BenhNhan>(bn));
}

void BacSi::xoaBenhNhan(const std::string& maBN) {
    danhSachBN.erase(std::remove_if(danhSachBN.begin(), danhSachBN.end(),
                                    [&maBN](const std::weak_ptr<BenhNhan>& bn) {
                                        if (auto ptr = bn.lock()) return ptr->getMaBN() == maBN;
                                        return true;
                                    }), danhSachBN.end());
}


// ============= ĐỊNH NGHĨA CÁC HÀM CỦA LỚP MAINWINDOW =============

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Thêm dữ liệu mẫu ban đầu
    qlbv.themBenhNhan(std::make_shared<BenhNhan>("BN001", "Nguyễn Văn A", "Nam", 1, 1, 1990, "Hà Nội", std::vector<std::string>{"Tim mạch"}, false));
    qlbv.themBenhNhan(std::make_shared<BenhNhan>("BN002", "Trần Thị C", "Nữ", 15, 7, 2000, "Đà Nẵng", std::vector<std::string>{"Da liễu"}, true));
    qlbv.themBacSi(std::make_shared<BacSi>("BS001", "Trần Thị B", "Nữ", 5, 5, 1980, "TP HCM", "Tim mạch"));
    qlbv.themBacSi(std::make_shared<BacSi>("BS002", "Lê Văn C", "Nam", 10, 10, 1975, "Hà Nội", "Nội khoa"));
    qlbv.themPhong(std::make_shared<PhongThuong>(101, 4));
    qlbv.themPhong(std::make_shared<PhongVIP>(201, 2));

    setupUI();
    updateBenhNhanTable();
    updateBacSiTable();
    updatePhongTable();
    updatePhanCongTable();

}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    createBenhNhanTab();
    createBacSiTab();
    createPhongTab();
    createDieuTriTab();
    createThongKeTab();
}

// ==================== HÀM XUẤT CSV CHUNG ====================

void MainWindow::exportTableToCsv(QTableWidget* table, const QString& defaultFileName, const QString& title) {
    // Mở hộp thoại để chọn vị trí lưu file
    QString fileName = QFileDialog::getSaveFileName(this, title,
                                                    defaultFileName,
                                                    "CSV files (*.csv);;All Files (*)");

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Lỗi", "Không thể mở file để ghi: " + file.errorString());
        return;
    }

    QTextStream out(&file);
    // Thiết lập mã hóa UTF-8 (chuẩn Qt 6) để hiển thị Tiếng Việt trong Excel
    out.setEncoding(QStringConverter::Utf8);

    // 1. Ghi tiêu đề cột (Header)
    QStringList headerList;
    for (int col = 0; col < table->columnCount(); ++col) {
        if (table->horizontalHeaderItem(col)) {
            headerList << table->horizontalHeaderItem(col)->text();
        } else {
            headerList << QString("Cột %1").arg(col + 1);
        }
    }
    // Dùng dấu chấm phẩy (;) để đảm bảo tương thích với Excel Tiếng Việt
    out << headerList.join(";") << "\n";

    // 2. Ghi dữ liệu từng dòng
    for (int row = 0; row < table->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < table->columnCount(); ++col) {
            QTableWidgetItem *item = table->item(row, col);
            QString cellData = (item && !item->text().isEmpty()) ? item->text() : "";

            // Xóa dấu xuống dòng và dấu chấm phẩy trong dữ liệu
            cellData.replace('\n', ' ').replace(';', ',');

            rowData << cellData;
        }
        out << rowData.join(";") << "\n";
    }

    file.close();
    QMessageBox::information(this, "Thành công", "Đã xuất dữ liệu thành công ra file:\n" + fileName);
}


// ==================== CÁC HÀM CẬP NHẬT BẢNG ====================

void MainWindow::updateBenhNhanTable() {
    tableBN->setRowCount(0);
    int row = 0;

    for (const auto& pair : qlbv.getDsBN()) {
        const auto& bn = pair.second;
        if (!bn->getTrangThaiNhapVien()) continue;

        tableBN->insertRow(row);

        tableBN->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(bn->getMaBN())));
        tableBN->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(bn->getHoTen())));
        tableBN->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(bn->getGioiTinh())));

        std::string benhLyStr = bn->getDanhSachBenhLy().empty() ? "N/A" : bn->getDanhSachBenhLy()[0];
        tableBN->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(benhLyStr)));

        row++;
    }
}

void MainWindow::updateBacSiTable() {
    tableBS->setRowCount(0);
    int row = 0;

    for (const auto& pair : qlbv.getDsBS()) {
        const auto& bs = pair.second;

        tableBS->insertRow(row);

        tableBS->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(bs->getMaBS())));
        tableBS->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(bs->getHoTen())));
        tableBS->setItem(row, 2, new QTableWidgetItem(QString::number(bs->tinhTuoi())));
        tableBS->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(bs->getChuyenKhoa())));

        row++;
    }
}

void MainWindow::updatePhongTable() {
    tablePhong->setRowCount(0);
    int row = 0;

    for (const auto& pair : qlbv.getDsPhong()) {
        const auto& phong = pair.second;

        tablePhong->insertRow(row);

        tablePhong->setItem(row, 0, new QTableWidgetItem(QString::number(phong->getSoPhong())));
        tablePhong->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(phong->getLoaiPhong())));
        tablePhong->setItem(row, 2, new QTableWidgetItem(QString::number(phong->getSoGiuong())));

        QString trangThai = QString("%1/%2").arg(phong->getSoBNDangNam()).arg(phong->getSoGiuong());
        tablePhong->setItem(row, 3, new QTableWidgetItem(trangThai));

        row++;
    }
}

void MainWindow::updatePhanCongTable() {
    tablePhanCong->setRowCount(0);
    int row = 0;

    for (const auto& pair : qlbv.getDsBN()) {
        const auto& bn = pair.second;
        // Chỉ hiển thị bệnh nhân đang nằm viện và đã được phân công
        if (bn->getTrangThaiNhapVien() && bn->getBacSiDieuTri() && bn->getSoPhongObj()) {

            tablePhanCong->insertRow(row);

            // Cột 1: Mã BN
            tablePhanCong->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(bn->getMaBN())));

            // Cột 2: Tên BN
            tablePhanCong->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(bn->getHoTen())));

            // Cột 3: Bác sĩ điều trị
            auto bs = bn->getBacSiDieuTri();
            QString bsInfo = QString::fromStdString(bs->getMaBS() + " - " + bs->getHoTen() + " (" + bs->getChuyenKhoa() + ")");
            tablePhanCong->setItem(row, 2, new QTableWidgetItem(bsInfo));

            // Cột 4: Phòng bệnh
            auto phong = bn->getSoPhongObj();
            QString phongInfo = QString("%1 (%2)").arg(phong->getSoPhong()).arg(QString::fromStdString(phong->getLoaiPhong()));
            tablePhanCong->setItem(row, 3, new QTableWidgetItem(phongInfo));

            row++;
        }
    }
}

// ==================== CÁC HÀM TẠO TAB ====================

void MainWindow::createBenhNhanTab()
{
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QLabel *lbl = new QLabel("Danh sách Bệnh nhân đang nằm viện");
    tableBN = new QTableWidget;
    tableBN->setColumnCount(4);
    tableBN->setHorizontalHeaderLabels({"Mã BN", "Họ tên", "Giới tính", "Bệnh lý chính"});
    tableBN->horizontalHeader()->setStretchLastSection(true);
    tableBN->setSelectionBehavior(QAbstractItemView::SelectRows);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnThemBN = new QPushButton("Thêm bệnh nhân");
    btnSuaBN = new QPushButton("Sửa bệnh nhân");
    btnXoaBN = new QPushButton("Xóa bệnh nhân");
    btnXuatBN = new QPushButton("Xuất Excel (CSV)");

    btnLayout->addWidget(btnThemBN);
    btnLayout->addWidget(btnSuaBN);
    btnLayout->addWidget(btnXoaBN);
    btnLayout->addWidget(btnXuatBN);

    layout->addWidget(lbl);
    layout->addWidget(tableBN);
    layout->addLayout(btnLayout);

    tabWidget->addTab(tab, "Bệnh nhân");

    connect(btnThemBN, &QPushButton::clicked, this, &MainWindow::onThemBenhNhan);
    connect(btnSuaBN, &QPushButton::clicked, this, &MainWindow::onSuaBenhNhan);
    connect(btnXoaBN, &QPushButton::clicked, this, &MainWindow::onXoaBenhNhan);
    connect(btnXuatBN, &QPushButton::clicked, this, &MainWindow::onXuatBenhNhan);
}

void MainWindow::createBacSiTab()
{
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QLabel *lbl = new QLabel("Danh sách Bác sĩ");
    tableBS = new QTableWidget;
    tableBS->setColumnCount(4);
    tableBS->setHorizontalHeaderLabels({"Mã BS", "Họ tên", "Tuổi", "Chuyên khoa"});
    tableBS->horizontalHeader()->setStretchLastSection(true);
    tableBS->setSelectionBehavior(QAbstractItemView::SelectRows);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnThemBS = new QPushButton("Thêm Bác sĩ");
    btnSuaBS = new QPushButton("Sửa Bác sĩ");
    btnXoaBS = new QPushButton("Xóa Bác sĩ");
    btnXuatBS = new QPushButton("Xuất Excel (CSV)");

    btnLayout->addWidget(btnThemBS);
    btnLayout->addWidget(btnSuaBS);
    btnLayout->addWidget(btnXoaBS);
    btnLayout->addWidget(btnXuatBS);

    layout->addWidget(lbl);
    layout->addWidget(tableBS);
    layout->addLayout(btnLayout);

    tabWidget->addTab(tab, "Bác sĩ");

    connect(btnThemBS, &QPushButton::clicked, this, &MainWindow::onThemBacSi);
    connect(btnSuaBS, &QPushButton::clicked, this, &MainWindow::onSuaBacSi);
    connect(btnXoaBS, &QPushButton::clicked, this, &MainWindow::onXoaBacSi);
    connect(btnXuatBS, &QPushButton::clicked, this, &MainWindow::onXuatBacSi);
}

void MainWindow::createPhongTab()
{
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QLabel *lbl = new QLabel("Danh sách Phòng bệnh");
    tablePhong = new QTableWidget;
    tablePhong->setColumnCount(4);
    tablePhong->setHorizontalHeaderLabels({"Số phòng", "Loại phòng", "Số giường", "Tình trạng"});
    tablePhong->horizontalHeader()->setStretchLastSection(true);
    tablePhong->setSelectionBehavior(QAbstractItemView::SelectRows);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnThemPhong = new QPushButton("Thêm Phòng");
    btnSuaPhong = new QPushButton("Sửa Phòng (Giường)");
    btnXuatPhong = new QPushButton("Xuất Excel (CSV)");

    btnLayout->addWidget(btnThemPhong);
    btnLayout->addWidget(btnSuaPhong);
    btnLayout->addWidget(btnXuatPhong);

    layout->addWidget(lbl);
    layout->addWidget(tablePhong);
    layout->addLayout(btnLayout);

    tabWidget->addTab(tab, "Phòng bệnh");

    connect(btnThemPhong, &QPushButton::clicked, this, &MainWindow::onThemPhong);
    connect(btnSuaPhong, &QPushButton::clicked, this, &MainWindow::onSuaPhong);
    connect(btnXuatPhong, &QPushButton::clicked, this, &MainWindow::onXuatPhong);
}

void MainWindow::createDieuTriTab()
{
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QLabel *lblTable = new QLabel("Danh sách Phân công điều trị (BN đang nằm viện)");
    tablePhanCong = new QTableWidget;
    tablePhanCong->setColumnCount(4);
    tablePhanCong->setHorizontalHeaderLabels({"Mã BN", "Tên BN", "Bác sĩ điều trị", "Phòng"});
    tablePhanCong->horizontalHeader()->setStretchLastSection(true);
    tablePhanCong->setSelectionBehavior(QAbstractItemView::SelectRows);

    QLabel *lblPhanCong = new QLabel("Thao tác Phân công/Xuất viện:");

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnPhanCong = new QPushButton("Thực hiện Phân công");
    btnRaVien = new QPushButton("Bệnh nhân Xuất viện/Ra viện");
    btnLayout->addWidget(btnPhanCong);
    btnLayout->addWidget(btnRaVien);

    layout->addWidget(lblTable);
    layout->addWidget(tablePhanCong);
    layout->addWidget(lblPhanCong);
    layout->addLayout(btnLayout);

    tabWidget->addTab(tab, "Điều trị/Phân công");

    connect(btnPhanCong, &QPushButton::clicked, this, &MainWindow::onPhanCong);
    connect(btnRaVien, &QPushButton::clicked, this, &MainWindow::onRaVien);
}

void MainWindow::createThongKeTab()
{
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    btnThongKe = new QPushButton("Lập báo cáo Thống kê");
    thongKeOutput = new QTextEdit;
    thongKeOutput->setReadOnly(true);

    layout->addWidget(btnThongKe);
    layout->addWidget(thongKeOutput);

    tabWidget->addTab(tab, "Thống kê/Báo cáo");

    connect(btnThongKe, &QPushButton::clicked, this, &MainWindow::onThongKe);
}

// ==================== HỘP THOẠI NHẬP LIỆU ====================

void MainWindow::showBenhNhanInputDialog(std::shared_ptr<BenhNhan> bn) {
    QDialog dialog(this);
    dialog.setWindowTitle(bn ? "Sửa Bệnh Nhân" : "Thêm Bệnh Nhân Mới");
    QGridLayout layout(&dialog);

    QLineEdit *maBNInput = new QLineEdit;
    if (bn) {
        maBNInput->setText(QString::fromStdString(bn->getMaBN()));
        maBNInput->setReadOnly(true);
    }
    layout.addWidget(new QLabel("Mã BN (*):"), 0, 0);
    layout.addWidget(maBNInput, 0, 1);

    QLineEdit *tenBNInput = new QLineEdit;
    if (bn) tenBNInput->setText(QString::fromStdString(bn->getHoTen()));
    layout.addWidget(new QLabel("Họ Tên (*):"), 1, 0);
    layout.addWidget(tenBNInput, 1, 1);

    QComboBox *gioiTinhCombo = new QComboBox;
    gioiTinhCombo->addItem("Nam");
    gioiTinhCombo->addItem("Nữ");
    if (bn) gioiTinhCombo->setCurrentText(QString::fromStdString(bn->getGioiTinh()));
    layout.addWidget(new QLabel("Giới tính:"), 2, 0);
    layout.addWidget(gioiTinhCombo, 2, 1);

    QDateEdit *ngaySinhEdit = new QDateEdit(QDate::currentDate().addYears(-20));
    if (bn) ngaySinhEdit->setDate(QDate(bn->getNamSinh(), bn->getThangSinh(), bn->getNgaySinh()));
    ngaySinhEdit->setCalendarPopup(true);
    ngaySinhEdit->setDisplayFormat("dd/MM/yyyy");
    layout.addWidget(new QLabel("Ngày sinh:"), 3, 0);
    layout.addWidget(ngaySinhEdit, 3, 1);

    QLineEdit *benhLyInput = new QLineEdit;
    if (bn && !bn->getDanhSachBenhLy().empty()) benhLyInput->setText(QString::fromStdString(bn->getDanhSachBenhLy()[0]));
    benhLyInput->setPlaceholderText("VD: Tim mạch");
    layout.addWidget(new QLabel("Bệnh lý chính (*):"), 4, 0);
    layout.addWidget(benhLyInput, 4, 1);

    QCheckBox *hoNgheoCheck = new QCheckBox("Có sổ hộ nghèo");
    if (bn) hoNgheoCheck->setChecked(bn->getCoSoHoNgheo());
    layout.addWidget(hoNgheoCheck, 5, 0, 1, 2);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *okButton = new QPushButton(bn ? "Lưu thay đổi" : "Thêm");
    QPushButton *cancelButton = new QPushButton("Hủy");
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout.addLayout(buttonLayout, 6, 0, 1, 2);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        std::string maBN = maBNInput->text().toStdString();
        std::string hoTen = tenBNInput->text().toStdString();
        std::string gioiTinh = gioiTinhCombo->currentText().toStdString();
        QDate ngaySinh = ngaySinhEdit->date();
        std::string benhLy = benhLyInput->text().toStdString();
        bool coSoHoNgheo = hoNgheoCheck->isChecked();

        if (maBN.empty() || hoTen.empty() || benhLy.empty()) {
            QMessageBox::warning(&dialog, "Lỗi", "Vui lòng nhập đầy đủ Mã BN, Họ Tên và Bệnh lý (các trường có dấu *).");
            return;
        }

        if (!bn && qlbv.timBenhNhan(maBN)) {
            QMessageBox::warning(&dialog, "Lỗi", "Mã BN đã tồn tại, vui lòng nhập mã khác.");
            return;
        }

        try {
            if (bn) {
                // Sửa thông tin
                bn->setHoTen(hoTen);
                // Các thông tin khác cần setter hoặc phải tạo lại đối tượng (tạm thời không thay đổi ngày sinh/giới tính)
            } else {
                // Thêm mới
                std::shared_ptr<BenhNhan> newBn = std::make_shared<BenhNhan>(
                    maBN, hoTen, gioiTinh,
                    ngaySinh.day(), ngaySinh.month(), ngaySinh.year(),
                    "Chưa cập nhật",
                    std::vector<std::string>{benhLy},
                    coSoHoNgheo
                    );
                qlbv.themBenhNhan(newBn);
            }
            updateBenhNhanTable();
            QMessageBox::information(this, "Thành công", QString(bn ? "Đã sửa thông tin BN: " : "Đã thêm bệnh nhân: ") + QString::fromStdString(hoTen));
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi Hệ thống", QString("Thao tác thất bại: %1").arg(e.what()));
        }
    }
}

void MainWindow::showBacSiInputDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Thêm Bác Sĩ Mới");
    QGridLayout layout(&dialog);

    QLineEdit *maBSInput = new QLineEdit;
    layout.addWidget(new QLabel("Mã BS (*):"), 0, 0);
    layout.addWidget(maBSInput, 0, 1);

    QLineEdit *tenBSInput = new QLineEdit;
    layout.addWidget(new QLabel("Họ Tên (*):"), 1, 0);
    layout.addWidget(tenBSInput, 1, 1);

    QComboBox *gioiTinhCombo = new QComboBox;
    gioiTinhCombo->addItem("Nam");
    gioiTinhCombo->addItem("Nữ");
    layout.addWidget(new QLabel("Giới tính:"), 2, 0);
    layout.addWidget(gioiTinhCombo, 2, 1);

    QDateEdit *ngaySinhEdit = new QDateEdit(QDate::currentDate().addYears(-30));
    ngaySinhEdit->setCalendarPopup(true);
    ngaySinhEdit->setDisplayFormat("dd/MM/yyyy");
    layout.addWidget(new QLabel("Ngày sinh:"), 3, 0);
    layout.addWidget(ngaySinhEdit, 3, 1);

    QComboBox *chuyenKhoaCombo = new QComboBox;
    for (const auto& pair : chuyenKhoaToBenhLy) {
        chuyenKhoaCombo->addItem(QString::fromStdString(pair.first));
    }
    layout.addWidget(new QLabel("Chuyên khoa (*):"), 4, 0);
    layout.addWidget(chuyenKhoaCombo, 4, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *okButton = new QPushButton("Thêm");
    QPushButton *cancelButton = new QPushButton("Hủy");
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout.addLayout(buttonLayout, 5, 0, 1, 2);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        std::string maBS = maBSInput->text().toStdString();
        std::string hoTen = tenBSInput->text().toStdString();
        std::string gioiTinh = gioiTinhCombo->currentText().toStdString();
        QDate ngaySinh = ngaySinhEdit->date();
        std::string chuyenKhoa = chuyenKhoaCombo->currentText().toStdString();

        if (maBS.empty() || hoTen.empty()) {
            QMessageBox::warning(&dialog, "Lỗi", "Vui lòng nhập đầy đủ Mã BS và Họ Tên.");
            return;
        }
        if (qlbv.timBacSi(maBS)) {
            QMessageBox::warning(&dialog, "Lỗi", "Mã BS đã tồn tại.");
            return;
        }

        try {
            std::shared_ptr<BacSi> newBs = std::make_shared<BacSi>(
                maBS, hoTen, gioiTinh,
                ngaySinh.day(), ngaySinh.month(), ngaySinh.year(),
                "Chưa cập nhật",
                chuyenKhoa
                );
            qlbv.themBacSi(newBs);
            updateBacSiTable();
            QMessageBox::information(this, "Thành công", "Đã thêm bác sĩ: " + QString::fromStdString(hoTen));
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi Hệ thống", QString("Thao tác thất bại: %1").arg(e.what()));
        }
    }
}

void MainWindow::showPhongInputDialog(std::shared_ptr<PhongBenh> phong) {
    QDialog dialog(this);
    dialog.setWindowTitle(phong ? "Sửa Phòng Bệnh" : "Thêm Phòng Bệnh Mới");
    QGridLayout layout(&dialog);

    QSpinBox *soPhongInput = new QSpinBox;
    soPhongInput->setRange(100, 999);
    if (phong) {
        soPhongInput->setValue(phong->getSoPhong());
        soPhongInput->setReadOnly(true);
    }
    layout.addWidget(new QLabel("Số phòng (*):"), 0, 0);
    layout.addWidget(soPhongInput, 0, 1);

    QComboBox *loaiPhongCombo = new QComboBox;
    loaiPhongCombo->addItem("Thuong");
    loaiPhongCombo->addItem("VIP");
    if (phong) {
        loaiPhongCombo->setCurrentText(QString::fromStdString(phong->getLoaiPhong()));
        loaiPhongCombo->setDisabled(true);
    }
    layout.addWidget(new QLabel("Loại phòng (*):"), 1, 0);
    layout.addWidget(loaiPhongCombo, 1, 1);

    QSpinBox *soGiuongInput = new QSpinBox;
    soGiuongInput->setRange(1, 10);
    if (phong) soGiuongInput->setValue(phong->getSoGiuong());
    layout.addWidget(new QLabel("Số giường (*):"), 2, 0);
    layout.addWidget(soGiuongInput, 2, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *okButton = new QPushButton(phong ? "Lưu thay đổi" : "Thêm");
    QPushButton *cancelButton = new QPushButton("Hủy");
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout.addLayout(buttonLayout, 3, 0, 1, 2);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        int soPhong = soPhongInput->value();
        std::string loaiPhong = loaiPhongCombo->currentText().toStdString();
        int soGiuong = soGiuongInput->value();

        if (!phong && qlbv.timPhong(soPhong)) {
            QMessageBox::warning(&dialog, "Lỗi", "Số phòng đã tồn tại.");
            return;
        }
        if (phong && soGiuong < phong->getSoBNDangNam()) {
            QMessageBox::critical(&dialog, "Lỗi", "Số giường mới không thể ít hơn số bệnh nhân đang nằm.");
            return;
        }

        try {
            if (phong) {
                phong->setSoGiuong(soGiuong);
                QMessageBox::information(this, "Thành công", QString("Đã sửa phòng %1: Số giường mới là %2.").arg(soPhong).arg(soGiuong));
            } else {
                std::shared_ptr<PhongBenh> newPhong;
                if (loaiPhong == "Thuong") newPhong = std::make_shared<PhongThuong>(soPhong, soGiuong);
                else newPhong = std::make_shared<PhongVIP>(soPhong, soGiuong);

                qlbv.themPhong(newPhong);
                QMessageBox::information(this, "Thành công", QString("Đã thêm phòng: %1 (%2)").arg(soPhong).arg(QString::fromStdString(loaiPhong)));
            }
            updatePhongTable();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi Hệ thống", QString("Thao tác thất bại: %1").arg(e.what()));
        }
    }
}

void MainWindow::showPhanCongDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Phân Công Điều Trị");
    QGridLayout layout(&dialog);

    QComboBox *bnCombo = new QComboBox;
    for (const auto& pair : qlbv.getDsBN()) {
        if(pair.second->getTrangThaiNhapVien()) {
            bnCombo->addItem(QString::fromStdString(pair.second->getMaBN() + " - " + pair.second->getHoTen()));
        }
    }
    layout.addWidget(new QLabel("Chọn Bệnh nhân:"), 0, 0);
    layout.addWidget(bnCombo, 0, 1);

    QComboBox *bsCombo = new QComboBox;
    for (const auto& pair : qlbv.getDsBS()) {
        bsCombo->addItem(QString::fromStdString(pair.second->getMaBS() + " - " + pair.second->getHoTen() + " (" + pair.second->getChuyenKhoa() + ")"));
    }
    layout.addWidget(new QLabel("Chọn Bác sĩ:"), 1, 0);
    layout.addWidget(bsCombo, 1, 1);

    QComboBox *phongCombo = new QComboBox;
    for (const auto& pair : qlbv.getDsPhong()) {
        const auto& p = pair.second;
        if(p->isTrong()) {
            phongCombo->addItem(QString::number(p->getSoPhong()) + " (" + QString::fromStdString(p->getLoaiPhong()) + " " + QString::number(p->getSoBNDangNam()) + "/" + QString::number(p->getSoGiuong()) + ")");
        }
    }
    layout.addWidget(new QLabel("Chọn Phòng:"), 2, 0);
    layout.addWidget(phongCombo, 2, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *okButton = new QPushButton("Phân công");
    QPushButton *cancelButton = new QPushButton("Hủy");
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout.addLayout(buttonLayout, 3, 0, 1, 2);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        if (bnCombo->currentIndex() == -1 || bsCombo->currentIndex() == -1 || phongCombo->currentIndex() == -1) {
            QMessageBox::warning(this, "Lỗi", "Vui lòng chọn đủ Bệnh nhân, Bác sĩ và Phòng.");
            return;
        }

        QString maBNStr = bnCombo->currentText().split(" - ").at(0);
        QString maBSStr = bsCombo->currentText().split(" - ").at(0);
        int soPhong = phongCombo->currentText().split(" ").at(0).toInt();

        auto bn = qlbv.timBenhNhan(maBNStr.toStdString());
        auto bs = qlbv.timBacSi(maBSStr.toStdString());
        auto phong = qlbv.timPhong(soPhong);

        if (!bn || !bs || !phong) return;

        try {
            qlbv.phanCong(bn, bs, phong);
            QMessageBox::information(this, "Thành công", QString("Đã phân công BN %1 cho BS %2 và Phòng %3.").arg(maBNStr).arg(maBSStr).arg(soPhong));
            updateBenhNhanTable();
            updatePhongTable();
            updatePhanCongTable(); // CẬP NHẬT BẢNG PHÂN CÔNG
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", QString("Phân công thất bại: %1").arg(e.what()));
        }
    }
}

// ==================== CÁC SLOTS XUẤT FILE ====================

void MainWindow::onXuatBenhNhan() {
    exportTableToCsv(tableBN, "DanhSachBenhNhan_DangDieuTri.csv", "Xuất danh sách Bệnh nhân");
}

void MainWindow::onXuatBacSi() {
    exportTableToCsv(tableBS, "DanhSachBacSi.csv", "Xuất danh sách Bác sĩ");
}

void MainWindow::onXuatPhong() {
    exportTableToCsv(tablePhong, "DanhSachPhongBenh.csv", "Xuất danh sách Phòng bệnh");
}

// ==================== CÁC SLOTS XỬ LÝ CHỨC NĂNG ====================

// --- Bệnh nhân ---
void MainWindow::onThemBenhNhan() {
    showBenhNhanInputDialog();
}

void MainWindow::onSuaBenhNhan() {
    QList<QTableWidgetItem*> selected = tableBN->selectedItems();
    if (selected.isEmpty() || tableBN->currentRow() < 0) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một Bệnh nhân để sửa.");
        return;
    }
    QString maBNStr = tableBN->item(tableBN->currentRow(), 0)->text();
    auto bn = qlbv.timBenhNhan(maBNStr.toStdString());
    if (bn) showBenhNhanInputDialog(bn);
}

void MainWindow::onXoaBenhNhan() {
    QList<QTableWidgetItem*> selected = tableBN->selectedItems();
    if (selected.isEmpty() || tableBN->currentRow() < 0) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một Bệnh nhân để xóa.");
        return;
    }
    QString maBNStr = tableBN->item(tableBN->currentRow(), 0)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận", "Bạn có chắc chắn muốn xóa Bệnh nhân có mã " + maBNStr + " không? (Hành động này không thể hoàn tác)",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // Dùng hàm raVien trước nếu BN đang nằm viện để giải phóng BS/Phòng
        auto bn = qlbv.timBenhNhan(maBNStr.toStdString());
        if(bn && bn->getTrangThaiNhapVien()) {
            bn->raVien();
        }

        qlbv.xoaBenhNhan(maBNStr.toStdString());
        updateBenhNhanTable();
        updatePhongTable(); // Cập nhật lại số BN trong phòng
        updatePhanCongTable(); // Cập nhật lại bảng phân công
        QMessageBox::information(this, "Thành công", "Đã xóa Bệnh nhân: " + maBNStr);
    }
}

// --- Bác sĩ ---
void MainWindow::onThemBacSi() {
    showBacSiInputDialog();
}

void MainWindow::onSuaBacSi() {
    QMessageBox::information(this, "Thông báo", "Chức năng Sửa bác sĩ (chuyên khoa) chưa được triển khai đầy đủ.");
}

void MainWindow::onXoaBacSi() {
    QList<QTableWidgetItem*> selected = tableBS->selectedItems();
    if (selected.isEmpty() || tableBS->currentRow() < 0) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một Bác sĩ để xóa.");
        return;
    }
    QString maBSStr = tableBS->item(tableBS->currentRow(), 0)->text();
    auto bs = qlbv.timBacSi(maBSStr.toStdString());

    if (bs) {
        // Kiểm tra xem BS này có đang điều trị BN nào không
        long soBN = 0;
        for(const auto& weak_bn : bs->getDanhSachBN()) {
            if (weak_bn.lock() && weak_bn.lock()->getTrangThaiNhapVien()) soBN++;
        }
        if (soBN > 0) {
            QMessageBox::critical(this, "Lỗi", "Không thể xóa bác sĩ này vì vẫn còn " + QString::number(soBN) + " bệnh nhân đang điều trị.");
            return;
        }
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận", "Bạn có chắc chắn muốn xóa Bác sĩ có mã " + maBSStr + " không?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        qlbv.xoaBacSi(maBSStr.toStdString());
        updateBacSiTable();
        QMessageBox::information(this, "Thành công", "Đã xóa Bác sĩ: " + maBSStr);
    }
}

// --- Phòng bệnh ---
void MainWindow::onThemPhong() {
    showPhongInputDialog();
}

void MainWindow::onSuaPhong() {
    QList<QTableWidgetItem*> selected = tablePhong->selectedItems();
    if (selected.isEmpty() || tablePhong->currentRow() < 0) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một Phòng để sửa (số giường).");
        return;
    }
    int soPhong = tablePhong->item(tablePhong->currentRow(), 0)->text().toInt();
    auto phong = qlbv.timPhong(soPhong);
    if (phong) showPhongInputDialog(phong);
}

// --- Điều trị/Phân công ---
void MainWindow::onPhanCong() {
    showPhanCongDialog();
}

void MainWindow::onRaVien() {
    QString maBNStr = QInputDialog::getText(this, "Xuất viện", "Nhập Mã Bệnh nhân xuất viện:");
    if (maBNStr.isEmpty()) return;

    auto bn = qlbv.timBenhNhan(maBNStr.toStdString());
    if (!bn || !bn->getTrangThaiNhapVien()) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy Bệnh nhân đang nằm viện với mã: " + maBNStr);
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận Xuất viện", "Bệnh nhân " + QString::fromStdString(bn->getHoTen()) + " sẽ được xuất viện. Tiếp tục?",
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        bn->raVien();
        updateBenhNhanTable();
        updatePhongTable();
        updatePhanCongTable(); // CẬP NHẬT BẢNG PHÂN CÔNG
        QMessageBox::information(this, "Thành công", "Bệnh nhân " + maBNStr + " đã xuất viện.");
    }
}

// --- Thống kê ---
void MainWindow::onThongKe() {
    std::stringstream ss;
    // 1. Số lượng Bệnh nhân
    long countBN = std::count_if(qlbv.getDsBN().begin(), qlbv.getDsBN().end(),
                                 [](const auto& pair){ return pair.second->getTrangThaiNhapVien(); });
    ss << "1. Tổng số Bệnh nhân đang nằm viện: " << countBN << " người.\n";

    // 2. Tình trạng phòng
    long phongTrong = std::count_if(qlbv.getDsPhong().begin(), qlbv.getDsPhong().end(),
                                    [](const auto& pair){ return pair.second->isTrong(); });
    ss << "2. Tổng số Phòng bệnh: " << qlbv.getDsPhong().size() << " phòng.\n";
    ss << "   - Số phòng còn chỗ: " << phongTrong << " phòng.\n\n";

    // 3. Tình trạng Bác sĩ
    ss << "3. Tình trạng công việc của Bác sĩ:\n";
    for (const auto& pair : qlbv.getDsBS()) {
        const auto& bs = pair.second;
        long soBN = 0;
        for (const auto& weak_bn : bs->getDanhSachBN()) {
            if (auto shared_bn = weak_bn.lock()) {
                if (shared_bn->getTrangThaiNhapVien()) { // Chỉ đếm BN đang nằm viện
                    soBN++;
                }
            }
        }
        ss << "   - BS " << bs->getHoTen() << " (" << bs->getChuyenKhoa() << "): đang điều trị " << soBN << " bệnh nhân.\n";
    }

    thongKeOutput->setText(QString::fromStdString(ss.str()));
    QMessageBox::information(this, "Thành công", "Đã lập báo cáo thống kê.");
}
