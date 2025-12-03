<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>HỆ THỐNG PHÂN LOẠI VẬT THỂ TỰ ĐỘNG THÔNG MINH (IoT Sorting Machine)</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            line-height: 1.6;
            margin: 20px;
            background-color: #f4f7f6;
            color: #333;
        }
        .container {
            max-width: 1000px;
            margin: auto;
            background: #fff;
            padding: 30px;
            border-radius: 12px;
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.1);
        }
        h1 {
            color: #007bff;
            border-bottom: 3px solid #007bff;
            padding-bottom: 10px;
            text-align: center;
        }
        h2 {
            color: #28a745;
            margin-top: 30px;
            border-bottom: 2px solid #28a745;
            padding-bottom: 5px;
        }
        h3 {
            color: #17a2b8;
            margin-top: 20px;
        }
        .section-header {
            display: flex;
            align-items: center;
            gap: 10px;
        }
        .feature-list {
            list-style-type: none;
            padding: 0;
        }
        .feature-list li {
            background: #e9ecef;
            margin-bottom: 8px;
            padding: 10px 15px;
            border-left: 5px solid #ffc107;
            border-radius: 4px;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin: 20px 0;
            font-size: 0.9em;
            text-align: left;
        }
        th, td {
            padding: 12px 15px;
            border: 1px solid #ddd;
        }
        th {
            background-color: #007bff;
            color: white;
            font-weight: bold;
        }
        tr:nth-child(even) {
            background-color: #f2f2f2;
        }
        .architecture-flow {
            border: 1px dashed #6c757d;
            padding: 15px;
            border-radius: 8px;
            background: #f8f9fa;
        }
        .note {
            padding: 10px;
            background-color: #fff3cd;
            color: #856404;
            border: 1px solid #ffeeba;
            border-radius: 5px;
            margin-top: 15px;
        }
        .footer {
            text-align: center;
            margin-top: 40px;
            padding-top: 20px;
            border-top: 1px solid #ccc;
            color: #6c757d;
        }
    </style>
</head>
<body>
    <div class="container">
    <header>
        <h1 id="top">🤖 HỆ THỐNG PHÂN LOẠI VẬT THỂ TỰ ĐỘNG THÔNG MINH (IoT Sorting Machine)</h1>
    </header>
    <div style="margin-top: 20px;">
        <div class="section-header">
            <span style="font-size: 1.5em;">💡</span>
            <h2>Giới Thiệu Dự Án</h2>
        </div>
        <p>Dự án này là một **hệ thống phân loại vật thể tự động** dựa trên kiến trúc IoT, được thiết kế để nhận dạng và phân loại các vật thể theo **màu sắc** (Đỏ, Vàng, Xanh) và **hình dạng** (Tròn, Vuông). Hệ thống tận dụng sức mạnh xử lý của **ESP32-CAM** cho tác vụ nhận dạng và WebServer, đồng thời sử dụng **Arduino Uno** để điều khiển các cơ cấu vật lý một cách chính xác. Toàn bộ quá trình được giám sát qua một **Web Dashboard** thời gian thực.</p>
    </div>
    <div style="margin-top: 30px;">
        <div class="section-header">
            <span style="font-size: 1.5em;">✨</span>
            <h2>Tính Năng Chính</h2>
        </div>
        <ul class="feature-list">
            <li>**Phân loại 6 mã sản phẩm:** TD (Tròn Đỏ), VD (Vuông Đỏ), TV (Tròn Vàng), VV (Vuông Vàng), TX (Tròn Xanh), VX (Vuông Xanh).</li>
            <li>**Giám sát Video Trực tiếp:** Truyền luồng video từ ESP32-CAM lên Dashboard.</li>
            <li>**Web Dashboard Thời gian thực:** Cập nhật các chỉ số phân loại, **Tốc độ** (PPM), **Độ chính xác** (Accuracy), và trạng thái cảm biến qua **WebSocket**.</li>
            <li>**Điều khiển Từ xa:** Điều chỉnh góc quét camera (Pan Control) và thiết lập lại bộ đếm từ giao diện web.</li>
        </ul>
    </div>
    <div style="margin-top: 30px;">
        <div class="section-header">
            <span style="font-size: 1.5em;">🛠️</span>
            <h2>Công Nghệ và Vai trò Chi tiết</h2>
        </div>
        <p>Dự án sử dụng kiến trúc phân tán giữa hai vi điều khiển để đảm bảo hiệu suất và độ tin cậy.</p>
        <div style="margin-top: 20px;">
            <div class="section-header">
                <span style="font-size: 1.2em;">🧠</span>
                <h3>1. ESP32-CAM (Xử lý thông minh & IoT)</h3>
            </div>
            <table>
                <thead>
                    <tr>
                        <th>Chức năng</th>
                        <th>Vai trò</th>
                        <th>Công nghệ</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><strong>Nhận dạng</strong></td>
                        <td>Chụp ảnh, phân tích màu sắc (RGB) và hình dạng để xác định mã phân loại.</td>
                        <td>OpenCV/Algorithm</td>
                    </tr>
                    <tr>
                        <td><strong>Giao tiếp IoT</strong></td>
                        <td>Xây dựng <strong>WebSocket Server</strong> để gửi dữ liệu thống kê và trạng thái lên Dashboard.</td>
                        <td>WebSocket</td>
                    </tr>
                    <tr>
                        <td><strong>Giao tiếp Serial</strong></td>
                        <td>Gửi lệnh điều khiển cấp cao (<code>SORT:TD</code>) đến Arduino Uno.</td>
                        <td>UART</td>
                    </tr>
                    <tr>
                        <td><strong>Web Server</strong></td>
                        <td>Cung cấp giao diện Dashboard và luồng video trực tiếp.</td>
                        <td>HTTP</td>
                    </tr>
                </tbody>
            </table>
        </div>
        <div style="margin-top: 20px;">
            <div class="section-header">
                <span style="font-size: 1.2em;">⚙️</span>
                <h3>2. Arduino Uno (Điều khiển Cơ cấu Chính xác)</h3>
            </div>
            <table>
                <thead>
                    <tr>
                        <th>Chức năng</th>
                        <th>Vai trò</th>
                        <th>Giao tiếp</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><strong>Nhận lệnh Serial</strong></td>
                        <td>Liên tục lắng nghe và phân tích các lệnh điều khiển (string) gửi từ ESP32-CAM.</td>
                        <td>UART (Serial)</td>
                    </tr>
                    <tr>
                        <td><strong>Điều khiển Servo</strong></td>
                        <td>Dựa trên mã phân loại, điều khiển góc quay của <strong>Servo MG996R</strong> để gạt vật thể vào đúng thùng.</td>
                        <td>PWM (Servo)</td>
                    </tr>
                    <tr>
                        <td><strong>Điều khiển Băng tải</strong></td>
                        <td>Bật/Tắt <strong>Motor DC</strong> (qua Motor Driver) theo yêu cầu của chu trình phân loại.</td>
                        <td>Digital I/O</td>
                    </tr>
                </tbody>
            </table>
        </div>
        <div style="margin-top: 20px;">
            <h3>3. Thành phần Phần cứng Chính</h3>
            <p><strong>Vi điều khiển:</strong> ESP32-CAM, Arduino Uno<br>
            <strong>Cơ cấu:</strong> Servo SG90, Motor DC & Driver.<br>
            <strong>Cảm biến:</strong> Cảm biến IR (Phát hiện vật thể).<br>
            <strong>Giao diện:</strong> HTML/CSS/JavaScript, Chart.js.</p>
        </div>
    </div>
    <div style="margin-top: 30px;">
        <div class="section-header">
            <span style="font-size: 1.5em;">⚙️</span>
            <h2>Sơ Đồ Hoạt Động (System Architecture)</h2>
        </div>
        <p>Dữ liệu di chuyển theo một luồng rõ ràng để đảm bảo tốc độ và sự đồng bộ trong quá trình phân loại.</p>
        <ol class="architecture-flow">
            <li><strong>Vật thể</strong> kích hoạt <strong>Cảm biến IR</strong>.</li>
            <li><strong>ESP32-CAM</strong> chụp ảnh, xử lý và xác định mã phân loại (Ví dụ: <code>VX</code>).</li>
            <li><strong>ESP32-CAM</strong> gửi lệnh <strong>SORT</strong> tới Arduino Uno qua Serial: <code>SORT:VX</code>.</li>
            <li><strong>ESP32-CAM</strong> gửi dữ liệu <strong>COMPLETED</strong> (Thống kê tổng hợp) và <strong>REALTIME</strong> (Trạng thái hiện tại) lên Web Dashboard qua <strong>WebSocket</strong>.</li>
            <li><strong>Arduino Uno</strong> nhận lệnh, điều khiển Servo gạt và Motor băng tải để hoàn thành chu trình phân loại.</li>
        </ol>
        <div style="margin-top: 20px;">
            <h3>Giao tiếp Dữ liệu Chính</h3>
            <table>
                <thead>
                    <tr>
                        <th>Kênh</th>
                        <th>Hướng</th>
                        <th>Ví dụ Lệnh/Dữ liệu</th>
                        <th>Mô tả</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><strong>Serial/UART</strong></td>
                        <td>ESP32 ➡️ Uno</td>
                        <td><code>SORT:TV</code>, <code>SERVO:90</code>, <code>STOP</code></td>
                        <td>Lệnh điều khiển vật lý cho Arduino Uno.</td>
                    </tr>
                    <tr>
                        <td><strong>WebSocket</strong></td>
                        <td>ESP32 ➡️ Web</td>
                        <td>Payload <code>COMPLETED</code></td>
                        <td>Gửi tổng số liệu đã phân loại và thời gian chu kỳ.</td>
                    </tr>
                    <tr>
                        <td><strong>WebSocket</strong></td>
                        <td>ESP32 ➡️ Web</td>
                        <td>Payload <code>REALTIME</code></td>
                        <td>Gửi dữ liệu nhận dạng hiện tại (Mã, R, G, B).</td>
                    </tr>
                    <tr>
                        <td><strong>WebSocket</strong></td>
                        <td>Web ➡️ ESP32</td>
                        <td><code>CMD:RESET_COUNTERS</code></td>
                        <td>Lệnh điều khiển từ Dashboard.</td>
                    </tr>
                </tbody>
            </table>
        </div>
    </div>
    <div style="margin-top: 30px;">
        <div class="section-header">
            <span style="font-size: 1.5em;">🚀</span>
            <h2>Hướng Dẫn Cài Đặt và Triển khai</h2>
        </div>
        <h3>Yêu cầu Phần mềm</h3>
        <ul>
            <li><strong>Arduino IDE</strong> (hoặc PlatformIO)</li>
            <li><strong>Visual Studio Code</strong> (Để chỉnh sửa Dashboard)</li>
            <li>Thư viện <strong>Chart.js</strong> (sử dụng CDN).</li>
        </ul>
        <h3>Các Bước Triển khai</h3>
        <ol>
            <li><strong>Cấu hình Firmware:</strong>
                <ul>
                    <li>Cập nhật thông tin WiFi (<code>SSID</code> và <code>Password</code>) trong code ESP32-CAM.</li>
                    <li>Upload code <code>.ino</code> riêng biệt cho Arduino Uno (Motor, Servo control) và ESP32-CAM (IoT, Image Processing).</li>
                </ul>
            </li>
            <li><strong>Thiết lập Web:</strong> Đảm bảo file Dashboard (<code>index.html</code>, <code>style.css</code>) được nạp vào bộ nhớ **SPIFFS/LITTLEFS** của ESP32-CAM.</li>
            <li><strong>Vận hành:</strong> Cấp nguồn cho hệ thống và truy cập địa chỉ IP của ESP32-CAM trên trình duyệt để mở Dashboard giám sát.</li>
        </ol>
    </div>
    <div class="footer">
        <div class="section-header" style="justify-content: center;">
            <span style="font-size: 1.2em;">🧑‍💻</span>
            <h3>Tác Giả & Bản quyền</h3>
        </div>
        <p>Dự án được phát triển bởi <strong>Nguyễn Quang Vinh</strong>.</p>
        <p>GitHub: <a href="https://github.com/VinhShindo" target="_blank" style="color: #007bff; text-decoration: none;">https://github.com/VinhShindo</a></p>
        <div class="note" style="border-left: 5px solid #007bff; background: #e0f7fa; color: #004d40;">
            Dự án này được phát hành dưới giấy phép **MIT**. Xem file <a href="LICENSE" style="color: #007bff; font-weight: bold; text-decoration: none;">[LICENSE]</a> để biết thêm chi tiết.
        </div>
    </div>
    </div>
</body>
</html>
