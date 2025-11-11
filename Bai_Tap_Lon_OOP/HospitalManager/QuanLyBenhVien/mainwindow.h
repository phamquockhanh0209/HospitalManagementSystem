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
const std::unordered_map<std::string, std::vector<std::string>> chuyenKhoaToBenhLy = {
    {"Tim mach", {"Tim mach", "Cao huyet ap", "Nhoi mau co tim"}},
    {"Da lieu", {"Mui tri", "Nam da", "Viem da", "Cham"}},
    {"Noi khoa", {"Tieu duong", "Benh gan", "Benh than"}},
    {"Xuong khop", {"Thoai hoa khop", "Gout", "Trat khop"}}
};

// ============= LỚP NGƯỜI =============
class Nguoi {
protected:
    std::string hoTen;
    std::string gioiTinh;
    int ngaySinh, thangSinh, namSinh;
    std::string diaChi;

public:
    Nguoi(const std::string& hoTen, const std::string& gioiTinh,
          int ngaySinh, int thangSinh, int namSinh, const std::string& diaChi)
        : hoTen(hoTen), gioiTinh(gioiTinh), ngaySinh(ngaySinh),
        thangSinh(thangSinh), namSinh(namSinh), diaChi(diaChi) {}

    virtual ~Nguoi() = default;

    std::string getHoTen() const { return hoTen; }
    void setHoTen(const std::string& ht) { hoTen = ht; }
    void setDiaChi(const std::string& dc) { diaChi = dc; }
    std::string getDiaChi() const { return diaChi; }
    std::string getGioiTinh() const { return gioiTinh; }
    int getNgaySinh() const { return ngaySinh; }
    int getThangSinh() const { return thangSinh; }
    int getNamSinh() const { return namSinh; }

    int tinhTuoi() const {
        QDate current = QDate::currentDate();
        int tuoi = current.year() - namSinh;
        if (current.month() < thangSinh ||
            (current.month() == thangSinh && current.day() < ngaySinh)) {
            tuoi--;
        }
        return tuoi;
    }

    virtual std::string toString() const = 0;
};

// ============= LỚP DỊCH VỤ =============
class DichVu {
private:
    std::string tenDV;
    double giaDV;
public:
    DichVu(const std::string& ten, double gia) : tenDV(ten), giaDV(gia) {}
    std::string getTen() const { return tenDV; }
    double getGia() const { return giaDV; }
};

class BenhNhan;

// ============= LỚP BÁC SĨ =============
class BacSi : public Nguoi {
private:
    std::string maBS;
    std::string chuyenKhoa;
    std::vector<std::weak_ptr<BenhNhan>> danhSachBN;

public:
    BacSi(const std::string& maBS, const std::string& hoTen, const std::string& gioiTinh,
          int ngaySinh, int thangSinh, int namSinh, const std::string& diaChi,
          const std::string& chuyenKhoa)
        : Nguoi(hoTen, gioiTinh, ngaySinh, thangSinh, namSinh, diaChi),
        maBS(maBS), chuyenKhoa(chuyenKhoa) {}

    std::string getMaBS() const { return maBS; }
    std::string getChuyenKhoa() const { return chuyenKhoa; }
    void setChuyenKhoa(const std::string& ck) { chuyenKhoa = ck; }

    void themBenhNhan(std::shared_ptr<BenhNhan> bn);
    void xoaBenhNhan(const std::string& maBN);
    const std::vector<std::weak_ptr<BenhNhan>>& getDanhSachBN() const { return danhSachBN; }

    std::string toString() const override {
        return "BS{" + maBS + " | " + hoTen + " | " +
               std::to_string(tinhTuoi()) + " tuoi | " + chuyenKhoa + "}";
    }
};

// ============= LỚP PHÒNG BỆNH =============
class PhongBenh {
protected:
    int soPhong;
    int soGiuong;
    int soBNDangNam;

public:
    PhongBenh(int soPhong, int soGiuong)
        : soPhong(soPhong), soGiuong(soGiuong), soBNDangNam(0) {}

    virtual ~PhongBenh() = default;

    bool isTrong() const { return soBNDangNam < soGiuong; }
    void themBenhNhan() { if (soBNDangNam < soGiuong) soBNDangNam++; }
    void xoaBenhNhan() { if (soBNDangNam > 0) soBNDangNam--; }

    int getSoPhong() const { return soPhong; }
    int getSoGiuong() const { return soGiuong; }
    int getSoBNDangNam() const { return soBNDangNam; }
    void setSoGiuong(int sg) { soGiuong = sg; }

    virtual double tinhPhiNgayCoBan() const = 0;
    virtual std::string getLoaiPhong() const = 0;
};

class PhongThuong : public PhongBenh {
public:
    PhongThuong(int soPhong, int soGiuong) : PhongBenh(soPhong, soGiuong) {}
    double tinhPhiNgayCoBan() const override {
        return CauHinhGia::getInstance().giaPhongThuong;
    }
    std::string getLoaiPhong() const override { return "Thuong"; }
};

class PhongVIP : public PhongBenh {
public:
    PhongVIP(int soPhong, int soGiuong) : PhongBenh(soPhong, soGiuong) {}
    double tinhPhiNgayCoBan() const override {
        return CauHinhGia::getInstance().giaPhongVIP;
    }
    std::string getLoaiPhong() const override { return "VIP"; }
};

// ============= LỚP BỆNH NHÂN =============
class BenhNhan : public Nguoi {
private:
    std::string maBN;
    std::vector<std::string> danhSachBenhLy;
    std::shared_ptr<BacSi> bacSiDieuTri;
    std::shared_ptr<PhongBenh> soPhong;
    bool trangThaiNhapVien;
    bool coSoHoNgheo;
    std::vector<DichVu> dichVuSuDung;

public:
    BenhNhan(const std::string& maBN, const std::string& hoTen,
             const std::string& gioiTinh, int ngaySinh, int thangSinh,
             int namSinh, const std::string& diaChi,
             const std::vector<std::string>& dsBenhLy, bool coSoHoNgheo)
        : Nguoi(hoTen, gioiTinh, ngaySinh, thangSinh, namSinh, diaChi),
        maBN(maBN), danhSachBenhLy(dsBenhLy), trangThaiNhapVien(true),
        coSoHoNgheo(coSoHoNgheo) {}

    std::string getMaBN() const { return maBN; }
    const std::vector<std::string>& getDanhSachBenhLy() const { return danhSachBenhLy; }
    std::shared_ptr<BacSi> getBacSiDieuTri() const { return bacSiDieuTri; }
    std::shared_ptr<PhongBenh> getSoPhongObj() const { return soPhong; }
    bool getCoSoHoNgheo() const { return coSoHoNgheo; }
    bool getTrangThaiNhapVien() const { return trangThaiNhapVien; }

    void themBenhLy(const std::string& bl) { danhSachBenhLy.push_back(bl); }
    void capNhatThongTin(std::shared_ptr<BacSi> bs, std::shared_ptr<PhongBenh> phong) {
        bacSiDieuTri = bs;
        soPhong = phong;
    }

    void raVien() {
        trangThaiNhapVien = false;
        if (soPhong) soPhong->xoaBenhNhan();
        if (bacSiDieuTri) bacSiDieuTri->xoaBenhNhan(maBN);
        bacSiDieuTri = nullptr;
        soPhong = nullptr;
    }

    std::string toString() const override {
        std::string bl = "";
        for (size_t i = 0; i < danhSachBenhLy.size(); ++i) {
            bl += danhSachBenhLy[i];
            if (i < danhSachBenhLy.size() - 1) bl += ", ";
        }
        return "BN{" + maBN + " | " + hoTen + " | " +
               std::to_string(tinhTuoi()) + " tuoi | " + bl + "}";
    }
};

// ============= QUẢN LÝ BỆNH VIỆN =============
class QuanLyBenhVien {
private:
    std::unordered_map<std::string, std::shared_ptr<BenhNhan>> dsBN;
    std::unordered_map<std::string, std::shared_ptr<BacSi>> dsBS;
    std::unordered_map<int, std::shared_ptr<PhongBenh>> dsPhong;

public:
    void themBenhNhan(std::shared_ptr<BenhNhan> bn) { dsBN[bn->getMaBN()] = bn; }
    void themBacSi(std::shared_ptr<BacSi> bs) { dsBS[bs->getMaBS()] = bs; }
    void themPhong(std::shared_ptr<PhongBenh> phong) { dsPhong[phong->getSoPhong()] = phong; }

    std::shared_ptr<BenhNhan> timBenhNhan(const std::string& maBN) {
        auto it = dsBN.find(maBN);
        return (it != dsBN.end()) ? it->second : nullptr;
    }

    std::shared_ptr<BacSi> timBacSi(const std::string& maBS) {
        auto it = dsBS.find(maBS);
        return (it != dsBS.end()) ? it->second : nullptr;
    }

    std::shared_ptr<PhongBenh> timPhong(int soPhong) {
        auto it = dsPhong.find(soPhong);
        return (it != dsPhong.end()) ? it->second : nullptr;
    }

    void xoaBenhNhan(const std::string& maBN) { dsBN.erase(maBN); }
    void xoaBacSi(const std::string& maBS) { dsBS.erase(maBS); }
    void xoaPhong(int soPhong) { dsPhong.erase(soPhong); }

    const std::unordered_map<std::string, std::shared_ptr<BenhNhan>>& getDsBN() const { return dsBN; }
    const std::unordered_map<std::string, std::shared_ptr<BacSi>>& getDsBS() const { return dsBS; }
    const std::unordered_map<int, std::shared_ptr<PhongBenh>>& getDsPhong() const { return dsPhong; }

    void phanCong(std::shared_ptr<BenhNhan> bn, std::shared_ptr<BacSi> bs,
                  std::shared_ptr<PhongBenh> phong) {
        if (!phong->isTrong()) throw std::runtime_error("Phòng đã đầy!");

        auto phongCu = bn->getSoPhongObj();
        if (phongCu && phongCu != phong) phongCu->xoaBenhNhan();

        auto bsCu = bn->getBacSiDieuTri();
        if (bsCu && bsCu != bs) bsCu->xoaBenhNhan(bn->getMaBN());

        bn->capNhatThongTin(bs, phong);
        bs->themBenhNhan(bn);
        if (!phongCu || phongCu != phong) phong->themBenhNhan();
    }
};

// ============= MAIN WINDOW =============
class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    QuanLyBenhVien qlbv;
    QTabWidget* tabWidget;
    QTextEdit* thongKeOutput;

    QTableWidget* tableBN;
    QTableWidget* tableBS;
    QTableWidget* tablePhong;
    QTableWidget* tablePhanCong; // KHAI BÁO BẢNG PHÂN CÔNG MỚI

    QPushButton* btnThemBN;
    QPushButton* btnSuaBN;
    QPushButton* btnXoaBN;
    QPushButton* btnXuatBN;

    QPushButton* btnThemBS;
    QPushButton* btnSuaBS;
    QPushButton* btnXoaBS;
    QPushButton* btnXuatBS;

    QPushButton* btnThemPhong;
    QPushButton* btnSuaPhong;
    QPushButton* btnXuatPhong;

    QPushButton* btnPhanCong;
    QPushButton* btnRaVien;
    QPushButton* btnThongKe;

    void setupUI();
    void createBenhNhanTab();
    void createBacSiTab();
    void createPhongTab();
    void createDieuTriTab();
    void createThongKeTab();

    void updateBenhNhanTable();
    void updateBacSiTable();
    void updatePhongTable();
    void updatePhanCongTable(); // KHAI BÁO HÀM CẬP NHẬT BẢNG MỚI

    void exportTableToCsv(QTableWidget* table, const QString& defaultFileName, const QString& title);

    void showBenhNhanInputDialog(std::shared_ptr<BenhNhan> bn = nullptr);
    void showBacSiInputDialog();
    void showPhongInputDialog(std::shared_ptr<PhongBenh> phong = nullptr);
    void showPhanCongDialog();


private slots:
    void onThemBenhNhan();
    void onSuaBenhNhan();
    void onXoaBenhNhan();
    void onXuatBenhNhan();

    void onThemBacSi();
    void onSuaBacSi();
    void onXoaBacSi();
    void onXuatBacSi();

    void onThemPhong();
    void onSuaPhong();
    void onXuatPhong();

    void onPhanCong();
    void onRaVien();
    void onThongKe();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};

#endif // MAINWINDOW_H
