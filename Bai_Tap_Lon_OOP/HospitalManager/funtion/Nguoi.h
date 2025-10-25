// Nguoi.h
#pragma once
#include <string>
#include <iostream>
#include <chrono>
using namespace std;

class Nguoi {
protected:
    string hoTen;
    string ngaySinh; // d?ng dd/mm/yyyy
    string gioiTinh;

public:
    Nguoi(string hoTen = "", string ngaySinh = "", string gioiTinh = "");
    virtual void nhap();
    virtual void xuat() const;
    int tinhTuoi() const; // tinh tu?i b?ng <chrono>
};
