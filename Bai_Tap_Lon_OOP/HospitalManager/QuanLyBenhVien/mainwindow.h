#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QGroupBox>
#include <QTabWidget>
#include <QDateEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QDate>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <QSet>
#include <sstream>

// ============= CẤU HÌNH GIÁ DỊCH VỤ =============
struct CauHinhGia {
    double giaPhongThuong = 500000.0;
    double giaPhongVIP = 2000000.0;
    double giamGiaHoNgheo = 0.5;

    static CauHinhGia& getInstance() {
        static CauHinhGia instance;
        return instance;
    }
};

// ============= ÁNH XẠ BỆNH LÝ - CHUYÊN KHOA =============
const std::unordered_map<std::string, std::vector<std::string>> chuyenKhoaMapping = {
    {"Nội tim mạch", {"Tim Mạch"}},
    {"Ngoại tim mạch", {"Tim Mạch"}},
     {"Hở van tim", {"Tim Mạch"}},

    {"Nội Hô Hấp", {"Hô Hấp"}},
    {"Nhi", {"Hô Hấp"}},
    {"Nội Tiêu Hóa", {"Tiêu Hóa"}},
    {"Ngoại Tiêu Hóa", {"Tiêu Hóa"}},
    {"Nội Thần Kinh", {"Thần Kinh"}},
    {"Ngoại Thần Kinh", {"Thần Kinh"}},
    {"Chấn Thương Chỉnh Hình", {"Cơ Xương Khớp"}},
    {"Nội Cơ Xương Khớp", {"Cơ Xương Khớp"}},
    {"Truyền Nhiễm", {"Truyền Nhiễm"}},
    {"Nội Tổng Quát", {"Truyền Nhiễm", "Khác"}},
    {"Ngoại Tổng Quát", {"Khác"}}
};

// =======================================================
// CÁC LỚP ĐỐI TƯỢNG CƠ BẢN
// =======================================================

class Nguoi {
private:
    std::string ma;
    std::string hoTen;
    std::string gioiTinh;
    QDate ngaySinh;
    std::string soDienThoai;
    std::string diaChi;

public:
    Nguoi(const std::string& ma, const std::string& hoTen, const std::string& gioiTinh,
          const QDate& ngaySinh, const std::string& sdt, const std::string& diaChi)
        : ma(ma), hoTen(hoTen), gioiTinh(gioiTinh), ngaySinh(ngaySinh),
        soDienThoai(sdt), diaChi(diaChi) {}
    virtual ~Nguoi() {}

    std::string getMa() const { return ma; }
    std::string getHoTen() const { return hoTen; }
    std::string getGioiTinh() const { return gioiTinh; }
    QDate getNgaySinh() const { return ngaySinh; }
    std::string getSoDienThoai() const { return soDienThoai; }
    std::string getDiaChi() const { return diaChi; }

    int getTuoi() const {
        QDate today = QDate::currentDate();
        int age = today.year() - ngaySinh.year();
        if (today.month() < ngaySinh.month() ||
            (today.month() == ngaySinh.month() && today.day() < ngaySinh.day())) {
            age--;
        }
        return age;
    }

    void setHoTen(const std::string& ten) { hoTen = ten; }
    void setGioiTinh(const std::string& gt) { gioiTinh = gt; }
    void setNgaySinh(const QDate& ns) { ngaySinh = ns; }
    void setSoDienThoai(const std::string& sdt) { soDienThoai = sdt; }
    void setDiaChi(const std::string& dc) { diaChi = dc; }

    virtual std::string toString() const = 0;
};

class BenhNhan : public Nguoi {
private:
    std::string benhLy;
    bool hoNgheo;
    std::string maBSPhuTrach;
    std::string maPhongDieuTri;
    QDate ngayNhapVien;
    QDate ngayRaVien;
    std::string phuongThucThanhToan;
    double tongChiPhi;
    bool daXuatVien;

public:
    BenhNhan(const std::string& maBN, const std::string& hoTen, const std::string& gioiTinh,
             const QDate& ngaySinh, const std::string& sdt, const std::string& diaChi,
             const std::string& benhLy, bool hoNgheo, const QDate& ngayNV)
        : Nguoi(maBN, hoTen, gioiTinh, ngaySinh, sdt, diaChi),
        benhLy(benhLy), hoNgheo(hoNgheo), ngayNhapVien(ngayNV),
        phuongThucThanhToan("Tiền Mặt"), tongChiPhi(0), daXuatVien(false) {
        maBSPhuTrach = "";
        maPhongDieuTri = "";
    }

    std::string getMaBN() const { return getMa(); }
    std::string getBenhLy() const { return benhLy; }
    bool isHoNgheo() const { return hoNgheo; }
    std::string getMaBSPhuTrach() const { return maBSPhuTrach; }
    std::string getMaPhongDieuTri() const { return maPhongDieuTri; }
    QDate getNgayNhapVien() const { return ngayNhapVien; }
    QDate getNgayRaVien() const { return ngayRaVien; }
    std::string getPhuongThucThanhToan() const { return phuongThucThanhToan; }
    double getTongChiPhi() const { return tongChiPhi; }
    bool isDaXuatVien() const { return daXuatVien; }

    void setBenhLy(const std::string& bl) { benhLy = bl; }
    void setHoNgheo(bool hn) { hoNgheo = hn; }
    void setMaBSPhuTrach(const std::string& maBS) { maBSPhuTrach = maBS; }
    void setMaPhongDieuTri(const std::string& maPhong) { maPhongDieuTri = maPhong; }
    void setPhuongThucThanhToan(const std::string& pttt) { phuongThucThanhToan = pttt; }
    void setNgayRaVien(const QDate& nrv) { ngayRaVien = nrv; }
    void setTongChiPhi(double cp) { tongChiPhi = cp; }
    void setDaXuatVien(bool dxv) { daXuatVien = dxv; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "Mã BN: " << getMaBN()
           << ", Tên: " << getHoTen()
           << ", Giới Tính: " << getGioiTinh()
           << ", Ngày Sinh: " << getNgaySinh().toString("dd/MM/yyyy").toStdString()
           << ", Tuổi: " << getTuoi()
           << ", SĐT: " << getSoDienThoai()
           << ", Địa Chỉ: " << getDiaChi()
           << ", Bệnh Lý: " << benhLy
           << ", Hộ Nghèo: " << (hoNgheo ? "Có" : "Không")
           << ", BS PT: " << maBSPhuTrach
           << ", Phòng: " << maPhongDieuTri
           << ", Ngày NV: " << ngayNhapVien.toString("dd/MM/yyyy").toStdString()
           << ", Đã xuất viện: " << (daXuatVien ? "Có" : "Không");
        return ss.str();
    }
};

class BacSi : public Nguoi {
private:
    std::string chuyenKhoa;

public:
    BacSi(const std::string& maBS, const std::string& hoTen, const std::string& gioiTinh,
          const QDate& ngaySinh, const std::string& sdt, const std::string& diaChi,
          const std::string& chuyenKhoa)
        : Nguoi(maBS, hoTen, gioiTinh, ngaySinh, sdt, diaChi), chuyenKhoa(chuyenKhoa) {}

    std::string getMaBS() const { return getMa(); }
    std::string getChuyenKhoa() const { return chuyenKhoa; }
    void setChuyenKhoa(const std::string& ck) { chuyenKhoa = ck; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "Mã BS: " << getMaBS()
           << ", Tên: " << getHoTen()
           << ", Giới Tính: " << getGioiTinh()
           << ", Ngày Sinh: " << getNgaySinh().toString("dd/MM/yyyy").toStdString()
           << ", Tuổi: " << getTuoi()
           << ", SĐT: " << getSoDienThoai()
           << ", Địa Chỉ: " << getDiaChi()
           << ", Chuyên Khoa: " << chuyenKhoa;
        return ss.str();
    }
};

class PhongBenh {
private:
    std::string maPhong;
    std::string loaiPhong;
    int soGiuong;
    int soBNDangNam;

public:
    PhongBenh(const std::string& maPhong, const std::string& loaiPhong, int soGiuong)
        : maPhong(maPhong), loaiPhong(loaiPhong), soGiuong(soGiuong), soBNDangNam(0) {}

    std::string getMaPhong() const { return maPhong; }
    std::string getLoaiPhong() const { return loaiPhong; }
    int getSoGiuong() const { return soGiuong; }
    int getSoBNDangNam() const { return soBNDangNam; }

    void setLoaiPhong(const std::string& lp) { loaiPhong = lp; }
    void setSoGiuong(int sg) { soGiuong = sg; }
    void tangBN() {
        if (soBNDangNam < soGiuong) soBNDangNam++;
        else throw std::runtime_error("Phòng đã đầy, không thể thêm bệnh nhân.");
    }
    void giamBN() {
        if (soBNDangNam > 0) soBNDangNam--;
        else throw std::runtime_error("Phòng không có bệnh nhân, không thể giảm.");
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "Mã Phòng: " << maPhong
           << ", Loại: " << loaiPhong
           << ", Số Giường: " << soGiuong
           << ", BN Đang Nằm: " << soBNDangNam;
        return ss.str();
    }
};

// =======================================================
// LỚP QUẢN LÝ
// =======================================================

class QuanLyBenhVien {
private:
    std::unordered_map<std::string, std::shared_ptr<BenhNhan>> dsBenhNhan;
    std::unordered_map<std::string, std::shared_ptr<BacSi>> dsBacSi;
    std::unordered_map<std::string, std::shared_ptr<PhongBenh>> dsPhong;
    int maBNCounter;
    int maBSCounter;

    void luuFile(const std::string& filename, const std::string& content) const;
    std::string docFile(const std::string& filename) const;

public:

    void luuDuLieu();
    void docDuLieu();
    QuanLyBenhVien();
    ~QuanLyBenhVien() { luuDuLieu(); }

    void tangMaBNCounter() { maBNCounter++; }
    void tangMaBSCounter() { maBSCounter++; }
    std::string taoMaBNTuDong() const;
    std::string taoMaBSTuDong() const;

    void themBenhNhan(std::shared_ptr<BenhNhan> bn);
    void themBacSi(std::shared_ptr<BacSi> bs);
    void themPhong(std::shared_ptr<PhongBenh> phong);

    void xoaBenhNhan(const std::string& maBN);
    void xoaBacSi(const std::string& maBS);
    void xoaPhong(const std::string& maPhong);

    void phanCongDieuTri(const std::string& maBN, const std::string& maBS, const std::string& maPhong);
    double raVien(const std::string& maBN, const QDate& ngayRaVien);

    std::shared_ptr<BenhNhan> getBenhNhan(const std::string& maBN) const;
    std::shared_ptr<BacSi> getBacSi(const std::string& maBS) const;
    std::shared_ptr<PhongBenh> getPhong(const std::string& maPhong) const;
    std::vector<std::shared_ptr<BenhNhan>> getBenhNhanPhuTrach(const std::string& maBS) const;

    const std::unordered_map<std::string, std::shared_ptr<BenhNhan>>& getDsBenhNhan() const { return dsBenhNhan; }
    const std::unordered_map<std::string, std::shared_ptr<BacSi>>& getDsBacSi() const { return dsBacSi; }
    const std::unordered_map<std::string, std::shared_ptr<PhongBenh>>& getDsPhong() const { return dsPhong; }

    std::string thongKeTongHop() const;

    bool isMaBNUnique(const std::string& ma) const { return dsBenhNhan.find(ma) == dsBenhNhan.end(); }
    bool isMaBSUnique(const std::string& ma) const { return dsBacSi.find(ma) == dsBacSi.end(); }
    bool isMaPhongUnique(const std::string& ma) const { return dsPhong.find(ma) == dsPhong.end(); }
};

// =======================================================
// LỚP GIAO DIỆN CHÍNH
// =======================================================

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();
    void setupBenhNhanTab();
    void setupBacSiTab();
    void setupPhongBenhTab();
    void setupDieuTriTab();
    void setupXuatVienTab();
    void setupThongKeTab();

    void hienThiBenhNhan();
    void hienThiBacSi();
    void hienThiPhong();
    void hienThiDieuTri();
    void hienThiXuatVien();
    void hienThiThongKe();

    void themSuaBenhNhanDialog(bool isThem = true, const std::string& maBN = "");
    void themSuaBacSiDialog(bool isThem = true, const std::string& maBS = "");
    void themSuaPhongDialog(bool isThem = true, const std::string& maPhong = "");
    void phanCongDialog();
    void raVienDialog();
    void xuatHoaDonDialog(const std::string& maBN, double chiPhi, int soNgay);

    bool checkFilterBenhNhan(const std::shared_ptr<BenhNhan>& bn) const;      // Thêm const + &
    bool checkFilterBacSi(const std::shared_ptr<BacSi>& bs) const;           // Thêm const + &
    bool checkFilterXuatVien(const std::shared_ptr<BenhNhan>& bn) const;

private slots:
    void onThemBenhNhan();
    void onSuaBenhNhan();
    void onXoaBenhNhan();
    void onXuatBenhNhan();
    void onFilterBenhNhan();

    void onThemBacSi();
    void onSuaBacSi();
    void onXoaBacSi();
    void onXuatBacSi();
    void onFilterBacSi();

    void onThemPhong();
    void onSuaPhong();
    void onXoaPhong();
    void onXuatPhong();

    void onPhanCong();
    void onRaVien();

    void onFilterXuatVien();
    void onXuatDanhSachXuatVien();
    void onXemChiTietXuatVien();

    void onThongKe();

private:
    QuanLyBenhVien qlbv;

    QTabWidget* tabWidget;

    // Tab BN
    QWidget* benhNhanTab;
    QTableWidget* benhNhanTable;
    QPushButton* btnThemBN;
    QPushButton* btnSuaBN;
    QPushButton* btnXoaBN;
    QPushButton* btnXuatBN;
    QLineEdit* filterBNInput;
    QComboBox* filterBNCombo;

    // Tab BS
    QWidget* bacSiTab;
    QTableWidget* bacSiTable;
    QPushButton* btnThemBS;
    QPushButton* btnSuaBS;
    QPushButton* btnXoaBS;
    QPushButton* btnXuatBS;
    QLineEdit* searchBacSiLine;
    QComboBox* filterChuyenKhoaBacSiCombo;

    // Tab Phòng
    QWidget* phongBenhTab;
    QTableWidget* phongBenhTable;
    QPushButton* btnThemPhong;
    QPushButton* btnSuaPhong;
    QPushButton* btnXoaPhong;
    QPushButton* btnXuatPhong;

    // Tab điều trị
    QWidget* dieuTriTab;
    QTableWidget* tableBenhNhanChuaPhanCong;
    QTableWidget* tableBenhNhanDaPhanCong;
    QPushButton* btnPhanCong;
    QPushButton* btnRaVien;
    QLineEdit* searchDieuTriInput;

    // Tab xuất viện
    QWidget* xuatVienTab;
    QTableWidget* xuatVienTable;
    QLineEdit* searchXuatVienInput;
    QComboBox* filterXuatVienCombo;
    QPushButton* btnXuatDSXuatVien;
    QPushButton* btnXemChiTietXV;

    // Tab thống kê
    QWidget* thongKeTab;
    QTextEdit* thongKeOutput;
    QPushButton* btnThongKe;

    QComboBox *filterBenhLyCombo;

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
