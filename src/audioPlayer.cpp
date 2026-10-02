#include "audioPlayer.h"
#include <Audio.h>

//list to store the valid music files
String validFiles[] = {".wav", ".mp3", ".flac"};

//stores information about the current song being played
String currentSong;
String currentExtension;
int currentidx;

String path;

uint32_t sampleRate = 44100;
float waveFreq = 440.0;
float pi = PI;
float phase = 0;

//I2S Pins
const int I2S_BLCK = 32;
const int I2S_LRC = 33;
const int I2S_DOUT = 19; 

//creates audio object for music playback
Audio audio;

String currentSongTitle;
String currentSongArtist;

//bool to store whether a song is playing 
bool didSongEnd = false;

void audioInfo(Audio::msg_t message) 
{
    // first filters only the important meta data
    if (message.e == Audio::evt_id3data) 
    {
      //resets the song title and artist
      String metadata = message.msg;
      //checks the extensions type first to do the proper parsing checks
      if (currentExtension == ".flac")
      {
        //since strcasecmp returns 0 if they are the same then we check for "!" 
        if (!strcasecmp("TITLE=", metadata.substring(0,6).c_str()))
        {
          //sets the current song title to the metadata so to not change the actual metadata
          currentSongTitle = metadata.substring(6);
          // Serial.println(currentSongTitle);
        }
        //then checks for the artist
        if (!strcasecmp("ARTIST=", metadata.substring(0,7).c_str()))
        {
          currentSongArtist = metadata.substring(7);
          // Serial.println(currentSongArtist);
        }
      }
      else if (currentExtension == ".mp3") //then does the same for mp3 files
      {
        //since strcasecmp returns 0 if they are the same then we check for "!" 
        if (!strcasecmp("TITLE: ", metadata.substring(0,7).c_str()))
        {
          //sets the current song title to the metadata so to not change the actual metadata
          currentSongTitle = metadata.substring(7);
          // Serial.println(currentSongTitle);
        }
        //then checks for the artist
        if (!strcasecmp("ARTIST: ", metadata.substring(0,8).c_str()))
        {
          currentSongArtist = metadata.substring(8);
          // Serial.println(currentSongArtist);
        }
      }
    }

    //checks to see when a song finsihed
    if (message.e == Audio::evt_eof)
    {
      //Serial.println("END");
      didSongEnd = true;
    }
}

void audioSetup()
{
  Audio::audio_info_callback = audioInfo; 
  audio.setPinout(I2S_BLCK, I2S_LRC, I2S_DOUT);

  //sets the volume to range from 0 - 50
  audio.setVolumeSteps(50);
  
}

void audioUpdate()
{
  audio.loop();
}


void startSong(String songPath)
{
  path = songPath;
  String songName = path;

  //checks whether the selected item is a valid file
  //gets the index of the last '.' to isolate the extension 
  int extensionIdx = songName.lastIndexOf(".");
  String extension = songName.substring(extensionIdx, songName.length());

  //removes the end extension
  currentSong = songName.substring(0, extensionIdx);

  //removes the prefixing path
  currentSong.remove(0,songName.lastIndexOf('/') + 1);


  //sets the bool to false by default
  bool isValid = false;
  for (const String& file : validFiles)
  {
    //goes through every file and checks if the extension equals any of them and sets fileCheck to true if yes
    if (file == extension)
    {
      //it was found so we can just return true since obviously there cant be another file
      currentExtension = extension;
      // Serial.println(currentSong);
      //Serial.println(currentExtension);
      isValid = true;
    }
  }

  //if none of the files were valid then return
  if (!isValid)
  {
    return;
  }
  
  //sets the current song to the playback file for default
  currentSongTitle = currentSong;
  //resets the current artist 
  currentSongArtist = "";

  // Serial.println(currentSong);
  // Serial.println(path);
  bool connected = audio.connecttoFS(SD_MMC, path.c_str());


  // Serial.println(connected);
  // Serial.println(currentSongTitle);
}

void pauseToggle()
{
  audio.pauseResume();
}

void setVolume(int newVolume)
{
  audio.setVolume(newVolume);
}

void getBufferStatus()
{
  audio.inBufferStatus();
}


bool songEnded()
{
  //gets the current song state
  bool songState = didSongEnd;
  //sets the state back to playing since we automatically play a new song 
  didSongEnd = false;
  //returns the saved song state before resetting
  return songState;
}

unsigned int getCurrentTime()
{
  return audio.getAudioCurrentTime();
}




//no longer need thius code since the mp3 decoder library takes care of it for us
// void setupI2S()
// {
//   //configures the i2s struct with all of the needed information
//   i2s_config_t i2s_config = {
//     //configures the mode using a 2bit thing so that the esp32 is the master and transmits the data
//     .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
//     .sample_rate = sampleRate,
//     .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
//     .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
//     .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
//     .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
//     .dma_buf_count = 8,
//     .dma_buf_len = 64,
//     .use_apll = false,
//     .tx_desc_auto_clear = true
//   };

//   //configures the i2s pinout
//   i2s_pin_config_t pin_config = {
//     .mck_io_num = I2S_PIN_NO_CHANGE,
//     .bck_io_num = 32,
//     .ws_io_num = 33,
//     .data_out_num = 19,
//     .data_in_num = I2S_PIN_NO_CHANGE
//   };

//   esp_err_t driverResult = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
//   esp_err_t pinResult = i2s_set_pin(I2S_NUM_0, &pin_config);

// } 


//not needed for now since we have the library doing all of the buffering for us 
// void testTone()
// {
//   const int frames = 128;

//   //2 values per frame: left + right
//   int16_t audiobuffer[frames *2];

//   float phaseIncrement = 2.0 * PI * waveFreq/sampleRate;

//   for (int i = 0; i < frames; i++)
//   {
//     int16_t sample =(int16_t)(5000*sin(phase));

//     phase += phaseIncrement;

//     if (phase >= 2.0 * PI)
//     {
//       phase -= 2.0 * PI;
//     }

//     //interleaved stereo PCM
//     audiobuffer[i*2] = sample;
//     audiobuffer[i * 2 + 1] = sample;
//   }

//   size_t bytesWritten;

//   esp_err_t writeResult = i2s_write(I2S_NUM_0, audiobuffer, sizeof(audiobuffer), &bytesWritten, portMAX_DELAY);


// }