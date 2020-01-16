#include <M5Stack.h>
#include <WiFiClientSecure.h>

WiFiClientSecure client;

const char *ssid = "";     // WiFi SSID
const char *password = ""; // WiFi PW
const char *host = "us-central1-smart-house-dash.cloudfunctions.net";
const char *path = "/api";

boolean wifiConnection()
{
    WiFi.begin(ssid, password);
    int count = 0;
    Serial.print("Waiting for Wi-Fi connection");
    while (count < 20)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println();
            Serial.println("Connected!");
            return (true);
        }
        delay(500);
        M5.Lcd.print(".");
        count++;
    }
    return false;
}

String httpsGet(String path)
{
    String data = "{\"sorena\": \"arena\"}";
    if (client.connect(host, 443))
    {
        client.println("POST " + path + " HTTP/1.1");
        client.println("host: " + String(host));
        client.println("accept: text/html");
        client.println("user-agent: M5 STACK FIRE program by narumincho");
        client.println("content-type: application/json;");
        client.println("content-length: " + String(data.length()));
        client.println();
        client.println(data);
        delay(10);
        String response = client.readString();
        client.stop();
        return response;
    }
    else
    {
        return "ERROR";
    }
}

void setup()
{
    M5.begin();
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(WHITE);

    M5.Lcd.println("request sample");
    if (wifiConnection())
    {
        M5.Lcd.println("GET to https://" + String(host) + path);
        String response = httpsGet(path);
        M5.Lcd.println(response);
        M5.Lcd.println("finish!");
    }
}

void loop()
{
}
