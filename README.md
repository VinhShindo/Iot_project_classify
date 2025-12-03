# 🤖 HỆ THỐNG PHÂN LOẠI VẬT THỂ TỰ ĐỘNG THÔNG MINH (IoT Sorting Machine)

## 🏆 Poster/Banner Dự Án

*(Chèn hình ảnh Poster hoặc Banner chuyên nghiệp của dự án tại đây.)*

****

---

## 💡 Giới Thiệu và Tầm Nhìn Dự Án

Dự án này là một **hệ thống phân loại vật thể tự động** dựa trên kiến trúc IoT, thể hiện sự kết hợp hài hòa giữa **Xử lý thị giác** (Vision Processing) và **Điều khiển cơ khí chính xác** (Precision Mechanical Control). Hệ thống được thiết kế để nhận dạng và phân loại các vật thể theo **màu sắc** (Đỏ, Vàng, Xanh) và **hình dạng** (Tròn, Vuông), hỗ trợ 6 mã sản phẩm khác nhau.

Mục tiêu chính là tạo ra một giải pháp giám sát và điều khiển từ xa, cung cấp dữ liệu vận hành thời gian thực (**Real-time Analytics**) thông qua một **Web Dashboard** hiện đại, làm nền tảng cho các ứng dụng **Nhà máy Thông minh (Smart Factory)** hoặc **Tự động hóa giáo dục**.

---

## ✨ Tính Năng Nổi Bật

* **Phân loại đa chiều:** Phân loại chính xác 6 mã sản phẩm dựa trên sự kết hợp của Màu sắc và Hình dạng:
    * **6 Mã sản phẩm:** **TD** (Tròn Đỏ), **VD** (Vuông Đỏ), **TV** (Tròn Vàng), **VV** (Vuông Vàng), **TX** (Tròn Xanh), **VX** (Vuông Xanh).
* **Giám sát & Phân tích IoT:** Web Dashboard tương tác, cập nhật qua **WebSocket**, cung cấp dữ liệu **Tốc độ (PPM)**, **Độ chính xác (Accuracy)**, thống kê biểu đồ phân phối sản phẩm (sử dụng **Chart.js**), và trạng thái cảm biến.
* **Video và Điều khiển Trực tiếp:** Truyền luồng **Video Live Stream** (MJPEG) và khả năng **Điều chỉnh góc quét Camera (Pan Control)** từ xa qua Dashboard.
* **Chẩn đoán Trạng thái:** Hiển thị trạng thái hoạt động của các cơ cấu chính (Arm, Conveyor) và cảnh báo vật lý (IR Sensor Blocked).

---

## 🛠️ Công Nghệ và Vai trò Chi tiết

Dự án sử dụng mô hình kiến trúc **Gateway-Client** phân tán giữa hai vi điều khiển để tối ưu hiệu suất.

### 1. 🧠 ESP32-CAM (Xử lý thông minh & IoT Gateway)

| Chức năng | Vai trò | Công nghệ |
| :--- | :--- | :--- |
| **Phân tích Thị giác** | Chụp ảnh, thực hiện các thuật toán nhận dạng màu sắc (RGB) và hình dạng. | OpenCV/Thuật toán Ảnh cơ bản |
| **Giao tiếp IoT** | Xây dựng **WebSocket Server** để đẩy dữ liệu JSON (REALTIME, STATUS, COMPLETED). | **WebSocket** |
| **Web Server & Stream** | Phục vụ Dashboard (HTML/JS) và cung cấp luồng **Video MJPEG**. | **HTTP Server** |
| **Giao tiếp Serial** | Gửi lệnh điều khiển cấp cao (`SORT:TD`) đến Arduino Uno. | **UART (Serial2)** |

### 2. ⚙️ Arduino Uno (Điều khiển Cơ cấu Chính xác - Slave)

| Chức năng | Vai trò | Giao tiếp |
| :--- | :--- | :--- |
| **Thực thi Lệnh** | Nhận, giải mã và thực thi các lệnh điều khiển (string) gửi từ ESP32-CAM. | **UART (Serial)** |
| **Điều khiển Cơ cấu** | Điều khiển góc quay của **Servo MG996R** và kiểm soát **Motor DC** (Băng tải). | **PWM, Digital I/O** |
| **Cảm biến Vật lý** | Đọc dữ liệu từ **Cảm biến IR** để phát hiện vật thể. | **Digital I/O** |

---

## ⚙️ Sơ Đồ Hoạt Động (System Flow Architecture)

Quá trình phân loại là một chu trình đồng bộ được điều khiển bởi luồng dữ liệu hai chiều:

****

1.  **Phát hiện:** Vật thể kích hoạt **Cảm biến IR** (hoặc cảm biến khoảng cách).
2.  **Xử lý:** **ESP32-CAM** chụp ảnh, xử lý và xác định **Mã phân loại** (`VX`).
3.  **Lệnh:** **ESP32-CAM** gửi lệnh **SORT** tới Arduino Uno qua Serial: `SORT:VX`.
4.  **Cập nhật:** **ESP32-CAM** đẩy dữ liệu thống kê (`COMPLETED`) và trạng thái hiện tại (`REALTIME`) lên **Web Dashboard** qua **WebSocket**.
5.  **Thực thi:** **Arduino Uno** nhận lệnh, điều khiển **Servo** và **Motor băng tải** để hoàn thành việc phân loại.

### Giao tiếp Dữ liệu Chính

| Kênh | Hướng | Ví dụ Lệnh/Dữ liệu | Mô tả |
| :--- | :--- | :--- | :--- |
| **Serial/UART** | ESP32 ➡️ Uno | `SORT:TV`, `SERVO:90`, `STOP` | Lệnh điều khiển vật lý cho Arduino Uno. |
| **WebSocket** | ESP32 ➡️ Web | Payload `COMPLETED` | Gửi tổng số liệu đã phân loại và thời gian chu kỳ. |
| **WebSocket** | ESP32 ➡️ Web | Payload `REALTIME` | Gửi dữ liệu nhận dạng hiện tại (Mã, R, G, B). |
| **WebSocket** | Web ➡️ ESP32 | `CMD:RESET_COUNTERS` | Lệnh điều khiển từ Dashboard. |

---

## 🖼️ Kết Quả Triển Khai Thực Tế

### 1. Hình ảnh Sản phẩm Cơ khí

*(Chèn các hình ảnh chất lượng cao về mô hình cơ khí thực tế của hệ thống phân loại.)*

* ****
* ****
* **

[Image of the color sensor and IR sensor setup]
**

### 2. Giao diện Web Dashboard

*(Chèn các ảnh chụp màn hình rõ nét của giao diện Dashboard khi hệ thống đang chạy.)*

* ****
* ****
* ****

### 3. Video Hoạt động

*(Chèn liên kết hoặc nhúng video hoạt động của hệ thống tại đây.)*

* **[Video of the sorting machine successfully classifying a variety of objects (e.g., Red Round, Yellow Square)]**
* **Link Video YouTube:** *(Chèn link của bạn tại đây)*

---

## 🚀 Hướng Dẫn Cài Đặt và Triển khai

### Yêu cầu Phần mềm
* **Arduino IDE** (hoặc PlatformIO)
* Thư viện **ESP32-CAM** (cho Board Manager)
* Thư viện **WebServer, WebSocketsServer, ArduinoJson** (cho ESP32).

### Các Bước Triển khai
1.  **Cấu hình Firmware:** Cập nhật thông tin WiFi (`WIFI_SSID`, `WIFI_PASS`) trong code ESP32-CAM.
2.  **Nạp Code:**
    * Upload code `.ino` cho **Arduino Uno** (Servo, Motor control).
    * Upload code `.ino` cho **ESP32-CAM** (IoT, WebServer).
3.  **Thiết lập Web:** Đảm bảo file Dashboard (HTML/CSS/JS) được nạp vào bộ nhớ **SPIFFS/LITTLEFS** của ESP32-CAM.
4.  **Vận hành:** Cấp nguồn và truy cập địa chỉ IP của ESP32-CAM trên trình duyệt để mở Dashboard: `http://[ESP32_IP_ADDRESS]`.

---

## 🧑‍💻 Tác Giả & Bản quyền

Dự án được phát triển bởi **Nguyễn Quang Vinh**.

* GitHub: https://github.com/VinhShindo

Dự án này được phát hành dưới giấy phép **MIT**. Xem file [LICENSE](LICENSE) để biết thêm chi tiết.
