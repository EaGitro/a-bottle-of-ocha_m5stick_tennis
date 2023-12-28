#include <Arduino.h>
#include <HTTPClient.h>
#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <math.h>

#include "WifiSecret.h"

WiFiMulti wifiMulti;
HTTPClient httpClient;


const char *ssid = ssid_ea;
const char *password = password_ea;
const char *token = token_secret;
// 加速度、格速度を入れるための変数宣言
float acc[3];        // 加速度測定値格納用（X、Y、Z）
float accOffset[3];  // 加速度オフセット格納用（X、Y、Z）
float gyro[3];       // 角速度測定値格納用（X、Y、Z）
float gyroOffset[3]; // 角速度オフセット格納用（X、Y、Z）
float roll = 0.0F;   // ロール格納用
float pitch = 0.0F;  // ピッチ格納用
float yaw = 0.0F;
const float pi = 3.14;
char buf[60];
const String dev_name = "M5StickCPlus_IMU_Data";
// 加速度、角速度　測定値取得用　関数
void readGyro() {
    M5.update();
    M5.IMU.getAccelData(&acc[0], &acc[1], &acc[2]);   // 加速度の取得
    M5.IMU.getGyroData(&gyro[0], &gyro[1], &gyro[2]); // 角速度の取得
    roll =
        atan(acc[0] / sqrt((acc[1] * acc[1]) + (acc[2] * acc[2]))) * 180 / pi;
    pitch =
        atan(acc[1] / sqrt((acc[0] * acc[0]) + (acc[2] * acc[2]))) * 180 / pi;
    yaw = atan(sqrt((acc[0] * acc[0]) + (acc[1] * acc[1])) / acc[2]) * 180 / pi;
}



/**
 *
 * @brief creates data, and stocks in arrays
 *
 */

int term_count = 0;

const int frequency_within_term = 10;
const int delay_within_term = 50;
float accs_1term[frequency_within_term][3];
float gyros_1term[frequency_within_term][3];
int timers_1term[frequency_within_term];
int counts_1term[frequency_within_term];
void get_1term_data(int frequency_within_term, int delay_within_term,
                    float accs_1term[][3], float gyros_1term[][3],
                    int *timers_1term, int *counts_1term) {
    for (int i = 0; i < frequency_within_term; i++) {
        readGyro();
        for (int xyz = 0; xyz < 3; xyz++) {
            accs_1term[i][xyz] = acc[xyz];
            gyros_1term[i][xyz] = gyro[xyz];
        }
        timers_1term[i] = millis();
        counts_1term[i] = i;
        delay(delay_within_term);
    }
}



/**
 * @brief Creates JSON from arrays, vars
 *
 * @details JSON scheme example:`
 * {
 *  term_cnt:0,
 *  freq:10,
 *  delay:50,
 *  moments:[
 *  {cnt: 0, ms: 432, acc:[0.232, -0.12, -1.2321], gyro:[0.213, -0.23, 0.123]},
 *  {...},
 * ]
 * }`
 */
void create_1term_json(char *json, const int frequency_within_term,
                       const int delay_within_term, const float accs_1term[][3],
                       const float gyros_1term[][3], const int *timers_1term,
                       const int *counts_1term) {

    sprintf(json, "{\"term_cnt\":%d,\"freq\":%d,\"delay\":%d,\"moments\":[", term_count++,
            frequency_within_term, delay_within_term);
    for (int i = 0; i < frequency_within_term; i++) {
        char acc[128] = {0};
        char gyro[128] = {0};
        array2json_arr(acc, accs_1term[i]);
        array2json_arr(gyro, gyros_1term[i]);
        if (i != frequency_within_term - 1) {
            sprintf(json, "%s{\"cnt\":%d,\"ms\":%d,\"acc\":%s,\"gyro\":%s},", json,
                    counts_1term[i], timers_1term[i], acc, gyro);
        } else {
            sprintf(json, "%s{\"cnt\":%d,\"ms\":%d,\"acc\":%s,\"gyro\":%s}]}", json,
                    counts_1term[i], timers_1term[i], acc, gyro);
        }
    }
}

void array2json_arr(char *str, const float *array) {
    sprintf(str, "[%.5f, %.5f, %.5f]", array[0], array[1], array[2]);
}



void setup() {
    // M5.begin();  // Initialize M5Stack
    // M5.Lcd.setRotation(3);
    // if (setupWifi(ssid, password)) {  // Connect to wifi.  连接到wifi
    //     M5.Lcd.printf("Success connecting to %s\n", ssid);
    // } else {
    //     M5.Lcd.printf("Connecting to %s failed\n", ssid);
    // }
    // M5.begin(); // 開始
    // Serial.begin(9600);
    // delay(500);
    // M5.Imu.Init();
    M5.begin();
    M5.IMU.Init();
    M5.Lcd.setRotation(3);           // Rotate the screen.  旋转屏幕
    wifiMulti.addAP(ssid, password); // Storage wifi configuration
                                     // information.  存储wifi配置信息
    M5.Lcd.print("\nConnecting Wifi...\n"); // print format output string on
                                            // lcd.  串口格式化输出字符串
}

void loop() {

    if (wifiMulti.run() // これを忘れてたのが原因
        == WL_CONNECTED) {

        
        /**
         * @brief 本番用 POST 消すな!
         * 
         */
        
        char json[2048] = {0};

        get_1term_data(frequency_within_term, delay_within_term, accs_1term,
                       gyros_1term, timers_1term,
                       counts_1term); // contains delay()
        create_1term_json(json, frequency_within_term, delay_within_term,
                          accs_1term, gyros_1term, timers_1term, counts_1term);

        Serial.printf("[JSON]: %s\n", json);

        char url[1024] =
            "http://eagitrodev.pythonanywhere.com/m5stick_tennis/post";

        // url
        httpClient.begin(url);
        // Content-Type
        httpClient.addHeader("Content-Type", "application/json; charset=ascii");

        int httpCode = httpClient.POST((uint8_t *)json, strlen(json));

        if (httpCode == 200) {
            String response = httpClient.getString();
            Serial.printf("[HTTP RESPONSE]: %s", response);

        } else {
            Serial.printf("[HTTP ERR CODE]: %d", httpCode);
            String response = httpClient.getString();
            Serial.printf("[HTTP RESPONSE]: %s\n", response);
        }

        httpClient.end();
        
        

        /**
         * @brief テスト用 GET 消すな!
         * 
         */
        /*
        char url[1024] =
            "https://eagitrodev.pythonanywhere.com/health/text";

        httpClient.begin(url);
        int status_code = httpClient.GET();

        String response = httpClient.getString();
        Serial.printf("[HTTP ERR CODE]: %d", status_code);
        Serial.printf("[HTTP RESPONSE]: %s\n", response);
        httpClient.end();

        delay(5000);
        */

        /**
         * @brief テスト用 POST, text
         * 
         */
        /*
        char url[1024] = "https://eagitrodev.pythonanywhere.com/health/post/text";
        
        char post_str[1024] = "hogehoge";
        // url
        httpClient.begin(url);
        // Content-Type
        httpClient.addHeader("Content-Type", "text/plain; charset=ascii");

        int httpCode = httpClient.POST((uint8_t *)post_str, strlen(post_str));
        String response = httpClient.getString();
        Serial.printf("[HTTP ERR CODE]: %d", httpCode);
        Serial.printf("[HTTP RESPONSE]: %s\n", response);
        httpClient.end();

        delay(5000);
        */
        
        /**
         * @brief テスト用 POST, JSON
         * 
         */
        /*
        char url[1024] = "https://eagitrodev.pythonanywhere.com/health/post/json";
        char json[1024] = "{\"hoge\":3}";
        httpClient.begin(url);
        // Content-Type
        httpClient.addHeader("Content-Type", "application/json; charset=ascii");

        int httpCode = httpClient.POST((uint8_t *)json, strlen(json));
        String response = httpClient.getString();
        Serial.printf("[HTTP ERR CODE]: %d", httpCode);
        Serial.printf("[HTTP RESPONSE]: %s\n", response);
        httpClient.end();

        delay(5000);
        */
        

    }
}
/*
void loop() {
    // Save 3 data in sequence to the first place of testList. 依次保存3个数据至
    // testList首位
    for (int i = 0; i < 3; i++) {
        if (addToList(token, “acc”, acc[i])) {
            M5.Lcd.printf(“Success sending %d to the list\n”, acc[i]);
        } else {
            M5.Lcd.print(“Fail to save data\n”);
        }
        delay(100);
    }
    delay(5000);
}
*/