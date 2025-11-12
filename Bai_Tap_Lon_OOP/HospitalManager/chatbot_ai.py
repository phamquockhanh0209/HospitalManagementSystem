# CHATBOT OFFLINE - 40+ CAU HOI THUONG GAP
import re


# Đọc câu hỏi
with open("chatbot_input.txt", "r", encoding="utf-8") as f:
    question = f.read().strip().lower()

# Kiểm tra thoát
if question in ["thoat", "exit", "quit"]:
    with open("chatbot_output.txt", "w", encoding="utf-8") as f:
        f.write("EXIT")
    exit()

# Database câu trả lời chi tiết
def get_response(q):
    
    # ========== 1. GIỜ MỞ CỬA & LỊCH LÀM VIỆC ==========
    if any(word in q for word in ["gio", "mo cua", "dong cua", "lam viec", "khi nao", "may gio"]):
        if "chu nhat" in q or "ngay le" in q or "tet" in q:
            return "Benh vien nghi chu nhat va cac ngay le, Tet. Truong hop cap cuu van tiep nhan 24/7."
        return "Benh vien CDI mo cua:\n- Gio lam viec: 7h00 - 17h00\n- Thu: Thu Hai den Thu Bay\n- Cap cuu: 24/7 (ca chu nhat)"
    
    # ========== 2. CHI PHÍ PHÒNG BỆNH ==========
    if "phong" in q and any(word in q for word in ["chi phi", "gia", "tien", "bao nhieu"]):
        if "vip" in q:
            return "Phong VIP: 2,000,000 VND/ngay\n- Phong rieng biet\n- Giuong dien tu\n- TV, tu lanh, dieu hoa\n- Phong tam rieng\n- An com rieng"
        elif "thuong" in q:
            return "Phong thuong: 500,000 VND/ngay\n- Phong tap the 4-6 giuong\n- Co dieu hoa\n- Nha ve sinh chung\n- An com tap the"
        else:
            return "Bang gia phong benh:\n- Phong thuong: 500,000 VND/ngay (tap the 4-6 nguoi)\n- Phong VIP: 2,000,000 VND/ngay (rieng tu, co TV, tu lanh)\n\nMien phi cho tre em duoi 3 tuoi (nam cung phu huynh)"
    
    # ========== 3. CHI PHÍ XÉT NGHIỆM ==========
    if "xet nghiem" in q:
        if "mau" in q:
            return "Xet nghiem mau:\n- Xet nghiem tong quat: 150,000 VND\n- Xet nghiem sinh hoa: 200,000 VND\n- Xet nghiem hormone: 300,000 VND\n- Co ket qua trong 2-4 gio"
        elif "nieu" in q:
            return "Xet nghiem nieu:\n- Xet nghiem tong quat: 100,000 VND\n- Co ket qua trong 1-2 gio"
        elif "gan" in q:
            return "Xet nghiem chuc nang gan: 250,000 VND\n- Bao gom: AST, ALT, Bilirubin\n- Co ket qua sau 4 gio"
        elif "than" in q or "tieu duong" in q:
            return "Xet nghiem duong huyet: 80,000 VND\n- Ket qua ngay lap tuc (15 phut)\n- Nen nhịn an truoc 8 gio"
        else:
            return "Bang gia xet nghiem:\n- Xet nghiem mau tong quat: 150k\n- Xet nghiem nieu: 100k\n- Xet nghiem duong huyet: 80k\n- Xet nghiem gan: 250k\n- Xet nghiem hormone: 300k"
    
    # ========== 4. CHI PHÍ CHẨN ĐOÁN HÌNH ẢNH ==========
    if any(word in q for word in ["x-quang", "xquang", "chup", "quang"]):
        return "Chup X-quang:\n- X-quang phoi: 150,000 VND\n- X-quang xuong khop: 180,000 VND\n- X-quang rang: 100,000 VND\n- Co ket qua ngay (30 phut)"
    
    if "sieu am" in q:
        return "Sieu am:\n- Sieu am bung: 200,000 VND\n- Sieu am thai: 250,000 VND\n- Sieu am tim: 300,000 VND\n- Thoi gian: 15-30 phut"
    
    if "ct" in q or "ct scan" in q or "chup cat lop" in q:
        return "Chup CT Scanner:\n- CT so nao: 1,500,000 VND\n- CT nguc/bung: 1,200,000 VND\n- Can dat lich truoc\n- Co ket qua trong ngay"
    
    if "mri" in q or "cong huong tu" in q:
        return "Chup MRI (Cong huong tu):\n- MRI so nao: 3,000,000 VND\n- MRI cot song: 2,500,000 VND\n- Can dat lich truoc 1-2 ngay\n- Thoi gian chup: 30-45 phut"
    
    # ========== 5. QUY TRÌNH NHẬP VIỆN ==========
    if any(word in q for word in ["quy trinh", "thu tuc", "lam sao", "nhap vien"]):
        return "Quy trinh nhap vien:\n\n1. DANG KY (Tang 1 - Quay tiep nhan)\n   - Xuat trinh CMND/CCCD\n   - So BHYT (neu co)\n\n2. KHAM SO BO\n   - Bac si kham tong quat\n   - Xac dinh benh ly\n\n3. PHAN CONG\n   - Bac si chuyen khoa\n   - Phong benh phu hop\n\n4. NHAP VIEN\n   - Nhan giuong benh\n   - Bat dau dieu tri\n\nThoi gian: 1-2 gio"
    
    # ========== 6. GIẤY TỜ CẦN THIẾT ==========
    if any(word in q for word in ["giay to", "can gi", "mang theo", "ho so"]):
        return "Giay to can mang:\n\n1. BAT BUOC:\n   - CMND/CCCD (ban goc)\n   - So kham benh (neu co)\n\n2. NEU CO BHYT:\n   - The BHYT (con han)\n   - Giay chuyen vien (neu tu benh vien khac)\n\n3. TAI LIEU BENH LY:\n   - Ket qua xet nghiem cu\n   - Don thuoc dang dung\n   - Phim chup X-quang (neu co)"
    
    # ========== 7. ĐỊA CHỈ & LIÊN HỆ ==========
    if any(word in q for word in ["dia chi", "o dau", "duong", "lien he", "sdt"]):
        return "Thong tin lien he:\n\n- Dia chi: 123 Nguyen Hue, Quan 1, TP.HCM\n- Hotline: 1900-xxxx (24/7)\n- Email: contact@benhviencdi.vn\n- Website: www.benhviencdi.vn\n- Facebook: fb.com/benhviencdi"
    
    if "hotline" in q or "goi" in q or "dien thoai" in q:
        return "Cac duong day hotline:\n- Tong dai: 1900-xxxx (24/7)\n- Cap cuu: 115\n- Dat lich kham: 028-xxxx-xxx1\n- Tu van BHYT: 028-xxxx-xxx2\n- Khieu nai: 028-xxxx-xxx3"
    
    # ========== 8. CHUYÊN KHOA CHI TIẾT ==========
    if "khoa tim" in q or "tim mach" in q:
        return "Khoa Tim mach:\n\nBenh chua tri:\n- Cao huyet ap\n- Nhoi mau co tim\n- Suy tim\n- Loan nhip tim\n- Benh mach vanh\n\nDich vu:\n- Kham chuyen khoa\n- Dien tam do (ECG)\n- Sieu am tim\n- Test gang suc\n\nBac si: 5 BS chuyen khoa, 15-20 nam kinh nghiem"
    
    if "khoa da" in q or "da lieu" in q:
        return "Khoa Da lieu:\n\nBenh chua tri:\n- Mui tri, Mun trung ca\n- Nam da, lang ben\n- Viem da co dia\n- Benh vay nen (Psoriasis)\n- U da, Nep nhan\n\nDich vu:\n- Kham chuyen khoa\n- Lam sach da\n- Tri mun, tri nam\n- Laser, Peel da\n\nPhong kham rieng tu, bao mat cao"
    
    if "khoa noi" in q or "noi khoa" in q:
        return "Khoa Noi khoa:\n\nBenh chua tri:\n- Tieu duong (type 1, 2)\n- Benh gan: Viem gan, Xo gan\n- Benh than: Suy than, Soi than\n- Benh tieu hoa\n- Benh ho hap\n\nDich vu:\n- Kham tong quat\n- Xet nghiem chuyen sau\n- Tu van dinh duong\n- Theo doi dai han\n\nCo chuong trinh quan ly benh nhan man tinh"
    
    if "khoa xuong" in q or "xuong khop" in q:
        return "Khoa Xuong khop:\n\nBenh chua tri:\n- Thoai hoa khop (goi, vai, hong)\n- Gout, Viem khop dang thap\n- Trat khop, Gay xuong\n- Dau lung, dau co\n- Loang xuong\n\nDich vu:\n- Kham chuyen khoa\n- Chup X-quang\n- Vat ly tri lieu\n- Phuc hoi chuc nang\n- Tiem khop\n\nCo phong tap phuc hoi hien dai"
    
    # ========== 9. BẢO HIỂM Y TẾ ==========
    if "bhyt" in q or "bao hiem" in q:
        if "muc giam" in q or "giam bao nhieu" in q:
            return "Muc ho tro BHYT:\n- Co the BHYT: Giam 50% chi phi\n- Ho ngheo: Giam 100% (mien phi)\n- Tre em duoi 6 tuoi: Mien phi\n\nLuu y: Chi ap dung cho chi phi kham, dieu tri. Khong ap dung cho phong VIP, dich vu cao cap."
        else:
            return "Ve bao hiem y te (BHYT):\n\n1. YEU CAU:\n   - The BHYT con han\n   - CMND khop voi ten tren the\n   - Dung tuyen (neu can)\n\n2. QUYEN LOI:\n   - Giam 50% chi phi kham benh\n   - Giam 50% chi phi thuoc\n   - Giam 50% chi phi xet nghiem\n\n3. QUY TRINH:\n   - Xuat trinh the tai quay BHYT (Tang 1)\n   - Kham va dieu tri binh thuong\n   - Thanh toan phan con lai\n\nLien he: Quay BHYT - 028-xxxx-xxx2"
    
    # ========== 10. ĐẶT LỊCH KHÁM ==========
    if "dat lich" in q or "hen kham" in q or "dang ky kham" in q:
        return "Dat lich kham:\n\n1. TRUC TIEP:\n   - Den quay dang ky (Tang 1)\n   - Gio: 7h-16h (T2-T7)\n\n2. QUA DIEN THOAI:\n   - Goi: 028-xxxx-xxx1\n   - Cung cap: Ho ten, SDT, Khoa kham\n\n3. ONLINE:\n   - Website: benhviencdi.vn\n   - Chon khoa > Chon ngay > Xac nhan\n\nLuu y: Nen dat truoc 1-2 ngay de dam bao co lich"
    
    # ========== 11. GIỜ THĂM BỆNH ==========
    if "tham benh" in q or "tham nuoi" in q or "gio tham" in q:
        return "Gio tham benh:\n\n- Sang: 8h00 - 10h00\n- Chieu: 16h00 - 18h00\n\nQuy dinh:\n- Toi da 2 nguoi/lan\n- Khong mang tre nho\n- Khong on ao\n- Khong mang do an manh\n- Tat dien thoai hoac de che do im lang\n\nPhong VIP: Tu do tham benh (24/7)"
    
    # ========== 12. CHI PHÍ PHẪU THUẬT ==========
    if "phau thuat" in q:
        if "thua ruot thua" in q or "apendix" in q:
            return "Phau thuat cat ruot thua:\n- Chi phi: 5,000,000 - 8,000,000 VND\n- Thoi gian: 1-2 gio\n- Nam vien: 3-5 ngay\n- BHYT ho tro 50%"
        elif "soi" in q:
            return "Phau thuat noi soi:\n- Soi ong mat: 15,000,000 - 25,000,000 VND\n- Soi tam vi: 3,000,000 VND\n- Soi khop: 8,000,000 - 12,000,000 VND\n\nCan dat lich truoc va nhịn an 8 gio"
        else:
            return "Chi phi phau thuat (uoc tinh):\n- Phau thuat nho: 3-5 trieu\n- Phau thuat trung binh: 8-15 trieu\n- Phau thuat lon: 20-50 trieu\n\nCan kham va tu van truc tiep de co bao gia cu the. BHYT ho tro 50% chi phi."
    
    # ========== 13. CẤP CỨU ==========
    if "cap cuu" in q or "응급" in q or "응급실" in q:
        return "Phong cap cuu:\n\n- Vi tri: Tang 1 (cua chinh)\n- Hoat dong: 24/7 (ca chu nhat, ngay le)\n- Hotline cap cuu: 115\n\nTruong hop cap cuu:\n- Dau nguc, kho tho\n- Tai nan, gay xuong\n- Sot cao, co giat\n- Ngo doc thuc pham\n- Cham thuoc, di ung\n\nCo xe cap cuu san sang di don benh nhan"
    
    # ========== 14. CHI PHÍ DỊCH VỤ KHÁC ==========
    if "dich truyen" in q or "truyen dich" in q:
        return "Dich truyen:\n- Dich truyen sinh ly: 500,000 VND/lan\n- Dich truyen bo sung vitamin: 800,000 VND\n- Thoi gian truyen: 1-2 gio"
    
    if "tiem" in q:
        return "Tiem thuoc:\n- Tiem tinh mach: 50,000 VND\n- Tiem bap: 30,000 VND\n- Tiem duoi da: 30,000 VND\n- Gia chua bao gom thuoc"
    
    # ========== 15. KHÁM THAI SẢN ==========
    if "thai san" in q or "kham thai" in q or "bau" in q:
        return "Kham thai san:\n\n- Kham thai dinh ky: 200,000 VND/lan\n- Sieu am thai: 250,000 VND\n- Xet nghiem mau thai phu: 300,000 VND\n- Goi kham thai tron goi: 3,500,000 VND (9 thang)\n\nDich vu:\n- Sieu am 3D/4D\n- Tu van dinh duong\n- Tap Yoga cho ba bau\n- Truong lop me va be\n\nBac si san khoa: 3 BS, 10+ nam kinh nghiem"
    
    # ========== 16. TRẺ EM ==========
    if "tre em" in q or "nhi khoa" in q or "be" in q:
        return "Kham tre em - Nhi khoa:\n\n- Kham tong quat: 150,000 VND\n- Tiem chung: 200,000 - 500,000 VND\n- Kham chuyen khoa nhi: 250,000 VND\n\nDich vu:\n- Tiem chung day du\n- Theo doi phat trien\n- Tu van dinh duong\n- Kham benh thuong gap\n\nPhong kham than thien, co do choi, nhan vien nhiet tinh"
    
    # ========== 17. TIÊM CHỦNG ==========
    if "tiem chung" in q or "vac xin" in q or "vaccine" in q:
        return "Tiem chung - Vaccine:\n\nTre em:\n- Vaccine 5 trong 1: 450,000 VND\n- Vaccine phong dau: 1,200,000 VND\n- Vaccine viem gan B: 200,000 VND\n- Vaccine cum: 280,000 VND\n\nNguoi lon:\n- Vaccine cum: 350,000 VND\n- Vaccine COVID-19: Mien phi\n- Vaccine viem gan B: 250,000 VND\n\nCan dat lich truoc, mang theo so tiem chung"
    
    # ========== 18. KHÁM TỔNG QUÁT ==========
    if "kham tong quat" in q or "kham suc khoe" in q:
        return "Goi kham tong quat:\n\nGOI CO BAN (1,500,000 VND):\n- Kham lam san\n- Xet nghiem mau, nieu\n- X-quang phoi\n- Dien tam do\n\nGOI NAG CAO (3,500,000 VND):\n- Kham chuyen khoa\n- Xet nghiem sinh hoa day du\n- Sieu am bung toan bo\n- Dien tam do, do noi ap mat\n- CT Scanner/MRI (tuy chon)\n\nCo ket qua trong ngay, tu van bac si"
    
    # ========== 19. NHA THUỐC ==========
    if "nha thuoc" in q or "mua thuoc" in q:
        return "Nha thuoc benh vien:\n\n- Vi tri: Tang 1 (canh phong kham)\n- Gio mo cua: 7h-21h (T2-CN)\n- Don thuoc: Can co don cua bac si\n- Thanh toan: Tien mat, the, chuyen khoan\n\nDay du cac loai thuoc ke don, khong ke don. Nhan vien duoc si tu van nhiet tinh."
    
    # ========== 20. WIFI & TIỆN ÍCH ==========
    if "wifi" in q or "internet" in q:
        return "Wifi mien phi:\n- Ten wifi: BenhVienCDI\n- Mat khau: CDI@2025\n\nHoac hoi nhan vien de duoc cap mat khau rieng (phong VIP)"
    
    if "can tin" in q or "an uong" in q or "com" in q:
        return "Can tin - Khu an uong:\n\n- Vi tri: Tang 1 (gan cua chinh)\n- Gio hoat dong: 6h30 - 19h00\n- Thuc don:\n  + Com phan: 40,000 - 60,000 VND\n  + Banh mi, pho: 25,000 - 35,000 VND\n  + Nuoc uong, trai cay\n\n- Dich vu giao com len phong\n- Thuc don cho benh nhan (theo chi dinh bac si)"
    
    if "atm" in q or "rut tien" in q:
        return "May ATM:\n- Vi tri: Tang 1, gan quay tiep nhan\n- Ngan hang: Vietcombank, Techcombank, BIDV\n- Hoat dong 24/7"
    
    if "bai xe" in q or "gui xe" in q or "do xe" in q:
        return "Bai do xe:\n- Vi tri: Tang ham, phia sau benh vien\n- Phi gui xe:\n  + Xe may: 5,000 VND\n  + Xe oto: 20,000 VND\n- Bao ve 24/7\n- Co camera giam sat"
    
    # ========== 21. THANH TOÁN ==========
    if "thanh toan" in q or "tra tien" in q:
        return "Hinh thuc thanh toan:\n\n1. Tien mat (VND)\n2. The ATM (Visa, Mastercard, Noi dia)\n3. Chuyen khoan ngan hang\n4. Quet ma QR (Momo, ZaloPay, ViettelPay)\n\nQuy dinh:\n- Thanh toan trong vong 24h sau ra vien\n- Neu co BHYT: Chi tra phan con lai\n- Hoa don VAT (neu yeu cau)\n\nQuay thanh toan: Tang 1"
    
    # ========== 22. KHIẾU NẠI ==========
    if "khieu nai" in q or "than phien" in q or "phan anh" in q:
        return "Khieu nai - Phan anh:\n\n1. Truc tiep:\n   - Phong Hanh chinh (Tang 2)\n   - Gio: 8h-16h (T2-T6)\n\n2. Hotline: 028-xxxx-xxx3\n\n3. Email: khieunai@benhviencdi.vn\n\n4. Hop thu gop y (Tang 1)\n\nCam ket giai quyet trong 3-5 ngay lam viec"
    
    # ========== 23. XIN GIẤY NGHỈ ỐM ==========
    if "giay nghi om" in q or "nghi phep" in q or "xin nghi" in q:
        return "Giay nghi om - Giay ra vien:\n\n- Bac si cap sau khi kham\n- Thoi gian: Tuy vao tinh trang benh\n- Phi: Mien phi (da bao gom trong tien kham)\n- Nhan tai quay hanh chinh (Tang 1)\n\nCan mang theo:\n- So kham benh\n- CMND"
    
    # ========== 24. DỊCH VỤ SANG ==========
    if "phong rieng" in q or "phong don" in q:
        return "Phong benh rieng (Single Room):\n- Chi phi: 3,500,000 VND/ngay\n- Dien tich: 25m2\n- Tien nghi: Giuong dien tu, Sofa, TV 43 inch, Tu lanh, Dieu hoa, Wifi\n- Phong tam rieng (nuoc nong)\n- Dich vu an uong theo yeu cau\n- Nguoi nha duoc o lai 24/7"
    
    # ========== 25. GIA ĐÌNH BỆNH NHÂN ==========
    if "nguoi nha" in q or "than nhan" in q or "ngu lai" in q:
        return "Nguoi nha benh nhan:\n\nPhong thuong:\n- Chi 1 nguoi duoc o lai\n- Khong cung cap giuong phu\n- Mang chan, goi rieng\n\nPhong VIP:\n- Duoc o lai thoai mai\n- Co sofa giuong\n- Dich vu an uong\n\nQuy dinh chung:\n- Giu trat tu, im lang\n- Khong hut thuoc\n- Tuan thu y lenh bac si"
    
    # ========== 26. HỎI VỀ BỆNH ==========
    if "trieu chung" in q or "dau" in q or "benh gi" in q:
        if "dau dau" in q or "dau dau" in q:
            return "Dau dau:\n- Neu dau dau nhe: Co the do stress, ngu it\n- Neu dau dau dap, buon non: Can kham ngay\n- Neu dau dau dot ngot, dau du doi: Goi cap cuu 115\n\nKham tai khoa: Noi khoa hoac Than kinh\n\nNen kham khi:\n- Dau keo dai tren 2 ngay\n- Dau tang dan\n- Kem theo sot, non, hong mat"
        elif "dau bung" in q:
            return "Dau bung:\n- Vi tri dau rat quan trong\n- Dau ben phai duoi: Co the ruot thua\n- Dau tren rau: Co the da day, gan\n- Dau bung toan bo: Can kham ngay\n\nKham tai khoa: Noi khoa hoac Tieu hoa\n\nNen kham ngay khi:\n- Dau dang, khong chiu duoc\n- Kem theo non, tieu chay\n- Bung cang, sung"
        elif "ho" in q:
            return "Ho, kho tho:\n- Ho kho >= 2 tuan: Can kham phoi\n- Ho co dom: Kham ngay\n- Kho tho: Can cap cuu\n\nKham tai khoa: Noi khoa - Ho hap\n\nXet nghiem: Chup X-quang phoi"
        else:
            return "De duoc tu van chinh xac, ban nen:\n1. Mo ta ro trieu chung\n2. Thoi gian bat dau\n3. Muc do nghiem trong\n4. Cac thuoc dang dung\n\nHoac den kham truc tiep tai benh vien. Hotline tu van: 1900-xxxx"
    
    # ========== 27. DỊCH VỤ ĐƯA ĐÓN ==========
    if "xe" in q or "dua don" in q or "di lai" in q:
        return "Dich vu xe cap cuu:\n- Xe cap cuu: 115 (Mien phi cap cuu)\n- Xe cua benh vien: 500,000 - 1,000,000 VND (tuy khoang cach)\n- Dat xe: 1900-xxxx\n\nCac hang taxi gan:\n- Vinasun: 028-38.27.27.27\n- Mai Linh: 028-38.38.38.38\n\nXe bus gan nhat: Tuyen 01, 52, 56 (cach 200m)"
    
    # ========== 28. ĐÁNH GIÁ ==========
    if "danh gia" in q or "chat luong" in q or "y kien" in q:
        return "Danh gia dich vu:\n\nCac kenh danh gia:\n1. Phieu khao sat (nhan tai quay)\n2. Website: benhviencdi.vn/danhgia\n3. Google Maps: Benh vien CDI\n4. Facebook: fb.com/benhviencdi\n\nY kien cua ban giup chung toi cai thien dich vu tot hon. Cam on!"
    
    # ========== 29. THUỐC VÀ ĐIỀU TRỊ ==========
    if "thuoc" in q and "gia" in q:
        return "Gia thuoc:\n- Gia thuoc phu thuoc don bac si\n- Trung binh: 100,000 - 500,000 VND/ngay\n- BHYT ho tro 50% tien thuoc\n\nCo the hoi gia cu the tai nha thuoc benh vien sau khi kham."
    
    # ========== 30. BÁC SĨ ==========
    if "bac si gioi" in q or "bac si nao" in q or "bac si noi tieng" in q:
        return "Doi ngu bac si:\n\nKhoa Tim mach:\n- BS. Nguyen Van A (Truong khoa) - 20 nam kinh nghiem\n- BS. Tran Thi B - Chuyen gia tim mach\n\nKhoa Da lieu:\n- BS. Le Van C - Chuyen gia da lieu\n- BS. Pham Thi D - Chuyen tri mun, nam\n\nKhoa Noi khoa:\n- BS. Hoang Van E (Pho khoa) - Chuyen tieu duong\n- BS. Nguyen Thi F - Chuyen benh gan\n\nKhoa Xuong khop:\n- BS. Tran Van G - Phau thuat xuong khop\n- BS. Le Thi H - Vat ly tri lieu\n\nTat ca deu co chung chi hanh nghe, nhieu nam kinh nghiem"
    
    if "bac si" in q or "bs" in q:
        return "Doi ngu y bac si:\n\n- Tong so: 25 bac si (10 BS chinh, 15 BS tro ly)\n- Trinh do: Dai hoc Y, Chuyen khoa 1, Chuyen khoa 2\n- Kinh nghiem: 10-25 nam\n- Dao tao lien tuc tai nuoc ngoai\n\nBac si lam viec:\n- Sang: 7h-12h (ca A)\n- Chieu: 13h-17h (ca B)\n- Toi: 18h-22h (ca C - cap cuu)\n\nCo the chi dinh bac si khi dat lich"
    
    # ========== 31. CHI PHÍ TỔNG HỢP ==========
    if any(word in q for word in ["chi phi", "gia", "tien", "bao nhieu"]) and "tat ca" in q:
        return "Tong hop chi phi (tham khao):\n\nKham benh:\n- Kham tong quat: 150k-250k\n- Kham chuyen khoa: 250k-350k\n\nXet nghiem:\n- Xet nghiem co ban: 100k-200k\n- Xet nghiem chuyen sau: 300k-500k\n\nChan doan hinh anh:\n- X-quang: 150k-300k\n- Sieu am: 200k-300k\n- CT scan: 1.2tr-1.5tr\n- MRI: 2.5tr-3tr\n\nPhong benh:\n- Phong thuong: 500k/ngay\n- Phong VIP: 2tr/ngay\n- Phong don: 3.5tr/ngay\n\nPhau thuat: 3tr-50tr (tuy loai)\n\nBHYT ho tro 50% hau het chi phi"
    
    # ========== 32. TÁI KHÁM ==========
    if "tai kham" in q or "kham lai" in q:
        return "Tai kham - Kham lai:\n\n- Thoi gian: Theo chi dinh cua bac si\n- Phi tai kham: 100,000 VND (re hon lan dau)\n- Mang theo: So kham benh, ket qua xet nghiem cu\n\nQuy trinh:\n1. Dang ky tai quay (noi da kham)\n2. Gap lai bac si cu (neu yeu cau)\n3. Kham va tu van\n4. Ke don thuoc (neu can)\n\nNen tai kham dung hen de bac si theo doi tot"
    
    # ========== 33. KẾT QUẢ XÉT NGHIỆM ==========
    if "ket qua" in q or "tra ket qua" in q:
        return "Tra cuu ket qua xet nghiem:\n\n1. TRUC TIEP:\n   - Quay xet nghiem (Tang 2)\n   - Mang theo phieu hen\n\n2. ONLINE:\n   - Website: benhviencdi.vn/ketqua\n   - Nhap ma benh nhan\n   - Tai file PDF\n\n3. EMAIL:\n   - Tu dong gui sau 4-6 gio\n   - Kiem tra hop thu\n\nThoi gian co ket qua:\n- Xet nghiem thuong: 2-4 gio\n- Xet nghiem phuc tap: 1-2 ngay\n- Sinh thiet: 5-7 ngay"
    
    # ========== 34. CHUYỂN VIỆN ==========
    if "chuyen vien" in q:
        return "Thu tuc chuyen vien:\n\n1. TU BENH VIEN KHAC DEN:\n   - Mang giay chuyen vien\n   - Ho so benh an\n   - Ket qua xet nghiem\n   - BHYT (neu co)\n\n2. TU BENH VIEN DEN NOI KHAC:\n   - Bac si lap giay chuyen vien\n   - Nhan tai quay hanh chinh\n   - Mien phi\n\nLy do chuyen vien:\n- Can dieu tri chuyen sau\n- Can trang thiet bi dac biet\n- Theo yeu cau benh nhan"
    
    # ========== 35. GIỚI THIỆU BỆNH VIỆN ==========
    if "gioi thieu" in q or "ve benh vien" in q or "benh vien nhu nao" in q:
        return "Gioi thieu Benh vien CDI:\n\nTHANH LAP: 2010 (15 nam hoat dong)\n\nQUY MO:\n- 200 giuong benh\n- 4 khoa chuyen khoa\n- 25 bac si\n- 80 dieu duong, nhan vien\n\nTITIU CHI:\n- ISO 9001:2015\n- Chung nhan Bo Y te\n- Benh vien hang 2 (Hang I dang nang cap)\n\nTAM NHIN:\n'Tro thanh benh vien hang dau khu vuc ve chat luong dich vu va cham soc benh nhan'\n\nGIA TRI COT LOI:\n- Tan tam\n- Chuyen nghiep\n- Hien dai\n- Nhan van"
    
    # ========== 36. ĐIỀU DƯỠNG ==========
    if "dieu duong" in q or "y ta" in q:
        return "Doi ngu dieu duong:\n\n- Tong so: 50 dieu duong\n- Trinh do: Trung cap, Cao dang, Dai hoc Dieu duong\n- Chia ca 24/7\n\nNhiem vu:\n- Cham soc benh nhan\n- Thuc hien y lenh\n- Quan sat theo doi\n- Tu van suc khoe\n\nTy le dieu duong/benh nhan: 1/4 (dam bao chat luong cham soc)\n\nNeu can ho tro, bam chuong goi y ta tai giuong benh"
    
    # ========== 37. AN TOÀN VỆ SINH ==========
    if "ve sinh" in q or "sach se" in q or "an toan" in q:
        return "Ve sinh - An toan benh vien:\n\nVE SINH:\n- Don dep 3 lan/ngay\n- Khu trung 2 lan/ngay\n- Thay ga giuong hang ngay\n- Giat ui quan ao benh nhan\n\nKIEM SOAT NHIEM KHUAN:\n- Dung dich rua tay khap noi\n- Khau trang mien phi\n- Phan loai rac y te\n- Khu trung dung cu\n\nAN TOAN:\n- Camera giam sat 24/7\n- Bao ve chuyen nghiep\n- He thong phong chay\n- Loi thoat hiem\n\nBenh vien dat tieu chuan ve sinh an toan thuc pham, moi truong"
    
    # ========== 38. CÁCH ĐẾN BỆNH VIỆN ==========
    if "di nhu nao" in q or "duong di" in q or "huong dan" in q:
        return "Huong dan den benh vien:\n\nDIA CHI: 123 Nguyen Hue, Quan 1, TP.HCM\n\nDI BANG XE:\n- Tu Ben Thanh: 5 phut (1.5km)\n- Tu san bay Tan Son Nhat: 20 phut (8km)\n- Tu Quan 2: 15 phut (5km)\n\nXE BUS:\n- Tuyen 01: Tu Ben Thanh\n- Tuyen 52: Tu Binh Tan\n- Tuyen 56: Tu Quan 9\n\nDI BO:\n- Tu Nha hat Thanh pho: 700m (8 phut)\n- Tu Bitexco: 400m (5 phut)\n\nCO BAI DO XE rong rai, tien loi"
    
    # ========== 39. COVID-19 ==========
    if "covid" in q or "corona" in q or "xet nghiem covid" in q:
        return "Dich vu COVID-19:\n\nXET NGHIEM:\n- Test nhanh (Rapid test): 150,000 VND (15 phut)\n- PCR: 600,000 VND (4-6 gio)\n- Lam viec ca ngay (7h-17h)\n\nTIEM VACCINE:\n- Vaccine COVID-19: Mien phi (theo chuong trinh Nha nuoc)\n- Dang ky: Truc tiep hoac online\n\nQUY DINH:\n- Benh nhan phai deo khau trang\n- Rua tay sat khuan\n- Khai bao y te\n- Giu khoang cach an toan\n\nKhu kham rieng cho benh nhan sot, ho, kho tho"
    
    # ========== 40. HỖ TRỢ TÀI CHÍNH ==========
    if "tra gop" in q or "ho tro tai chinh" in q or "tam ung" in q:
        return "Ho tro tai chinh:\n\nTRA GOP:\n- Lien ket voi cac cong ty tai chinh\n- Ky han: 3-12 thang\n- Lai suat: 0-1.5%/thang\n- Ho so don gian: CMND + Xac nhan thu nhap\n\nTAM UNG VIEN PHI:\n- Can co nguoi than bao lanh\n- Mien phi cho truong hop kho khan\n- Xet duyet trong ngay\n\nHO TRO:\n- Ho ngheo, nguoi cao tuoi don than: Giam 10-30%\n- Hoc sinh, sinh vien: Giam 15%\n- Thuong binh, liet si: Giam 50%\n\nLien he phong Cong tac xa hoi (Tang 2) de duoc tu van"
    
    # ========== 41. DỊCH VỤ THEO YÊU CẦU ĐẶC BIỆT ==========
    if "vip" in q or "dac biet" in q or "cao cap" in q:
        return "Dich vu VIP - Cao cap:\n\nGOI KHAM SANG:\n- Kham tai nha: 2,000,000 VND\n- Bac si den tan noi\n- Mang thiet bi y te\n\nDICH VU DIEU DUONG:\n- Dieu duong rieng: 1,500,000 VND/ngay\n- Cham soc 24/7\n- Chuyen nghiep, kinh nghiem\n\nDICH VU AN UONG:\n- An com rieng (thuc don rieng)\n- Dieu chinh theo yeu cau\n- Chi phi: 200,000-500,000 VND/ngay\n\nXE CAP CUU RIENG:\n- Co san 24/7\n- Trang bi hien dai\n- Phi: 1,000,000-2,000,000 VND/chuyen"
    
    # ========== 42. HỎI ĐÁP NHANH ==========
    if "co" in q and any(word in q for word in ["khong", "duoc khong", "can khong"]):
        if "wifi" in q:
            return "Co wifi mien phi. Ten: BenhVienCDI, Pass: CDI@2025"
        elif "ngu lai" in q:
            return "Phong thuong: 1 nguoi. Phong VIP: Tu do. Mang chan goi rieng."
        elif "atm" in q:
            return "Co 3 may ATM tai tang 1, hoat dong 24/7"
        elif "bai xe" in q:
            return "Co bai xe tang ham. Xe may 5k, oto 20k"
        elif "an" in q or "can tin" in q:
            return "Co can tin tang 1, mo cua 6h30-19h"
        else:
            return "Vui long mo ta cu the hon, hoac goi hotline 1900-xxxx de duoc tu van"
    
    # ========== 43. THỜI GIAN CHỜ ==========
    if "cho" in q or "cho bao lau" in q or "thoi gian" in q:
        if "kham" in q:
            return "Thoi gian cho kham:\n- Co hen truoc: 15-30 phut\n- Khong hen: 30-60 phut\n- Cao diem (sang): Co the lau hon\n\nMeo: Dat lich truoc de giam thoi gian cho"
        elif "xet nghiem" in q:
            return "Thoi gian cho ket qua:\n- Xet nghiem thuong: 2-4 gio\n- Xet nghiem mau nhanh: 30 phut\n- PCR COVID: 4-6 gio\n- Sinh thiet: 5-7 ngay"
        else:
            return "Thoi gian cho phu thuoc dich vu. Ban can cho thoi gian nao?"
    
    # ========== MẶC ĐỊNH ==========
    return """Xin loi, toi chua hieu ro cau hoi cua ban.

Ban co the hoi toi ve:

Thong tin lien he & Gio mo cua
+ Chi phi (phong, xet nghiem, phau thuat)
+ Quy trinh nhap vien & Giay to can thiet
+ Cac chuyen khoa (Tim mach, Da lieu, Noi khoa, Xuong khop)
+ Bao hiem y te (BHYT)
+ Dat lich kham, Tai kham
+ Cap cuu 24/7
+ Thai san, Tre em, Tiem chung
+ Tien ich (Wifi, Can tin, ATM, Bai xe)
+ Thanh toan, Tra gop

Hoac goi hotline: 1900-6754 de duoc ho tro truc tiep!"""

# Tạo câu trả lời
response = get_response(question)

# Ghi kết quả
with open("chatbot_output.txt", "w", encoding="utf-8") as f:
    f.write(response)