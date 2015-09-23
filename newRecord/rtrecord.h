#pragma once
#include "RtAudio.h"
#include "record.h"
#include "pcmaudio.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);                \
    TypeName& operator=(const TypeName&)


class RtRecord : public Record
{
public:
	RtRecord(const RecordParameters& params) :m_params(params) {}
	virtual void WriteDataToRawFile(const std::string& fileName);
	virtual void Start()=0;
protected:
	RecordData m_data;
	RecordParameters m_params;
	virtual void ResetTimeAxis() { m_data.frameCounter = 0; }
private:
	DISALLOW_COPY_AND_ASSIGN(RtRecord);
};


class RtRecord16Bits : public RtRecord
{
public:
	RtRecord16Bits(const RecordParameters& params);
	virtual void Start();
};
