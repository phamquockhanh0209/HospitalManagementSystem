// HoaDon.h
#pragma once
#include <iostream>
#include <string>
using namespace std;

class HoaDon {
private:
    string maHoaDon;
    string maBenhNhan;
    double tongTien;

public:
    HoaDon(string maHD = "", string maBN = "", double tong = 0);
    void nhap();
    void xuat() const;
};
