#include "rtrecord.h"
#include "recordfactory.h"
int main()
{
	RecordParameters params;
	params.nSeconds = 5;
	params.nChannels = 1;
	params.sampleRate = 44100;
	params.framesPerBuffer = 512;
	params.audioFormat = AUDIO_SIN16;

	auto recordFactory = RtRecordFactory::GetInstance();

	Record* record = recordFactory->GetRecorder(params);
	record->Start();
	record->WriteDataToRawFile("1_16_44100.raw");

}
