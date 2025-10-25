// BacSi.h
#pragma once
#include "Nguoi.h"

class BacSi : public Nguoi {
private:
    string maBacSi;
    string chuyenKhoa;

public:
    BacSi(string ma = "", string hoTen = "", string ngaySinh = "", string gioiTinh = "", string ck = "");
    void nhap() override;
    void xuat() const override;
};
