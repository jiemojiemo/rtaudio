#pragma once
#include "RtAudio.h"
#include <assert.h>
#include <iostream>
using std::cout;
using std::endl;
/*!
*  ��Ƶ��ʽ
*  Ŀǰֻ֧�� float32��int16��int32
*/
typedef unsigned long AudioFormat;
//static const AudioFormat AUDIO_SIN8	= RTAUDIO_SINT8;	//��Ӧ������ʹ��char
static const AudioFormat AUDIO_SIN16 = RTAUDIO_SINT16;		//��Ӧ������ʹ��short
															//static const AudioFormat AUDIO_SIN24 = RTAUDIO_SINT24;	//��Ӧ������ʹ��...��Ҳ��֪��
static const AudioFormat AUDIO_SIN32 = RTAUDIO_SINT32;		//��Ӧ������ʹ��int
static const AudioFormat AUDIO_FLOAT32 = RTAUDIO_FLOAT32;	//��Ӧ������ʹ��float
															//static const AudioFormat AUDIO_FLOAT64 = RTAUDIO_FLOAT64;	//��Ӧ������ʹ��double

															//¼��ʱ����Ҫ�Ĳ���
struct RecordParameters
{
	unsigned int nSeconds;
	unsigned int nChannels;
	unsigned int sampleRate;
	unsigned int framesPerBuffer;
	AudioFormat audioFormat;
};

//����pcm�ļ�ʱ��Ҫ�Ĳ���
struct PcmPlayParameters
{
	unsigned int nChannels;
	unsigned int sampleRate;
	unsigned int framesPerBuffer;
	AudioFormat audioFormat;
};

//���pcm���ݵĽṹ��
template <typename T>
struct PcmData
{
	T* buffer;						//���������ݵ�ַ
	unsigned long bufferBytes;		//ÿһ֡��Ҫ��bytes
	unsigned long totalFrames;		//һ���ж���֡
	unsigned long frameCounter;		//֡ ������
	unsigned long channels;			//������
};

#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ ) || defined( __WINDOWS_WASAPI__ )
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

#define TRY_CATCH_RTAUDIOERROR(x) try {x;}catch(RtAudioError& e){cout<<'\n'<<e.getMessage()<<endl;}