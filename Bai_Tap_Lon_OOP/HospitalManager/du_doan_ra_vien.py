# du_doan_ra_vien.py - AI DỰ ĐOÁN NGÀY RA VIỆN (dựa trên dudoan.csv)
import csv
import os
from datetime import datetime, timedelta

DATA_FILE = os.path.join(os.path.dirname(__file__), "dudoan.csv")

def predict():
    if not os.path.exists(DATA_FILE):
        return "Chưa có dữ liệu ra viện!"

    user = {}
    print("AI DỰ ĐOÁN NGÀY RA VIỆN")
    print("Nhập lần lượt:")

    user["tuoi"] = input("Tuổi: ").strip()
    user["khoa"] = input("Khoa (Tim mach/Da lieu/Noi khoa/Xuong khop): ").strip().title()
    user["mucdo"] = input("Mức độ (Nhe/Trung binh/Nang): ").strip().title()
    user["ngaynhap"] = input("Ngày nhập viện (YYYY-MM-DD): ").strip()

    data = []
    try:
        with open(DATA_FILE, "r", encoding="utf-8") as f:
            reader = csv.DictReader(f)
            for row in reader:
                if (row["ChuyenKhoa"] == user["khoa"] and 
                    user["mucdo"] in row["MucDo"]):
                    data.append(int(row["SoNgayNamVien"]))
        
        if not data:
            return f"Chưa có ca {user['khoa']} - {user['mucdo']} nào ra viện!"

        avg = sum(data) // len(data)
        if int(user["tuoi"]) > 60: avg += 2
        elif int(user["tuoi"]) < 18: avg -= 1
        
        date_in = datetime.strptime(user["ngaynhap"], "%Y-%m-%d")
        date_out = date_in + timedelta(days=avg)
        
        return f"**DỰ ĐOÁN: {date_out.strftime('%d/%m/%Y')}** (sau {avg} ngày)"

    except Exception as e:
        return f"Lỗi: {e}"

# === CHẠY AI ===
print(predict())