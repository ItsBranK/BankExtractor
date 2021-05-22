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
	Original repository: https://github.com/eXpl0it3r/BankExtractor/
	Current fork: https://github.com/ItsBranK/BankExtractor/

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
	Unknown = 0x0,
	Settings = 0x1,
	SoundEffects = 0x2,
	EventAction = 0x3,
	Event = 0x4,
	SequenceContainer = 0x5,
	SwitchContainer = 0x6,
	ActorMixer = 0x7,
	AudioBus = 0x8,
	BlendContainer = 0x9,
	MusicSegment = 0xA,
	MusicTrack = 0xB,
	MusicSwitchContainer = 0xC,
	MusicPlaylistContainer = 0xD,
	Attenuation = 0xE,
	DialogueEvent = 0xF,
	MotionBus = 0x10,
	MotionFx = 0x11,
	Effect = 0x12,
	AuxiliaryBus = 0x13
};

class Section
{
public:
	char Id[4]; // Name of the section (four characters long).
	size_t Length; // Length of the section.

public:
	Section();
	~Section();
};

class ObjectSection : public Section
{
public:
	uint32_t Count; // Number of objects.

public:
	ObjectSection();
	~ObjectSection();
};

class SoundSection : public Section
{
public:
	uint8_t UnknownData[0x4];
	uint32_t Count; // Number of sound banks.
};

class SoundTypeSection : public Section
{
public:
	uint8_t UnknownData[0x4];
	uint32_t Count; // Number of sound banks.
};

class BKHD : public Section
{
public:
	uint32_t Version; // Soundbank version.
	uint32_t Id; // Soundbank id.
	uint8_t UnknownData[0x8];

public:
	BKHD();
	~BKHD();
};

class DIDX
{
public:
	uint32_t Id; // Id of the ".wem" file.
	size_t Offset; // DATA section offset.
	size_t Length; // Length of the ".wem" file.

public:
	DIDX();
	~DIDX();
};

class HIRC
{
public:
	WwiseObjects Type; // Object type identifier.
	size_t Length; // Length of the object.
	uint32_t Id; // Internal id for the object.

public:
	HIRC();
	~HIRC();

public:
	std::string GetObjectName() const;
};

class STID
{
public:
	uint32_t SoundId; // Sound id to match the object listed in the HIRC section.
	uint8_t NameLength; // Length of characters the sounds name is.

public:
	STID();
	~STID();
};

class STID_2 : public STID
{
public:
	char Name[256]; // Name of the sound bank, 256 buffer.

public:
	STID_2();
	~STID_2();
};

class SoundBank
{
private:
	std::string FileName;
	std::filesystem::path FilePath;
	std::fstream BankFile;

private:
	BKHD BankHeader;
	size_t DataOffset;
	std::vector<DIDX> WemData;
	std::vector<HIRC> WwiseObjects;

private:

public:
	SoundBank(std::filesystem::path soundBankFile);
	SoundBank(const std::string& soundBankFile);
	~SoundBank();

public:
	int32_t FileCount() const;
	int32_t ObjectCount() const;
	void PhraseSections();
	void ExtractFiles();
	void PrintHeader();
	void PrintObjects();
};
