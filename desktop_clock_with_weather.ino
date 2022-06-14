//project references:
//https://did321.github.io/2021/07/28/ESP32-TFT-%E5%88%86%E5%85%89%E6%A3%B1%E9%95%9C%E5%AE%9E%E7%8E%B0%E9%80%8F%E6%98%8E%E5%B0%8F%E7%94%B5%E8%A7%86/
//https://www.electronicshub.org/esp32-oled-display/
//https://randomnerdtutorials.com/esp32-http-get-open-weather-map-thingspeak-arduino/
//Author: Yiang Gong
//5/30/2021

//includes-------------------------------
#include "config.h"
#include <stdio.h>
#include <iostream>
#include <string.h>
using namespace std;
#include <time.h>

//dependencies for ssd1306
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//dependencies for Time adn WIFI
#include <NTPClient.h>
#include <WiFi.h> // for WiFi shield
#include <WiFiUdp.h>

//dependencies for Openweathermap
#include <Arduino_JSON.h>
#include <HTTPClient.h>

//constants------------------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI   23
#define OLED_CLK    18
#define OLED_DC     16
#define OLED_CS     5
#define OLED_RESET  17

char buf[80];
char buf_old[80];

String jsonBuffer;
double tempTemp, tempTemp_old;
String weather, weather_old;
unsigned long timer, timer_old;

int currentHour, currentMinute, currentSecond, currentHour_old=0, currentMinute_old=0, currentSecond_old=0;
char timeBuff[10] = "10:10:10";

//objects----------------------------------
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

WiFiClient client;
HTTPClient http;

JSONVar weatherObj;

//==========================================
void setup(){
  Serial.begin(115200);
  //SSD1306 init
  if(!display.begin(SSD1306_SWITCHCAPVCC)){
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.display();

  //wifi init
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ){
    delay ( 500 );
    LEDPrintString("WIFI Connecting to: ", 1, 1, 1);
    LEDPrintString(ssid, 1, 1, 15);
    Serial.print ( "." );
  }
  Serial.println( "Wifi Connected!" );
  display.clearDisplay();
  LEDPrintString("WIFI Connected Yeahhhhh!", 1, 1, 1);
  delay ( 1000 );
  
  //NTP init
  timeClient.begin();
  timeClient.setTimeOffset(-14400);//US eastern time

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverPath.c_str());

  display.clearDisplay();
  LEDPrintString(city + ", " + countryCode, 1, 1, 30);

}

void loop(){
  if(WiFi.status()== WL_CONNECTED){
    timeClient.update();
    getRealDate();
    getWeather();
    
    //display.clearDisplay();
    displayTime();
    displayDate();
    displayTemp();
    displayWeather();
  }else{
    display.clearDisplay();
    LEDPrintString("WiFi Disconnected", 1, 1, 1);
    Serial.println("WiFi Disconnected");
  }
  delay(10000);//openweathermap query time need to be less than 60/min
}

//print real date to buffer string
//Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
void getRealDate(){
  time_t rawtime = timeClient.getEpochTime();
  struct tm  ts;
  ts = *localtime(&rawtime);
  //strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
  strftime(buf, sizeof(buf), "%a %Y-%m-%d", &ts);
}

void displayTime(){
  
  currentHour = timeClient.getHours();
  currentMinute = timeClient.getMinutes();
  
  if(currentMinute != currentMinute_old){
    clearRegion(1, 10, 127, 28);
    sprintf(timeBuff, "%02d:%02d", currentHour, currentMinute);
    LEDPrintString(timeBuff, 2, 35, 10);
    display.display();

    currentMinute_old = currentMinute;
  }
}

void displayDate(){
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(1,1);
  if(strcmp(buf,buf_old)!=0){
    clearRegion(1, 1, 127, 8);
    display.println(buf);
    memcpy(buf_old, buf, sizeof(buf));
  }
}

void LEDPrintString(String text, int textSize, int cursorX, int cursorY){
  //display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(cursorX,cursorY);
  display.println(text);
  //Serial.println(buf);

  display.display();
}

void LEDPrintString(String text, int textSize, int cursorX, int cursorY, int color){
  //display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(color);
  display.setCursor(cursorX,cursorY);
  display.println(text);
  //Serial.println(buf);

  display.display();
}

String httpGETRequest() {
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    //Serial.print("HTTP Response code: ");
    //Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  //http.end();

  return payload;
}

void getWeather(){
  jsonBuffer = httpGETRequest();
  weatherObj = JSON.parse(jsonBuffer);
  if (JSON.typeof(weatherObj) == "undefined") {
    Serial.println("Parsing input failed!");
  }

  Serial.print("Temperature: ");
  Serial.println(weatherObj["main"]["temp"]);

  //Serial.print("Humidity: ");
  //Serial.println(weatherObj["main"]["humidity"]);
  
  //Serial.print("Weather: ");
  //Serial.println(weatherObj["weather"][0]["description"]);

  //Serial.print("Weather: ");
  //Serial.println(weatherObj);
}

void displayTemp(){
  tempTemp = (double)weatherObj["main"]["temp"]-273.15;
  if(tempTemp != tempTemp_old){
    clearRegion(1, 40, 127, 49);
    LEDPrintString("Temp: " + JSON.stringify(floor(tempTemp*100.0)/100) + " C", 1, 1, 40);
    tempTemp_old = tempTemp;
  }
  
}

void displayWeather(){
  weather = JSON.stringify(weatherObj["weather"][0]["description"]);
  if(weather!=weather_old){
    clearRegion(1, 50, 128, 64);
    LEDPrintString("Weather:" + weather, 1, 1, 50);
    weather_old = weather;
  }
}

void clearRegion(int x1, int y1, int x2, int y2){
  int j = y1;
  for(x1;x1<x2;x1++){
    for(y1;y1<y2;y1++){
      display.drawPixel(x1, y1, SSD1306_BLACK);
      //display.display();
    }
    y1 = j;
  }
  display.display();
}
