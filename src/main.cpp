/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "ImageData.h"
#include <stdlib.h>
#include "WiFi.h"
#include <HTTPClient.h>

const char *ssid = "37C3-open";
// https://cloud.chemnitz.freifunk.net/s/wXSZokxmCq8WCPs/download/test_8bit_sw.bmp
String HOST_NAME = "https://cloud.chemnitz.freifunk.net";              // CHANGE IT
String PATH_NAME = "/s/FLqfzPxSDjD7Hpx/download/test_8bit_sw_640.bmp"; // CHANGE IT

HTTPClient http;

void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

const unsigned char *doHttpRequest()
{
  Serial.println("Start request");
  http.begin(HOST_NAME + PATH_NAME); // HTTP

  int httpCode = http.GET();
  Serial.print("Http Code: ");
  Serial.println(httpCode);

  if (httpCode > 0)
  {
    // file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      int size = http.getSize();
      Serial.print("Response Payload Size: ");
      Serial.println(size);

      String payload = http.getString();

      // Length (with one extra character for the null terminator)
      unsigned int str_len = payload.length();
      Serial.print("Response Payload length: ");
      Serial.println(str_len);
      if (str_len < 10000)
      {
        Serial.print("Payload zu klein!?");
        //  Panic!
        while (1)
          ;
      }

      // Serial.println("toCharArray..");
      // // char char_array[str_len];
      // const int zu_viel = 10000; // 125 lt. HEX Dump
      // String payload_nach_header = payload.substring(10000, 20000);
      unsigned char char_array_ohne_header[10000]; // str_len - zu_viel];
      return char_array_ohne_header;
      // payload_nach_header.toCharArray(char_array_ohne_header, 10000); // str_len);

      // 800 x 480 = 384.000
      // erste 40 Byte sind BMP Header
      // 800 x 480 = 384.000
      // gImage_7in5 ist 30720
      // haben gerade    30850
      // sind also         130 zu viel

      // Serial.println("Caste dein erstes Kind auf Int..");
      // unsigned char unsig_char_array_ohne_header[10000]; // str_len - zu_viel];
      // for (int i = 0; i < 10000; i++)
      // {
      //   unsig_char_array_ohne_header[i] = (unsigned char)char_array_ohne_header[i];
      // }

      Serial.println("Write to display");
      // Paint_DrawBitMap(gImage_7in5);
      // Paint_DrawBitMap((unsigned char *) char_array_ohne_header);
      Serial.println("request done");
    }
    else
    {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET/POST... code: %d\n", httpCode);
    }
  }
  else
  {
    Serial.printf("[HTTP] GET/POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
}

/* Entry point ----------------------------------------------------------------*/
void setup()
{
  Serial.begin(115200);
  initWiFi();
  DEV_Delay_ms(500);

  DEV_Module_Init();
  Serial.println("Init EINK");
  EPD_7IN5_Init();
  EPD_7IN5_Clear();
  DEV_Delay_ms(500);

  Serial.println("Create a new image cache");
  UBYTE *BlackImage;
  /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
  UWORD Imagesize = ((EPD_7IN5_WIDTH % 8 == 0) ? (EPD_7IN5_WIDTH / 8) : (EPD_7IN5_WIDTH / 8 + 1)) * EPD_7IN5_HEIGHT;
  if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL)
  {
    Serial.println("Failed to apply for black memory...");
    while (1)
      ;
  }
  Serial.println("Paint_NewImage");
  // START BLOCK
  Paint_NewImage(BlackImage, EPD_7IN5_WIDTH, EPD_7IN5_HEIGHT, 0, WHITE);

  Serial.println("show image for array");
  Paint_SelectImage(BlackImage);
  Paint_Clear(WHITE);
  // Paint_DrawBitMap(gImage_7in5);
  Paint_DrawBitMap(doHttpRequest());
  Serial.println("EPD_7IN5_Display BlackImage");
  EPD_7IN5_Display(BlackImage);
  // END BLOCK
  DEV_Delay_ms(500);

  // Serial.println("doHttpRequest");
  // Paint_SelectImage(BlackImage);
  // Paint_Clear(WHITE);
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
  // Paint_DrawBitMap(gImage_7in5);
  //  reset nach 1 Min
  // DEV_Delay_ms(60 * 1000);
  // TODO Reset ..
  // ESP.restart();
}
