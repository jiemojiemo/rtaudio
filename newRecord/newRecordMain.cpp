#include "record.h"

int main()
{
	RecordParameters params;
	params.nSeconds = 5;
	params.nChannels = 1;
	params.sampleRate = 44100;
	params.framesPerBuffer = 512;
	params.audioFormat = AUDIO_SIN16;

	//根据audioFormat，使用float作为模板
	CRecord<short> reocrd( params );
	reocrd.Start();
	reocrd.WriteDataToRawFile( "1_16_44100.raw" );
}
