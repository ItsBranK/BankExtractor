#pragma once
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <string>
#include <cstdint>
#include <sstream>

/*
	Original repository: https://github.com/eXpl0it3r/bnkextr
	Current fork: https://github.com/ItsBranK/BankExtractor

	Thanks to XenTax for the information on sound bank files.
	http://wiki.xentax.com/index.php/Wwise_SoundBank_(*.bnk)
*/

namespace Settings
{
	extern bool Swap;
	extern bool NoDir;
	extern bool Index;
	extern bool Info;
}

enum class WwiseObjects : uint8_t
{
	None = 0,
	Settings = 1,
	SoundEffects = 2,
	EventAction = 3,
	Event = 4,
	SequenceContainer = 5,
	SwitchContainer = 6,
	ActorMixer = 7,
	AudioBus = 8,
	BlendContainer = 9,
	MusicSegment = 10,
	MusicTrack = 11,
	MusicSwitchContainer = 12,
	MusicPlaylistContainer = 13,
	Attenuation = 14,
	DialogueEvent = 15,
	MotionBus = 16,
	MotionFx = 17,
	Effect = 18,
	Unknown = 19,
	AuxiliaryBus = 20
};

enum class EventActionScope : uint8_t
{
	Unknown = 0,
	SwitchOrTrigger = 1,
	Global = 2,
	GameObject = 3,
	State = 4,
	All = 5,
	AllExcept = 6
};

enum class EventActionType : uint8_t
{
	Unknown = 0,
	Stop = 1,
	Pause = 2,
	Resume = 3,
	Play = 4,
	Trigger = 5,
	Mute = 6,
	UnMute = 7,
	SetVoicePitch = 8,
	ResetVoicePitch = 9,
	SetVoiceVolume = 10,
	ResetVoiceVolume = 11,
	SetBusVolume = 12,
	ResetBusVolume = 13,
	SetVoiceLowPassFilter = 14,
	ResetVoiceLowPassFilter = 15,
	EnableState = 16,
	DisableState = 17,
	SetState = 18,
	SetGameParameter = 19,
	ResetGameParameter = 20,
	SetSwitch = 21,
	ToggleBypass = 22,
	ResetBypassEffect = 23,
	Break = 24,
	Seek = 25
};

enum class EventActionParameterType : uint8_t
{
	Delay = 0x0E,
	Play = 0x0F,
	Probability = 0x10
};

#pragma pack (push, 0x1)
class Section
{
public:
	char Id[4]; // Name of the section (four characters long).
	uint32_t Length; // Length of the section.

public:
	Section();
	Section(const Section& section);
	~Section();

public:
	Section& operator=(const Section& section);
};

class BKHD : public Section
{
public:
	uint32_t Version; // Soundbank version.
	uint32_t Id; // Soundbank id.

public:
	BKHD();
	BKHD(const BKHD& bkhd);
	~BKHD();

public:
	BKHD& operator=(const BKHD& bkhd);
};

class DIDX
{
public:
	uint32_t Id; // Id of the ".wem" file.
	uint32_t Offset; // DATA section offset.
	uint32_t Length; // Length of the ".wem" file.

public:
	DIDX();
	DIDX(const DIDX& didx);
	~DIDX();

public:
	DIDX& operator=(const DIDX& didx);
};

// Implementation of this class is unfinished, I can't find any files that use it so I can't verify if it works or not.

class HIRC
{
public:
	WwiseObjects Type; // Object type identifier.
	uint32_t Length; // Length of the object.
	uint32_t Id; // Internal id for the object.

public:
	HIRC();
	HIRC(const HIRC& hirc);
	~HIRC();

public:
	std::string GetTypeStr() const;

public:
	HIRC& operator=(const HIRC& hirc);
};

// Implementation of this class is unfinished, I can't find any files that use it so I can't verify if it works or not.

class STID
{
public:
	uint8_t SoundId; // Sound id to match the object listed in the HIRC section.
	uint32_t NameLength; // Length of characters of the sounds name.

public:
	STID();
	STID(const STID& stid);
	~STID();

public:
	STID& operator=(const STID& stid);
};

// Implementation of this class is unfinished, related to the "HIRC" section.

class ObjSection : public Section
{
public:
	uint32_t Count; // Number of objects.

public:
	ObjSection();
	ObjSection(const ObjSection& objSection);
	~ObjSection();

public:
	ObjSection& operator=(const ObjSection& objSection);
};

// Implementation of this class is unfinished, related to the "STID" section.

class SfxSection : public Section
{
public:
	uint8_t UnknownData[0x4];
	uint32_t Count; // Number of sound banks.

public:
	SfxSection();
	SfxSection(const SfxSection& sfxSection);
	~SfxSection();

public:
	SfxSection& operator=(const SfxSection& sfxSection);
};
#pragma pack (pop)

class SoundBank
{
private: // Custom fields.
	std::filesystem::path _filePath;
	std::ifstream _bankFile;

private: // File fields.
	BKHD _bankHeader;
	size_t _dataOffset;
	std::vector<DIDX> _wemData;
	std::vector<HIRC> _wemObjects;

public:
	SoundBank(const std::filesystem::path& filePath);
	SoundBank(const SoundBank& soundBank);
	~SoundBank();

private:
	size_t Swap(size_t value);

public:
	bool ParseSections();
	void ExtractFiles();
	void PrintHeader();
	void PrintObjects();

public:
	SoundBank& operator=(const SoundBank& soundBank);
};