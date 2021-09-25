#pragma once
#include <cstddef>
#include <cstdint>

class bf_write 
{
public:
    unsigned char* data;
    int data_bytes;
    int data_bits;
    int cur_bit;
    bool overflow;
    bool assert_on_overflow;
    const char* debug_name;

    void start_writing(void* m_data, int m_bytes, int m_start_bit = 0, int m_bits = -1) 
	{
        m_bytes &= ~3;

        data = (unsigned char*)m_data;
        data_bytes = m_bytes;

        if (m_bits == -1) {
            data_bits = m_bytes << 3;
        }
        else {
            data_bits = m_bits;
        }

        cur_bit = m_start_bit;
        overflow = false;
    }

    bf_write() 
	{
        data = NULL;
        data_bytes = 0;
        data_bits = -1;
        cur_bit = 0;
        overflow = false;
        assert_on_overflow = true;
        debug_name = NULL;
    }

    bf_write(void* m_data, int m_bytes, int m_bits = -1) 
	{
        assert_on_overflow = true;
        debug_name = NULL;
        start_writing(m_data, m_bytes, 0, m_bits);
    }

    bf_write(const char* m_debug_name, void* m_data, int m_bytes, int m_bits = -1) 
	{
        assert_on_overflow = true;
        debug_name = m_debug_name;
        start_writing(m_data, m_bytes, 0, m_bits);
    }
};

class INetChannel
{
public:
    char pad_0000[20];           //0x0000
    bool m_bProcessingMessages;  //0x0014
    bool m_bShouldDelete;        //0x0015
    char pad_0016[2];            //0x0016
    int32_t m_nOutSequenceNr;    //0x0018 last send outgoing sequence number
    int32_t m_nInSequenceNr;     //0x001C last received incoming sequnec number
    int32_t m_nOutSequenceNrAck; //0x0020 last received acknowledge outgoing sequnce number
    int32_t m_nOutReliableState; //0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
    int32_t m_nInReliableState;  //0x0028 state of incoming reliable data
    int32_t m_nChokedPackets;    //0x002C number of choked packets
    char pad_0030[1044];         //0x0030
}; //Size: 0x0444

class CClockDriftMgr
{
public:
    float m_ClockOffsets[16];   //0x0000
    uint32_t m_iCurClockOffset; //0x0044
    uint32_t m_nServerTick;     //0x0048
    uint32_t m_nClientTick;     //0x004C
}; //Size: 0x0050

class CEventInfo
{
public:
    enum
    {
        EVENT_INDEX_BITS = 8,
        EVENT_DATA_LEN_BITS = 11,
        MAX_EVENT_DATA = 192,  
    };

    inline CEventInfo()
    {
        classID = 0;
        fire_delay = 0.0f;
        flags = 0;
        pSendTable = NULL;
        pClientClass = NULL;
        m_Packed = 0;
    }

    short classID;
    short pad;
    float fire_delay;
    const void* pSendTable;
    const ClientClass* pClientClass;
    int m_Packed;
    int		flags;
    int filter[8];
    CEventInfo* next;
};

class ClientState
{
public:
    void ForceFullUpdate()
    {
        m_nDeltaTick = -1;
    }

    char pad_0000[156];
    INetChannel* m_NetChannel;
    int m_nChallengeNr;
    char pad_00A4[100];
    int m_nSignonState;
    int signon_pads[2];
    float m_flNextCmdTime;
    int m_nServerCount;
    int m_nCurrentSequence;
    int musor_pads[2];
    CClockDriftMgr m_ClockDriftMgr;
    int m_nDeltaTick;
    bool m_bPaused;
    char paused_align[3];
    int m_nViewEntity;
    int m_nPlayerSlot;
    int bruh;
    char m_szLevelName[260];
    char m_szLevelNameShort[80];
    char m_szGroupName[80];
    char pad_032[92];
    int m_nMaxClients;
    char pad_0314[18828];
    float m_nLastServerTickTime;
    bool m_bInSimulation;
    char pad_4C9D[3];
    int m_nOldTickCount;
    float m_flTickReminder;
    float m_flFrametime;
    int m_nLastOutgoingCommand;
    int m_nChokedCommands;
    int m_nLastCommandAck;
    int m_nPacketEndTickUpdate;
    int m_nCommandAck;
    int m_nSoundSequence;
    char pad_4CCD[76];
    QAngle viewangles;
    int pads[54];
    CEventInfo* m_pEvents;
};

extern ClientState* g_pClientState;
