#include <M5Stack.h>

const int32_t micPin = 36;

const int32_t micNumberOfSample = 500;
const int32_t micSamplingFrequency = 40000;
const uint32_t displayWidth = 320;
const uint32_t displayHeight = 240;
const uint8_t fontNumber = 2;
const int16_t fontHeight = M5.Lcd.fontHeight(fontNumber);

void setup()
{
    M5.begin();
    M5.Lcd.setBrightness(20);
    M5.Lcd.setTextColor(TFT_ORANGE);
    M5.Lcd.println("Read mic acc data and Send data on http. by narumincho");
    pinMode(micPin, INPUT);
}

void loop()
{
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
        M5.Lcd.drawLine(i, 8, i, 8 + 128, BLACK);
        M5.Lcd.drawLine(i, 8 + 128 / 2, i, 8 + data / 2, WHITE);
        sum += abs(data - 128);
    }
    M5.Lcd.setCursor(0, 220);
    M5.Lcd.fillRect(0, 220, 7 * 6, fontHeight, BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.printf("% 7d", sum);
    delay(16);
}