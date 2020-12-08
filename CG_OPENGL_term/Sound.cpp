#include "Sound.h"
#include <iostream>
FMOD_SYSTEM* CSound::g_sound_system;

CSound::CSound(const char* path, bool loop) {
    if (loop) {
        FMOD_RESULT result;
        result = FMOD_System_CreateSound(g_sound_system, path, FMOD_LOOP_NORMAL, 0, &m_sound);
        if (result != FMOD_OK)
        {
            std::cout << "FMOD error! (" << result << ")\n";// "<< FMOD_ErrorString(result))<<"\n";
        }
    }
    else {
        FMOD_RESULT result;
        result = FMOD_System_CreateSound(g_sound_system, path, FMOD_DEFAULT, 0, &m_sound);
        if (result != FMOD_OK)
        {
            std::cout << "FMOD error! (" << result << ")\n";// "<< FMOD_ErrorString(result))<<"\n";
        }
    }

    m_channel = nullptr;
    m_volume = SOUND_DEFAULT;
}

CSound::~CSound() {
    FMOD_Sound_Release(m_sound);
}


void CSound::Init() {
    FMOD_RESULT result;
    result = FMOD_System_Create(&g_sound_system);
    if (result != FMOD_OK) {
        std::cout << "FMOD create error! (" << result << ")\n";
    }
    else {
        std::cout << "FMOD create (" << result << ")\n";
    }
    result = FMOD_System_Init(g_sound_system, 32, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK) {
        std::cout << "FMOD init error! (" << result << ")\n";
    }
    else {
        std::cout << "FMOD init (" << result << ")\n";
    }
}

void CSound::Release() {
    FMOD_System_Close(g_sound_system);
    FMOD_System_Release(g_sound_system);
}


void CSound::play() {
    FMOD_System_PlaySound(g_sound_system, m_sound, 0, false, &m_channel);
}

void CSound::pause() {
    FMOD_Channel_SetPaused(m_channel, true);
}

void CSound::resume() {
    FMOD_Channel_SetPaused(m_channel, false);
}

void CSound::stop() {
    FMOD_Channel_Stop(m_channel);
}

void CSound::volumeUp() {
    if (m_volume < SOUND_MAX) {
        m_volume += SOUND_WEIGHT;
    }
    else {
        m_volume = SOUND_MAX;
    }

    FMOD_Channel_SetVolume(m_channel, m_volume);
}

void CSound::volumeDown() {
    if (m_volume > SOUND_MIN) {
        m_volume -= SOUND_WEIGHT;
    }
    else {
        m_volume = SOUND_MIN;
    }

    FMOD_Channel_SetVolume(m_channel, m_volume);
}

void CSound::Update() {
    FMOD_Channel_IsPlaying(m_channel, &m_bool);

    if (m_bool) {
        FMOD_System_Update(g_sound_system);
    }
}