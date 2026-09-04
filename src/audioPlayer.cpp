#include "audioPlayer.h"
#include <driver/i2s.h>

//list to store the valid music files
String validFiles[] = {".wav", ".mp3", ".flac"};

//stores the current song that is being playd and its extension format
String currentSong;
String currentExtension;

uint32_t sampleRate = 44100;
float waveFreq = 440.0;
float pi = PI;

void setupI2S()
{
  //configures the i2s struct with all of the needed information
  i2s_config_t i2s_config = {
    //configures the mode using a 2bit thing so that the esp32 is the master and transmits the data
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = sampleRate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true
  };

  //configures the i2s pinout
  i2s_pin_config_t pin_config = {
    .mck_io_num = I2S_PIN_NO_CHANGE,
    .bck_io_num = 32,
    .ws_io_num = 33,
    .data_out_num = 19,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  esp_err_t driverResult = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  esp_err_t pinResult = i2s_set_pin(I2S_NUM_0, &pin_config);

  Serial.println(driverResult);
  Serial.println(pinResult);
} 


bool isValidFile(String selected)
{
  String selectedItem = selected;
  //checks whether the selected item is a valid file
  //gets the index of the last '.' to isolate the extension 
  int extensionIdx = selectedItem.lastIndexOf(".");
  String extension = selectedItem.substring(extensionIdx, selectedItem.length());
  for (const String& file : validFiles)
  {
    //goes through every file and checks if the extension equals any of them and sets fileCheck to true if yes
    if (file == extension)
    {
      //it was found so we can just return true since obviously there cant be another file
      currentSong = selectedItem;
      currentExtension = extension;
      return true;
    }
  }
  //if it gets to this point then the file obvi wasnt found
  return false;
}