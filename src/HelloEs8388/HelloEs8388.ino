/*
  ESP32-LyraT I2S ES8388 loopback example
  This simple example demonstrates using the I2S library in combination
  with the ES8388 codec on the ESP32-LyraT board to record and play back
  audio data.

  Don't forget to enable the PSRAM in the Tools menu!

  Created for arduino-esp32 on 20 Dec, 2023
  by Lucas Saavedra Vaz (lucasssvaz)
*/

#include <ESP_I2S.h>
#include "Wire.h"
#include "key.h"
#include "ES8388.h"
#include "xl9555.h"
#include "spi_sdcard.h"
// #include "FS.h"
#include <SD.h>
#include "myfs.h"

/* Pin definitions */

const uint32_t I2s_FREQ = 44100;

/* I2S */
const uint8_t I2S_MCLK = 3;   /* Master clock */
const uint8_t I2S_SCK = 46;    /* Audio data bit clock */
const uint8_t I2S_WS = 9;    /* Audio data left and right clock */
const uint8_t I2S_SDOUT = 10; /* ESP32 audio data output (to speakers) */
const uint8_t I2S_SDIN = 14;  /* ESP32 audio data input (from microphone) */

/* PA */
const uint8_t PA_ENABLE = 21; /* Power amplifier enable */
I2SClass i2s;
ES8388 es8388;
uint8_t *wav_buffer;
size_t wav_size;
File myWavFile;

uint16_t constantsVolume = 20;
uint16_t constantsBalance = 500;

/*测试音频*/
extern const unsigned char _ac2591[180044UL + 1];

File myWavFileTemp;
const int myWavFileBufferSize = 512*1024;  // 每次从SD卡读取的字节数（可以调整）
byte myWavFileAudioBuffer[myWavFileBufferSize];

void setup() {
  // Initialize the serial port
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  key_init();
  xl9555_init();
  if (sdcard_init() == 1)    /* 检测不到SD卡 */    
  {
    Serial.println("Failed to initialize SD CARD!");
  }
  pinMode(PA_ENABLE, OUTPUT);
  digitalWrite(PA_ENABLE, HIGH);

  Serial.println("Initializing I2S bus...");
  // Set up the pins used for audio input
  i2s.setPins(I2S_SCK, I2S_WS, I2S_SDOUT, I2S_SDIN, I2S_MCLK);


  // Initialize the I2S bus in standard mode
  if (!i2s.begin(I2S_MODE_STD, 44100, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO, I2S_STD_SLOT_BOTH)) {
    Serial.println("Failed to initialize I2S bus!");
    return;
  }

  Serial.println("Initializing ES8388...");
  if (!es8388.begin(i2s)) {
    Serial.println("Failed to initialize ES8388!");
    return;
  }
  Serial.println("Initializing xl9555_io_config...");


  es8388.playWAV((uint8_t *)_ac2591, sizeof(_ac2591),constantsVolume,constantsBalance);
  delay(2000);
  Serial.println("Application complete.");
}

void loop() {
    if (KEYBoot == 0)
    {
      Serial.println("按下boot按键");
      // Record 5 seconds of audio data
      wav_buffer = es8388.recordWAV(5, &wav_size);

      Serial.println("Recording complete. Playing audio data in 2 seconds.");
      delay(2000);

      // Play the audio data
      Serial.println("Playing audio data...");
      uint64_t audioSize = wav_size / (1024);
      Serial.printf("audio Size: %lluKB\n", audioSize);
      es8388.playWAV(wav_buffer, wav_size,constantsVolume,constantsBalance);

      // Create a file on the SD card
      File fileTemp = SD.open("/MUSIC/temp.WAV", FILE_WRITE);
      if (!fileTemp) {
        Serial.println("Failed to open file for writing!");
        return;
      }

      Serial.println("Writing audio data to file...");

      // Write the audio data to the file

      if (fileTemp.write(wav_buffer, wav_size) != wav_size) {
        Serial.println("Failed to write audio data to file!");
        return;
      }

      // Close the file
      fileTemp.close();

      Serial.println("Application complete.");
    }
    if (KEY0_Press == 0)
    {
      Serial.println("按下KEY0按键");
      palyWavDemo();
    }
    if (KEY1_Press == 0)
    {
      Serial.println("按下KEY1按键");
      es8388.playWAV((uint8_t *)_ac2591, sizeof(_ac2591),constantsVolume,constantsBalance);
    }
    delay(1000);
}
void palyWavDemo(void)
{
  // Create a file on the SD card
  myWavFileTemp = SD.open("/MUSIC/temp.WAV",FILE_READ);
  // myWavFileTemp = SD.open("/MUSIC/风起天阑.WAV",FILE_READ);
  if (!myWavFileTemp) {
    Serial.println("Failed to open file!");
    return;
  }

  unsigned long fileSize = myWavFileTemp.size();
  
  // 将字节转换为KB
  float fileSizeKB = fileSize / 1024.0;
  
  // 打印文件大小
  Serial.print("文件 myWavFileTemp.txt 大小为: ");
  Serial.print(fileSizeKB);
  Serial.println(" KB");
  // Write the audio data to the file
  size_t wavSizeTemp = myWavFileTemp.size();
  // 打印文件大小
  Serial.print("文件 myWavFileTemp.txt 大小为: ");
  Serial.print(wavSizeTemp/1024);
  Serial.println(" KB");

  myWavFileTemp.seek(44);

  while (myWavFileTemp.available()) {
      // 每次读取一块数据到缓冲区
      int bytesRealSize = myWavFileTemp.read(myWavFileAudioBuffer, myWavFileBufferSize);
      // 打印文件大小
      Serial.print("文件 bytesRealSize 大小为: ");
      Serial.print(bytesRealSize);
      Serial.println(" KB");
      // 调用你现有的播放方法来播放缓冲区的数据
      // 示例：playAudioBuffer(audioBuffer, bytesRead);
      // 你需要将这部分替换为实际的播放代码
      playAudioBuffer(myWavFileAudioBuffer, bytesRealSize);

      // 可以根据播放设备的要求加上延时或者检查播放是否完成
  }

  myWavFileTemp.close();
  // es8388.playWAV(wavBufferTemp, myWavFileTemp.size(),constantsVolume,constantsBalance);
  // delete[] wavBufferTemp;
  // xl9555_pin_set(BEEP,IO_SET_LOW);
  // xl9555_pin_set(BEEP,IO_SET_HIGH);
}
void playAudioBuffer(byte *buffer, int bytesRealSize) {
  // 这里替换为你实际的音频播放代码
  // 比如将数据写入DAC，或者通过PWM输出播放音频
    // 示例：假设音频数据可以直接通过PWM或DAC输出
  es8388.playWAV((uint8_t *)buffer, sizeof(bytesRealSize),constantsVolume,constantsBalance);
}
