#include <iostream>
#include <vector>
#include <string>
#include <fstream>       // thư viện xử lý file(fstream)
#include <sstream>       // thư viện xử lý luồng chuỗi(istringstream, ostringstream)
#include <algorithm>     // thư viện xử lý thuật toán(transform)
#include <cctype>        // thư viện xử lý ký tự (chuyển chữ hoa thành chữ thường)
#include <stdexcept>     // thư viện xử lý ngoại lệ(runtime_error)
#include <unordered_map> // thư viện bản đồ băm(unordered_map)
#include <chrono>        // thư viện thời gian(chrono)
#include <memory>        //thư viện quản lý bộ nhớ thông minh
#include <limits>        // thư viện giới hạn số học(numeric_limits)
#include <mutex>         // thư viện đa luồng(mutex, call_once, once_flag)
#include <iomanip>       // thư viện định dạng đầu ra(iomanip)
#include <windows.h>     // CHO GetModuleFileNameA
#include <conio.h>       // thư viện _getch()
#define MAX_PATH 260
using namespace std;

// ============= CAU HINH GIA DICH VU =============
struct CauHinhGia
{
    double giaPhongThuong = 500000.0;
    double giaPhongVIP = 2000000.0;
    double giamGiaBHYT = 0.5; // 50%

    static CauHinhGia &getInstance()
    {
        static CauHinhGia instance;
        return instance;
    }
};

// ============= ANH XA BENH LY - CHUYEN KHOA =============
const unordered_map<string, vector<string>> chuyenKhoaToBenhLy = {
    {"Tim mach", {"Tim mach", "Cao huyet ap", "Nhoi mau co tim"}},
    {"Da lieu", {"Mui tri", "Nam da", "Viem da", "Cham"}},
    {"Noi khoa", {"Tieu duong", "Benh gan", "Benh than"}},
    {"Xuong khop", {"Thoai hoa khop", "Gout", "Trat khop"}}};

// Hàm lấy đường dẫn project
string getProjectPath()
{
    char buffer[MAX_PATH];                      // Bộ đệm để lưu trữ đường dẫn
    GetModuleFileNameA(NULL, buffer, MAX_PATH); // Lấy đường dẫn file .exe
    string path = buffer;                       // Chuyển sang chuỗi C++
    size_t pos = path.find_last_of("\\/");      // Tìm vị trí cuối cùng của dấu phân cách thư mục
    return path.substr(0, pos + 1);             // Trả về thư mục chứa .exe
}

// tích hợp chatbot AI đẻ hỗ trợ người dùng 24/7
string callChatbotAI(const string &question)
{
    // Ghi câu hỏi vào file
    string inputPath = getProjectPath() + "chatbot_input.txt";
    string outputPath = getProjectPath() + "chatbot_output.txt";

    ofstream outFile(inputPath);
    if (!outFile.is_open())
    {
        return "[Loi] Khong the tao file input!";
    }
    outFile << question;
    outFile.close();

    // Gọi Python script
    string pythonPath = getProjectPath() + "chatbot_ai.py";
    string command = "python \"" + pythonPath + "\"";
    system(command.c_str());

    // Đọc kết quả
    ifstream inFile(outputPath);
    if (!inFile.is_open())
    {
        return "[Loi] Khong the doc ket qua!";
    }

    string response((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    return response;
}

// Chuyển chuỗi sang chữ thường
string toLower(string s)
{
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

string timKhoaChoBenh(const string &benhLy)
{
    string benhLyLower = toLower(benhLy); // Chuẩn hóa về chữ thường
    for (const auto &pair : chuyenKhoaToBenhLy)
    {
        const string &khoa = pair.first;
        const vector<string> &dsBenh = pair.second;
        for (const auto &benh : dsBenh)
        {
            if (toLower(benh) == benhLyLower)
            { // So sánh không phân biệt hoa thường
                return khoa;
            }
        }
    }
    throw runtime_error("Benh ly khong hop le!");
}

// ============= HAM TIEN ICH =============

// Validate ngay thang (da sua nam nhuan)
bool isValidDate(int day, int month, int year)
{
    if (year < 1900 || year > 2025)
        return false;
    if (month < 1 || month > 12)
        return false;
    if (day < 1 || day > 31)
        return false;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Kiem tra nam nhuan
    if (month == 2)
    {
        bool isLeap = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
        return day <= (isLeap ? 29 : 28);
    }

    return day <= daysInMonth[month - 1];
}

// Chuyen doi string sang int an toan
int safeStoi(const string &str, const string &fieldName)
{
    try
    {
        size_t pos;
        int value = stoi(str, &pos);
        if (pos != str.size())
            throw invalid_argument("Invalid number format");
        return value;
    }
    catch (const exception &e)
    {
        throw runtime_error("Loi chuyen doi " + fieldName + ": " + e.what());
    }
}

// Nhap chuoi khong rong
string nhapChuoiKhongRong(const string &prompt)
{
    string input;
    do
    {
        cout << prompt;
        getline(cin, input);
        // Loai bo khoang trang dau/cuoi
        input.erase(0, input.find_first_not_of(" \t\n\r"));
        input.erase(input.find_last_not_of(" \t\n\r") + 1);

        if (!input.empty())
            return input;
        cout << "[!] Khong duoc de trong! Vui long nhap lai.\n";
    } while (true);
}

// Nhap so nguyen trong khoang
int nhapSoNguyen(const string &prompt, int min, int max)
{
    int value;
    string input;
    while (true)
    {
        cout << prompt;
        getline(cin, input);
        try
        {
            value = safeStoi(input, "so nguyen");
            if (value >= min && value <= max)
            {
                return value;
            }
            cout << "[!] Gia tri phai tu " << min << " den " << max << "!\n";
        }
        catch (const exception &e)
        {
            cout << "[!] " << e.what() << " Vui long nhap lai.\n";
        }
    }
}

// Nhap gioi tinh
string nhapGioiTinh()
{
    while (true)
    {
        string gt = nhapChuoiKhongRong("Gioi tinh (Nam/Nu): ");
        if (gt == "Nam" || gt == "Nu")
            return gt;
        cout << "[!] Gioi tinh chi duoc nhap 'Nam' hoac 'Nu'!\n";
    }
}

// Xac nhan hanh dong
bool xacNhan(const string &message)
{
    cout << "[?] " << message << " (y/n): ";
    char c;
    cin >> c;
    cin.ignore(999999, '\n');
    return (c == 'y' || c == 'Y');
}

// Forward declarations
class BenhNhan;
class BacSi;
class PhongBenh;

// ============= LOP NGUOI =============
class Nguoi
{
protected:
    string hoTen;
    string gioiTinh;
    int ngaySinh, thangSinh, namSinh;
    string diaChi;

public:
    Nguoi(const string &hoTen, const string &gioiTinh, int ngaySinh, int thangSinh, int namSinh, const string &diaChi)
        : hoTen(hoTen), gioiTinh(gioiTinh), ngaySinh(ngaySinh), thangSinh(thangSinh), namSinh(namSinh), diaChi(diaChi)
    {
        if (!isValidDate(ngaySinh, thangSinh, namSinh))
        {
            throw runtime_error("Ngay sinh khong hop le!");
        }
    }

    virtual ~Nguoi() = default;

    string getHoTen() const { return hoTen; }
    void setHoTen(const string &hoTen) { this->hoTen = hoTen; }
    void setDiaChi(const string &diaChi) { this->diaChi = diaChi; }

    int tinhTuoi() const
    {
        auto now = chrono::system_clock::now();
        time_t currentTime = chrono::system_clock::to_time_t(now);
        tm currentDate;
#ifdef _WIN32
        localtime_s(&currentDate, &currentTime);
#else
        localtime_r(&currentTime, &currentDate);
#endif
        int currentYear = currentDate.tm_year + 1900;
        int currentMonth = currentDate.tm_mon + 1;
        int currentDay = currentDate.tm_mday;

        int tuoi = currentYear - namSinh;
        if (currentMonth < thangSinh || (currentMonth == thangSinh && currentDay < ngaySinh))
        {
            tuoi--;
        }
        return tuoi;
    }

    virtual string toString() const = 0;
    int getNgaySinh() const { return ngaySinh; }
    int getThangSinh() const { return thangSinh; }
    string getDiaChi() const { return diaChi; }
};

// ============= LOP DICH VU =============
class DichVu
{
private:
    string tenDV;
    double giaDV;

public:
    DichVu(const string &ten, double gia) : tenDV(ten), giaDV(gia)
    {
        if (gia < 0)
            throw runtime_error("Gia dich vu khong the am!");
    }
    string getTen() const { return tenDV; }
    double getGia() const { return giaDV; }
    string toString() const
    {
        return tenDV + " (" + to_string(static_cast<long long>(giaDV)) + " VND)";
    }
};

// ============= LOP BAC SI =============
class BacSi : public Nguoi
{
private:
    string maBS;
    string chuyenKhoa;
    vector<weak_ptr<BenhNhan>> danhSachBN;

public:
    BacSi(const string &maBS, const string &hoTen, const string &gioiTinh,
          int ngaySinh, int thangSinh, int namSinh, const string &diaChi, const string &chuyenKhoa)
        : Nguoi(hoTen, gioiTinh, ngaySinh, thangSinh, namSinh, diaChi),
          maBS(maBS), chuyenKhoa(chuyenKhoa)
    {
        if (chuyenKhoaToBenhLy.find(chuyenKhoa) == chuyenKhoaToBenhLy.end())
        {
            throw runtime_error("Chuyen khoa khong hop le!");
        }
    }

    void themBenhNhan(shared_ptr<BenhNhan> bn);
    void xoaBenhNhan(const string &maBN);
    void xemDanhSachBN() const;

    string getMaBS() const { return maBS; }
    string getChuyenKhoa() const { return chuyenKhoa; }
    void setChuyenKhoa(const string &ck)
    {
        if (chuyenKhoaToBenhLy.find(ck) == chuyenKhoaToBenhLy.end())
        {
            throw runtime_error("Chuyen khoa khong hop le!");
        }
        chuyenKhoa = ck;
    }

    string toString() const override
    {
        return "BS{" + maBS + " | " + hoTen + " | " + to_string(tinhTuoi()) + " tuoi | " + chuyenKhoa + "}";
    }

    string toCSV() const
    {
        return "BS," + maBS + "," + hoTen + "," + gioiTinh + "," + to_string(ngaySinh) + "," + to_string(thangSinh) + "," + to_string(namSinh) + "," + diaChi + "," + chuyenKhoa;
    }
};

// ============= LOP PHONG BENH =============
class PhongBenh
{
protected:
    int soPhong;
    int soGiuong;
    int soBNDangNam;

public:
    PhongBenh(int soPhong, int soGiuong)
        : soPhong(soPhong), soGiuong(soGiuong), soBNDangNam(0)
    {
        if (soGiuong <= 0)
            throw runtime_error("So giuong phai lon hon 0!");
    }

    virtual ~PhongBenh() = default;

    bool isTrong() const { return soBNDangNam < soGiuong; }

    void themBenhNhan()
    {
        if (soBNDangNam >= soGiuong)
            throw runtime_error("Phong da day!");
        soBNDangNam++;
    }

    void xoaBenhNhan()
    {
        if (soBNDangNam > 0)
            soBNDangNam--;
    }

    int getSoPhong() const { return soPhong; }
    int getSoGiuong() const { return soGiuong; }
    int getSoBNDangNam() const { return soBNDangNam; }
    void setSoGiuong(int sg)
    {
        if (sg < soBNDangNam)
            throw runtime_error("So giuong moi khong the nho hon so BN dang nam!");
        soGiuong = sg;
    }

    virtual double tinhPhiNgayCoBan() const = 0;
    virtual string getLoaiPhong() const = 0;
    virtual string toString() const = 0;
    virtual string toCSV() const = 0;
};

// Lop Phong Thuong
class PhongThuong : public PhongBenh
{
public:
    PhongThuong(int soPhong, int soGiuong) : PhongBenh(soPhong, soGiuong) {}
    double tinhPhiNgayCoBan() const override
    {
        return CauHinhGia::getInstance().giaPhongThuong;
    }
    string getLoaiPhong() const override { return "Thuong"; }
    string toString() const override
    {
        return "Phong Thuong #" + to_string(soPhong) + " [" + to_string(soBNDangNam) + "/" + to_string(soGiuong) + " giuong]";
    }
    string toCSV() const override
    {
        return "PhongThuong," + to_string(soPhong) + "," + to_string(soGiuong);
    }
};

// Lop Phong VIP
class PhongVIP : public PhongBenh
{
private:
    vector<DichVu> dichVuThem = {{"TV", 0}, {"Tu lanh", 0}};

public:
    PhongVIP(int soPhong, int soGiuong) : PhongBenh(soPhong, soGiuong) {}
    double tinhPhiNgayCoBan() const override
    {
        return CauHinhGia::getInstance().giaPhongVIP;
    }
    string getLoaiPhong() const override { return "VIP"; }
    string toString() const override
    {
        string dv = "";
        for (const auto &d : dichVuThem)
            dv += d.getTen() + ", ";
        if (!dv.empty())
            dv = dv.substr(0, dv.size() - 2);
        return "Phong VIP #" + to_string(soPhong) + " [" + to_string(soBNDangNam) + "/" + to_string(soGiuong) + " giuong] - DV: " + dv;
    }
    string toCSV() const override
    {
        return "PhongVIP," + to_string(soPhong) + "," + to_string(soGiuong);
    }
};

// ============= LOP BENH NHAN =============
class BenhNhan : public Nguoi
{
private:
    string maBN;
    vector<string> danhSachBenhLy; // Danh sach b?nh ly
    shared_ptr<BacSi> bacSiDieuTri;
    shared_ptr<PhongBenh> soPhong;
    bool trangThaiNhapVien;
    bool coBHYT;
    vector<DichVu> dichVuSuDung;

public:
    BenhNhan(const string &maBN, const string &hoTen, const string &gioiTinh,
             int ngaySinh, int thangSinh, int namSinh, const string &diaChi,
             const vector<string> &dsBenhLy, bool coBHYT)
        : Nguoi(hoTen, gioiTinh, ngaySinh, thangSinh, namSinh, diaChi),
          maBN(maBN), danhSachBenhLy(dsBenhLy), trangThaiNhapVien(true), coBHYT(coBHYT)
    {
        // Kiem tra tat ca benh ly co hop le khong
        for (const auto &bl : dsBenhLy)
        {
            timKhoaChoBenh(bl);
        }
    }

    const vector<string> &getDanhSachBenhLy() const { return danhSachBenhLy; }

    void themBenhLy(const string &benhLy)
    {
        timKhoaChoBenh(benhLy); // Kiem tra hop le
        // Kiem tra trung
        if (find(danhSachBenhLy.begin(), danhSachBenhLy.end(), benhLy) != danhSachBenhLy.end())
        {
            throw runtime_error("Benh ly da ton tai!");
        }
        danhSachBenhLy.push_back(benhLy);
    }

    void xoaBenhLy(const string &benhLy)
    {
        auto it = find(danhSachBenhLy.begin(), danhSachBenhLy.end(), benhLy);
        if (it != danhSachBenhLy.end())
        {
            danhSachBenhLy.erase(it);
        }
        else
        {
            throw runtime_error("Benh ly khong ton tai!");
        }
    }

    void capNhatThongTin(shared_ptr<BacSi> bs, shared_ptr<PhongBenh> phong)
    {
        bacSiDieuTri = bs;
        soPhong = phong;
    }

    void raVien()
    {
        if (!trangThaiNhapVien)
            throw runtime_error("Benh nhan da ra vien!");
        trangThaiNhapVien = false;

        if (soPhong)
        {
            soPhong->xoaBenhNhan();
        }

        if (bacSiDieuTri)
        {
            bacSiDieuTri->xoaBenhNhan(maBN);
        }

        bacSiDieuTri = nullptr;
        soPhong = nullptr;
    }

    void xuatThongTin() const
    {
        cout << "\n+------------------------------------------+\n";
        cout << "|       THONG TIN BENH NHAN                |\n";
        cout << "+------------------------------------------+\n";
        cout << "| Ma BN       : " << left << setw(25) << maBN << "|\n";
        cout << "| Ho ten      : " << left << setw(25) << hoTen << "|\n";
        cout << "| Tuoi        : " << left << setw(25) << tinhTuoi() << "|\n";

        // Hien thi danh sach benh ly
        if (danhSachBenhLy.empty())
        {
            cout << "| Benh ly     : " << left << setw(25) << "Khong co" << "|\n";
        }
        else
        {
            cout << "| Benh ly     : " << left << setw(25) << "" << "|\n";
            for (size_t i = 0; i < danhSachBenhLy.size(); ++i)
            {
                string prefix = (i == 0) ? "   + " : "   + ";
                cout << "|   " << left << setw(35) << (prefix + danhSachBenhLy[i]) << "|\n";
            }
        }

        cout << "| Trang thai  : " << left << setw(25) << (trangThaiNhapVien ? "Dang nhap vien" : "Da ra vien") << "|\n";
        cout << "|  BHYT     : " << left << setw(25) << (coBHYT ? "Co" : "Khong") << "|\n";
        if (bacSiDieuTri)
        {
            string bsInfo = bacSiDieuTri->getHoTen() + " (" + bacSiDieuTri->getMaBS() + ")";
            cout << "| Bac si      : " << left << setw(25) << bsInfo << "|\n";
        }
        if (soPhong)
        {
            string phongInfo = to_string(soPhong->getSoPhong()) + " (" + soPhong->getLoaiPhong() + ")";
            cout << "| Phong       : " << left << setw(25) << phongInfo << "|\n";
        }
        cout << "| Dich vu     : ";
        if (dichVuSuDung.empty())
            cout << left << setw(25) << "Khong co" << "|\n";
        else
        {
            string dv = "";
            for (const auto &d : dichVuSuDung)
                dv += d.getTen() + ", ";
            if (!dv.empty())
                dv = dv.substr(0, dv.size() - 2);
            cout << left << setw(25) << dv << "|\n";
        }
        cout << "+------------------------------------------+\n";
    }

    void themDichVu(const DichVu &dv) { dichVuSuDung.push_back(dv); }

    string getMaBN() const { return maBN; }
    shared_ptr<BacSi> getBacSiDieuTri() const { return bacSiDieuTri; }
    shared_ptr<PhongBenh> getSoPhongObj() const { return soPhong; }
    bool getCoBHYT() const { return coBHYT; }
    bool getTrangThaiNhapVien() const { return trangThaiNhapVien; }

    string toString() const override
    {
        string bl = "";
        for (size_t i = 0; i < danhSachBenhLy.size(); ++i)
        {
            bl += danhSachBenhLy[i];
            if (i < danhSachBenhLy.size() - 1)
                bl += ", ";
        }
        if (bl.empty())
            bl = "Khong co";
        return "BN{" + maBN + " | " + hoTen + " | " + to_string(tinhTuoi()) + " tuoi | " + bl + " | " + (coBHYT ? "BHYT" : "Thuong") + "}";
    }

    string toCSV() const
    {
        string bl = "";
        for (size_t i = 0; i < danhSachBenhLy.size(); ++i)
        {
            bl += danhSachBenhLy[i];
            if (i < danhSachBenhLy.size() - 1)
                bl += ";";
        }
        return "BN," + maBN + "," + hoTen + "," + gioiTinh + "," +
               to_string(ngaySinh) + "," + to_string(thangSinh) + "," +
               to_string(namSinh) + "," + diaChi + "," + bl + "," +
               (coBHYT ? "1" : "0");
    }

    double tinhTongPhiDichVu() const
    {
        double tong = 0;
        for (const auto &dv : dichVuSuDung)
            tong += dv.getGia();
        return tong;
    }
};

// ============= DINH NGHIA PHUONG THUC BAC SI =============
void BacSi::themBenhNhan(shared_ptr<BenhNhan> bn)
{
    danhSachBN.push_back(weak_ptr<BenhNhan>(bn));
}

void BacSi::xoaBenhNhan(const string &maBN)
{
    danhSachBN.erase(remove_if(danhSachBN.begin(), danhSachBN.end(),
                               [&maBN](const weak_ptr<BenhNhan> &bn)
                               {
                                   if (auto ptr = bn.lock())
                                   {
                                       return ptr->getMaBN() == maBN;
                                   }
                                   return true;
                               }),
                     danhSachBN.end());
}

void BacSi::xemDanhSachBN() const
{
    cout << "\n=== DANH SACH BENH NHAN CUA BS " << hoTen << " ===\n";
    int count = 0;
    for (const auto &weakBn : danhSachBN)
    {
        if (auto bn = weakBn.lock())
        {
            cout << ++count << ". " << bn->toString() << endl;
        }
    }
    if (count == 0)
        cout << "Khong co benh nhan nao.\n";
}

// ============= LOP HOA DON =============
class HoaDon
{
private:
    string maBN;
    string tenBN;
    double phiPhong;
    double phiDichVu;
    double tongTien;
    double giamGia;
    int soNgay;
    string phuongThucThanhToan;

public:
    HoaDon(const shared_ptr<BenhNhan> &bn, int soNgay) : maBN(bn->getMaBN()), tenBN(bn->getHoTen()), soNgay(soNgay), giamGia(0)
    {
        if (soNgay <= 0)
            throw runtime_error("So ngay nam vien phai lon hon 0!");
        phiPhong = bn->getSoPhongObj() ? bn->getSoPhongObj()->tinhPhiNgayCoBan() * soNgay : 0;
        phiDichVu = bn->tinhTongPhiDichVu();
        tongTien = phiPhong + phiDichVu;

        if (bn->getCoBHYT())
        {
            giamGia = tongTien * CauHinhGia::getInstance().giamGiaBHYT;
            tongTien -= giamGia;
        }
    }

    void setPhuongThucThanhToan(const string &pt)
    {
        if (pt != "Tien mat" && pt != "Chuyen khoan")
        {
            throw runtime_error("Phuong thuc thanh toan khong hop le!");
        }
        phuongThucThanhToan = pt;
    }

    void xuatHoaDon() const
    {
        cout << "\n+==========================================+\n";
        cout << "|         HOA DON THANH TOAN               |\n";
        cout << "+==========================================+\n";
        cout << "| Ma BN       : " << left << setw(25) << maBN << "|\n";
        cout << "| Ho ten      : " << left << setw(25) << tenBN << "|\n";
        cout << "| So ngay     : " << left << setw(25) << (to_string(soNgay) + " ngay") << "|\n";
        cout << "+------------------------------------------+\n";
        cout << "| Phi phong   : " << left << setw(25) << (to_string(static_cast<long long>(phiPhong)) + " VND") << "|\n";
        cout << "| Phi dich vu : " << left << setw(25) << (to_string(static_cast<long long>(phiDichVu)) + " VND") << "|\n";
        if (giamGia > 0)
        {
            cout << "| Giam gia    : " << left << setw(25) << ("-" + to_string(static_cast<long long>(giamGia)) + " VND") << "|\n";
        }
        cout << "+==========================================+\n";
        cout << "| TONG CONG   : " << left << setw(25) << (to_string(static_cast<long long>(tongTien)) + " VND") << "|\n";
        cout << "| Thanh toan  : " << left << setw(25) << (phuongThucThanhToan.empty() ? "Chua chon" : phuongThucThanhToan) << "|\n";

        if (phuongThucThanhToan == "Chuyen khoan")
        {
            string maQR = "QR_" + maBN + "_" + to_string(static_cast<long long>(tongTien));
            cout << "| Ma QR       : " << left << setw(25) << maQR << "|\n";
            cout << "| (Quet ma QR de thanh toan)               |\n";
        }

        cout << "+==========================================+\n";
    }
};

// ============= LOP QUAN LY BENH VIEN (SINGLETON CAI TIEN) =============
class QuanLyBenhVien
{
private:
    static unique_ptr<QuanLyBenhVien> instance;
    static once_flag initFlag;

    unordered_map<string, shared_ptr<BenhNhan>> dsBN;
    unordered_map<string, shared_ptr<BacSi>> dsBS;
    unordered_map<int, shared_ptr<PhongBenh>> dsPhong;

    QuanLyBenhVien()
    {
        string filename = getProjectPath() + "dulieu.csv";
        docDuLieuTuFile(filename);
    }

    vector<string> split(const string &s, char delimiter)
    {
        vector<string> tokens;
        string token;
        istringstream tokenStream(s);
        bool inQuotes = false;
        char c;
        while (tokenStream.get(c))
        {
            if (c == '"')
            {
                inQuotes = !inQuotes;
            }
            else if (c == delimiter && !inQuotes)
            {
                tokens.push_back(token);
                token.clear();
            }
            else
            {
                token += c;
            }
        }
        if (!token.empty())
            tokens.push_back(token);
        return tokens;
    }

public:
    static QuanLyBenhVien &getInstance()
    {
        call_once(initFlag, []()
                  { instance.reset(new QuanLyBenhVien()); });
        return *instance;
    }

    ~QuanLyBenhVien() = default;
    QuanLyBenhVien(const QuanLyBenhVien &) = delete;
    QuanLyBenhVien &operator=(const QuanLyBenhVien &) = delete;

    void themBenhNhan(shared_ptr<BenhNhan> bn)
    {
        if (dsBN.find(bn->getMaBN()) != dsBN.end())
        {
            throw runtime_error("Ma benh nhan da ton tai!");
        }
        dsBN[bn->getMaBN()] = bn;
    }

    void themBacSi(shared_ptr<BacSi> bs)
    {
        if (dsBS.find(bs->getMaBS()) != dsBS.end())
        {
            throw runtime_error("Ma bac si da ton tai!");
        }
        dsBS[bs->getMaBS()] = bs;
    }

    void themPhong(shared_ptr<PhongBenh> phong)
    {
        if (dsPhong.find(phong->getSoPhong()) != dsPhong.end())
        {
            throw runtime_error("So phong da ton tai!");
        }
        dsPhong[phong->getSoPhong()] = phong;
    }

    shared_ptr<BenhNhan> timBenhNhan(const string &maBN) const
    {
        auto it = dsBN.find(maBN);
        return (it != dsBN.end()) ? it->second : nullptr;
    }

    shared_ptr<BacSi> timBacSi(const string &maBS) const
    {
        auto it = dsBS.find(maBS);
        return (it != dsBS.end()) ? it->second : nullptr;
    }

    shared_ptr<PhongBenh> timPhong(int soPhong) const
    {
        auto it = dsPhong.find(soPhong);
        return (it != dsPhong.end()) ? it->second : nullptr;
    }

    // HAM: Quan ly benh ly cua benh nhan
    void quanLyBenhLyBenhNhan(const string &maBN)
    {
        auto bn = timBenhNhan(maBN);
        if (!bn)
            throw runtime_error("Khong tim thay benh nhan!");

        int choice;
        do
        {
            cout << "\n=== QUAN LY BENH LY CUA BENH NHAN " << bn->getHoTen() << " ===\n";

            // Hien thi danh sach benh ly
            const auto &dsBenhLy = bn->getDanhSachBenhLy();
            cout << "Benh ly hien tai:\n";
            if (dsBenhLy.empty())
            {
                cout << "  (Chua co benh ly nao)\n";
            }
            else
            {
                for (size_t i = 0; i < dsBenhLy.size(); ++i)
                {
                    cout << "  " << (i + 1) << ". " << dsBenhLy[i] << endl;
                }
            }

            cout << "\n1. Them benh ly moi\n";
            cout << "2. Xoa benh ly\n";
            cout << "0. Quay lai\n";

            choice = nhapSoNguyen("Chon: ", 0, 2);

            try
            {
                switch (choice)
                {
                case 1:
                {
                    cout << "\n--- DANH SACH BENH LY HOP LE ---\n";
                    int count = 1;
                    for (const auto &pair : chuyenKhoaToBenhLy)
                    {
                        const string &khoa = pair.first;
                        const vector<string> &dsBenh = pair.second;
                        cout << khoa << ":\n";
                        for (const auto &benh : dsBenh)
                        {
                            cout << "  " << count++ << ". " << benh << endl;
                        }
                    }

                    string benhLyMoi = nhapChuoiKhongRong("\nNhap benh ly can them: ");
                    bn->themBenhLy(benhLyMoi);
                    cout << "[+] Them benh ly thanh cong!\n";
                    break;
                }
                case 2:
                {
                    if (dsBenhLy.empty())
                    {
                        cout << "[!] Khong co benh ly nao de xoa!\n";
                        break;
                    }

                    string benhLyXoa = nhapChuoiKhongRong("Nhap ten benh ly can xoa: ");
                    bn->xoaBenhLy(benhLyXoa);
                    cout << "[+] Xoa benh ly thanh cong!\n";
                    break;
                }
                case 0:
                    break;
                }
            }
            catch (const exception &e)
            {
                cout << "[-] Loi: " << e.what() << endl;
            }
        } while (choice != 0);
    }

    void suaThongTinBN(const string &maBN)
    {
        auto bn = timBenhNhan(maBN);
        if (!bn)
            throw runtime_error("Khong tim thay benh nhan!");

        cout << "\n=== SUA THONG TIN BENH NHAN ===\n";
        cout << "1. Sua ho ten\n";
        cout << "2. Sua dia chi\n";
        cout << "3. Quan ly benh ly\n";
        cout << "0. Huy\n";

        int choice = nhapSoNguyen("Chon: ", 0, 3);

        switch (choice)
        {
        case 1:
        {
            string hoTen = nhapChuoiKhongRong("Ho ten moi: ");
            bn->setHoTen(hoTen);
            cout << "[+] Cap nhat thanh cong!\n";
            break;
        }
        case 2:
        {
            string diaChi = nhapChuoiKhongRong("Dia chi moi: ");
            bn->setDiaChi(diaChi);
            cout << "[+] Cap nhat thanh cong!\n";
            break;
        }
        case 3:
        {
            quanLyBenhLyBenhNhan(maBN);
            break;
        }
        case 0:
            cout << "Huy thao tac.\n";
            break;
        }
    }

    void suaThongTinBS(const string &maBS)
    {
        auto bs = timBacSi(maBS);
        if (!bs)
            throw runtime_error("Khong tim thay bac si!");

        cout << "\n=== SUA THONG TIN BAC SI ===\n";
        cout << "1. Sua ho ten\n";
        cout << "2. Sua dia chi\n";
        cout << "3. Sua chuyen khoa\n";
        cout << "0. Huy\n";

        int choice = nhapSoNguyen("Chon: ", 0, 3);

        switch (choice)
        {
        case 1:
        {
            string hoTen = nhapChuoiKhongRong("Ho ten moi: ");
            bs->setHoTen(hoTen);
            cout << "[+] Cap nhat thanh cong!\n";
            break;
        }
        case 2:
        {
            string diaChi = nhapChuoiKhongRong("Dia chi moi: ");
            bs->setDiaChi(diaChi);
            cout << "[+] Cap nhat thanh cong!\n";
            break;
        }
        case 3:
        {
            cout << "Chuyen khoa hop le: Tim mach, Da lieu, Xuong khop, Noi khoa\n";
            string chuyenKhoa = nhapChuoiKhongRong("Chuyen khoa moi: ");
            bs->setChuyenKhoa(chuyenKhoa);
            cout << "[+] Cap nhat thanh cong!\n";
            break;
        }
        case 0:
            cout << "Huy thao tac.\n";
            break;
        }
    }

    void suaThongTinPhong(int soPhong)
    {
        auto phong = timPhong(soPhong);
        if (!phong)
            throw runtime_error("Khong tim thay phong!");

        cout << "\n=== SUA THONG TIN PHONG ===\n";
        cout << "Hien tai: " << phong->toString() << endl;

        int soGiuongMoi = nhapSoNguyen("So giuong moi (hien tai: " + to_string(phong->getSoGiuong()) + "): ", phong->getSoBNDangNam(), 100);
        phong->setSoGiuong(soGiuongMoi);
        cout << "[+] Cap nhat thanh cong!\n";
    }

    void phanCong(shared_ptr<BenhNhan> bn, shared_ptr<BacSi> bs, shared_ptr<PhongBenh> phong)
    {
        // Kiem tra xem bac si co phu hop voi it nhat 1 benh ly khong
        const auto &dsBenhLy = bn->getDanhSachBenhLy();
        if (dsBenhLy.empty())
        {
            throw runtime_error("Benh nhan chua co benh ly nao!");
        }

        bool phuHop = false;
        for (const auto &benhLy : dsBenhLy)
        {
            string khoaCanThiet = timKhoaChoBenh(benhLy);
            if (khoaCanThiet == bs->getChuyenKhoa())
            {
                phuHop = true;
                break;
            }
        }

        if (!phuHop)
        {
            throw runtime_error("Bac si khong phu hop voi bat ky benh ly nao cua benh nhan!");
        }

        if (!phong->isTrong())
        {
            throw runtime_error("Phong da day!");
        }

        auto phongCu = bn->getSoPhongObj();
        if (phongCu && phongCu != phong)
        {
            phongCu->xoaBenhNhan();
        }

        auto bsCu = bn->getBacSiDieuTri();
        if (bsCu && bsCu != bs)
        {
            bsCu->xoaBenhNhan(bn->getMaBN());
        }

        bn->capNhatThongTin(bs, phong);
        bs->themBenhNhan(bn);

        if (!phongCu || phongCu != phong)
        {
            phong->themBenhNhan();
        }
    }

    void xoaBenhNhan(const string &maBN)
    {
        auto it = dsBN.find(maBN);
        if (it == dsBN.end())
        {
            throw runtime_error("Khong tim thay benh nhan de xoa!");
        }
        if (it->second->getTrangThaiNhapVien())
        {
            throw runtime_error("Benh nhan chua ra vien!");
        }
        dsBN.erase(it);
    }

    void xuatDanhSachBNTheoKhoa(const string &chuyenKhoa) const
    {
        cout << "\n=== DANH SACH BENH NHAN THEO KHOA " << chuyenKhoa << " ===\n";
        int count = 0;
        for (const auto &pair : dsBN)
        {
            const auto &bn = pair.second;
            if (bn->getBacSiDieuTri() && bn->getBacSiDieuTri()->getChuyenKhoa() == chuyenKhoa)
            {
                cout << ++count << ". " << bn->toString() << endl;
            }
        }
        if (count == 0)
            cout << "Khong tim thay benh nhan nao trong khoa nay.\n";
    }

    void thongKeBNTheoTuoi() const
    {
        int treEm = 0, nguoiLon = 0, caoTuoi = 0;
        for (const auto &pair : dsBN)
        {
            int tuoi = pair.second->tinhTuoi();
            if (tuoi < 15)
                treEm++;
            else if (tuoi <= 60)
                nguoiLon++;
            else
                caoTuoi++;
        }
        cout << "\n+===========================================+\n";
        cout << "|    THONG KE BENH NHAN THEO DO TUOI      |\n";
        cout << "+===========================================+\n";
        cout << "| Tre em (< 15 tuoi)   : " << left << setw(16) << treEm << "|\n";
        cout << "| Nguoi lon (15-60)    : " << left << setw(16) << nguoiLon << "|\n";
        cout << "| Nguoi cao tuoi (>60) : " << left << setw(16) << caoTuoi << "|\n";
        cout << "+===========================================+\n";
    }

    void timBacSiTheoBenhLy(const string &benhLy) const
    {
        string chuyenKhoa;
        try
        {
            chuyenKhoa = timKhoaChoBenh(benhLy);
        }
        catch (const exception &)
        {
            cout << "[!] Benh ly khong hop le!\n";
            return;
        }

        cout << "\n=== BAC SI PHU HOP VOI BENH LY '" << benhLy << "' ===\n";
        int count = 0;
        for (const auto &pair : dsBS)
        {
            if (pair.second->getChuyenKhoa() == chuyenKhoa)
            {
                cout << ++count << ". " << pair.second->toString() << endl;
            }
        }
        if (count == 0)
            cout << "Khong tim thay bac si phu hop!\n";
    }

    void timPhongTrongTheoLoai(const string &loai) const
    {
        cout << "\n=== PHONG " << loai << " CON TRONG ===\n";
        int count = 0;
        for (const auto &pair : dsPhong)
        {
            const auto &phong = pair.second;
            if (phong->getLoaiPhong() == loai && phong->isTrong())
            {
                cout << ++count << ". " << phong->toString() << " - Con "
                     << (phong->getSoGiuong() - phong->getSoBNDangNam()) << " giuong\n";
            }
        }
        if (count == 0)
            cout << "Khong co phong " << loai << " nao con trong.\n";
    }

    void luuVaoFile(const string &filename) const
    {
        string projectPath = getProjectPath();
        string localPath = projectPath + filename;

        // Ghi file trong project
        ofstream outFile(localPath);
        if (!outFile.is_open())
            throw runtime_error("Loi ghi file: " + localPath);
        outFile << "# Du lieu benh vien\n";
        for (const auto &p : dsBS)
            outFile << p.second->toCSV() << '\n';
        for (const auto &p : dsPhong)
            outFile << p.second->toCSV() << '\n';
        for (const auto &p : dsBN)
            outFile << p.second->toCSV() << '\n';
        outFile.close();

        // Sao chép lên Desktop
        string desktop = string(getenv("USERPROFILE")) + "\\Desktop\\";
        auto copyToDesktop = [&](const string &src, const string &name)
        {
            ifstream in(src);
            if (in.good())
            {
                ofstream out(desktop + name);
                out << in.rdbuf();
                in.close();
                out.close();
                cout << "[+] Da sao chep " << name << " len Desktop!\n";
            }
            else
            {
                cout << "[!] Chua co " << name << "\n";
            }
        };

        copyToDesktop(localPath, filename);
        copyToDesktop(projectPath + "dudoan.csv", "dudoan.csv");

        cout << "[+] DA LUU THANH CONG VAO DESKTOP!\n";
    }

    // 3. docDuLieuTuFile
    void docDuLieuTuFile(const string &filename)
    {
        ifstream inFile(filename);

        if (!inFile)
        {
            cout << "[!] KHONG TIM THAY FILE: " << filename << "\n";
            cout << "    -> Bat dau voi du lieu trong.\n";
            cout << "    -> File se duoc tao khi luu lan dau.\n";
            return;
        }

        string line;
        while (getline(inFile, line))
        {
            try
            {
                vector<string> data = split(line, ',');
                if (data.empty() || data[0].empty() || data[0][0] == '#')
                    continue;

                if (data[0] == "BS" && data.size() >= 9)
                {
                    auto bs = make_shared<BacSi>(data[1], data[2], data[3],
                                                 safeStoi(data[4], "ngaySinh"), safeStoi(data[5], "thangSinh"),
                                                 safeStoi(data[6], "namSinh"), data[7], data[8]);
                    if (dsBS.find(bs->getMaBS()) == dsBS.end())
                    {
                        dsBS[bs->getMaBS()] = bs;
                    }
                }
                else if (data[0] == "PhongThuong" && data.size() >= 3)
                {
                    auto phong = make_shared<PhongThuong>(safeStoi(data[1], "soPhong"),
                                                          safeStoi(data[2], "soGiuong"));
                    if (dsPhong.find(phong->getSoPhong()) == dsPhong.end())
                    {
                        dsPhong[phong->getSoPhong()] = phong;
                    }
                }
                else if (data[0] == "PhongVIP" && data.size() >= 3)
                {
                    auto phong = make_shared<PhongVIP>(safeStoi(data[1], "soPhong"),
                                                       safeStoi(data[2], "soGiuong"));
                    if (dsPhong.find(phong->getSoPhong()) == dsPhong.end())
                    {
                        dsPhong[phong->getSoPhong()] = phong;
                    }
                }
                else if (data[0] == "BN" && data.size() >= 10)
                {
                    vector<string> dsBenhLy = split(data[8], ';');
                    auto bn = make_shared<BenhNhan>(data[1], data[2], data[3],
                                                    safeStoi(data[4], "ngaySinh"), safeStoi(data[5], "thangSinh"),
                                                    safeStoi(data[6], "namSinh"), data[7], dsBenhLy, data[9] == "1");
                    if (dsBN.find(bn->getMaBN()) == dsBN.end())
                    {
                        dsBN[bn->getMaBN()] = bn;
                    }
                }
            }
            catch (const exception &e)
            {
                cerr << "[!] Loi doc dong: " << line << " - " << e.what() << endl;
            }
        }
        inFile.close();
        cout << "[+] TAI DU LIEU THANH CONG TU: " << filename << "\n";
    }

    void kiemTraSinhNhatHomNay() const
    {
        auto now = chrono::system_clock::now();
        time_t currentTime = chrono::system_clock::to_time_t(now);
        tm currentDate;
#ifdef _WIN32
        localtime_s(&currentDate, &currentTime);
#else
        localtime_r(&currentTime, &currentDate);
#endif
        int currentMonth = currentDate.tm_mon + 1;
        int currentDay = currentDate.tm_mday;

        cout << "\n=== SINH NHAT HOM NAY (" << currentDay << "/" << currentMonth << ") ===\n";
        bool hasBirthday = false;

        for (const auto &pair : dsBN)
        {
            const auto &bn = pair.second;
            if (bn->getNgaySinh() == currentDay && bn->getThangSinh() == currentMonth)
            {
                cout << "[*] Benh nhan " << bn->getHoTen() << " (" << bn->getMaBN() << ") - " << bn->tinhTuoi() << " tuoi!\n";
                hasBirthday = true;
            }
        }

        for (const auto &pair : dsBS)
        {
            const auto &bs = pair.second;
            if (bs->getNgaySinh() == currentDay && bs->getThangSinh() == currentMonth)
            {
                cout << "[*] Bac si " << bs->getHoTen() << " (" << bs->getMaBS() << ") - " << bs->tinhTuoi() << " tuoi!\n";
                hasBirthday = true;
            }
        }

        if (!hasBirthday)
        {
            cout << "Khong co ai sinh nhat hom nay.\n";
        }
    }

    void sapXepBNTheoTuoi(vector<shared_ptr<BenhNhan>> &list)
    {
        sort(list.begin(), list.end(), [](const shared_ptr<BenhNhan> &a, const shared_ptr<BenhNhan> &b)
             { return a->tinhTuoi() < b->tinhTuoi(); });
    }

    void xuatDanhSachBNDaSapXep()
    {
        vector<shared_ptr<BenhNhan>> list;
        for (const auto &pair : dsBN)
        {
            list.push_back(pair.second);
        }
        sapXepBNTheoTuoi(list);

        cout << "\n=== DANH SACH BENH NHAN (Sap xep theo tuoi tang dan) ===\n";
        int count = 0;
        for (const auto &bn : list)
        {
            cout << ++count << ". " << bn->toString() << endl;
        }
        if (count == 0)
            cout << "Khong co benh nhan nao.\n";
    }

    void xuatDanhSachBS() const
    {
        cout << "\n=== DANH SACH BAC SI ===\n";
        int count = 0;
        for (const auto &pair : dsBS)
        {
            cout << ++count << ". " << pair.second->toString() << endl;
        }
        if (count == 0)
            cout << "Khong co bac si nao.\n";
    }

    void xuatDanhSachPhong() const
    {
        cout << "\n=== DANH SACH PHONG ===\n";
        int count = 0;
        for (const auto &pair : dsPhong)
        {
            const auto &phong = pair.second;
            cout << ++count << ". " << phong->toString() << endl;
        }
        if (count == 0)
            cout << "Khong co phong nao.\n";
    }

    void timKiemNangCao() const
    {
        cout << "\n=== TIM KIEM NANG CAO ===\n";
        cout << "1. Tim benh nhan theo ten\n";
        cout << "2. Tim benh nhan theo do tuoi\n";
        cout << "3. Tim bac si theo ten\n";
        cout << "0. Huy\n";

        int choice = nhapSoNguyen("Chon: ", 0, 3);

        switch (choice)
        {
        case 1:
        {
            string keyword = nhapChuoiKhongRong("Nhap ten can tim: ");
            transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);

            cout << "\n--- KET QUA TIM KIEM ---\n";
            int count = 0;
            for (const auto &pair : dsBN)
            {
                string tenBN = pair.second->getHoTen();
                transform(tenBN.begin(), tenBN.end(), tenBN.begin(), ::tolower);
                if (tenBN.find(keyword) != string::npos)
                {
                    cout << ++count << ". " << pair.second->toString() << endl;
                }
            }
            if (count == 0)
                cout << "Khong tim thay!\n";
            break;
        }
        case 2:
        {
            int tuoiMin = nhapSoNguyen("Tuoi tu: ", 0, 150);
            int tuoiMax = nhapSoNguyen("Den: ", tuoiMin, 150);

            cout << "\n--- KET QUA TIM KIEM ---\n";
            int count = 0;
            for (const auto &pair : dsBN)
            {
                int tuoi = pair.second->tinhTuoi();
                if (tuoi >= tuoiMin && tuoi <= tuoiMax)
                {
                    cout << ++count << ". " << pair.second->toString() << endl;
                }
            }
            if (count == 0)
                cout << "Khong tim thay!\n";
            break;
        }
        case 3:
        {
            string keyword = nhapChuoiKhongRong("Nhap ten bac si can tim: ");
            transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);

            cout << "\n--- KET QUA TIM KIEM ---\n";
            int count = 0;
            for (const auto &pair : dsBS)
            {
                string tenBS = pair.second->getHoTen();
                transform(tenBS.begin(), tenBS.end(), tenBS.begin(), ::tolower);
                if (tenBS.find(keyword) != string::npos)
                {
                    cout << ++count << ". " << pair.second->toString() << endl;
                }
            }
            if (count == 0)
                cout << "Khong tim thay!\n";
            break;
        }
        case 0:
            cout << "Huy thao tac.\n";
            break;
        }
    }

    void baoCaoDoanhThu() const
    {
        cout << "\n+===========================================+\n";
        cout << "|         BAO CAO THONG KE                 |\n";
        cout << "+===========================================+\n";
        cout << "| Tong so benh nhan : " << left << setw(20) << dsBN.size() << "|\n";
        cout << "| Tong so bac si    : " << left << setw(20) << dsBS.size() << "|\n";
        cout << "| Tong so phong     : " << left << setw(20) << dsPhong.size() << "|\n";

        int phongTrong = 0, tongGiuong = 0, giuongDaDung = 0;
        for (const auto &pair : dsPhong)
        {
            if (pair.second->isTrong())
                phongTrong++;
            tongGiuong += pair.second->getSoGiuong();
            giuongDaDung += pair.second->getSoBNDangNam();
        }

        cout << "+-------------------------------------------+\n";
        cout << "| Phong con trong   : " << left << setw(20) << phongTrong << "|\n";
        cout << "| Tong so giuong    : " << left << setw(20) << tongGiuong << "|\n";
        cout << "| Giuong da dung    : " << left << setw(20) << giuongDaDung << "|\n";

        double tyLeLapDay = tongGiuong > 0 ? (giuongDaDung * 100.0 / tongGiuong) : 0;
        cout << "| Ty le lap day     : " << left << setw(20) << (to_string(static_cast<int>(tyLeLapDay)) + "%") << "|\n";
        cout << "+===========================================+\n";
    }

    void xuatDanhSachBN() const
    {
        cout << "\n=== DANH SACH BENH NHAN ===\n";
        int count = 0;
        for (const auto &pair : dsBN)
        {
            cout << ++count << ". " << pair.second->toString() << endl;
        }
        if (count == 0)
            cout << "Khong co benh nhan nao.\n";
    }

    void chatbotBenhVien()
    {
        cout << "\n";
        cout << "+===========================================+\n";
        cout << "|        CHATBOT HO TRO BENH VIEN          |\n";
        cout << "+===========================================+\n";
        cout << "| Xin chao! Toi co the giup gi cho ban?   |\n";
        cout << "| (Ghi 'thoat' de ket thuc tro chuyen)    |\n";
        cout << "+===========================================+\n";
        cout << "\nVi du cau hoi:\n";
        cout << "  - Gio mo cua, lich lam viec?\n";
        cout << "  - Chi phi phong benh (thuong, VIP, don)?\n";
        cout << "  - Chi phi xet nghiem (mau, nuoc tieu, gan, duong huyet)?\n";
        cout << "  - Chi phi X-quang, CT scan, MRI?\n";
        cout << "  - Chi phi sieu am?\n";
        cout << "  - Quy trinh nhap vien nhu the nao?\n";
        cout << "  - Giay to can thiet khi nhap vien?\n";
        cout << "  - Dia chi & lien he benh vien?\n";
        cout << "  - Hotline cac bo phan?\n";
        cout << "  - Khoa Tim mach (chi tiet)?\n";
        cout << "  - Khoa Da lieu (chi tiet)?\n";
        cout << "  - Khoa Noi khoa (chi tiet)?\n";
        cout << "  - Khoa Xuong khop (chi tiet)?\n";
        cout << "  - Bao hiem y te (BHYT)?\n";
        cout << "  - Dat lich kham?\n";
        cout << "  - Gio tham benh?\n";
        cout << "  - Chi phi phau thuat?\n";
        cout << "  - Cap cuu 24/7?\n";
        cout << "  - Dich truyen, tiem thuoc?\n";
        cout << "  - Kham thai san?\n";
        cout << "  - Kham tre em - Nhi khoa?\n";
        cout << "  - Tiem chung, vaccine?\n";
        cout << "  - Kham tong quat?\n";
        cout << "  - Nha thuoc?\n";
        cout << "  - Wifi mien phi?\n";
        cout << "  - Can tin, an uong?\n";
        cout << "  - ATM, rut tien?\n";
        cout << "  - Bai xe, gui xe?\n";
        cout << "  - Thanh toan?\n";
        cout << "  - Khieu nai, phan anh?\n";
        cout << "  - Xin giay nghi om?\n";
        cout << "  - Phong rieng, phong don?\n";
        cout << "  - Nguoi nha benh nhan?\n";
        cout << "  - Trieu chung benh (dau dau, dau bung, ho)?\n";
        cout << "  - Dich vu xe dua don?\n";
        cout << "  - Danh gia dich vu?\n";
        cout << "  - Gia thuoc?\n";
        cout << "  - Bac si gioi, bac si noi tieng?\n";
        cout << "  - Tong hop chi phi?\n";
        cout << "  - Tai kham?\n";
        cout << "  - Tra cuu ket qua xet nghiem?\n";
        cout << "  - Chuyen vien?\n";
        cout << "  - Gioi thieu benh vien?\n";

        while (true)
        {
            string question = nhapChuoiKhongRong("[Ban]: ");

            if (toLower(question) == "thoat" || toLower(question) == "exit")
            {
                cout << "[Bot]: Hen gap lai! Chuc ban suc khoe!\n";
                break;
            }

            cout << "\n[Bot dang suy nghi...]\n";

            string response = callChatbotAI(question);

            if (response == "EXIT")
            {
                break;
            }

            cout << "\n[Bot]: " << response << "\n\n";
            cout << "-------------------------------------------\n";
        }
    }
};

unique_ptr<QuanLyBenhVien> QuanLyBenhVien::instance = nullptr;
once_flag QuanLyBenhVien::initFlag;

// ============= MENU CHINH =============
void menu()
{
    QuanLyBenhVien &qlbv = QuanLyBenhVien::getInstance();
    vector<DichVu> dsDichVuCoTheThem = {
        {"Xet nghiem mau", 150000.0},
        {"Chup X-quang", 300000.0},
        {"Sieu am", 200000.0},
        {"Dich truyen", 500000.0},
        {"Xet nghiem nieu", 100000.0}};

    int ch;
    do
    {
        cout << "\n\n";
        cout << "+===========================================+\n";
        cout << "|    HE THONG QUAN LY BENH VIEN CDI         |\n";
        cout << "+===========================================+\n";
        cout << "\n--- QUAN LY BENH NHAN ---\n";
        cout << " 1. Them benh nhan moi\n";
        cout << " 2. Xem thong tin benh nhan\n";
        cout << " 3. Sua thong tin benh nhan\n";
        cout << " 4. Xoa benh nhan (da ra vien)\n";
        cout << " 5. Xuat danh sach benh nhan\n";
        cout << "\n--- QUAN LY BAC SI ---\n";
        cout << " 6. Them bac si moi\n";
        cout << " 7. Sua thong tin bac si\n";
        cout << " 8. Xuat danh sach bac si\n";
        cout << "\n--- QUAN LY PHONG ---\n";
        cout << " 9. Them phong moi\n";
        cout << "10. Sua thong tin phong\n";
        cout << "11. Xuat danh sach phong\n";
        cout << "\n--- DIEU TRI & DICH VU ---\n";
        cout << "12. Phan cong BS/Phong cho BN\n";
        cout << "13. Them dich vu cho benh nhan\n";
        cout << "14. Ra vien va xuat hoa don\n";
        cout << "\n--- TIM KIEM & THONG KE ---\n";
        cout << "15. Tim kiem nang cao\n";
        cout << "16. Xuat DS benh nhan theo khoa\n";
        cout << "17. Tim bac si theo benh ly\n";
        cout << "18. Tim phong trong theo loai\n";
        cout << "19. Sap xep benh nhan theo tuoi\n";
        cout << "20. Thong ke benh nhan theo tuoi\n";
        cout << "21. Bao cao tong hop\n";
        cout << "\n--- KHAC ---\n";
        cout << "22. Kiem tra sinh nhat hom nay\n";
        cout << "23. Luu du lieu vao file\n";
        cout << "24. Chatbot ho tro 24/7\n";
        cout << " 0. Thoat\n";
        cout << "+===========================================+\n";

        ch = nhapSoNguyen("=> Chon chuc nang: ", 0, 24);

        try
        {
            switch (ch)
            {
            case 1:
            {
                cout << "\n=== THEM BENH NHAN MOI ===\n";
                string maBN = nhapChuoiKhongRong("Ma BN: ");
                string hoTen = nhapChuoiKhongRong("Ho ten: ");
                string gt = nhapGioiTinh();

                int ngay = nhapSoNguyen("Ngay sinh: ", 1, 31);
                int thang = nhapSoNguyen("Thang sinh: ", 1, 12);
                int nam = nhapSoNguyen("Nam sinh: ", 1900, 2025);

                if (!isValidDate(ngay, thang, nam))
                {
                    throw runtime_error("Ngay sinh khong hop le!");
                }

                string dc = nhapChuoiKhongRong("Dia chi: ");

                // Nhap nhieu benh ly
                cout << "\n--- DANH SACH BENH LY HOP LE ---\n";
                int count = 1;
                for (const auto &pair : chuyenKhoaToBenhLy)
                {
                    const string &khoa = pair.first;
                    const vector<string> &dsBenh = pair.second;
                    cout << khoa << ":\n";
                    for (const auto &benh : dsBenh)
                    {
                        cout << "  " << count++ << ". " << benh << endl;
                    }
                }

                vector<string> dsBenhLy;
                int soLuongBenh = nhapSoNguyen("\nSo luong benh ly can them (1-10): ", 1, 10);

                for (int i = 0; i < soLuongBenh; i++)
                {
                    string benh = nhapChuoiKhongRong("Benh ly thu " + to_string(i + 1) + ": ");
                    // Kiem tra hop le
                    try
                    {
                        timKhoaChoBenh(benh);
                        // Kiem tra trung
                        if (find(dsBenhLy.begin(), dsBenhLy.end(), benh) != dsBenhLy.end())
                        {
                            cout << "[!] Benh ly da duoc them! Bo qua.\n";
                            i--;
                            continue;
                        }
                        dsBenhLy.push_back(benh);
                    }
                    catch (const exception &e)
                    {
                        cout << "[-] " << e.what() << " Vui long nhap lai.\n";
                        i--;
                    }
                }

                int co = nhapSoNguyen("Co so BHYT khong? (Nhap 1 neu co,nhap 0 neu khong): ", 0, 1);

                qlbv.themBenhNhan(make_shared<BenhNhan>(maBN, hoTen, gt, ngay, thang, nam, dc, dsBenhLy, co == 1));
                cout << "[+] Them benh nhan thanh cong!\n";
                break;
            }
            case 2:
            {
                string maBN = nhapChuoiKhongRong("Ma BN can xem: ");
                auto bn = qlbv.timBenhNhan(maBN);
                if (bn)
                    bn->xuatThongTin();
                else
                    cout << "[-] Khong tim thay benh nhan!\n";
                break;
            }
            case 3:
            {
                string maBN = nhapChuoiKhongRong("Ma BN can sua: ");
                qlbv.suaThongTinBN(maBN);
                break;
            }
            case 4:
            {
                string maBN = nhapChuoiKhongRong("Ma BN can xoa: ");
                if (xacNhan("Ban co chac muon xoa benh nhan nay?"))
                {
                    qlbv.xoaBenhNhan(maBN);
                    cout << "[+] Xoa thanh cong!\n";
                }
                break;
            }
            case 5:
            {
                qlbv.xuatDanhSachBN();
                break;
            }
            case 6:
            {
                cout << "\n=== THEM BAC SI MOI ===\n";
                string maBS = nhapChuoiKhongRong("Ma BS: ");
                string hoTen = nhapChuoiKhongRong("Ho ten: ");
                string gt = nhapGioiTinh();

                int ngay = nhapSoNguyen("Ngay sinh: ", 1, 31);
                int thang = nhapSoNguyen("Thang sinh: ", 1, 12);
                int nam = nhapSoNguyen("Nam sinh: ", 1900, 2025);

                if (!isValidDate(ngay, thang, nam))
                {
                    throw runtime_error("Ngay sinh khong hop le!");
                }

                string dc = nhapChuoiKhongRong("Dia chi: ");

                cout << "\nChuyen khoa hop le:\n";
                cout << "  - Tim mach\n  - Da lieu\n  - Xuong khop\n  - Noi khoa\n";
                string ck = nhapChuoiKhongRong("Chuyen khoa: ");

                qlbv.themBacSi(make_shared<BacSi>(maBS, hoTen, gt, ngay, thang, nam, dc, ck));
                cout << "[+] Them bac si thanh cong!\n";
                break;
            }
            case 7:
            {
                string maBS = nhapChuoiKhongRong("Ma BS can sua: ");
                qlbv.suaThongTinBS(maBS);
                break;
            }
            case 8:
            {
                qlbv.xuatDanhSachBS();
                break;
            }
            case 9:
            {
                cout << "\n=== THEM PHONG MOI ===\n";
                int so = nhapSoNguyen("So phong: ", 1, 9999);
                int giuong = nhapSoNguyen("So giuong: ", 1, 100);
                int loai = nhapSoNguyen("Loai phong (1=Thuong, 2=VIP): ", 1, 2);

                if (loai == 1)
                    qlbv.themPhong(make_shared<PhongThuong>(so, giuong));
                else
                    qlbv.themPhong(make_shared<PhongVIP>(so, giuong));

                cout << "[+] Them phong thanh cong!\n";
                break;
            }
            case 10:
            {
                int soPhong = nhapSoNguyen("So phong can sua: ", 1, 9999);
                qlbv.suaThongTinPhong(soPhong);
                break;
            }
            case 11:
            {
                qlbv.xuatDanhSachPhong();
                break;
            }
            case 12:
            {
                cout << "\n=== PHAN CONG BAC SI VA PHONG ===\n";
                string maBN = nhapChuoiKhongRong("Ma BN: ");
                string maBS = nhapChuoiKhongRong("Ma BS: ");
                int soPhong = nhapSoNguyen("So phong: ", 1, 9999);

                auto bn = qlbv.timBenhNhan(maBN);
                auto bs = qlbv.timBacSi(maBS);
                auto phong = qlbv.timPhong(soPhong);

                if (!bn)
                    throw runtime_error("Khong tim thay benh nhan!");
                if (!bs)
                    throw runtime_error("Khong tim thay bac si!");
                if (!phong)
                    throw runtime_error("Khong tim thay phong!");

                qlbv.phanCong(bn, bs, phong);
                cout << "[+] Phan cong thanh cong!\n";
                break;
            }
            case 13:
            {
                string maBN = nhapChuoiKhongRong("Ma BN: ");
                auto bn = qlbv.timBenhNhan(maBN);
                if (!bn)
                    throw runtime_error("Khong tim thay benh nhan!");

                cout << "\n--- DANH SACH DICH VU ---\n";
                for (size_t i = 0; i < dsDichVuCoTheThem.size(); ++i)
                {
                    cout << i + 1 << ". " << dsDichVuCoTheThem[i].toString() << endl;
                }

                int luaChon = nhapSoNguyen("Chon dich vu: ", 1, dsDichVuCoTheThem.size());
                bn->themDichVu(dsDichVuCoTheThem[luaChon - 1]);
                cout << "[+] Them dich vu thanh cong!\n";
                break;
            }
            case 14:
            {
                cout << "\n=== RA VIEN & XUAT HOA DON ===\n";
                string maBN = nhapChuoiKhongRong("Ma BN ra vien: ");
                auto bn = qlbv.timBenhNhan(maBN);
                if (!bn)
                    throw runtime_error("Khong tim thay benh nhan!");
                if (!bn->getTrangThaiNhapVien())
                    throw runtime_error("Benh nhan da ra vien truoc do!");

                int soNgay = nhapSoNguyen("So ngay nam vien: ", 1, 365);
                HoaDon hd(bn, soNgay);

                cout << "\n=== CHON PHUONG THUC THANH TOAN ===\n";
                cout << "1. Tien mat\n";
                cout << "2. Chuyen khoan (voi ma QR)\n";
                int ptChoice = nhapSoNguyen("Chon: ", 1, 2);
                string pt = (ptChoice == 1) ? "Tien mat" : "Chuyen khoan";
                hd.setPhuongThucThanhToan(pt);
                hd.xuatHoaDon();

                if (xacNhan("Xac nhan ra vien?"))
                {
                    // === GHI DU LIEU RA VIEN CHO AI HOC ===
                    string khoa = bn->getBacSiDieuTri() ? bn->getBacSiDieuTri()->getChuyenKhoa() : "Chua phan cong";
                    string mucDo = "Trung binh"; // Có thể mở rộng sau

                    // Tính ngày nhập viện: hôm nay - soNgay
                    auto now = chrono::system_clock::now();
                    time_t tt = chrono::system_clock::to_time_t(now);
                    tm today;
#ifdef _WIN32
                    localtime_s(&today, &tt);
#else
                    localtime_r(&tt, &today);
#endif
                    today.tm_mday -= soNgay;
                    mktime(&today); // Chuẩn hóa ngày
                    char ngayNhap[11];
                    strftime(ngayNhap, sizeof(ngayNhap), "%Y-%m-%d", &today);

                    // === GHI VAO dudoan.csv (có header nếu là file mới) ===
                    string logPath = getProjectPath() + "dudoan.csv";
                    bool isNewFile = !ifstream(logPath).good(); // Kiểm tra file có tồn tại chưa

                    ofstream log(logPath, ios::app);
                    if (log.is_open())
                    {
                        if (isNewFile)
                        {
                            log << "MaBN,HoTen,Tuoi,Khoa,MucDo,NgayNhap,SoNgayNamVien\n";
                        }
                        log << bn->getMaBN() << ","
                            << bn->getHoTen() << ","
                            << bn->tinhTuoi() << ","
                            << khoa << ","
                            << mucDo << ","
                            << ngayNhap << ","
                            << soNgay << "\n";
                        log.close();
                        cout << "[+] Da ghi du lieu ra vien vao dudoan.csv (AI se hoc tu day)\n";
                    }
                    else
                    {
                        cerr << "[!] KHONG MO DUOC FILE dudoan.csv de ghi!\n";
                    }

                    // Ra viện
                    bn->raVien();
                    cout << "[+] Benh nhan da ra vien thanh cong!\n";
                }
                break;
            }
            case 15:
            {
                qlbv.timKiemNangCao();
                break;
            }
            case 16:
            {
                cout << "\nChuyen khoa hop le: Tim mach, Da lieu, Xuong khop, Noi khoa\n";
                string chuyenKhoa = nhapChuoiKhongRong("Nhap chuyen khoa: ");
                qlbv.xuatDanhSachBNTheoKhoa(chuyenKhoa);
                break;
            }
            case 17:
            {
                cout << "\nBenh ly hop le:\n";
                int count = 1;
                for (const auto &pair : chuyenKhoaToBenhLy)
                {
                    const string &khoa = pair.first;
                    const vector<string> &dsBenh = pair.second;
                    cout << khoa << ":\n";
                    for (const auto &benh : dsBenh)
                    {
                        cout << "  " << count++ << ". " << benh << endl;
                    }
                }
                string benhLy = nhapChuoiKhongRong("\nNhap benh ly: ");
                qlbv.timBacSiTheoBenhLy(benhLy);
                break;
            }
            case 18:
            {
                int loai = nhapSoNguyen("Loai phong (1=Thuong, 2=VIP): ", 1, 2);
                if (loai == 1)
                    qlbv.timPhongTrongTheoLoai("Thuong");
                else
                    qlbv.timPhongTrongTheoLoai("VIP");
                break;
            }
            case 19:
            {
                qlbv.xuatDanhSachBNDaSapXep();
                break;
            }
            case 20:
            {
                qlbv.thongKeBNTheoTuoi();
                break;
            }
            case 21:
            {
                qlbv.baoCaoDoanhThu();
                break;
            }
            case 22:
            {
                qlbv.kiemTraSinhNhatHomNay();
                break;
            }
            case 23:
            {
                qlbv.luuVaoFile("dulieu.csv");
                break;
            }
            case 24:
            {
                system("cls");
                cout << "\n=== AI BENH VIEN CDI ===\n";
                cout << "1. Tu van 40+ cau hoi\n";
                cout << "2. Du doan ngay ra vien\n";
                cout << "0. Quay lai\n";
                int sub = nhapSoNguyen("Chon: ", 0, 2);
                if (sub == 0)
                    break;

                string pyFile = (sub == 1) ? "chatbot_ai.py" : "du_doan_ra_vien.py";
                string fullPath = getProjectPath() + pyFile;

                if (ifstream(fullPath).good())
                {
                    string cmd = "python \"" + fullPath + "\"";
                    cout << "\nDang khoi dong AI...\n";
                    int result = system(cmd.c_str());
                    cout << (result == 0 ? "[+] Hoan thanh!\n" : "[!] LOI Python!\n");
                }
                else
                {
                    cout << "[!] Khong tim thay file: " << pyFile << "\n";
                }
                cout << "Nhan Enter de tiep tuc...\n";
                _getch();
                break;
            }
            case 0:
                cout << "\n[!] Tam biet! Dang luu du lieu...\n";
                qlbv.luuVaoFile("dulieu.csv");
                cout << "[+] Da luu thanh cong!\n";
                break;
            default:
                cout << "[!] Lua chon khong hop le!\n";
            }
        }
        catch (const exception &e)
        {
            cout << "[-] Loi: " << e.what() << endl;
        }
    } while (ch != 0);
}

// ============= MAIN =============
int main()
{
    try
    {
        menu();
    }
    catch (const exception &e)
    {
        cerr << "[-] Loi nghiem trong: " << e.what() << endl;
        return 1;
    }
    return 0;
}
