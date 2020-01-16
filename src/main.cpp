#include <M5Stack.h>
#include <driver/dac.h> //Arduino-ESP32 driver

uint32_t beep_last_time = 0;
uint8_t beep_volume = 20; //min 1, max 255
uint32_t beep_total_time = 0;

void setup()
{
    delay(2000);
    beep_total_time = millis();

    //これを実行すると、M5Stackスピーカーから破裂音が出る
    dac_output_enable(DAC_CHANNEL_1); //DAC channel 1 is GPIO #25
}

void loop()
{
    if (millis() - beep_total_time < 10000)
    { //10秒後にbeep音停止
        uint32_t b_period = millis() - beep_last_time;
        if (b_period < 500)
        {
            dac_output_voltage(DAC_CHANNEL_1, 0);
            delay(1); //約500Hz
            //delayMicroseconds(500); //約1kHz
            dac_output_voltage(DAC_CHANNEL_1, beep_volume);
            delay(1); //約500Hz
                      //delayMicroseconds(500); //約1kHz
        }
        else if (b_period >= 500 && b_period < 1000)
        {
            dac_output_voltage(DAC_CHANNEL_1, 0);
        }
        else
        {
            beep_last_time = millis();
        }
    }
    else
    {
        //これを実行するとGPIO #25のDAC outputが無効になり、スピーカーからノイズが出なくなる。
        //ただし、次にdac_output_enableを実行する時に破裂音が出る。
        dac_output_disable(DAC_CHANNEL_1);
        delay(100); //10秒無音状態
        beep_total_time = millis();

        //beep音を鳴らす為にはこれが必要。ただし破裂音が出る。
        dac_output_enable(DAC_CHANNEL_1);
    }
}