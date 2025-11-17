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
    {"Nội Tim Mạch", {"Tim Mạch"}},
    {"Ngoại Tim Mạch", {"Tim Mạch"}},

    {"Nội Hô Hấp", {"Hô Hấp"}},
    {"Nhi", {"Hô Hấp"}},

    {"Nội Tiêu Hóa", {"Tiêu Hóa"}},
    {"Ngoại Tiêu Hóa", {"Tiêu Hóa"}},

    {"Nội Thần Kinh", {"Thần Kinh"}},
    {"Ngoại Thần Kinh", {"Thần Kinh"}},

    {"Chấn Thương Chỉnh Hình", {"Cơ Xương Khớp"}},
    {"Nội Cơ Xương Khớp", {"Cơ Xương Khớp"}},

    {"Truyền Nhiễm", {"Truyền Nhiễm"}},

    // Các chuyên khoa Tổng Quát có thể bao quát nhiều loại bệnh lý
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
    int tuoi;

public:
    Nguoi(const std::string& ma, const std::string& hoTen, const std::string& gioiTinh, int tuoi)
        : ma(ma), hoTen(hoTen), gioiTinh(gioiTinh), tuoi(tuoi) {}
    virtual ~Nguoi() {}

    // Getters
    std::string getMa() const { return ma; }
    std::string getHoTen() const { return hoTen; }
    std::string getGioiTinh() const { return gioiTinh; }
    int getTuoi() const { return tuoi; }

    // Setters
    void setHoTen(const std::string& ten) { hoTen = ten; }
    void setGioiTinh(const std::string& gt) { gioiTinh = gt; }
    void setTuoi(int t) { tuoi = t; }

    virtual std::string toString() const = 0;
};

class BenhNhan : public Nguoi {
private:
    std::string benhLy;
    bool hoNgheo; // True nếu thuộc hộ nghèo, False nếu không
    std::string maBSPhuTrach; // Mã bác sĩ phụ trách
    std::string maPhongDieuTri; // Mã phòng điều trị
    QDate ngayNhapVien;

public:
    BenhNhan(const std::string& maBN, const std::string& hoTen, const std::string& gioiTinh, int tuoi,
             const std::string& benhLy, bool hoNgheo, const QDate& ngayNV)
        : Nguoi(maBN, hoTen, gioiTinh, tuoi), benhLy(benhLy), hoNgheo(hoNgheo), ngayNhapVien(ngayNV) {
        maBSPhuTrach = "";
        maPhongDieuTri = "";
    }

    // Getters
    std::string getMaBN() const { return getMa(); }
    std::string getBenhLy() const { return benhLy; }
    bool isHoNgheo() const { return hoNgheo; }
    std::string getMaBSPhuTrach() const { return maBSPhuTrach; }
    std::string getMaPhongDieuTri() const { return maPhongDieuTri; }
    QDate getNgayNhapVien() const { return ngayNhapVien; }

    // Setters
    void setBenhLy(const std::string& bl) { benhLy = bl; }
    void setHoNgheo(bool hn) { hoNgheo = hn; }
    void setMaBSPhuTrach(const std::string& maBS) { maBSPhuTrach = maBS; }
    void setMaPhongDieuTri(const std::string& maPhong) { maPhongDieuTri = maPhong; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "Mã BN: " << getMaBN()
           << ", Tên: " << getHoTen()
           << ", Giới Tính: " << getGioiTinh()
           << ", Tuổi: " << getTuoi()
           << ", Bệnh Lý: " << benhLy
           << ", Hộ Nghèo: " << (hoNgheo ? "Có" : "Không")
           << ", BS PT: " << maBSPhuTrach
           << ", Phòng: " << maPhongDieuTri
           << ", Ngày NV: " << ngayNhapVien.toString("dd/MM/yyyy").toStdString();
        return ss.str();
    }
};

class BacSi : public Nguoi {
private:
    std::string chuyenKhoa;

public:
    BacSi(const std::string& maBS, const std::string& hoTen, const std::string& gioiTinh, int tuoi,
          const std::string& chuyenKhoa)
        : Nguoi(maBS, hoTen, gioiTinh, tuoi), chuyenKhoa(chuyenKhoa) {}

    // Getters
    std::string getMaBS() const { return getMa(); }
    std::string getChuyenKhoa() const { return chuyenKhoa; }

    // Setters
    void setChuyenKhoa(const std::string& ck) { chuyenKhoa = ck; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "Mã BS: " << getMaBS()
           << ", Tên: " << getHoTen()
           << ", Giới Tính: " << getGioiTinh()
           << ", Tuổi: " << getTuoi()
           << ", Chuyên Khoa: " << chuyenKhoa;
        return ss.str();
    }
};

class PhongBenh {
private:
    std::string maPhong;
    std::string loaiPhong; // Thường/VIP
    int soGiuong;
    int soBNDangNam;

public:
    PhongBenh(const std::string& maPhong, const std::string& loaiPhong, int soGiuong)
        : maPhong(maPhong), loaiPhong(loaiPhong), soGiuong(soGiuong) {
        soBNDangNam = 0;
    }

    // Getters
    std::string getMaPhong() const { return maPhong; }
    std::string getLoaiPhong() const { return loaiPhong; }
    int getSoGiuong() const { return soGiuong; }
    int getSoBNDangNam() const { return soBNDangNam; }

    // Setters
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

    void luuFile(const std::string& filename, const std::string& content) const;
    std::string docFile(const std::string& filename) const;
    void luuDuLieu();
    void docDuLieu();

public:
    QuanLyBenhVien();
    ~QuanLyBenhVien() { luuDuLieu(); }

    // Thêm
    void themBenhNhan(std::shared_ptr<BenhNhan> bn);
    void themBacSi(std::shared_ptr<BacSi> bs);
    void themPhong(std::shared_ptr<PhongBenh> phong);

    // Xóa
    void xoaBenhNhan(const std::string& maBN);
    void xoaBacSi(const std::string& maBS);
    void xoaPhong(const std::string& maPhong);

    // Sửa (có thể dùng getter + setter để sửa trực tiếp)
    // Cập nhật quan hệ (phân công / ra viện)
    void phanCongDieuTri(const std::string& maBN, const std::string& maBS, const std::string& maPhong);
    double raVien(const std::string& maBN, const QDate& ngayRaVien); // Trả về chi phí

    // Tra cứu
    std::shared_ptr<BenhNhan> getBenhNhan(const std::string& maBN) const;
    std::shared_ptr<BacSi> getBacSi(const std::string& maBS) const;
    std::shared_ptr<PhongBenh> getPhong(const std::string& maPhong) const;
    std::vector<std::shared_ptr<BenhNhan>> getBenhNhanPhuTrach(const std::string& maBS) const;

    // Get toàn bộ danh sách
    const std::unordered_map<std::string, std::shared_ptr<BenhNhan>>& getDsBenhNhan() const { return dsBenhNhan; }
    const std::unordered_map<std::string, std::shared_ptr<BacSi>>& getDsBacSi() const { return dsBacSi; }
    const std::unordered_map<std::string, std::shared_ptr<PhongBenh>>& getDsPhong() const { return dsPhong; }

    // Thống kê
    std::string thongKeTongHop() const;

    // Helper kiểm tra
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
    // Setup UI tabs
    void setupUI();
    void setupBenhNhanTab();
    void setupBacSiTab();
    void setupPhongBenhTab();
    void setupDieuTriTab();
    void setupThongKeTab();

    // Hiển thị dữ liệu
    void hienThiBenhNhan();
    void hienThiBacSi();
    void hienThiPhong();
    void hienThiDieuTri();
    void hienThiThongKe();

    // Dialogs
    void themSuaBenhNhanDialog(bool isThem = true, const std::string& maBN = "");
    void themSuaBacSiDialog(bool isThem = true, const std::string& maBS = "");
    void themSuaPhongDialog(bool isThem = true, const std::string& maPhong = "");
    void phanCongDialog();
    void raVienDialog();

    // Helpers
    bool checkFilterBenhNhan(std::shared_ptr<BenhNhan> bn);
    bool checkFilterBacSi(std::shared_ptr<BacSi> bs);

private slots:
    // Slots BN
    void onThemBenhNhan();
    void onSuaBenhNhan();
    void onXoaBenhNhan();
    void onXuatBenhNhan();
    void onFilterBenhNhan();

    // Slots BS
    void onThemBacSi();
    void onSuaBacSi();
    void onXoaBacSi();
    void onXuatBacSi();
    void onFilterBacSi();

    // Slots Phòng
    void onThemPhong();
    void onSuaPhong();
    void onXoaPhong();
    void onXuatPhong();

    // Slots Điều trị
    void onPhanCong();
    void onRaVien();

    // Slots Thống kê
    void onThongKe();

private:
    QuanLyBenhVien qlbv;

    // UI elements
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

    // Tab Điều trị
    QWidget* dieuTriTab;
    QListWidget* listBenhNhanChuaPhanCong;
    QListWidget* listBenhNhanDaPhanCong;
    QPushButton* btnPhanCong;
    QPushButton* btnRaVien;

    // Tab Thống kê
    QWidget* thongKeTab;
    QTextEdit* thongKeOutput;
    QPushButton* btnThongKe;
};

#endif // MAINWINDOW_H
