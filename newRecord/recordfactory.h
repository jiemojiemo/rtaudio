#pragma once
#include "rtrecord.h"

class RecordFactory
{
public:
	virtual Record* GetRecorder(const RecordParameters& params) = 0;
public:
	RecordFactory() {}
	virtual ~RecordFactory() {};

};

class RtRecordFactory : public RecordFactory
{
private:
	static RecordFactory* m_pFactory;
	RtRecordFactory() {}
public:
	static RecordFactory* GetInstance()
	{
		if (m_pFactory == NULL)
			m_pFactory = new RtRecordFactory();
		return m_pFactory;
	}

	virtual Record* GetRecorder(const RecordParameters& params)
	{
		switch (params.audioFormat)
		{
		case AUDIO_SIN16:
			return new RtRecord16Bits(params);
		default:
			return new RtRecord16Bits(params);
		}
	}

};

RecordFactory* RtRecordFactory::m_pFactory=nullptr;