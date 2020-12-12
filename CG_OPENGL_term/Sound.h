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
����(�ʵ�)
* ���� ����� ������ �� �� �̻� ��� ���, �Ƹ� ������ ���̴�.
(�Ҹ��� �ȳ��� ���̴�)

���α׷� ���� �����, �� �ѹ�
1

CSound::Init();
    cs
�ڵ尡 ����Ǿ�� �Ѵ�.

���α׷� ���� ����,
1

CSound::Release();
    cs
�ڵ尡 ����Ǿ�� �Ѵ�.


��ü ������
1

CSound *m_bgm = new CSound("assets/MainBGM.mp3", false);
    cs
�� ���� �Ѵ�.

ù ���ڴ� exe���� �Ҹ� ���� ���,
�ι�° ���ڴ� �Ҹ��� ���� �ݺ� ������� �����̴�.
(true : ���� �ݺ� ���, false : �� ���� ���)


��ü �Ҹ���
1

delete m_bgm;
    cs
�� ���� �Ѵ�.


���α׷� ��ƾ ����
1

m_bgm->Update();
    cs
�ڵ尡 �־�� �Ѵ�.
(������ Update �Լ��� ȣ���ؾ� �Ѵ�)


�Ҹ� ��� ������ ������ ����.
1. �Ҹ� ���(Play)
2. �Ҹ� �Ͻ� ����(Pause) -> �Ҹ� �ٽ� ���(Resume)
3. �Ҹ� ��� ����(Stop)

stop() �Ŀ��� �ٽ� play() �ؾ߸� �Ҹ��� ���´�.
�Ͻ� ���� �� �ٽ� ����� pause() -> resume() ������ �����ؾ� �ϰ�, ���ѹ� ������ �� �ִ�.


�Ҹ� ũ�� ������ volumeUp(), volumeDown()�� ȣ���Ͽ� ����� �� �ִ�.

https://dev.dwer.kr/2020/06/c-fmod-library-with-visual-studio.html
*/