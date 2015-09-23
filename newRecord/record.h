#pragma once

#include "RtAudio.h"
#include "scopeguard.h"
#include "pcmaudio.h"
#include "pcm2wav.h"
#include <assert.h>
#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ ) || defined( __WINDOWS_WASAPI__ )
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

#define TRY_CATCH_RTAUDIOERROR(x) try {x;}catch(RtAudioError& e){cout<<'\n'<<e.getMessage()<<endl;}


template <typename T>
int inputCallback(void */*outputBuffer*/, void* inputBuffer, unsigned int nBufferFrames, 
	double, RtAudioStreamStatus, void* data)
{
	PcmData<T> *iData = (PcmData<T>*) data;
	unsigned int frames = nBufferFrames;

	if (iData->frameCounter + nBufferFrames > iData->totalFrames) {
		frames = iData->totalFrames - iData->frameCounter;
		iData->bufferBytes = frames * iData->channels * sizeof(T);
	}

	unsigned long offset = iData->frameCounter * iData->channels;
	memcpy( iData->buffer+offset, inputBuffer, iData->bufferBytes );
	iData->frameCounter += frames;

	//return 2 ��ʾ¼������
	if (iData->frameCounter >= iData->totalFrames)
		return 2;
	return 0;
}

/*!
 * \class : CRecord
 *
 * \brief : ¼��
 *
 * TODO: ���ݲ�������¼��
 *
 * \note 
 *
 * \author hw
 *
 * \version 1.0
 *
 * \date ���� 2015
 *
 * Contact: 
 *
 */
template <typename T>
class CRecord {

private:
	void MemoryAlloc();
	void InitInputParams( RtAudio::StreamParameters& params, RtAudio& adc );
	void PrintRecordMessage();
	void WaitForFinish( const RtAudio& adc )const;

private:
	PcmData<T> m_data;
	RecordParameters m_params;
public:
	CRecord(const RecordParameters& params);
	~CRecord();

	void Start();
	//Ŀǰֻ֧��float32����int16����int32
	void WriteDataToRawFile(const string& fileName)const;
	void WriteDataToWavFile(const string& fileName)const;
	inline void GetTotalBytes()const {return m_data.frameCounter*m_data.channels*sizeof(T);}
	inline T* GetDataPointer()const { return m_data.buffer; }
};

template<typename T>
inline CRecord<T>::CRecord(const RecordParameters & params):m_params(params)
{
	m_data.channels = params.nChannels;
	m_data.buffer = nullptr;
	m_data.bufferBytes = params.framesPerBuffer * params.nChannels*sizeof(T);
	m_data.frameCounter = 0;
	m_data.totalFrames = (unsigned long)params.sampleRate * params.nSeconds;
}

template<typename T>
inline CRecord<T>::~CRecord()
{
	free( m_data.buffer );
}

template<typename T>
void CRecord<T>::Start()
{
	RtAudio adc;
	assert( adc.getDeviceCount() >= 1 );
	ON_SCOPE_EXIT([&]() {if (adc.isStreamOpen()) adc.closeStream(); });
	//��ʼ��������
	RtAudio::StreamParameters iParams;
	InitInputParams( iParams, adc );
	//����ʱ����
	m_data.frameCounter = 0;
	//����Ƶ����ͬʱ����ȫ�Ĳ����쳣����
	TRY_CATCH_RTAUDIOERROR(adc.openStream(NULL, &iParams, m_params.audioFormat,
		m_params.sampleRate, &m_params.framesPerBuffer, &inputCallback<T>, (void*)&m_data) );
	//�����ڴ�ռ������¼������
	MemoryAlloc();
	//��ʼ¼����ͬʱ����ȫ�Ĳ����쳣����
	TRY_CATCH_RTAUDIOERROR( adc.startStream() );
	//��ӡ¼����Ϣ
	PrintRecordMessage();
	//�ȴ�¼������
	WaitForFinish(adc);
}

template<typename T>
void CRecord<T>::MemoryAlloc()
{
	unsigned long totalBytes = m_data.totalFrames * m_data.channels * sizeof(T);
	m_data.buffer = (T*)malloc( totalBytes );
	if (m_data.buffer == nullptr) {
		cout << "Memory allocation error....exit!\n";
		exit(-1);
	}
}

template<typename T>
inline void CRecord<T>::InitInputParams(RtAudio::StreamParameters& params, RtAudio& adc)
{
	params.deviceId = adc.getDefaultInputDevice();
	params.nChannels = m_data.channels;
	params.firstChannel = 0;
}

template<typename T>
inline void CRecord<T>::PrintRecordMessage()
{
	cout << "\nRecording for " << m_params.nSeconds << " seconds. (buffer frames = " << m_params.framesPerBuffer << ")." << endl;
	cout << "\nChannels:" << m_params.nChannels << "\nSample Rate:" << m_params.sampleRate << endl;
}

template<typename T>
inline void CRecord<T>::WaitForFinish(const RtAudio & adc) const
{
	while (adc.isStreamRunning())
	{
		SLEEP(100);
	}
}

template<typename T>
inline void CRecord<T>::WriteDataToRawFile(const string& fileName) const
{
	unsigned long totalBytes = m_data.totalFrames*m_data.channels*sizeof(T);
	FILE* fd = fopen( fileName.c_str(), "wb" );
	fwrite( m_data.buffer, 1, totalBytes, fd);
	fclose(fd);
}

template<typename T>
void CRecord<T>::WriteDataToWavFile(const string& fileName) const
{
	unsigned long totalBytes = m_data.totalFrames*m_data.channels*sizeof(T);
	CPcm2Wav converter( m_data.buffer, totalBytes, fileName);
	Pcm2WavParameter convertParams;
	convertParams.channels = m_params.nChannels;
	convertParams.sampleRate = m_params.sampleRate;
	convertParams.formatTag = 1;
	convertParams.sampleBits = 32;
	if (m_params.audioFormat == AUDIO_FLOAT32)
	{
		convertParams.formatTag = 3;
		convertParams.sampleBits = 32;
	}
	else if (m_params.audioFormat == AUDIO_SIN32)
	{
		convertParams.formatTag = 1;
		convertParams.sampleBits = 32;
	}
	else if (m_params.audioFormat == AUDIO_SIN16)
	{
		convertParams.formatTag = 1;
		convertParams.sampleBits = 16;
	}
	converter.Pcm2Wav(convertParams);
}
