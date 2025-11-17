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
#include <QSet> // Thêm QSet

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
const std::unordered_map<std::string, std::vector<std::string>> chuyenKhoaToBenhLy = {
    {"Tim mạch", {"Nhồi máu cơ tim", "Suy tim", "Tăng huyết áp"}},
    {"Hô hấp", {"Viêm phổi", "Hen suyễn", "Lao phổi"}},
    {"Tiêu hóa", {"Viêm loét dạ dày", "Viêm ruột thừa", "Sỏi mật"}},
    {"Thần kinh", {"Đột quỵ", "Parkinson", "Đau đầu migraine"}},
    {"Nội tiết", {"Tiểu đường", "Basedow", "Béo phì"}},
    {"Sản khoa", {"Thai ngoài tử cung", "Tiền sản giật", "Rong kinh"}},
};

// HÀM TIỆN ÍCH: Thu thập tất cả bệnh lý duy nhất (dùng trong ComboBox)
inline std::vector<std::string> getAllUniqueBenhLy() {
    QSet<QString> uniqueDiseases;
    for (const auto& pair : chuyenKhoaToBenhLy) {
        for (const auto& disease : pair.second) {
            uniqueDiseases.insert(QString::fromStdString(disease));
        }
    }
    std::vector<std::string> result;
    for (const QString& qstr : uniqueDiseases.values()) {
        result.push_back(qstr.toStdString());
    }
    std::sort(result.begin(), result.end());
    return result;
}

// ============= CÁC CLASS DỮ LIỆU CƠ BẢN =============

class Nguoi {
private:
    std::string ma;
    std::string hoTen;
    std::string gioiTinh;
    std::string sdt;
    std::string diaChi;

public:
    Nguoi(const std::string& ma, const std::string& hoTen, const std::string& gioiTinh, const std::string& sdt, const std::string& diaChi)
        : ma(ma), hoTen(hoTen), gioiTinh(gioiTinh), sdt(sdt), diaChi(diaChi) {}
    virtual ~Nguoi() = default;

    // Getters
    std::string getMa() const { return ma; }
    std::string getHoTen() const { return hoTen; }
    std::string getGioiTinh() const { return gioiTinh; }
    std::string getSdt() const { return sdt; }
    std::string getDiaChi() const { return diaChi; }

    // Setters
    void setHoTen(const std::string& ht) { hoTen = ht; }
    void setGioiTinh(const std::string& gt) { gioiTinh = gt; }
    void setSdt(const std::string& s) { sdt = s; }
    void setDiaChi(const std::string& dc) { diaChi = dc; }
};

class BenhNhan; // Forward declaration

class BacSi : public Nguoi {
private:
    std::string chuyenKhoa;
    std::vector<std::weak_ptr<BenhNhan>> danhSachBN;

public:
    BacSi(const std::string& ma, const std::string& hoTen, const std::string& gioiTinh, const std::string& sdt, const std::string& diaChi, const std::string& chuyenKhoa)
        : Nguoi(ma, hoTen, gioiTinh, sdt, diaChi), chuyenKhoa(chuyenKhoa) {}

    std::string getMaBS() const { return getMa(); }
    std::string getChuyenKhoa() const { return chuyenKhoa; }
    int getSoBNPhuTrach() const {
        int count = 0;
        for (const auto& weak_bn : danhSachBN) {
            if (weak_bn.lock()) count++;
        }
        return count;
    }

    void setChuyenKhoa(const std::string& ck) { chuyenKhoa = ck; }

    void themBenhNhan(std::shared_ptr<BenhNhan> bn);
    void xoaBenhNhan(const std::string& maBN);
};

class BenhNhan : public Nguoi {
private:
    std::string ngaySinh;
    std::string ngayNhapVien;
    std::string benhLy;
    bool laHoNgheo;
    bool trangThaiNhapVien;
    std::string maPhongHienTai;

public:
    BenhNhan(const std::string& ma, const std::string& hoTen, const std::string& gioiTinh, const std::string& sdt, const std::string& diaChi, const std::string& ngaySinh, const std::string& ngayNhapVien, const std::string& benhLy, bool laHoNgheo)
        : Nguoi(ma, hoTen, gioiTinh, sdt, diaChi), ngaySinh(ngaySinh), ngayNhapVien(ngayNhapVien), benhLy(benhLy), laHoNgheo(laHoNgheo), trangThaiNhapVien(true) {}

    std::string getMaBN() const { return getMa(); }
    std::string getNgaySinh() const { return ngaySinh; }
    std::string getNgayNhapVien() const { return ngayNhapVien; }
    std::string getBenhLy() const { return benhLy; }
    bool getLaHoNgheo() const { return laHoNgheo; }
    bool getTrangThaiNhapVien() const { return trangThaiNhapVien; }
    std::string getMaPhongHienTai() const { return maPhongHienTai; }

    void setNgaySinh(const std::string& ns) { ngaySinh = ns; }
    void setNgayNhapVien(const std::string& nnv) { ngayNhapVien = nnv; }
    void setBenhLy(const std::string& bl) { benhLy = bl; }
    void setLaHoNgheo(bool lhn) { laHoNgheo = lhn; }
    void setTrangThaiNhapVien(bool tt) { trangThaiNhapVien = tt; }
    void setMaPhongHienTai(const std::string& mp) { maPhongHienTai = mp; }
};

class PhongBenh {
private:
    std::string maPhong;
    std::string loaiPhong; // Thường, VIP
    int soGiuong;
    int soBNDangNam;
    std::string trangThai; // Trống, Đầy, Sửa chữa

public:
    PhongBenh(const std::string& ma, const std::string& loai, int soGiuong)
        : maPhong(ma), loaiPhong(loai), soGiuong(soGiuong), soBNDangNam(0) {
        capNhatTrangThai();
    }

    std::string getMaPhong() const { return maPhong; }
    std::string getLoaiPhong() const { return loaiPhong; }
    int getSoGiuong() const { return soGiuong; }
    int getSoBNDangNam() const { return soBNDangNam; }
    std::string getTrangThai() const { return trangThai; }

    void setLoaiPhong(const std::string& lp) { loaiPhong = lp; }
    void setSoGiuong(int sg) { soGiuong = sg; capNhatTrangThai(); }
    void setTrangThai(const std::string& tt) { trangThai = tt; }

    void tangBN() { soBNDangNam++; capNhatTrangThai(); }
    void giamBN() { soBNDangNam = std::max(0, soBNDangNam - 1); capNhatTrangThai(); }

private:
    void capNhatTrangThai() {
        if (trangThai == "Sửa chữa") return;
        if (soBNDangNam >= soGiuong) {
            trangThai = "Đầy";
        } else if (soBNDangNam == 0) {
            trangThai = "Trống";
        } else {
            trangThai = "Còn chỗ";
        }
    }
};

struct PhanCong {
    std::string maBN;
    std::string maBS;
    std::string maPhong;
};

// ============= QUẢN LÝ BỆNH VIỆN =============
class QuanLyBenhVien {
private:
    std::unordered_map<std::string, std::shared_ptr<BenhNhan>> dsBN;
    std::unordered_map<std::string, std::shared_ptr<BacSi>> dsBS;
    std::unordered_map<std::string, std::shared_ptr<PhongBenh>> dsPhong;
    std::unordered_map<std::string, PhanCong> dsPhanCong; // Key: MaBN

public:
    // Thêm/Sửa
    void themBN(std::shared_ptr<BenhNhan> bn) { dsBN[bn->getMaBN()] = bn; }
    void suaBN(std::shared_ptr<BenhNhan> bn) { if (dsBN.count(bn->getMaBN())) dsBN[bn->getMaBN()] = bn; }
    void themBS(std::shared_ptr<BacSi> bs) { dsBS[bs->getMaBS()] = bs; }
    void suaBS(std::shared_ptr<BacSi> bs) { if (dsBS.count(bs->getMaBS())) dsBS[bs->getMaBS()] = bs; }
    void themPhong(std::shared_ptr<PhongBenh> phong) { dsPhong[phong->getMaPhong()] = phong; }
    void suaPhong(std::shared_ptr<PhongBenh> phong) { if (dsPhong.count(phong->getMaPhong())) dsPhong[phong->getMaPhong()] = phong; }

    // Xóa
    void xoaBN(const std::string& maBN);
    void xoaBS(const std::string& maBS);
    void xoaPhong(const std::string& maPhong);

    // Tìm kiếm
    std::shared_ptr<BenhNhan> timBN(const std::string& maBN) { return dsBN.count(maBN) ? dsBN.at(maBN) : nullptr; }
    std::shared_ptr<BacSi> timBS(const std::string& maBS) { return dsBS.count(maBS) ? dsBS.at(maBS) : nullptr; }
    std::shared_ptr<PhongBenh> timPhong(const std::string& maPhong) { return dsPhong.count(maPhong) ? dsPhong.at(maPhong) : nullptr; }

    // Phân công & Ra viện
    void phanCong(const std::string& maBN, const std::string& maBS, const std::string& maPhong);
    void raVien(const std::string& maBN);

    // Getters
    const std::unordered_map<std::string, std::shared_ptr<BenhNhan>>& getDsBN() const { return dsBN; }
    const std::unordered_map<std::string, std::shared_ptr<BacSi>>& getDsBS() const { return dsBS; }
    const std::unordered_map<std::string, std::shared_ptr<PhongBenh>>& getDsPhong() const { return dsPhong; }
    const std::unordered_map<std::string, PhanCong>& getDsPhanCong() const { return dsPhanCong; }
};

// ============= MAINWINDOW =============

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // Setup UI
    void setupUI();
    void createBenhNhanTab();
    void createBacSiTab();
    void createPhongTab();
    void createDieuTriTab();
    void createThongKeTab();

    // Hàm hỗ trợ tạo mã
    std::string generateNextMaBN();
    std::string generateNextMaBS();

    // Cập nhật bảng
    void updateBenhNhanTable(const std::string& keyword = "", const std::string& filterDisease = "");
    void updateBacSiTable(const std::string& keyword = "", const std::string& filterChuyenKhoa = "");
    void updatePhongTable();
    void updatePhanCongTable();

    // Hộp thoại & Xuất file
    void showBenhNhanInputDialog(std::shared_ptr<BenhNhan> bn = nullptr);
    void showBacSiInputDialog(std::shared_ptr<BacSi> bs = nullptr);
    void showPhongInputDialog(std::shared_ptr<PhongBenh> phong = nullptr);
    void showPhanCongDialog();
    void showRaVienDialog(); // Thủ tục Ra viện
    void exportTableToCsv(QTableWidget* table, const QString& defaultFileName, const QString& title);
    void exportHoaDon(std::shared_ptr<BenhNhan> bn, int soNgayNam, double tongTienChuaGiam, double giamGia, double chiPhiSauGiam, std::shared_ptr<PhongBenh> phong, std::shared_ptr<BacSi> bs);


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
    QWidget* phongTab;
    QTableWidget* phongTable;
    QPushButton* btnThemPhong;
    QPushButton* btnSuaPhong;
    QPushButton* btnXoaPhong;
    QPushButton* btnXuatPhong;

    // Tab Điều trị
    QWidget* dieuTriTab;
    QTableWidget* phanCongTable;
    QPushButton* btnPhanCong;
    QPushButton* btnRaVien;

    // Tab Thống kê
    QWidget* thongKeTab;
    QTextEdit* thongKeOutput;
    QPushButton* btnThongKe;
};
#endif // MAINWINDOW_H
