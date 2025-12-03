# 🤖 HỆ THỐNG PHÂN LOẠI VẬT THỂ TỰ ĐỘNG THÔNG MINH (IoT Sorting Machine)

## 💡 Giới Thiệu Dự Án

Dự án này là một **hệ thống phân loại vật thể tự động** dựa trên kiến trúc IoT, được thiết kế để nhận dạng và phân loại các vật thể theo **màu sắc** (Đỏ, Vàng, Xanh) và **hình dạng** (Tròn, Vuông). Hệ thống tận dụng **Arduino Uno** để điều khiển các cơ cấu vật lý và thực hiện toàn bộ logic phân loại. **ESP32-CAM** đóng vai trò là **IoT Gateway** và **Web Server** cung cấp một **Web Dashboard** thời gian thực để giám sát và điều khiển từ xa.

---
## ✨ Tính Năng Chính

* **Phân loại 6 mã sản phẩm:** TD (Tròn Đỏ), VD (Vuông Đỏ), TV (Tròn Vàng), VV (Vuông Vàng), TX (Tròn Xanh), VX (Vuông Xanh).
* **Giám sát Video Trực tiếp:** Truyền luồng video từ ESP32-CAM lên Dashboard qua HTTP Live Stream.
* **Web Dashboard Thời gian thực:** Cập nhật các chỉ số phân loại, **Tốc độ** (PPM), **Độ chính xác** (Accuracy), và trạng thái cảm biến qua kênh **WebSocket** tốc độ cao.
* **Điều khiển Từ xa:** Điều chỉnh góc quét camera (Pan Control) và thiết lập lại bộ đếm từ giao diện web.
* **Tự động phục hồi lỗi:** Cơ chế **thử lại** (Retry) nếu vật thể không được gắp thành công sau khi phân loại.

---

## 🛠️ Công Nghệ và Vai trò Chi tiết

Dự án sử dụng kiến trúc phân tán giữa hai vi điều khiển để đảm bảo hiệu suất và độ tin cậy.

### 1. 🧠 ESP32-CAM (IoT Gateway & Web Server)

| Chức năng | Vai trò | Công nghệ |
| :--- | :--- | :--- |
| **Giao tiếp IoT** | Xây dựng **WebSocket Server** (Port 81) để gửi dữ liệu thống kê và trạng thái lên Dashboard. | WebSocket |
| **Giao tiếp Serial** | **Nhận** dữ liệu trạng thái **JSON** từ Arduino. **Gửi** lệnh điều khiển cấp thấp (như `SERVO:90`, `STOP`) đến Arduino. | UART (Serial 2) |
| **Web Server** | Cung cấp giao diện Dashboard (`/`) và luồng video trực tiếp (`/stream`). | HTTP |
| **Camera** | Cung cấp luồng video trực tiếp (Live Stream) cho người dùng giám sát. | OV2640 |

### 2. ⚙️ Arduino Uno (Điều khiển Cơ cấu Chính xác & Logic Phân loại)

| Chức năng | Vai trò | Giao tiếp |
| :--- | :--- | :--- |
| **Thu thập Cảm biến** | Đọc dữ liệu từ HC-SR04, GY-31, và cảm biến IR. | Digital/Analog I/O |
| **Logic Phân loại** | **Xác định Mã Phân loại** (TD, VX,...) dựa trên dữ liệu màu sắc (GY-31) và hình khối (HC-SR04). | C Logic |
| **Điều khiển Cơ cấu** | Điều khiển **Servo MG996R** (cánh tay gạt) và **Motor DC** (băng tải) theo kết quả phân loại. | PWM, Digital I/O |
| **Giao tiếp Serial** | **Gửi** các gói **JSON** chứa dữ liệu trạng thái (Mã, RGB, Hình khối, Bộ đếm) đến ESP32. **Nhận** các lệnh điều khiển từ ESP32. | UART (Serial) |

### 3. Thành phần Phần cứng Chính

* **Vi điều khiển:** **ESP32-CAM**, **Arduino Uno**
* **Cơ cấu:** **Servo MG996R** (Cánh tay gạt), **Motor DC** & Driver (Băng tải).
* **Cảm biến:** Cảm biến **IR** (Phát hiện vật thể), Cảm biến **GY-31** (Nhận dạng màu sắc), Cảm biến **HC-SR04** (Phát hiện vật thể và nhận dạng hình khối).
* **Giao diện:** HTML/CSS/JavaScript, **Chart.js**.

---

## ⚙️ Sơ Đồ Hoạt Động (System Architecture)

Luồng hoạt động dưới đây mô tả chu trình phân loại và giao tiếp giữa các thành phần. 

1. **Phát hiện & Phân tích Vật thể (Uno):**
    * **HC-SR04** phát hiện vật thể $\rightarrow$ Dừng băng tải.
    * **HC-SR04** quét vật thể để nhận dạng **Hình khối**.
    * **GY-31** quét để xác định **Màu sắc** (RGB).
    * **Uno** sử dụng dữ liệu này để xác định **Mã phân loại** (ví dụ: `TD`).
2. **Cập nhật Realtime (Uno $\rightarrow$ ESP32 $\rightarrow$ Web):**
    * **Arduino Uno** gửi gói JSON `REALTIME` (chứa Mã, RGB, Hình khối) qua Serial 2.
    * **ESP32** nhận, phân tích, và phát sóng qua **WebSocket** lên Dashboard.
3. **Chờ tín hiệu Phân loại:**
    * **Uno** khởi động băng tải, chờ vật thể di chuyển đến vị trí gạt.
    * **Cảm biến IR** phát hiện vật thể $\rightarrow$ Dừng băng tải, bắt đầu quy trình gạt.
4. **Phân loại & Xử lý lỗi (Uno):**
    * **Uno** điều khiển **Servo MG996R** gạt vật thể theo mã phân loại.
    * Nếu **Cảm biến IR** vẫn *Blocked* sau khi gạt $\rightarrow$ Thực hiện **thử lại** (Retry).
    * Nếu thử lại thất bại quá 2 lần $\rightarrow$ Đánh dấu **Skipped** và tiếp tục.
5. **Cập nhật Thống kê (Uno $\rightarrow$ ESP32 $\rightarrow$ Web):**
    * Sau khi hoàn thành chu kỳ, **Arduino Uno** gửi gói JSON `COMPLETED` (chứa tổng bộ đếm mới và thời gian chu kỳ) qua Serial 2.
    * **ESP32** cập nhật và phát sóng JSON `COMPLETED` lên Dashboard để vẽ lại Biểu đồ và cập nhật các chỉ số tổng thể.

### Giao tiếp Dữ liệu Chính

| Kênh | Hướng | Ví dụ Lệnh/Dữ liệu | Mô tả |
| :--- | :--- | :--- | :--- |
| **Serial/UART** | ESP32 ➡️ Uno | `SERVO_POS:90`, `STOP`, `RESET` | Lệnh điều khiển vật lý cho Arduino Uno (từ Dashboard). |
| **Serial/UART** | Uno ➡️ ESP32 | `{"type":"COMPLETED", ...}\n` | Gửi dữ liệu trạng thái và thống kê theo định dạng JSON. |
| **WebSocket** | ESP32 ➡️ Web | Payload `COMPLETED`, `REALTIME`, `STATUS` | Cập nhật Dashboard theo ba loại dữ liệu chính. |
| **WebSocket** | Web ➡️ ESP32 | `CMD:PAN=90`, `CMD:EMERGENCY_STOP` | Lệnh điều khiển từ Dashboard. |

---

## 🚀 Hướng Dẫn Cài Đặt và Triển khai

### Yêu cầu Phần mềm
* **Arduino IDE** (hoặc PlatformIO)
* **Thư viện C++:** **`ArduinoJson`**, **`WebSocketsServer`** (cho ESP32).
* **Frontend:** HTML, CSS, JavaScript (sử dụng **Chart.js** qua CDN).

### Các Bước Triển khai
1. **Cấu hình Firmware:**
    * Cập nhật thông tin WiFi (`SSID` và `Password`) trong code **ESP32-CAM**.
    * Tải code `.ino` riêng biệt cho **Arduino Uno** và **ESP32-CAM**.
2. **Kết nối Serial (UART):**
    * Kết nối chân **RX2 (GPIO 15)** và **TX2 (GPIO 14)** của ESP32-CAM với chân **TX** và **RX** của Arduino Uno. *(Sử dụng Bộ chuyển đổi mức logic nếu cần thiết do khác biệt điện áp 3.3V/5V).*
3. **Vận hành:** Cấp nguồn và truy cập địa chỉ IP của ESP32-CAM trên trình duyệt để mở Dashboard giám sát.

---

## 🧑‍💻 Tác Giả & Bản quyền

Dự án được phát triển bởi **Nguyễn Quang Vinh**.

* GitHub: https://github.com/VinhShindo

Dự án này được phát hành dưới giấy phép MIT. Xem file [LICENSE](LICENSE) để biết thêm chi tiết.
