// Nguoi.cpp
#include "Nguoi.h"
#include <sstream>

Nguoi::Nguoi(string hoTen, string ngaySinh, string gioiTinh)
    : hoTen(hoTen), ngaySinh(ngaySinh), gioiTinh(gioiTinh) {}

void Nguoi::nhap() {
    cout << "Nhap ho ten: "; getline(cin, hoTen);
    cout << "Nhap ngay sinh (dd/mm/yyyy): "; getline(cin, ngaySinh);
    cout << "Nhap gioi tinh: "; getline(cin, gioiTinh);
}

void Nguoi::xuat() const {
    cout << "Ho ten: " << hoTen << " | Ngay sinh: " << ngaySinh
         << " | Gioi tinh: " << gioiTinh
         << " | Tuoi: " << tinhTuoi() << endl;
}

int Nguoi::tinhTuoi() const {
    int d, m, y;
    char c;
    stringstream ss(ngaySinh);
    ss >> d >> c >> m >> c >> y;

    time_t t = time(nullptr);
    tm* now = localtime(&t);
    int namHienTai = now->tm_year + 1900;

    return namHienTai - y;
}
