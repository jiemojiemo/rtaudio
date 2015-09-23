#ifndef PCM2WAV_H
#define PCM2WAV_H

#include <string>
using std::string;

//wave��ʽ�бȽϸ��ӵļ������
//�������ʹ����򵥵�������ܹ�֧�ֲ���������
//���帴��������Բ��� http://blog.csdn.net/yueyihua/article/details/5494905
//----------------���¶���һЩ��ַ

//RIFF,��Դ������־
#define RIFFID			0x00
//����һ����ַ��ʼ���ļ�β�����ֽ���
#define CHUNKE_SIZE		0x04
//WAVE, ����WAV�ļ���ʽ
#define WAVE			0x08
//FMT,���θ�ʽ��־
#define FMT				0x0c
//16����18��18��ʾ�ж�����Ϣ
#define FMT_CKSIZE		0x10
//Ϊ1ʱ��ʾ����PCM���룬����1ʱ��ʾ��ѹ���ı���
//WAVE_FORMAT_IEEE_FLOATΪ32bit float׼��
#define AUDIO_FORMAT	0x14
//������
#define CHANNELS		0x16
//������
#define SAMPLE_RATE		0x18
//BYTE��=����Ƶ��*��Ƶͨ����*ÿ�β����õ�������λ��/8
#define BYTE_RATE		0x1c
//�����=ͨ����*ÿ�β����õ�������λ��/8
#define BLOCKALIGN		0x20
//��������λ��
#define BITS_PER_SAMPLE	0x22
//data, һ����ʾ����
#define DATAID			0x24
//Wav�ļ�ʵ����Ƶ������ռ��С
#define DATA_SIZE		0x28
//ʵ�����ݿ�ʼ��
#define DATA_BEGIN		0x2c

#define DWORD_SIZE		4
#define WORD_SIZE		2

struct Pcm2WavParameter{
	unsigned int channels;		//������
	unsigned int sampleRate;		//������
	unsigned int sampleBits;		//������λ��
	unsigned int formatTag;		//��ʽ��־
									//3 Ϊ 32λfloat
									//1 λ 16λpcm
};


class CPcm2Wav{
private:
	CPcm2Wav() {}
	CPcm2Wav( const CPcm2Wav& cp ){}
	CPcm2Wav& operator=( const CPcm2Wav& cp ){}
	
	void Pcm2Wav_File2File();
	void Pcm2Wav_Buffer2File();
	void ReadFileToMemory();
	void OpenOutputFile();
	void CloseOutputFile() { fclose(m_pOutFile); }
	void WriteToOutputFile();
	unsigned long GetDataFileSize();
	void WriteSomeDataToOutFile( const void* str, int size );

	void WriteRIFFChunk();
	void WriteRIFFID();
	void WriteRIFFChunkSize();
	void WriteWAVEID();

	void WriteFMTChunk();
	void WriteFMTID();
	void WriteFMTChunkSize();
	void WriteFormatTag();
	void WriteChannels();
	void WriteSampleRate();
	void WriteDataRate();
	void WriteBlockAlign();
	void WriteBitsPerSample();
	
	void WriteDataChunk();
	void WriteDataID();
	void WriteDataSize();
	void WriteAudioData();
private:
	Pcm2WavParameter m_parameter;
	bool m_isFile2File;
	FILE* m_pOutFile;
	FILE* m_pInFile;
	void* m_pbuffer;
	unsigned long m_bufferSize;
	int m_outFileIndex;
	string m_inFileName;
	string m_outFileName;
public:
	CPcm2Wav( const string& inFile, const string& outFile );
	CPcm2Wav( float* buffer, int bufferSize, const string& outFileName );
	~CPcm2Wav();
	/**
	* @brief : ����infomation���з�װ 
	*/
	int Pcm2Wav( const Pcm2WavParameter& info );
};

#endif