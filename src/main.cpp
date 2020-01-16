#include <M5Stack.h>
#include <WiFi.h>

// https://deviceplus.jp/hobby/entry_k02/

void setup()
{

  M5.begin(); //M5Stackオブジェクトの初期化
  M5.Lcd.print("scan SSID");

  WiFi.mode(WIFI_STA); //STAモード（子機）として使用
  WiFi.disconnect();   //Wi-Fi切断
  delay(100);
}

void loop()
{
  int n = WiFi.scanNetworks(); //ネットワークをスキャンして数を取得

  M5.Lcd.fillScreen(BLACK); //LCDをクリア
  M5.Lcd.setCursor(0, 0);   //カーソル位置設定

  if (n == 0)
  {
    //ネットワークが見つからないとき
    M5.Lcd.println("no networks found");
  }
  else
  {
    //ネットワークが見つかったとき
    M5.Lcd.print(n);
    M5.Lcd.println(" networks found\n");
    for (int i = 0; i < n; i++)
    {
      M5.Lcd.print(i + 1);
      M5.Lcd.print(": ");
      M5.Lcd.print(WiFi.channel(i)); //チャンネルを表示
      M5.Lcd.print("CH (");
      M5.Lcd.print(WiFi.RSSI(i)); //RSSI(受信信号の強度)を表示
      M5.Lcd.print(")");
      M5.Lcd.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*"); //暗号化の種類がOPENか否か
      M5.Lcd.print("  ");
      M5.Lcd.print(WiFi.SSID(i)); //SSID(アクセスポイントの識別名)を表示
      M5.Lcd.println("");
      delay(10);
    }
  }
  delay(3000);
}