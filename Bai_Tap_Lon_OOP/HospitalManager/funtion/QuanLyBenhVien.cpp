// QuanLyBenhVien.h
#pragma once
#include <vector>
#include "BenhNhan.h"
#include "BacSi.h"
#include "PhongBenh.h"
#include "HoaDon.h"

class QuanLyBenhVien {
private:
    vector<BenhNhan> dsBenhNhan;
    vector<BacSi> dsBacSi;
    vector<PhongBenh> dsPhong;
    vector<HoaDon> dsHoaDon;

public:
    void menu();
    void themBenhNhan();
    void xuatDSBenhNhan() const;
    // co th? them: tim ki?m, th?ng ke,...
};
