#include <M5Stack.h>
#include <MPU6050_tockn.h>
#include <Wire.h>

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

void setup()
{
    M5.begin();
    M5.Speaker.begin();
    M5.Speaker.setVolume(20);

    M5.Lcd.setBrightness(20);
    M5.Lcd.setTextFont(fontNumber);
    M5.Lcd.setTextColor(TFT_ORANGE);
    M5.Lcd.println("Read data and Send data on http. \nnarumincho");
    pinMode(micPin, INPUT);

    Wire.begin();
    Wire.setClock(400000); // Set I2C frequency to 400kHz

    mpu6050.begin();
}

void loop()
{
    M5.update();
    mpu6050.update();

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

    int32_t sum = 0;
    for (int i = 0; i < displayWidth; i++)
    {
        uint16_t data = micBuffer[i * micNumberOfSample / displayWidth];
        M5.Lcd.drawLine(i, micGraphOffset, i, micGraphOffset + 128, BLACK);
        M5.Lcd.drawLine(i, micGraphOffset + 128 / 2, i, micGraphOffset + data / 2, WHITE);
        sum += abs(data - 128);
    }
    M5.Lcd.setCursor(0, dataTableOffset);
    M5.Lcd.setTextColor(WHITE);

    M5.Lcd.fillRect(7 * 8, dataTableOffset, 7 * 7, fontHeight, ORANGE);
    M5.Lcd.printf("micSum: %7d\n", sum);

    M5.Lcd.fillRect(7 * 8, dataTableOffset + fontHeight, 7 * 7, fontHeight, ORANGE);
    float accX = mpu6050.getAccX();
    M5.Lcd.printf("accX  : %+1.4f\n", accX);
    M5.Lcd.fillRect((displayWidth + 7 * 15) / 2, dataTableOffset + fontHeight, (displayWidth - 7 * 15) * accX / 2, fontHeight, GREENYELLOW);

    float accY = mpu6050.getAccY();
    M5.Lcd.fillRect(7 * 8, dataTableOffset + fontHeight * 2, 7 * 7, fontHeight, ORANGE);
    M5.Lcd.printf("accY  : %+1.4f\n", accY);
    M5.Lcd.fillRect((displayWidth + 7 * 15) / 2, dataTableOffset + fontHeight * 2, (displayWidth - 7 * 15) * accY / 2, fontHeight, GREENYELLOW);

    float accZ = mpu6050.getAccY();
    M5.Lcd.fillRect(7 * 8, dataTableOffset + fontHeight * 3, 7 * 7, fontHeight, ORANGE);
    M5.Lcd.printf("accZ  : %+1.4f\n", accZ);
    M5.Lcd.fillRect((displayWidth + 7 * 15) / 2, dataTableOffset + fontHeight * 3, (displayWidth - 7 * 15) * accZ / 2, fontHeight, GREENYELLOW);

    M5.Speaker.tone(accZ * 1000, 16);
    delay(24);
}