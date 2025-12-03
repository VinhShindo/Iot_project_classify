# 🤖 HỆ THỐNG PHÂN LOẠI VẬT THỂ TỰ ĐỘNG THÔNG MINH (IoT Sorting Machine)

## 💡 Giới Thiệu Dự Án

Dự án này là một **hệ thống phân loại vật thể tự động** dựa trên kiến trúc IoT, được thiết kế để nhận dạng và phân loại các vật thể theo **màu sắc** (Đỏ, Vàng, Xanh) và **hình dạng** (Tròn, Vuông). Hệ thống tận dụng sức mạnh xử lý của **ESP32-CAM** cho tác vụ nhận dạng và WebServer, đồng thời sử dụng **Arduino Uno** để điều khiển các cơ cấu vật lý một cách chính xác. Toàn bộ quá trình được giám sát qua một **Web Dashboard** thời gian thực.

---

## ✨ Tính Năng Chính

* **Phân loại 6 mã sản phẩm:** TD (Tròn Đỏ), VD (Vuông Đỏ), TV (Tròn Vàng), VV (Vuông Vàng), TX (Tròn Xanh), VX (Vuông Xanh).
* **Giám sát Video Trực tiếp:** Truyền luồng video từ ESP32-CAM lên Dashboard.
* **Web Dashboard Thời gian thực:** Cập nhật các chỉ số phân loại, **Tốc độ** (PPM), **Độ chính xác** (Accuracy), và trạng thái cảm biến qua **WebSocket**.
* **Điều khiển Từ xa:** Điều chỉnh góc quét camera (Pan Control) và thiết lập lại bộ đếm từ giao diện web.

---

## 🛠️ Công Nghệ và Vai trò Chi tiết

Dự án sử dụng kiến trúc phân tán giữa hai vi điều khiển để đảm bảo hiệu suất và độ tin cậy.

### 1. 🧠 ESP32-CAM (Xử lý thông minh & IoT)

| Chức năng | Vai trò | Công nghệ |
| :--- | :--- | :--- |
| **Nhận dạng** | Chụp ảnh, phân tích màu sắc (RGB) và hình dạng để xác định mã phân loại. | OpenCV/Algorithm |
| **Giao tiếp IoT** | Xây dựng **WebSocket Server** để gửi dữ liệu thống kê và trạng thái lên Dashboard. | WebSocket |
| **Giao tiếp Serial** | Gửi lệnh điều khiển cấp cao (`SORT:TD`) đến Arduino Uno. | UART |
| **Web Server** | Cung cấp giao diện Dashboard và luồng video trực tiếp. | HTTP |

### 2. ⚙️ Arduino Uno (Điều khiển Cơ cấu Chính xác)

| Chức năng | Vai trò | Giao tiếp |
| :--- | :--- | :--- |
| **Nhận lệnh Serial** | Liên tục lắng nghe và phân tích các lệnh điều khiển (string) gửi từ ESP32-CAM. | UART (Serial) |
| **Điều khiển Servo** | Dựa trên mã phân loại, điều khiển góc quay của **Servo MG996R** để gạt vật thể vào đúng thùng. | PWM (Servo) |
| **Điều khiển Băng tải** | Bật/Tắt **Motor DC** (qua Motor Driver) theo yêu cầu của chu trình phân loại. | Digital I/O |

### 3. Thành phần Phần cứng Chính

* **Vi điều khiển:** ESP32-CAM, Arduino Uno
* **Cơ cấu:** Servo SG90, Motor DC & Driver.
* **Cảm biến:** Cảm biến IR (Phát hiện vật thể).
* **Giao diện:** HTML/CSS/JavaScript, Chart.js.

---

## ⚙️ Sơ Đồ Hoạt Động (System Architecture)

Dữ liệu di chuyển theo một luồng rõ ràng để đảm bảo tốc độ và sự đồng bộ trong quá trình phân loại.

1.  **Vật thể** kích hoạt **Cảm biến IR**.
2.  **ESP32-CAM** chụp ảnh, xử lý và xác định mã phân loại (Ví dụ: `VX`).
3.  **ESP32-CAM** gửi lệnh **SORT** tới Arduino Uno qua Serial: `SORT:VX`.
4.  **ESP32-CAM** gửi dữ liệu **COMPLETED** (Thống kê tổng hợp) và **REALTIME** (Trạng thái hiện tại) lên Web Dashboard qua **WebSocket**.
5.  **Arduino Uno** nhận lệnh, điều khiển Servo gạt và Motor băng tải để hoàn thành chu trình phân loại.



### Giao tiếp Dữ liệu Chính

| Kênh | Hướng | Ví dụ Lệnh/Dữ liệu | Mô tả |
| :--- | :--- | :--- | :--- |
| **Serial/UART** | ESP32 ➡️ Uno | `SORT:TV`, `SERVO:90`, `STOP` | Lệnh điều khiển vật lý cho Arduino Uno. |
| **WebSocket** | ESP32 ➡️ Web | Payload `COMPLETED` | Gửi tổng số liệu đã phân loại và thời gian chu kỳ. |
| **WebSocket** | ESP32 ➡️ Web | Payload `REALTIME` | Gửi dữ liệu nhận dạng hiện tại (Mã, R, G, B). |
| **WebSocket** | Web ➡️ ESP32 | `CMD:RESET_COUNTERS` | Lệnh điều khiển từ Dashboard. |

---

## 🚀 Hướng Dẫn Cài Đặt và Triển khai

### Yêu cầu Phần mềm
* **Arduino IDE** (hoặc PlatformIO)
* **Visual Studio Code** (Để chỉnh sửa Dashboard)
* Thư viện **Chart.js** (sử dụng CDN).

### Các Bước Triển khai
1.  **Cấu hình Firmware:**
    * Cập nhật thông tin WiFi (`SSID` và `Password`) trong code ESP32-CAM.
    * Upload code `.ino` riêng biệt cho Arduino Uno (Motor, Servo control) và ESP32-CAM (IoT, Image Processing).
2.  **Thiết lập Web:** Đảm bảo file Dashboard (`index.html`, `style.css`) được nạp vào bộ nhớ **SPIFFS/LITTLEFS** của ESP32-CAM.
3.  **Vận hành:** Cấp nguồn cho hệ thống và truy cập địa chỉ IP của ESP32-CAM trên trình duyệt để mở Dashboard giám sát.

---

## 🧑‍💻 Tác Giả & Bản quyền

Dự án được phát triển bởi **Nguyễn Quang Vinh**.

* GitHub: https://github.com/VinhShindo

Dự án này được phát hành dưới giấy phép MIT. Xem file [LICENSE](LICENSE) để biết thêm chi tiết.
