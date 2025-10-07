#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include <Arduino.h>
#include "driver/i2s.h"
#include <Wire.h>

// Configuration for I2S
#define I2S_NUM         I2S_NUM_0
#define SAMPLE_RATE     44000
#define I2S_BCK_IO      26
#define I2S_WS_IO       25
#define I2S_DATA_IN_IO  22

#define ADC_I2C_ADDR 0x4E // Replace with your ADC's I2C address : 1001 1100 for a write. 1001 1101 for a read.

// Software I2C ADC
#define SOFT_I2C_SDA 21
#define SOFT_I2C_SCL 23

// EEPROM configuration for file counter
#define EEPROM_SIZE 4
#define EEPROM_ADDR 0

// SPI configuration for SD card
#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_CS   15

#define DURATION_SECONDS 600  // 10 min

SPIClass hspi(HSPI);
File dataFile;

unsigned long lastFileTime = 0;
uint32_t fileCounter = 0;

TwoWire SoftWire(0);

const size_t bufferSize = 4096;

void adcWriteRegister(uint8_t reg, uint8_t value) {
  SoftWire.beginTransmission(ADC_I2C_ADDR);
  SoftWire.write(reg);
  SoftWire.write(value);
  SoftWire.endTransmission();
}

void setupI2S() {
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = 0,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false
  };

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCK_IO,
    .ws_io_num = I2S_WS_IO,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_DATA_IN_IO
  };

  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM, &pin_config);
}

void loadFileCounter() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(EEPROM_ADDR, fileCounter);
}

void saveFileCounter() {
  EEPROM.put(EEPROM_ADDR, fileCounter);
  EEPROM.commit();
}

String getFileName() {
  char filename[32];
  sprintf(filename, "/rec_%05d.bin", fileCounter);
  return String(filename);
}

bool createNewFile() {
  if (dataFile) dataFile.close();

  String filename = getFileName();

  dataFile = SD.open(filename, FILE_WRITE);
  if (!dataFile) {
    return false;
  }

  lastFileTime = millis();
  fileCounter++;
  saveFileCounter();
  return true;
}

void setup() {
  delay(100);
  setupI2S();

  // Initialize software I2C
  SoftWire.begin(SOFT_I2C_SDA, SOFT_I2C_SCL);

  delay(300);
  adcWriteRegister(0x02, 0x81);
  delay(300);
  adcWriteRegister(0x07, 0x38); // 00111000 , 7-6 : TDM mode ,  5-4 : 32 bits
  delay(300);
  adcWriteRegister(0x73, 0xC0);
  delay(300);
  adcWriteRegister(0x74, 0xC0);
  delay(300);
  adcWriteRegister(0x75, 0xE0);
  delay(300);
  adcWriteRegister(0x3D, 0x00); //gain, entre 00 et 80
  delay(300);
  adcWriteRegister(0x42, 0x00); //gain aussi, entre 00 et 80
  delay(300);

  // Page 0, Registre 11 → Register 0x0B
  // FS_SEL = 0x00 = 48 kHz
  adcWriteRegister(0x0B, 0x00);
  delay(300);

  loadFileCounter();

  hspi.begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_CS);
  if (!SD.begin(HSPI_CS, hspi)) {
    while (1);
  }

  if (!createNewFile()) {
    while (1);
  }
}

void loop() {
  // Nouveau fichier toutes les 10 minutes
  if (millis() - lastFileTime >= DURATION_SECONDS * 1000UL) {
    createNewFile();
  }

  static unsigned long lastFlushTime = 0;

  uint8_t buffer[bufferSize];
  size_t bytes_read;

  // Read from I2S
  i2s_read(I2S_NUM, &buffer, sizeof(buffer), &bytes_read, portMAX_DELAY);

  dataFile.write(buffer, bytes_read);

  // Flush toutes les 200ms pour s'assurer que les données sont écrites sur la SD
  if (millis() - lastFlushTime >= 200) {
    dataFile.flush();  // Sécurise l'écriture (optionnel selon ton besoin de perf/sécurité)
    lastFlushTime = millis();
  }
}