#include "rtrecord.h"
#include "scopeguard.h"
#include <iostream>
#include <assert.h>

using std::cout;
using std::cerr;
using std::endl;



#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ ) || defined( __WINDOWS_WASAPI__ )
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

#define TRY_CATCH_RTAUDIOERROR(x) try {x;}catch(RtAudioError& e){cout<<'\n'<<e.getMessage()<<endl;}

typedef short RTAUDIO_SIN16_TYPE;
typedef float RTAUDIO_FLOAT32_TYPE;

//申请内存
static void* MemoryAlloc(unsigned long totalBytes)
{
	assert(totalBytes > 0);
	void* buffer = malloc(totalBytes);
	memset( buffer, 0, totalBytes );
	if (buffer == nullptr) {
		cout << "Memory allocation error....exit!\n";
		exit(-1);
	}
	return buffer;
}
//等待录音结束
static void WaitForFinish(const RtAudio& adc)
{
	while (adc.isStreamRunning())
	{
		SLEEP(100);
	}
}

static void PrintRecordMessage(const RecordParameters& recordinfo)
{
	cout << "\nRecording for " << recordinfo.nSeconds << "seconds";
	cout << "\nChannels:" << recordinfo.nChannels;
	cout << "\nSample Rate:" << recordinfo.sampleRate;
}

template <typename T>
static int inputCallback(void */*outputbuffer*/, void* inputBuffer, unsigned int nBufferFrames,
	double, RtAudioStreamStatus, void* data)
{
	unsigned int frames = nBufferFrames;
	RecordData* iData = (RecordData*)data;

	if (iData->frameCounter + nBufferFrames > iData->totalFrames) {
		frames = iData->totalFrames - iData->frameCounter;
		iData->bytesPerFrame = frames * iData->channels * sizeof(T);
	}

	unsigned long offset = iData->frameCounter * iData->channels;
	T* buffer = (T*)iData->buffer;
	memcpy(buffer + offset, inputBuffer, iData->bytesPerFrame);
	iData->frameCounter += frames;
	//return 2 表示录音结束
	if (iData->frameCounter >= iData->totalFrames)
		return 2;
	return 0;
}

template <typename T>
static void DoRecord( RecordData& recordData, RecordParameters& params )
{
	RtAudio adc;
	assert(adc.getDeviceCount() >= 1);
	ON_SCOPE_EXIT([&]() {if (adc.isStreamOpen()) adc.closeStream(); });

	//初始化流参数
	RtAudio::StreamParameters iParams;
	iParams.deviceId = adc.getDefaultInputDevice();
	iParams.nChannels = recordData.channels;

	//打开音频流，同时做安全的捕获异常处理
	TRY_CATCH_RTAUDIOERROR(adc.openStream(NULL, &iParams, params.audioFormat,
		params.sampleRate, &params.framesPerBuffer, &inputCallback<short>, (void*)&recordData));
	
	//开始录音，同时做安全的捕获异常处理
	TRY_CATCH_RTAUDIOERROR(adc.startStream());

	//等待录音结束
	WaitForFinish(adc);

	//打印录音信息
	PrintRecordMessage(params);
}



//RtRecord
void RtRecord::WriteDataToRawFile(const std::string& fileName)
{
	assert(fileName != "");
	FILE* fd = fopen(fileName.c_str(), "wb");
	fwrite(m_data.buffer, 1, m_data.totalBytes, fd);
	fclose(fd);
}

RtRecord16Bits::RtRecord16Bits(const RecordParameters& params) :
	RtRecord(params)
{
	m_data.channels = params.nChannels;
	m_data.buffer = nullptr;
	m_data.bytesPerFrame = params.framesPerBuffer * params.nChannels * sizeof(short);
	m_data.frameCounter = 0;
	m_data.totalFrames = params.sampleRate * params.nSeconds;
	m_data.totalBytes = m_data.totalFrames*m_data.channels*sizeof(short);

	m_data.buffer = MemoryAlloc(m_data.totalBytes);
}


void RtRecord16Bits::Start()
{
	//重置时间轴
	ResetTimeAxis();
	//开始录音
	DoRecord<RTAUDIO_SIN16_TYPE>(m_data, m_params);
}