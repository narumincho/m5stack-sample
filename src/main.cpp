#include <M5Stack.h>
#include <MPU6050_tockn.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <ArduinoJson.h>

WiFiClientSecure client;
StaticJsonDocument<4> jsonDoc;

const char *ssid = "elecom2g-b4cd21";  // WiFi SSID
const char *password = "dnmhjuda6wmf"; // WiFi PW

MPU6050 mpu6050(Wire);

const int32_t micPin = 36;

const int32_t micNumberOfSample = 500;
const int32_t micSamplingFrequency = 40000;
const uint32_t displayWidth = 320;
const uint32_t displayHeight = 240;
const uint8_t fontNumber = 2;
const int16_t fontHeight = M5.Lcd.fontHeight(fontNumber);
const int16_t micGraphOffset = fontHeight * 2;
const int16_t dataTableOffset = micGraphOffset + 130;
uint8_t toneTable[] = {0, 2, 4, 5, 7, 9, 11};
String toneNameTable[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
enum class State
{
    Start,
    WaitWiFiConnection,
    WaitRequestSend,
    RequestSending
};
State state = State::Start;

void setup()
{
    M5.begin();
    M5.Speaker.begin();

    M5.Lcd.setBrightness(20);
    M5.Lcd.setTextFont(fontNumber);
    M5.Lcd.setTextColor(TFT_ORANGE);
    M5.Lcd.println("Read data and Send data on http. \nnarumincho");
    pinMode(micPin, INPUT);

    Wire.begin();
    Wire.setClock(400000); // Set I2C frequency to 400kHz

    mpu6050.begin();
}

void drawBar(float value, uint8_t offsetY)
{
    value = constrain(value, -1, 1);
    if (value < 0)
    {
        M5.Lcd.fillRect(
            (displayWidth + 7 * 15) / 2 + (displayWidth - 7 * 15) * value / 2,
            offsetY,
            (displayWidth - 7 * 15) * -value / 2,
            fontHeight - 1,
            GREENYELLOW);
        return;
    }
    M5.Lcd.fillRect((displayWidth + 7 * 15) / 2,
                    offsetY,
                    (displayWidth - 7 * 15) * value / 2,
                    fontHeight - 1,
                    GREENYELLOW);
}

void loop()
{
    M5.update();
    mpu6050.update();
    M5.Speaker.update();

    uint16_t micBuffer[micNumberOfSample];
    uint32_t sampling_period_us = round(1000000 * (1.0 / micSamplingFrequency));

    for (int i = 0; i < micNumberOfSample; i++)
    {
        uint64_t start = micros();
        micBuffer[i] = analogRead(micPin);
        while ((micros() - start) < sampling_period_us)
        {
        };
    }

    int32_t micSum = 0;
    for (int i = 0; i < displayWidth; i++)
    {
        uint16_t data = micBuffer[i * micNumberOfSample / displayWidth];
        M5.Lcd.drawLine(i, micGraphOffset, i, micGraphOffset + 128, BLACK);
        M5.Lcd.drawLine(i, micGraphOffset + 128 / 2, i, micGraphOffset + data / 2, WHITE);
        micSum += abs(data - 128);
    }
    M5.Lcd.setCursor(0, dataTableOffset);
    M5.Lcd.setTextColor(WHITE);

    M5.Lcd.fillRect(7 * 8, dataTableOffset, 7 * 7, fontHeight, NAVY);
    M5.Lcd.printf("micSum: %7d\n", micSum);

    M5.Lcd.fillRect(50, dataTableOffset + fontHeight * 1, displayWidth - 50, fontHeight, NAVY);
    float accX = mpu6050.getAccX();
    M5.Lcd.printf("accX  : %+1.4f\n", accX);
    drawBar(accX, dataTableOffset + fontHeight);

    float accY = mpu6050.getAccY();
    M5.Lcd.fillRect(50, dataTableOffset + fontHeight * 2, displayWidth - 50, fontHeight, NAVY);
    M5.Lcd.printf("accY  : %+1.4f\n", accY);
    drawBar(accY, dataTableOffset + fontHeight * 2);

    float accZ = mpu6050.getAccZ();
    M5.Lcd.fillRect(50, dataTableOffset + fontHeight * 3, displayWidth - 50, fontHeight, NAVY);
    M5.Lcd.printf("accZ  : %+1.4f\n", accZ);
    drawBar(accZ, dataTableOffset + fontHeight * 3);

    int8_t octave = floor((1 - accY) * 5 / 2) + 1;
    int8_t toneInOctave = toneTable[(int8_t)floor((1 - accX) * 7 / 2)] + (M5.BtnC.read() ? 1 : 0);
    int32_t tone = octave * 12 + toneInOctave;

    M5.Lcd.fillRect(0, displayHeight - fontHeight, 100, fontHeight, NAVY);
    double frequency = 32.703 * pow(pow(2, (double)1 / 12), tone);
    M5.Lcd.print(toneNameTable[toneInOctave] + String(octave) + " " + String(frequency) + "Hz");
    if (M5.BtnB.read())
    {
        M5.Speaker.setVolume(1);
        M5.Speaker.tone(frequency, 32);
    }
    else
    {
        M5.Speaker.mute();
    }

    switch (state)
    {
    case State::Start:
        WiFi.begin(ssid, password);
        state = State::WaitWiFiConnection;
        M5.Lcd.fillRect(100, displayHeight - fontHeight, 50, fontHeight, NAVY);
        M5.Lcd.println(" Waiting for Wi-Fi connection");
        break;
    case State::WaitWiFiConnection:
        if (WiFi.status() == WL_CONNECTED)
        {
            M5.Lcd.fillRect(100, displayHeight - fontHeight, 50, fontHeight, NAVY);
            M5.Lcd.println("Wi-Fi connected!");
            state = State::WaitRequestSend;
        }
        break;
    case State::WaitRequestSend:
        jsonDoc["micSum"] = micSum;
        jsonDoc["accX"] = accX;
        jsonDoc["accY"] = accY;
        jsonDoc["axxZ"] = accZ;
        String data;
        serializeJson(jsonDoc, data);
        const char *host = "us-central1-smart-house-dash.cloudfunctions.net";
        const char *path = "/api";
        if (client.connect(host, 443))
        {
            client.println("POST " + (String)path + " HTTP/1.1");
            client.println("host: " + String(host));
            client.println("accept: text/html");
            client.println("user-agent: M5 STACK FIRE program by narumincho");
            client.println("content-type: application/json;");
            client.println("content-length: " + String(data.length()));
            client.println();
            client.println(data);
            delay(10);
            String response = client.readString();
            M5.Lcd.fillRect(100, displayHeight - fontHeight, 50, fontHeight, NAVY);
            M5.Lcd.print(response);
            client.stop();
        }
        else
        {
            M5.Lcd.fillRect(100, displayHeight - fontHeight, 50, fontHeight, NAVY);
            M5.Lcd.print("connection error");
        }
    }

    delay(24);
}