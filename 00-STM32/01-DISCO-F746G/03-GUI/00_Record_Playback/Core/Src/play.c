/*
 * record.c
 *
 *  Created on: Sep 15, 2022
 *      Author: HP
 */

#include "play.h"

static AUDIO_OUT_BufferTypeDef  BufferCtl;
static int16_t FilePos = 0;
static __IO uint32_t uwVolume = 70;
static uint32_t play_t0 = 0;
static int16_t mono_temp[AUDIO_OUT_BUFFER_SIZE / 4]; /* Temp buffer for mono read before stereo expansion */

int tes1,tes2;

AUDIO_PLAYBACK_StateTypeDef AudioState;

WAVE_FormatTypeDef WaveReadFormat;
FILELIST_FileTypeDef FileList;

/* Private function prototypes -----------------------------------------------*/
static AUDIO_ErrorTypeDef GetFileInfo(uint16_t file_idx, WAVE_FormatTypeDef *info);
static uint8_t PlayerInit(uint32_t AudioFreq);

FILELIST_FileTypeDef FileList;
uint16_t NumObs = 0;

/**
  * @brief  Copies disk content in the explorer list.
  * @param  None
  * @retval Operation result
  */
FRESULT AUDIO_StorageParse(void)
{
  FRESULT res = FR_OK;
  FILINFO fno;
  DIR dir;
  char *fn;

  res = f_opendir(&dir, SDPath);
  FileList.ptr = 0;

  if(res == FR_OK)
  {
    while(BSP_SD_IsDetected())
    {
      res = f_readdir(&dir, &fno);
      if(res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      if(fno.fname[0] == '.')
      {
        continue;
      }
      fn = fno.fname;

      if(FileList.ptr < FILEMGR_LIST_DEPDTH)
      {
        if((fno.fattrib & AM_DIR) == 0)
        {
          if((strstr(fn, "wav")) || (strstr(fn, "WAV")))
          {
            strncpy((char *)FileList.file[FileList.ptr].name, (char *)fn, FILEMGR_FILE_NAME_SIZE);
            FileList.file[FileList.ptr].type = FILETYPE_FILE;
            FileList.ptr++;
          }
        }
      }
    }
  }
  else
  {
  	printf("cannot open dir : %d \n",res);
	  serialPrintln(&vcp,  "cannot open dir : %d",res);

  }
  NumObs = FileList.ptr;
  printf("NumbObs : %d\n",NumObs);
  serialPrintln(&vcp,"NumbObs : %d",NumObs);
  f_closedir(&dir);
  return res;
}


/**
  * @brief  Shows audio file (*.wav) on the root
  * @param  None
  * @retval None
  */
uint8_t AUDIO_ShowWavFiles(void)
{
  if(AUDIO_StorageParse() ==  FR_OK)
  {
    if(FileList.ptr > 0)
    {
      return 0;
    }
    return 1;
  }
  return 2;
}

/**
  * @brief  Gets Wav Object Number.
  * @param  None
  * @retval None
  */
uint16_t AUDIO_GetWavObjectNumber(void)
{
  return NumObs;
}

AUDIO_ErrorTypeDef AUDIO_PLAYER_Init(void)
{
  if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, uwVolume, AUDIO_FREQUENCY_48K) == 0)
  {
    return AUDIO_ERROR_NONE;
  }
  else
  {
    return AUDIO_ERROR_IO;
  }
}

/**
  * @brief  Expand mono 16-bit samples to stereo interleaved
  * @param  mono: Source mono buffer (int16_t array)
  * @param  stereo: Destination stereo buffer (int16_t array, 2x size)
  * @param  mono_samples: Number of mono samples to convert
  * @retval None
  */
static void MonoToStereo(int16_t *mono, int16_t *stereo, uint32_t mono_samples)
{
  for (uint32_t i = 0; i < mono_samples; i++)
  {
    stereo[2*i]     = mono[i];  // Left
    stereo[2*i + 1] = mono[i];  // Right
  }
}

/**
  * @brief  Initializes the Wave player.
  * @param  AudioFreq: Audio sampling frequency
  * @retval None
  */
static uint8_t PlayerInit(uint32_t AudioFreq)
{
  printf("PLAY_INIT_REQ: freq=%lu volume=%lu\r\n", AudioFreq, uwVolume);
  /* Set frequency first to configure PLL clock before SAI init */
  BSP_AUDIO_OUT_SetFrequency(AudioFreq);
  /* Initialize the Audio codec and all related peripherals */
  if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, uwVolume, AudioFreq) != 0)
  {
    printf("PLAY_INIT_DONE: FAILED\r\n");
    return 1;
  }
  else
  {
    printf("PLAY_INIT_DONE: freq=%lu init_ok\r\n", AudioFreq);
    BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
    return 0;
  }
}

/**
  * @brief  Gets the file info.
  * @param  file_idx: File index
  * @param  info: Pointer to WAV file info
  * @retval Audio error
  */
static AUDIO_ErrorTypeDef GetFileInfo(uint16_t file_idx, WAVE_FormatTypeDef *info)
{
  uint32_t bytesread;
  uint32_t duration;
  uint8_t str[FILEMGR_FILE_NAME_SIZE + 20];

  if(f_open(&SDFile, (char *)FileList.file[file_idx].name, FA_OPEN_EXISTING | FA_READ) == FR_OK)
  {
    /* Fill the buffer to Send */
    if(f_read(&SDFile, info, sizeof(WaveReadFormat), (void *)&bytesread) == FR_OK)
    {
      printf("PLAY_HDR: SR=%lu BR=%lu BA=%u BPS=%u CH=%u FileSize=%lu\r\n",
             info->SampleRate, info->ByteRate, info->BlockAlign,
             info->BitPerSample, info->NbrChannels, info->FileSize);
      return AUDIO_ERROR_NONE;
    }
    f_close(&SDFile);
  }
  return AUDIO_ERROR_IO;
}


AUDIO_ErrorTypeDef AUDIO_PLAYER_Start(uint8_t idx)
{
  uint32_t bytesread;

  f_close(&SDFile);
  if(AUDIO_GetWavObjectNumber() > idx)
  {

	GetFileInfo(idx, &WaveReadFormat);


    /*Adjust the Audio frequency */
    printf("Audio: WAV SampleRate=%lu, ByteRate=%lu, FileSize=%lu\n", 
           WaveReadFormat.SampleRate, WaveReadFormat.ByteRate, WaveReadFormat.FileSize);
    PlayerInit(WaveReadFormat.SampleRate);

    BufferCtl.state = BUFFER_OFFSET_NONE;

    /* Get Data from USB Flash Disk */
    f_lseek(&SDFile, 0);
    play_t0 = HAL_GetTick();

    /* Fill whole buffer at first time - read mono then expand to stereo */
    uint32_t mono_bytes = AUDIO_OUT_BUFFER_SIZE / 2; /* 4096 bytes mono = 2048 samples */
    if(f_read(&SDFile, mono_temp, mono_bytes, (void *)&bytesread) == FR_OK)
    {
        if(bytesread != 0)
        {
          uint32_t mono_samples = bytesread / sizeof(int16_t);
          MonoToStereo(mono_temp, (int16_t*)BufferCtl.buff, mono_samples);
          uint32_t stereo_samples = mono_samples * 2; /* L+R pairs */
          uint32_t stereo_bytes = stereo_samples * sizeof(int16_t); /* 8192 bytes */
          printf("PLAY_START: mono_samples=%lu stereo_samples=%lu stereo_bytes=%lu\r\n",
                 mono_samples, stereo_samples, stereo_bytes);
          BSP_AUDIO_OUT_Play((uint16_t*)BufferCtl.buff, stereo_bytes);
          BufferCtl.fptr = bytesread;
          return AUDIO_ERROR_NONE;
        }
    }
  }
  return AUDIO_ERROR_IO;
}



AUDIO_ErrorTypeDef playStart(int index)
{
	FRESULT res;
	uint8_t wav_res;
	AUDIO_ErrorTypeDef ares;

	printf("init play\n");
	serialPrintln(&vcp,"init play");
	res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 1);
	if(res != FR_OK)
	{
		printf("error mount, code error : %d\n",res);
	  serialPrintln(&vcp,"error mount, code error : %d",res);
	  return AUDIO_ERROR_IO;
	}

	wav_res = AUDIO_ShowWavFiles();
	if (wav_res != 0)
	{
			printf("AUDIO_ShowWavFiles failed: %u\n", wav_res);
			serialPrintln(&vcp, "AUDIO_ShowWavFiles failed: %u", wav_res);
			return AUDIO_ERROR_IO;
	}

	ares = AUDIO_PLAYER_Start(index);
	if(ares != AUDIO_ERROR_NONE)
	{
		printf("error start audio : %d\n",ares);
	  serialPrintln(&vcp,"error start audio : %d",ares);
	}

	printf("audio start gaes\n");
	serialPrintln(&vcp,"audio start gaes");

	return (AUDIO_ERROR_NONE);
}

AUDIO_ErrorTypeDef playProcess(void)
{
  uint32_t bytesread, elapsed_time;
  AUDIO_ErrorTypeDef audio_error = AUDIO_ERROR_NONE;
  static uint32_t prev_elapsed_time = 0xFFFFFFFF;


    if(BufferCtl.fptr >= WaveReadFormat.FileSize)
    {
      BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
      audio_error = AUDIO_ERROR_EOF;
    }

    if(BufferCtl.state == BUFFER_OFFSET_HALF)
    {
      uint32_t mono_bytes = AUDIO_OUT_BUFFER_SIZE / 4; /* 2048 bytes mono = 1024 samples */
      if(f_read(&SDFile, mono_temp, mono_bytes, (void *)&bytesread) != FR_OK)
      {
        BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
        return AUDIO_ERROR_IO;
      }
      uint32_t mono_samples = bytesread / sizeof(int16_t);
      MonoToStereo(mono_temp, (int16_t*)&BufferCtl.buff[0], mono_samples);
      BufferCtl.state = BUFFER_OFFSET_NONE;
      BufferCtl.fptr += bytesread;
    }

    if(BufferCtl.state == BUFFER_OFFSET_FULL)
    {
      uint32_t mono_bytes = AUDIO_OUT_BUFFER_SIZE / 4; /* 2048 bytes mono = 1024 samples */
      if(f_read(&SDFile, mono_temp, mono_bytes, (void *)&bytesread) != FR_OK)
      {
        BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
        return AUDIO_ERROR_IO;
      }
      uint32_t mono_samples = bytesread / sizeof(int16_t);
      MonoToStereo(mono_temp, (int16_t*)&BufferCtl.buff[AUDIO_OUT_BUFFER_SIZE / 2], mono_samples);
      BufferCtl.state = BUFFER_OFFSET_NONE;
      BufferCtl.fptr += bytesread;
    }

    /* Display elapsed time */
    elapsed_time = BufferCtl.fptr / WaveReadFormat.ByteRate;
    if(prev_elapsed_time != elapsed_time)
    {
      prev_elapsed_time = elapsed_time;
      printf("PLAY_RATE: fptr=%lu elapsed_sec=%lu est_Bps=%lu\r\n",
             BufferCtl.fptr, elapsed_time,
             (elapsed_time ? BufferCtl.fptr / elapsed_time : 0));
    }
  return audio_error;
}


void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
    BufferCtl.state = BUFFER_OFFSET_FULL;
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
    BufferCtl.state = BUFFER_OFFSET_HALF;
}

AUDIO_ErrorTypeDef playStop(void)
{
  uint32_t play_ms = HAL_GetTick() - play_t0;
  printf("PLAY_REAL: ms=%lu file_bytes=%lu expected_ms=%lu\r\n",
         play_ms,
         WaveReadFormat.FileSize,
         WaveReadFormat.ByteRate ? (1000UL * WaveReadFormat.FileSize / WaveReadFormat.ByteRate) : 0);

  AudioState = AUDIO_STATE_STOP;
  FilePos = 0;

  BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
  f_close(&SDFile);

  printf("selesai play\r\n");
  serialPrintln(&vcp,"selesai play");
  return AUDIO_ERROR_NONE;
}
