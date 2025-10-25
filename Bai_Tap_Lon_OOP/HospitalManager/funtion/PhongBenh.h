// PhongBenh.h
#pragma once
#include <iostream>
using namespace std;

class PhongBenh {
private:
    int maPhong;
    int soGiuong;
    int soGiuongTrong;

public:
    PhongBenh(int ma = 0, int tong = 0, int trong = 0);
    void nhap();
    void xuat() const;
    bool conTrong() const { return soGiuongTrong > 0; }
    void giamGiuong() { if (soGiuongTrong > 0) soGiuongTrong--; }
};
