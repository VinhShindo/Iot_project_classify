#include <Servo.h>
#include <ArduinoJson.h>
// #include <SoftwareSerial.h>

// SoftwareSerial espSerial(A4, A5);
const size_t JSON_DOC_SIZE = 512;
StaticJsonDocument<JSON_DOC_SIZE> doc;

#define PIN_DE 3
#define PIN_TAY_DON 5
#define PIN_CANG 6
#define PIN_KEP 9
#define PIN_CAM 12

#define RELAY_PIN 2
#define TRIG_PIN 8
#define ECHO_PIN 10
#define IR_PIN 11

#define S0 A2
#define S1 A3
#define S2 A0
#define S3 A1
#define sensorOut 7
#define LED 4

Servo servoDe;
Servo servoTayDon;
Servo servoCang;
Servo servoKep;
Servo servoCam;

const int GOC_DE_BAN_DAU = 0;
const int GOC_TAY_DON_BAN_DAU = 90;
const int GOC_CANG_BAN_DAU = 70;
const int GOC_KEP_BAN_DAU = 90;
const int GOC_CAM_BAN_DAU = 90;

const int GOC_KEP_DONG = 90;
const int GOC_KEP_MO = 120;

const int GOC_CAM_MIN = 0;
const int GOC_CAM_MAX = 180;
const int TOC_DO_CAM = 10;
const int STEP_CAM = 1;

const float D_REF = 8;
const float THRESHOLD = 2.5;
const int SCAN_POINTS = 20;

const int DELAY_CHUNG = 500;
const int DELAY_KEP_VAT = 1500;
const int DELAY_THAO_TAC_NHO = 300;

const int SO_LAN_GAP_TOI_DA = 2;

struct VungDatVat {
    String maVatThe;
    int gocXoayDe;
    int gocVuonCang;
    int gocHaTayDon;
};

const VungDatVat GocPhanLoai[] = {
    {"T_D", 130, 80, 40},
    {"V_D", 120, 80, 10},
    {"T_V", 105, 70, 70},
    {"V_V", 105, 70, 10},
    {"T_X", 80, 85, 50},
    {"V_X", 80, 90, 10}
};

const int SO_LUONG_VAT_THE = sizeof(GocPhanLoai) / sizeof(GocPhanLoai[0]);

VungDatVat vatTheCanXuLy = {"KHAC", 90, 80, 110};

float profile_data[SCAN_POINTS];

int minR = 20, maxR = 200;
int minG = 25, maxG = 210;
int minB = 30, maxB = 220;

int redFrequency, greenFrequency, blueFrequency;
int R, G, B;

int count_TD = 0;
int count_VD = 0;
int count_TV = 0;
int count_VV = 0;
int count_TX = 0;
int count_VX = 0;
int count_SKIP = 0;

unsigned long previousMillis = 0;
int currentStage = 0;
bool chuTrinhDangChay = false;
bool chuTrinhThanhCong = false;
bool shouldSendCompleted = false;

int gocCamHienTai = GOC_CAM_BAN_DAU;
int huongCam = 1;
unsigned long previousCamMillis = 0;
int soLanGap = 0;

int currentServoAngle = GOC_CAM_BAN_DAU;
bool armActive = false;
bool conveyorActive = false;
bool irBlocked = false;
bool alertStatus = false;
int retryCount = 0;
int totalSorted = 0;
float systemAccuracy = 0.0;
int sortingSpeedPPM = 0;
long cycleTimeMs = 0;

unsigned long lastJsonSendRealtime = 0;
const long JSON_SEND_INTERVAL_REALTIME = 500;

bool previousArmActive = false;
bool previousConveyorActive = false;
bool previousIrBlocked = false;

void mapAndWrite(Servo& servo, int angle);
void moveServo(Servo& servo, int targetAngle, int delayTime);
float readDistanceCM();
float averageHeight(float data[], int size);
int mapColor(int value, int minVal, int maxVal);
String detectColor();
VungDatVat timGocTheoMa(String maVatThe);
void tangBienDem(String maVatThe);
void quanLyChuTrinh();
void xuLyVatTheIR();
void scanObject();
String createJsonPayload_Realtime();
String createJsonPayload_Status(bool forceSend);
void handleIncomingData(String data);
void setup();
void loop();

String createJsonPayload_Realtime() {
    doc.clear();
    doc["type"] = "REALTIME";
    doc["code"] = vatTheCanXuLy.maVatThe;
    doc["shape"] = (vatTheCanXuLy.maVatThe.charAt(0) == 'T' ? "TRON" : (vatTheCanXuLy.maVatThe.charAt(0) == 'V' ? "VUONG" : "N/A"));
    doc["colorName"] = (vatTheCanXuLy.maVatThe.endsWith("D") ? "DO" : (vatTheCanXuLy.maVatThe.endsWith("V") ? "VANG" : (vatTheCanXuLy.maVatThe.endsWith("X") ? "XANH DUONG" : "N/A")));
    doc["R"] = R;
    doc["G"] = G;
    doc["B"] = B;
    doc["retryCount"] = retryCount;
    doc["currentStage"] = currentStage;
    
    String output;
    serializeJson(doc, output);
    return output;
}

String createJsonPayload_Status(bool forceSend) {
    bool hasChanged = (armActive != previousArmActive) || (conveyorActive != previousConveyorActive) || (irBlocked != previousIrBlocked);

    if (forceSend || hasChanged) {
        doc.clear();
        doc["type"] = "STATUS";
        doc["armActive"] = armActive;
        doc["conveyorActive"] = conveyorActive;
        doc["irBlocked"] = irBlocked;
        doc["alertStatus"] = alertStatus;

        previousArmActive = armActive;
        previousConveyorActive = conveyorActive;
        previousIrBlocked = irBlocked;

        String output;
        serializeJson(doc, output);
        return output;
    }
    return "";
}

String createJsonPayload_Completed() {
    doc.clear();
    doc["type"] = "COMPLETED";
    
    // Dữ liệu sự kiện cuối chu trình
    doc["code"] = vatTheCanXuLy.maVatThe;
    doc["isSuccess"] = chuTrinhThanhCong; // Cờ báo thành công/thất bại
    doc["cycleTimeMs"] = cycleTimeMs;
    doc["retryCount"] = soLanGap;

    // Dữ liệu đếm tổng hợp (Đồng bộ hóa/Khởi tạo trạng thái)
    JsonObject stats = doc.createNestedObject("stats");
    stats["total"] = totalSorted;
    stats["skipped"] = count_SKIP;
    stats["TD"] = count_TD;
    stats["VD"] = count_VD;
    stats["TV"] = count_TV;
    stats["VV"] = count_VV;
    stats["TX"] = count_TX;
    stats["VX"] = count_VX;

    String output;
    serializeJson(doc, output);
    return output;
}

void handleIncomingData(String data) {
    
    if (data.startsWith("SERVO_POS:")) {
        int newAngle = data.substring(10).toInt();
        if (newAngle != currentServoAngle) {
            currentServoAngle = newAngle;
            mapAndWrite(servoCam, currentServoAngle);
        }
    }
    else if (data.indexOf("RESET") != -1) {
        totalSorted = 0;
        count_TD = count_VD = count_TV = count_VV = count_TX = count_VX = count_SKIP = 0;
        systemAccuracy = 0.0;
        sortingSpeedPPM = 0;
    }
    else if (data.indexOf("STOP") != -1) {
        digitalWrite(RELAY_PIN, HIGH);
        conveyorActive = false;
        chuTrinhDangChay = false;
        currentStage = 0;
    }
}

void mapAndWrite(Servo& servo, int angle) {
    int pulseWidth = map(angle, 0, 180, 500, 2500);
    servo.writeMicroseconds(pulseWidth);
}

void moveServo(Servo& servo, int targetAngle, int delayTime) {
    int currentAngle = servo.read();
    int step = (targetAngle > currentAngle) ? 1 : -1;

    for (int angle = currentAngle; angle != targetAngle; angle += step) {
        mapAndWrite(servo, angle);
        delay(delayTime);
    }
}

float readDistanceCM() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    float distance = duration * 0.034 / 2.0;
    if (distance > 200 || distance <= 0) return D_REF;
    return distance;
}

float averageHeight(float data[], int size) {
    float sum = 0;
    for (int i = 0; i < size; i++) sum += data[i];
    return sum / size;
}

int mapColor(int value, int minVal, int maxVal) {
    value = constrain(value, minVal, maxVal);
    return map(value, minVal, maxVal, 255, 0);
}

String detectColor() {
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    redFrequency = pulseIn(sensorOut, LOW, 30000);

    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    greenFrequency = pulseIn(sensorOut, LOW, 30000);

    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    blueFrequency = pulseIn(sensorOut, LOW, 30000);

    R = mapColor(redFrequency, minR, maxR);
    G = mapColor(greenFrequency, minG, maxG);
    B = mapColor(blueFrequency, minB, maxB);

    if (R > 80 && G < 100 && B < 100)
        return "Do";
    else if (R < 80 && G < 130 && B > 100)
        return "Xanh Duong";
    else if (R > 130 && G > 100 && B < 180)
        return "Vang";
    else
        return "Khac";
}

VungDatVat timGocTheoMa(String maVatThe) {
    for (int i = 0; i < SO_LUONG_VAT_THE; i++) {
        if (GocPhanLoai[i].maVatThe == maVatThe) {
            return GocPhanLoai[i];
        }
    }

    return {"KHAC", 90, 80, 100};
}


void tangBienDem(String maVatThe) {
    if (maVatThe == "T_D") count_TD++;
    else if (maVatThe == "V_D") count_VD++;
    else if (maVatThe == "T_V") count_TV++;
    else if (maVatThe == "V_V") count_VV++;
    else if (maVatThe == "T_X") count_TX++;
    else if (maVatThe == "V_X") count_VX++;
}

void quanLyChuTrinh() {
    armActive = chuTrinhDangChay;
    conveyorActive = (digitalRead(RELAY_PIN) == LOW);
    irBlocked = (digitalRead(IR_PIN) == LOW);
    if (!chuTrinhDangChay) return;

    unsigned long currentMillis = millis();

    switch (currentStage) {
        case 0:
            soLanGap++;
            currentStage = 10;
            break;

        case 10:
            moveServo(servoDe, 20, 15);
            moveServo(servoKep, GOC_KEP_MO, 15);
            moveServo(servoCang, 120, 15);
            moveServo(servoTayDon, 10, 15);

            previousMillis = currentMillis;
            currentStage = 11;
            break;

        case 11:
            if (currentMillis - previousMillis >= DELAY_CHUNG) {
                moveServo(servoCang, 75, 15);
                
                previousMillis = currentMillis;
                currentStage = 20;
            }
            break;

        case 20:
            if (currentMillis - previousMillis >= DELAY_CHUNG) {
                moveServo(servoKep, GOC_KEP_DONG, 15);
                previousMillis = currentMillis;
                currentStage = 21;
            }
            break;

        case 21:
            if (currentMillis - previousMillis >= DELAY_KEP_VAT) {
                moveServo(servoTayDon, 90, 15); 
                moveServo(servoCang, GOC_CANG_BAN_DAU, 15);
                
                previousMillis = currentMillis;
                currentStage = 30;
            }
            break;

        case 30:
            if (currentMillis - previousMillis >= DELAY_CHUNG) {
                moveServo(servoDe, vatTheCanXuLy.gocXoayDe, 15);
                moveServo(servoTayDon, vatTheCanXuLy.gocHaTayDon, 15);
                moveServo(servoCang, vatTheCanXuLy.gocVuonCang, 15);

                previousMillis = currentMillis;
                currentStage = 40;
            }
            break;
            
        case 40:
            if (currentMillis - previousMillis >= 1500) {
                moveServo(servoKep, GOC_KEP_MO, 15);
                previousMillis = currentMillis;
                currentStage = 50;
            }
            break;

        case 50:
            if (currentMillis - previousMillis >= DELAY_THAO_TAC_NHO) {
                moveServo(servoKep, GOC_KEP_DONG, 15);
                moveServo(servoCang, GOC_CANG_BAN_DAU, 15);
                moveServo(servoTayDon, GOC_TAY_DON_BAN_DAU, 15);

                previousMillis = currentMillis;
                currentStage = 55;
            }
            break;
            
        case 55:
            if (currentMillis - previousMillis < DELAY_CHUNG) {
                return;
            }
            
            if (digitalRead(IR_PIN) == HIGH) {
                previousMillis = currentMillis;
                currentStage = 60;
                
            } else {
                if (soLanGap < SO_LAN_GAP_TOI_DA) {
                    previousMillis = currentMillis;
                    currentStage = 0;
                } else {
                    count_SKIP++;
                    digitalWrite(RELAY_PIN, HIGH);
                    conveyorActive = true;
                    previousMillis = currentMillis;
                    currentStage = 99;
                }
            }
            break;

        case 60:
            if (currentMillis - previousMillis >= 1000) {
                digitalWrite(RELAY_PIN, HIGH);
                conveyorActive = false;
                moveServo(servoDe, GOC_DE_BAN_DAU, 15);
                previousMillis = currentMillis;
                currentStage = 99;
                chuTrinhThanhCong = true;
            }
            break;

        case 99:
            if (currentMillis - previousMillis >= 500) {
                long elapsed = currentMillis - (previousMillis - 500);
                cycleTimeMs = elapsed;

                if (chuTrinhThanhCong) {
                    tangBienDem(vatTheCanXuLy.maVatThe);
                    totalSorted++;
                } else {
                    if (soLanGap >= SO_LAN_GAP_TOI_DA && !chuTrinhThanhCong) {
                         count_SKIP++;
                    }
                }
                
                sortingSpeedPPM = (cycleTimeMs > 0) ? (int)(60000.0 / cycleTimeMs) : 0;
                shouldSendCompleted = true;
                chuTrinhDangChay = false;
                chuTrinhThanhCong = false;
                currentStage = 0;
            }
            break;
    }
}

void xuLyVatTheIR() {
    if (chuTrinhDangChay) {
        return;
    }

    digitalWrite(RELAY_PIN, LOW);
    conveyorActive = false;
    irBlocked = true;

    if (vatTheCanXuLy.maVatThe == "KHAC") {
        digitalWrite(RELAY_PIN, HIGH);
        delay(400); 
        return;
    }

    if (!chuTrinhDangChay) {
        chuTrinhDangChay = true;
        soLanGap = 0;
        chuTrinhThanhCong = false;
        currentStage = 0;
    }
}

void scanObject() {
    for (int i = 0; i < SCAN_POINTS; i++) {
        float current_distance = readDistanceCM();
        profile_data[i] = D_REF - current_distance;
        delay(50);
    }

    float mean_height = averageHeight(profile_data, SCAN_POINTS);
    String shape = (mean_height < 5.35) ? "Hinh Tron" : "Hinh Vuong";
    String color = detectColor();
    String maVatThe = "KHAC";

    if (shape == "Hinh Tron") {
        if (color == "Do") {maVatThe = "T_D"; }
        else if (color == "Vang") {maVatThe = "T_V"; }
        else if (color == "Xanh Duong") {maVatThe = "T_X"; }
    } else if (shape == "Hinh Vuong") {
        if (color == "Do") {maVatThe = "V_D"; }
        else if (color == "Vang") {maVatThe = "V_V"; }
        else if (color == "Xanh Duong") {maVatThe = "V_X"; }
    }

    vatTheCanXuLy = timGocTheoMa(maVatThe);
}

void setup() {
    Serial.begin(9600);
    // espSerial.begin(9600);

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(IR_PIN, INPUT_PULLUP);

    servoDe.attach(PIN_DE);
    servoTayDon.attach(PIN_TAY_DON);
    servoCang.attach(PIN_CANG);
    servoKep.attach(PIN_KEP);
    servoCam.attach(PIN_CAM);

    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    pinMode(sensorOut, INPUT);
    pinMode(LED, OUTPUT);

    digitalWrite(LED, HIGH);
    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);

    mapAndWrite(servoDe, GOC_DE_BAN_DAU);
    mapAndWrite(servoTayDon, GOC_TAY_DON_BAN_DAU);
    mapAndWrite(servoCang, GOC_CANG_BAN_DAU);
    mapAndWrite(servoKep, GOC_KEP_BAN_DAU);
    mapAndWrite(servoCam, GOC_CAM_BAN_DAU);

    digitalWrite(RELAY_PIN, HIGH);
    delay(1000);
}

void loop() {
    if (Serial.available()) {
        String incomingData = Serial.readStringUntil('\n');
        incomingData.trim();
        handleIncomingData(incomingData);
    }

    quanLyChuTrinh();
    irBlocked = (digitalRead(IR_PIN) == LOW);

    if (!chuTrinhDangChay) {
        float current_distance = readDistanceCM();

        if (current_distance < (D_REF - THRESHOLD)) {
            digitalWrite(RELAY_PIN, LOW);
            delay(400);

            scanObject();

            digitalWrite(RELAY_PIN, HIGH);
            delay(800);
        }
    }

    if (digitalRead(IR_PIN) == LOW) {
        xuLyVatTheIR();
    } else {
        if (irBlocked) {
            irBlocked = false;
        }
    }

    if (millis() - lastJsonSendRealtime >= JSON_SEND_INTERVAL_REALTIME) {
        lastJsonSendRealtime = millis();
        String jsonPayload = createJsonPayload_Realtime();
        // espSerial.println(jsonPayload);
        Serial.println(jsonPayload);
    }
    
    String statusPayload = createJsonPayload_Status(false);
    if (statusPayload.length() > 0) {
        // espSerial.println(statusPayload);
        Serial.println(statusPayload);
    }

    if (shouldSendCompleted) {
        String completedPayload = createJsonPayload_Completed();
        // espSerial.println(completedPayload);
        Serial.println(completedPayload);
        shouldSendCompleted = false;
    }

    delay(100);
}
