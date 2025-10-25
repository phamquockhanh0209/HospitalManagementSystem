// BenhNhan.h
#pragma once
#include "Nguoi.h"

class BenhNhan : public Nguoi {
private:
    string maBenhNhan;
    string benhLy;
    int soPhong;

public:
    BenhNhan(string ma = "", string hoTen = "", string ngaySinh = "", string gioiTinh = "",
             string benhLy = "", int soPhong = 0);
    void nhap() override;
    void xuat() const override;

    string getMa() const { return maBenhNhan; }
    int getPhong() const { return soPhong; }
};
