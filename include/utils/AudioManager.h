//
// Created by Víctor Jiménez Rugama on 1/21/22.
//

#ifndef INC_4PT_AUDIOMANAGER_H
#define INC_4PT_AUDIOMANAGER_H
#include <cstdint>
#include <cstdlib>

class AudioManager{
private:
    bool term;
    static int32_t audio;
    void * currentAudioWav;
    size_t currentCount;
    bool cancelled;
    void runLoop();
    int playAudioPrivate();
public:
    static int initAudioManager();
    static AudioManager * mainAudioManager;
    int playAudio(void * audioWav,  size_t count);
    int playAudio(const char * dir);
    static void * loadAudioFile(const char * dir, size_t * count);
    int termAudioManager();
    ~AudioManager();
};
#endif //INC_4PT_AUDIOMANAGER_H
