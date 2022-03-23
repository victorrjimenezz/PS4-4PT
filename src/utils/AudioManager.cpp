//
// Created by Víctor Jiménez Rugama on 1/21/22.
//

#include "../../include/utils/AudioManager.h"

#define DR_WAV_IMPLEMENTATION
#include "../../include/utils/DrWav.h"
#define PARAMS16 ORBIS_AUDIO_OUT_PARAM_FORMAT_S16_STEREO

#include "../../include/utils/logger.h"
#include "../../include/utils/threadPool.h"

#include <orbis/AudioOut.h>

int32_t AudioManager::audio = 0;

int AudioManager::playAudioPrivate(){

    // Play the sample
    int32_t sOffs = 0;
    auto * audioWav = (drwav_int16 *)currentAudioWav;
    drwav_int16 *pSample;
    //bool finished = false;

    // Play the song in a loop
    while (sOffs < currentCount && !cancelled) {
        pSample = &audioWav[sOffs];

        /* Output audio */
        sceAudioOutOutput(audio, NULL);	// NULL: wait for completion

        if (sceAudioOutOutput(audio, pSample) < 0) {
            LOG << "Failed to output audio";
            currentAudioWav = nullptr;
            currentCount =0;
            return -1;
        }

        sOffs += 256 * 2;


        //if (sOffs >= currentCount)
        //    sOffs = 0;
    }
    currentAudioWav = nullptr;
    currentCount =0;
    cancelled = false;
    return 0;
}

int AudioManager::playAudio(void *audioWav, const size_t count) {
    cancelled = true;
    while(cancelled)
        continue;
    currentAudioWav = audioWav;
    currentCount = count;
    return 0;
}

int AudioManager::playAudio(const char * dir){

    drwav_int16 * audioFile = nullptr;
    size_t audioSize;
    int ret;
    if(loadAudioFile(dir,&audioSize) == nullptr)
        return -1;

    cancelled = true;

    while(cancelled)
        continue;

    currentCount = audioSize;
    currentAudioWav = audioFile;

    while(currentAudioWav!= nullptr)
        continue;

    if(audioFile != nullptr)
        free(audioFile);

    return ret;
}

void * AudioManager::loadAudioFile(const char * dir, size_t * count){
    drwav wav;
    drwav_int16 * audioData = nullptr;
    if (!drwav_init_file(&wav, dir, NULL)) {
        LOG << "UNABLE TO LOAD AUDIO FILE " <<dir;
        return audioData;
    }

    // Calculate the sample count and allocate a buffer for the sample data accordingly
    *count = wav.totalPCMFrameCount * wav.channels;
    audioData = (drwav_int16 *)malloc(*count * sizeof(uint16_t));

    // Decode the wav into pSampleData
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, audioData);

    drwav_uninit(&wav);
    return audioData;
}

int AudioManager::termAudioManager(){
    term = true;
    if(audio!=0)
        sceAudioOutClose(audio);
    audio = 0;
    return 0;
}
AudioManager::~AudioManager(){
    termAudioManager();
}
void AudioManager::runLoop() {
    currentAudioWav = nullptr;
    term = false;
    cancelled = false;
    currentCount = 0;
    replay:
    cancelled = false;
    while(currentAudioWav == nullptr && !term && !cancelled)
        continue;

    if(currentAudioWav!= nullptr)
        playAudioPrivate();

    if(!term)
        goto replay;
}

AudioManager::AudioManager() {
    int rc;
    rc = sceAudioOutInit();

    if (rc != 0) {
        LOG << ("[ERROR] Failed to initialize audio output");
        return;
    }

    // Open a handle to audio output device
    audio = sceAudioOutOpen(ORBIS_USER_SERVICE_USER_ID_SYSTEM, ORBIS_AUDIO_OUT_PORT_TYPE_MAIN, 0, 256, 48000, PARAMS16);

    if (audio <= 0) {
        LOG << "[ERROR] Failed to open audio on main port\n";
        return;
    }
    threadPool::addJob([&] {this->runLoop();},true);

}

