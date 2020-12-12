#pragma once
#ifndef _CSOUND_H_
#define _CSOUND_H_

#pragma comment(lib, "fmod_vc.lib")
#include <fmod.h>

constexpr float SOUND_MAX{ 1.0f };
constexpr float SOUND_MIN{ 0.0f };
constexpr float SOUND_DEFAULT{ 0.5f };
constexpr float SOUND_WEIGHT{ 0.1f };

class CSound {
private:
    static FMOD_SYSTEM* g_sound_system;

    FMOD_SOUND* m_sound;
    FMOD_CHANNEL* m_channel;

    float m_volume;
    FMOD_BOOL m_bool;
public:
    CSound(const char* path, bool loop);
    ~CSound();

    static void Init();
    static void Release();

    void play();
    void pause();
    void resume();
    void stop();
    void setVolume(float vol);
    void volumeUp();
    void volumeDown();

    void Update();
};

#endif



/*
사용법(필독)
* 여기 기재된 내용을 한 개 이상 어길 경우, 아마 실패할 것이다.
(소리가 안나올 것이다)

프로그램 최초 실행시, 딱 한번
1

CSound::Init();
    cs
코드가 실행되어야 한다.

프로그램 종료 직전,
1

CSound::Release();
    cs
코드가 실행되어야 한다.


객체 생성은
1

CSound *m_bgm = new CSound("assets/MainBGM.mp3", false);
    cs
와 같이 한다.

첫 인자는 exe기준 소리 파일 경로,
두번째 인자는 소리를 무한 반복 재생할지 여부이다.
(true : 무한 반복 재생, false : 한 번만 재생)


객체 소멸은
1

delete m_bgm;
    cs
와 같이 한다.


프로그램 루틴 내에
1

m_bgm->Update();
    cs
코드가 있어야 한다.
(꾸준히 Update 함수를 호출해야 한다)


소리 재생 패턴은 다음과 같다.
1. 소리 재생(Play)
2. 소리 일시 정지(Pause) -> 소리 다시 재생(Resume)
3. 소리 재생 종료(Stop)

stop() 후에는 다시 play() 해야만 소리가 나온다.
일시 정지 후 다시 재생은 pause() -> resume() 순으로 실행해야 하고, 무한번 실행할 수 있다.


소리 크기 조절은 volumeUp(), volumeDown()을 호출하여 사용할 수 있다.

https://dev.dwer.kr/2020/06/c-fmod-library-with-visual-studio.html
*/