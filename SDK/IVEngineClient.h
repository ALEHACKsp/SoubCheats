#pragma once

#include "Definitions.h"
#include "KeyValues.h"
#include "..\Utils\Color.h"
#include "PlayerInfo.h"
#include "Vector.h"
#include "VMatrix.h"

#define FLOW_OUTGOING	0
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		// in & out

typedef struct InputContextHandle_t__ *InputContextHandle_t;
struct client_textmessage_t;
struct model_t;
class SurfInfo;
class IMaterial;
class CSentence;
class CAudioSource;
class AudioState_t;
class ISpatialQuery;
class IMaterialSystem;
class CPhysCollide;
class IAchievementMgr;

class INetChannelInfo
{
public:
    enum {
        GENERIC = 0,    // must be first and is default group
        LOCALPLAYER,    // bytes for local player entity update
        OTHERPLAYERS,   // bytes for other players update
        ENTITIES,       // all other entity bytes
        SOUNDS,         // game sounds
        EVENTS,         // event messages
        USERMESSAGES,   // user messages
        ENTMESSAGES,    // entity messages
        VOICE,          // voice data
        STRINGTABLE,    // a stringtable update
        MOVE,           // client move cmds
        STRINGCMD,      // string command
        SIGNON,         // various signondata
        TOTAL,          // must be last and is not a real group
    };

    virtual const char  *GetName(void) const = 0;	            // get channel name
    virtual const char  *GetAddress(void) const = 0;            // get channel IP address as string
    virtual float       GetTime(void) const = 0;	            // current net time
    virtual float       GetTimeConnected(void) const = 0;	    // get connection time in seconds
    virtual int         GetBufferSize(void) const = 0;	        // netchannel packet history size
    virtual int         GetDataRate(void) const = 0;            // send data rate in byte/sec

    virtual bool        IsLoopback(void) const = 0;             // true if loopback channel
    virtual bool        IsTimingOut(void) const = 0;            // true if timing out
    virtual bool        IsPlayback(void) const = 0;             // true if demo playback

    virtual float       GetLatency(int flow) const = 0;         // current latency (RTT), more accurate but jittering
    virtual float		GetAvgLatency(int flow) const = 0;      // average packet latency in seconds
    virtual float       GetAvgLoss(int flow) const = 0;         // avg packet loss[0..1]
    virtual float       GetAvgChoke(int flow) const = 0;        // avg packet choke[0..1]
    virtual float       GetAvgData(int flow) const = 0;         // data flow in bytes/sec
    virtual float       GetAvgPackets(int flow) const = 0;      // avg packets/sec
    virtual int         GetTotalData(int flow) const = 0;       // total flow in/out in bytes
    virtual int         GetSequenceNr(int flow) const = 0;      // last send seq number
    virtual bool        IsValidPacket(int flow, int frame_number) const = 0;                // true if packet was not lost/dropped/chocked/flushed
    virtual float       GetPacketTime(int flow, int frame_number) const = 0;                // time when packet was send
    virtual int         GetPacketBytes(int flow, int frame_number, int group) const = 0;    // group size of this packet
    virtual bool        GetStreamProgress(int flow, int *received, int *total) const = 0;   // TCP progress if transmitting
    virtual float       GetTimeSinceLastReceived(void) const = 0;// get time since last recieved packet in seconds
    virtual	float       GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
    virtual void		GetPacketResponseLatency(int flow, int frame_number, int *pnLatencyMsecs, int *pnChoke) const = 0;
    virtual void		GetRemoteFramerate(float *pflFrameTime, float *pflFrameTimeStdDeviation) const = 0;

    virtual float		GetTimeoutSeconds() const = 0;
};

class ISPSharedMemory;
class CGamestatsData;
class CSteamAPIContext;
struct Frustum_t;

typedef enum
{
    NA_NULL = 0,
    NA_LOOPBACK,
    NA_BROADCAST,
    NA_IP,
} netadrtype_t;

typedef struct netadr_s
{
public:
    netadrtype_t	type;
    unsigned char	ip[4];
    unsigned short	port;
} netadr_t;

class INetMessageBinder;
class IDemoRecorder;
class INetMessage;
class INetChannelHandler;
class bf_write;

class NetChannel : public INetChannelInfo
{
public:

    virtual	~NetChannel(void) {};

    virtual void	SetDataRate(float rate) = 0;
    virtual bool	RegisterMessage(INetMessageBinder* msg) = 0;
    virtual bool	UnregisterMessage(INetMessageBinder* msg) = 0;
    virtual bool	StartStreaming(unsigned int challengeNr) = 0;
    virtual void	ResetStreaming(void) = 0;
    virtual void	SetTimeout(float seconds, bool bForceExact = false) = 0;
    virtual void	SetDemoRecorder(IDemoRecorder* recorder) = 0;
    virtual void	SetChallengeNr(unsigned int chnr) = 0;

    virtual void	Reset(void) = 0;
    virtual void	Clear(void) = 0;
    virtual void	Shutdown(const char* reason) = 0;

    virtual void	ProcessPlayback(void) = 0;
    virtual bool	ProcessStream(void) = 0;
    virtual void	ProcessPacket(struct netpacket_s* packet, bool bHasHeader) = 0;

    virtual bool	SendNetMsg(INetMessage& msg, bool bForceReliable = false, bool bVoice = false) = 0;

    virtual bool	SendData(bf_write& msg, bool bReliable = true) = 0;
    virtual bool	SendFile(const char* filename, unsigned int transferID, bool isReplayDemo) = 0;
    virtual void	DenyFile(const char* filename, unsigned int transferID, bool isReplayDemo) = 0;
    virtual void	RequestFile_OLD(const char* filename, unsigned int transferID) = 0;	// get rid of this function when we version the 
    virtual void	SetChoked(void) = 0;
    virtual int		SendDatagram(bf_write* data) = 0;
    virtual bool	Transmit(bool onlyReliable = false) = 0;

    virtual const netadr_t& GetRemoteAddress(void) const = 0;
    virtual INetChannelHandler* GetMsgHandler(void) const = 0;
    virtual int				GetDropNumber(void) const = 0;
    virtual int				GetSocket(void) const = 0;
    virtual unsigned int	GetChallengeNr(void) const = 0;
    virtual void			GetSequenceData(int& nOutSequenceNr, int& nInSequenceNr, int& nOutSequenceNrAck) = 0;
    virtual void			SetSequenceData(int nOutSequenceNr, int nInSequenceNr, int nOutSequenceNrAck) = 0;

    virtual void	UpdateMessageStats(int msggroup, int bits) = 0;
    virtual bool	CanPacket(void) const = 0;
    virtual bool	IsOverflowed(void) const = 0;
    virtual bool	IsTimedOut(void) const = 0;
    virtual bool	HasPendingReliableData(void) = 0;

    virtual void	SetFileTransmissionMode(bool bBackgroundMode) = 0;
    virtual void	SetCompressionMode(bool bUseCompression) = 0;
    virtual unsigned int RequestFile(const char* filename, bool isReplayDemoFile) = 0;
    virtual float	GetTimeSinceLastReceived(void) const = 0;	// get time since last received packet in seconds

    virtual void	SetMaxBufferSize(bool bReliable, int nBytes, bool bVoice = false) = 0;

    virtual bool	IsNull() const = 0;
    virtual int		GetNumBitsWritten(bool bReliable) = 0;
    virtual void	SetInterpolationAmount(float flInterpolationAmount) = 0;
    virtual void	SetRemoteFramerate(float flFrameTime, float flFrameTimeStdDeviation) = 0;

    // Max # of payload bytes before we must split/fragment the packet
    virtual void	SetMaxRoutablePayloadSize(int nSplitSize) = 0;
    virtual int		GetMaxRoutablePayloadSize() = 0;

    // For routing messages to a different handler
    virtual bool	SetActiveChannel(NetChannel* pNewChannel) = 0;
    virtual void	AttachSplitPlayer(int nSplitPlayerSlot, NetChannel* pChannel) = 0;
    virtual void	DetachSplitPlayer(int nSplitPlayerSlot) = 0;

    virtual bool	IsRemoteDisconnected() const = 0;

    virtual bool	WasLastMessageReliable() const = 0;

    char pad_0000[20]; //0x0000
    bool m_bProcessingMessages; //0x0014
    bool m_bShouldDelete; //0x0015
    char pad_0016[2]; //0x0016
    int32_t m_nOutSequenceNr;    //0x0018 last send outgoing sequence number
    int32_t m_nInSequenceNr;     //0x001C last received incoming sequnec number
    int32_t m_nOutSequenceNrAck; //0x0020 last received acknowledge outgoing sequnce number
    int32_t m_nOutReliableState; //0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
    int32_t m_nInReliableState;  //0x0028 state of incoming reliable data
    int32_t m_nChokedPackets;    //0x002C number of choked packets
    char pad_0030[1044]; //0x0030
}; //Size: 0x0444

class IVEngineClient
{
public:
    virtual int                   GetIntersectingSurfaces(const model_t *model, const Vector &vCenter, const float radius, const bool bOnlyVisibleSurfaces, SurfInfo *pInfos, const int nMaxInfos) = 0;
    virtual Vector                GetLightForPoint(const Vector &pos, bool bClamp) = 0;
    virtual IMaterial*            TraceLineMaterialAndLighting(const Vector &start, const Vector &end, Vector &diffuseLightColor, Vector& baseColor) = 0;
    virtual const char*           ParseFile(const char *data, char *token, int maxlen) = 0;
    virtual bool                  CopyFile(const char *source, const char *destination) = 0;
    virtual void                  GetScreenSize(int& width, int& height) = 0;
    virtual void                  ServerCmd(const char *szCmdString, bool bReliable = true) = 0;
    virtual void                  ClientCmd(const char *szCmdString) = 0;
    virtual bool                  GetPlayerInfo(int ent_num, PlayerInfo_t *pinfo) = 0;
    virtual int                   GetPlayerForUserID(int userID) = 0;
    virtual client_textmessage_t* TextMessageGet(const char *pName) = 0; // 10
    virtual bool                  Con_IsVisible(void) = 0;
    virtual int                   GetLocalPlayer(void) = 0;
    virtual const model_t*        LoadModel(const char *pName, bool bProp = false) = 0;
    virtual float                 GetLastTimeStamp(void) = 0;
    virtual CSentence*            GetSentence(CAudioSource *pAudioSource) = 0; // 15
    virtual float                 GetSentenceLength(CAudioSource *pAudioSource) = 0;
    virtual bool                  IsStreaming(CAudioSource *pAudioSource) const = 0;
    virtual void                  GetViewAngles(Vector& va) = 0;
    virtual void                  SetViewAngles(Vector& va) = 0;
    virtual int                   GetMaxClients(void) = 0; // 20
    virtual   const char*         Key_LookupBinding(const char *pBinding) = 0;
    virtual const char*           Key_BindingForKey(int &code) = 0;
    virtual void                  Key_SetBinding(int, char const*) = 0;
    virtual void                  StartKeyTrapMode(void) = 0;
    virtual bool                  CheckDoneKeyTrapping(int &code) = 0;
    virtual bool                  IsInGame(void) = 0;
    virtual bool                  IsConnected(void) = 0;
    virtual bool                  IsDrawingLoadingImage(void) = 0;
    virtual void                  HideLoadingPlaque(void) = 0;
    virtual void                  Con_NPrintf(int pos, const char *fmt, ...) = 0; // 30
    virtual void                  Con_NXPrintf(const struct con_nprint_s *info, const char *fmt, ...) = 0;
    virtual int                   IsBoxVisible(const Vector& mins, const Vector& maxs) = 0;
    virtual int                   IsBoxInViewCluster(const Vector& mins, const Vector& maxs) = 0;
    virtual bool                  CullBox(const Vector& mins, const Vector& maxs) = 0;
    virtual void                  Sound_ExtraUpdate(void) = 0;
    virtual const char*           GetGameDirectory(void) = 0;
    virtual const VMatrix&        WorldToScreenMatrix() = 0;
    virtual const VMatrix&        WorldToViewMatrix() = 0;
    virtual int                   GameLumpVersion(int lumpId) const = 0;
    virtual int                   GameLumpSize(int lumpId) const = 0; // 40
    virtual bool                  LoadGameLump(int lumpId, void* pBuffer, int size) = 0;
    virtual int                   LevelLeafCount() const = 0;
    virtual ISpatialQuery*        GetBSPTreeQuery() = 0;
    virtual void                  LinearToGamma(float* linear, float* gamma) = 0;
    virtual float                 LightStyleValue(int style) = 0; // 45
    virtual void                  ComputeDynamicLighting(const Vector& pt, const Vector* pNormal, Vector& color) = 0;
    virtual void                  GetAmbientLightColor(Vector& color) = 0;
    virtual int                   GetDXSupportLevel() = 0;
    virtual bool                  SupportsHDR() = 0;
    virtual void                  Mat_Stub(IMaterialSystem *pMatSys) = 0; // 50
    virtual void                  GetChapterName(char *pchBuff, int iMaxLength) = 0;
    virtual char const*           GetLevelName(void) = 0;
    virtual char const*           GetLevelNameShort(void) = 0;
    virtual char const*           GetMapGroupName(void) = 0;
    virtual struct IVoiceTweak_s* GetVoiceTweakAPI(void) = 0;
    virtual void                  SetVoiceCasterID(unsigned int someint) = 0; // 56
    virtual void                  EngineStats_BeginFrame(void) = 0;
    virtual void                  EngineStats_EndFrame(void) = 0;
    virtual void                  FireEvents() = 0;
    virtual int                   GetLeavesArea(unsigned short *pLeaves, int nLeaves) = 0;
    virtual bool                  DoesBoxTouchAreaFrustum(const Vector &mins, const Vector &maxs, int iArea) = 0; // 60
    virtual int                   GetFrustumList(Frustum_t **pList, int listMax) = 0;
    virtual bool                  ShouldUseAreaFrustum(int i) = 0;
    virtual void                  SetAudioState(const AudioState_t& state) = 0;
    virtual int                   SentenceGroupPick(int groupIndex, char *name, int nameBufLen) = 0;
    virtual int                   SentenceGroupPickSequential(int groupIndex, char *name, int nameBufLen, int sentenceIndex, int reset) = 0;
    virtual int                   SentenceIndexFromName(const char *pSentenceName) = 0;
    virtual const char*           SentenceNameFromIndex(int sentenceIndex) = 0;
    virtual int                   SentenceGroupIndexFromName(const char *pGroupName) = 0;
    virtual const char*           SentenceGroupNameFromIndex(int groupIndex) = 0;
    virtual float                 SentenceLength(int sentenceIndex) = 0;
    virtual void                  ComputeLighting(const Vector& pt, const Vector* pNormal, bool bClamp, Vector& color, Vector *pBoxColors = NULL) = 0;
    virtual void                  ActivateOccluder(int nOccluderIndex, bool bActive) = 0;
    virtual bool                  IsOccluded(const Vector &vecAbsMins, const Vector &vecAbsMaxs) = 0; // 74
    virtual int                   GetOcclusionViewId(void) = 0;
    virtual void*                 SaveAllocMemory(size_t num, size_t size) = 0;
    virtual void                  SaveFreeMemory(void *pSaveMem) = 0;
    virtual INetChannelInfo*      GetNetChannelInfo(void) = 0;
    virtual void                  DebugDrawPhysCollide(const CPhysCollide *pCollide, IMaterial *pMaterial, const matrix3x4_t& transform, const Color &color) = 0; //79
    virtual void                  CheckPoint(const char *pName) = 0; // 80
    virtual void                  DrawPortals() = 0;
    virtual bool                  IsPlayingDemo(void) = 0;
    virtual bool                  IsRecordingDemo(void) = 0;
    virtual bool                  IsPlayingTimeDemo(void) = 0;
    virtual int                   GetDemoRecordingTick(void) = 0;
    virtual int                   GetDemoPlaybackTick(void) = 0;
    virtual int                   GetDemoPlaybackStartTick(void) = 0;
    virtual float                 GetDemoPlaybackTimeScale(void) = 0;
    virtual int                   GetDemoPlaybackTotalTicks(void) = 0;
    virtual bool                  IsPaused(void) = 0; // 90
    virtual float                 GetTimescale(void) const = 0;
    virtual bool                  IsTakingScreenshot(void) = 0;
    virtual bool                  IsHLTV(void) = 0;
    virtual bool                  IsLevelMainMenuBackground(void) = 0;
    virtual void                  GetMainMenuBackgroundName(char *dest, int destlen) = 0;
    virtual void                  SetOcclusionParameters(const int /*OcclusionParams_t*/ &params) = 0; // 96
    virtual void                  GetUILanguage(char *dest, int destlen) = 0;
    virtual int                   IsSkyboxVisibleFromPoint(const Vector &vecPoint) = 0;
    virtual const char*           GetMapEntitiesString() = 0;
    virtual bool                  IsInEditMode(void) = 0; // 100
    virtual float                 GetScreenAspectRatio(int viewportWidth, int viewportHeight) = 0;
    virtual bool                  REMOVED_SteamRefreshLogin(const char *password, bool isSecure) = 0; // 100
    virtual bool                  REMOVED_SteamProcessCall(bool & finished) = 0;
    virtual unsigned int          GetEngineBuildNumber() = 0; // engines build
    virtual const char *          GetProductVersionString() = 0; // mods version number (steam.inf)
    virtual void                  GrabPreColorCorrectedFrame(int x, int y, int width, int height) = 0;
    virtual bool                  IsHammerRunning() const = 0;
    virtual void                  ExecuteClientCmd(const char *szCmdString) = 0; //108
    virtual bool                  MapHasHDRLighting(void) = 0;
    virtual bool                  MapHasLightMapAlphaData(void) = 0;
    virtual int                   GetAppID() = 0;
    virtual Vector                GetLightForPointFast(const Vector &pos, bool bClamp) = 0;
    virtual void                  ClientCmd_Unrestricted(char  const*, int, bool);
    virtual void                  ClientCmd_Unrestricted(const char *szCmdString, bool bDelayed) = 0; // 114
    virtual void                  SetRestrictServerCommands(bool bRestrict) = 0;
    virtual void                  SetRestrictClientCommands(bool bRestrict) = 0;
    virtual void                  SetOverlayBindProxy(int iOverlayID, void *pBindProxy) = 0;
    virtual bool                  CopyFrameBufferToMaterial(const char *pMaterialName) = 0;
    virtual void                  ReadConfiguration(const int iController, const bool readDefault) = 0;
    virtual void                  SetAchievementMgr(IAchievementMgr *pAchievementMgr) = 0;
    virtual IAchievementMgr*      GetAchievementMgr() = 0;
    virtual bool                  MapLoadFailed(void) = 0;
    virtual void                  SetMapLoadFailed(bool bState) = 0;
    virtual bool                  IsLowViolence() = 0;
    virtual const char*           GetMostRecentSaveGame(void) = 0;
    virtual void                  SetMostRecentSaveGame(const char *lpszFilename) = 0;
    virtual void                  StartXboxExitingProcess() = 0;
    virtual bool                  IsSaveInProgress() = 0;
    virtual bool                  IsAutoSaveDangerousInProgress(void) = 0;
    virtual unsigned int          OnStorageDeviceAttached(int iController) = 0;
    virtual void                  OnStorageDeviceDetached(int iController) = 0;
    virtual char* const           GetSaveDirName(void) = 0;
    virtual void                  WriteScreenshot(const char *pFilename) = 0;
    virtual void                  ResetDemoInterpolation(void) = 0;
    virtual int                   GetActiveSplitScreenPlayerSlot() = 0;
    virtual int                   SetActiveSplitScreenPlayerSlot(int slot) = 0;
    virtual bool                  SetLocalPlayerIsResolvable(char const *pchContext, int nLine, bool bResolvable) = 0;
    virtual bool                  IsLocalPlayerResolvable() = 0;
    virtual int                   GetSplitScreenPlayer(int nSlot) = 0;
    virtual bool                  IsSplitScreenActive() = 0;
    virtual bool                  IsValidSplitScreenSlot(int nSlot) = 0;
    virtual int                   FirstValidSplitScreenSlot() = 0; // -1 == invalid
    virtual int                   NextValidSplitScreenSlot(int nPreviousSlot) = 0; // -1 == invalid
    virtual ISPSharedMemory*      GetSinglePlayerSharedMemorySpace(const char *szName, int ent_num = (1 << 11)) = 0;
    virtual void                  ComputeLightingCube(const Vector& pt, bool bClamp, Vector *pBoxColors) = 0;
    virtual void                  RegisterDemoCustomDataCallback(const char* szCallbackSaveID, pfnDemoCustomDataCallback pCallback) = 0;
    virtual void                  RecordDemoCustomData(pfnDemoCustomDataCallback pCallback, const void *pData, size_t iDataLength) = 0;
    virtual void                  SetPitchScale(float flPitchScale) = 0;
    virtual float                 GetPitchScale(void) = 0;
    virtual bool                  LoadFilmmaker() = 0;
    virtual void                  UnloadFilmmaker() = 0;
    virtual void                  SetLeafFlag(int nLeafIndex, int nFlagBits) = 0;
    virtual void                  RecalculateBSPLeafFlags(void) = 0;
    virtual bool                  DSPGetCurrentDASRoomNew(void) = 0;
    virtual bool                  DSPGetCurrentDASRoomChanged(void) = 0;
    virtual bool                  DSPGetCurrentDASRoomSkyAbove(void) = 0;
    virtual float                 DSPGetCurrentDASRoomSkyPercent(void) = 0;
    virtual void                  SetMixGroupOfCurrentMixer(const char *szgroupname, const char *szparam, float val, int setMixerType) = 0;
    virtual int                   GetMixLayerIndex(const char *szmixlayername) = 0;
    virtual void                  SetMixLayerLevel(int index, float level) = 0;
    virtual int                   GetMixGroupIndex(char  const* groupname) = 0;
    virtual void                  SetMixLayerTriggerFactor(int i1, int i2, float fl) = 0;
    virtual void                  SetMixLayerTriggerFactor(char  const* char1, char  const* char2, float fl) = 0;
    virtual bool                  IsCreatingReslist() = 0;
    virtual bool                  IsCreatingXboxReslist() = 0;
    virtual void                  SetTimescale(float flTimescale) = 0;
    virtual void                  SetGamestatsData(CGamestatsData *pGamestatsData) = 0;
    virtual CGamestatsData*       GetGamestatsData() = 0;
    virtual void                  GetMouseDelta(int &dx, int &dy, bool b) = 0; // unknown
    virtual   const char*         Key_LookupBindingEx(const char *pBinding, int iUserId = -1, int iStartCount = 0, int iAllowJoystick = -1) = 0;
    virtual int                   Key_CodeForBinding(char  const*, int, int, int) = 0;
    virtual void                  UpdateDAndELights(void) = 0;
    virtual int                   GetBugSubmissionCount() const = 0;
    virtual void                  ClearBugSubmissionCount() = 0;
    virtual bool                  DoesLevelContainWater() const = 0;
    virtual float                 GetServerSimulationFrameTime() const = 0;
    virtual void                  SolidMoved(class IClientEntity *pSolidEnt, class ICollideable *pSolidCollide, const Vector* pPrevAbsOrigin, bool accurateBboxTriggerChecks) = 0;
    virtual void                  TriggerMoved(class IClientEntity *pTriggerEnt, bool accurateBboxTriggerChecks) = 0;
    virtual void                  ComputeLeavesConnected(const Vector &vecOrigin, int nCount, const int *pLeafIndices, bool *pIsConnected) = 0;
    virtual bool                  IsInCommentaryMode(void) = 0;
    virtual void                  SetBlurFade(float amount) = 0;
    virtual bool                  IsTransitioningToLoad() = 0;
    virtual void                  SearchPathsChangedAfterInstall() = 0;
    virtual void                  ConfigureSystemLevel(int nCPULevel, int nGPULevel) = 0;
    virtual void                  SetConnectionPassword(char const *pchCurrentPW) = 0;
    virtual CSteamAPIContext*     GetSteamAPIContext() = 0;
    virtual void                  SubmitStatRecord(char const *szMapName, unsigned int uiBlobVersion, unsigned int uiBlobSize, const void *pvBlob) = 0;
    virtual void                  ServerCmdKeyValues(KeyValues *pKeyValues) = 0; // 203
    virtual void                  SpherePaintSurface(const model_t* model, const Vector& location, unsigned char chr, float fl1, float fl2) = 0;
    virtual bool                  HasPaintmap(void) = 0;
    virtual void                  EnablePaintmapRender() = 0;
    //virtual void                TracePaintSurface( const model_t *model, const Vector& position, float radius, CUtlVector<Color>& surfColors ) = 0;
    virtual void                  SphereTracePaintSurface(const model_t* model, const Vector& position, const Vector &vec2, float radius, /*CUtlVector<unsigned char, CUtlMemory<unsigned char, int>>*/ int& utilVecShit) = 0;
    virtual void                  RemoveAllPaint() = 0;
    virtual void                  PaintAllSurfaces(unsigned char uchr) = 0;
    virtual void                  RemovePaint(const model_t* model) = 0;
    virtual bool                  IsActiveApp() = 0;
    virtual bool                  IsClientLocalToActiveServer() = 0;
    virtual void                  TickProgressBar() = 0;
    virtual InputContextHandle_t  GetInputContext(int /*EngineInputContextId_t*/ id) = 0;
    virtual void                  GetStartupImage(char* filename, int size) = 0;
    virtual bool                  IsUsingLocalNetworkBackdoor(void) = 0;
    virtual void                  SaveGame(const char*, bool, char*, int, char*, int) = 0;
    virtual void                  GetGenericMemoryStats(/* GenericMemoryStat_t */ void **) = 0;
    virtual bool                  GameHasShutdownAndFlushedMemory(void) = 0;
    virtual int                   GetLastAcknowledgedCommand(void) = 0;
    virtual void                  FinishContainerWrites(int i) = 0;
    virtual void                  FinishAsyncSave(void) = 0;
    virtual int                   GetServerTick(void) = 0;
    virtual const char*           GetModDirectory(void) = 0;
    virtual bool                  AudioLanguageChanged(void) = 0;
    virtual bool                  IsAutoSaveInProgress(void) = 0;
    virtual void                  StartLoadingScreenForCommand(const char* command) = 0;
    virtual void                  StartLoadingScreenForKeyValues(KeyValues* values) = 0;
    virtual void                  SOSSetOpvarFloat(const char*, float) = 0;
    virtual void                  SOSGetOpvarFloat(const char*, float &) = 0;
    virtual bool                  IsSubscribedMap(const char*, bool) = 0;
    virtual bool                  IsFeaturedMap(const char*, bool) = 0;
    virtual void                  GetDemoPlaybackParameters(void) = 0;
    virtual int                   GetClientVersion(void) = 0;
    virtual bool                  IsDemoSkipping(void) = 0;
    virtual void                  SetDemoImportantEventData(const KeyValues* values) = 0;
    virtual void                  ClearEvents(void) = 0;
    virtual int                   GetSafeZoneXMin(void) = 0;
    virtual bool                  IsVoiceRecording(void) = 0;
    virtual void                  ForceVoiceRecordOn(void) = 0;
    virtual bool                  IsReplay(void) = 0;

	template <typename t>
	static t DUMBNIGGERSLOL(void* class_pointer, size_t index) {
		return (*(t**)class_pointer)[index];
	}

	NetChannel* GetNetChannel() // i wanna fucking die
	{
		typedef NetChannel*(__thiscall* Fn)(void*);
		return DUMBNIGGERSLOL<Fn>(this, 78)(this);
	}
};
extern IVEngineClient* g_pEngine;