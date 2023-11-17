#include "BankExtractor.hpp"

namespace Settings
{
    bool Swap = false;
    bool NoDir = false;
    bool Index = false;
    bool Info = false;
}

Section::Section() : Length(0)
{
    memset(Id, 0, _countof(Id));
}

Section::Section(const Section& section) : Length(section.Length)
{
    memcpy_s(Id, _countof(Id), section.Id, _countof(section.Id));
}

Section::~Section() {}

Section& Section::operator=(const Section& section)
{
    memcpy_s(Id, _countof(Id), section.Id, _countof(section.Id));
    Length = section.Length;
    return *this;
}

BKHD::BKHD() : Version(0), Id(0) {}

BKHD::BKHD(const BKHD& bkhd) : Version(bkhd.Version), Id(bkhd.Id) {}

BKHD::~BKHD() {}

BKHD& BKHD::operator=(const BKHD& bkhd)
{
    Version = bkhd.Version;
    Id = bkhd.Id;
    return *this;
}

DIDX::DIDX() : Id(0), Offset(0), Length(0) {}

DIDX::DIDX(const DIDX& didx) : Id(didx.Id), Offset(didx.Offset), Length(didx.Length) {}

DIDX::~DIDX() {}

DIDX& DIDX::operator=(const DIDX& didx)
{
    Id = didx.Id;
    Offset = didx.Offset;
    Length = didx.Length;
    return *this;
}

HIRC::HIRC() : Type(WwiseObjects::Unknown), Length(0), Id(0) {}

HIRC::HIRC(const HIRC& hirc) : Type(hirc.Type), Length(hirc.Length), Id(hirc.Id) {}

HIRC::~HIRC() {}

std::string HIRC::GetTypeStr() const
{
    switch (Type)
    {
    case WwiseObjects::Settings:
        return "Settings";
    case WwiseObjects::SoundEffects:
        return "Sound SFX/Voice";
    case WwiseObjects::EventAction:
        return "Event Action";
    case WwiseObjects::Event:
        return "Event";
    case WwiseObjects::SequenceContainer:
        return "Sequence Container";
    case WwiseObjects::SwitchContainer:
        return "Switch Container";
    case WwiseObjects::ActorMixer:
        return "Actor Mixer";
    case WwiseObjects::AudioBus:
        return "Audio Bus";
    case WwiseObjects::BlendContainer:
        return "Blend Container";
    case WwiseObjects::MusicSegment:
        return "Music Segment";
    case WwiseObjects::MusicTrack:
        return "Music Track";
    case WwiseObjects::MusicSwitchContainer:
        return "Music Switch Container";
    case WwiseObjects::MusicPlaylistContainer:
        return "Music Playlist Container";
    case WwiseObjects::Attenuation:
        return "Attenuation";
    case WwiseObjects::DialogueEvent:
        return "Dialogue Event";
    case WwiseObjects::MotionBus:
        return "Motion Bus";
    case WwiseObjects::MotionFx:
        return "Motion FX";
    case WwiseObjects::Effect:
        return "Effect";
    case WwiseObjects::AuxiliaryBus:
        return "Auxiliary Bus";
    default:
        return "Unknown";
    };
}

HIRC& HIRC::operator=(const HIRC& hirc)
{
    Type = hirc.Type;
    Length = hirc.Length;
    Id = hirc.Id;
    return *this;
}

STID::STID() : SoundId(0), NameLength(0) {}

STID::STID(const STID& stid) : SoundId(stid.SoundId), NameLength(stid.NameLength) {}

STID::~STID() {}

STID& STID::operator=(const STID& stid)
{
    SoundId = stid.SoundId;
    NameLength = stid.NameLength;
    return *this;
}

ObjSection::ObjSection() : Count(0) {}

ObjSection::ObjSection(const ObjSection& objSection) : Count(objSection.Count) {}

ObjSection::~ObjSection() {}

ObjSection& ObjSection::operator=(const ObjSection& objSection)
{
    Count = objSection.Count;
    return *this;
}

SfxSection::SfxSection() : Count(0) {}

SfxSection::SfxSection(const SfxSection& sfxSection) : Count(sfxSection.Count) {}

SfxSection::~SfxSection() {}

SfxSection& SfxSection::operator=(const SfxSection& sfxSection)
{
    Count = sfxSection.Count;
    return *this;
}

SoundBank::SoundBank(const std::filesystem::path& filePath) : _filePath(std::filesystem::absolute(filePath)), _dataOffset(0) {}

SoundBank::SoundBank(const SoundBank& soundBank) :
    _filePath(soundBank._filePath),
    _bankHeader(soundBank._bankHeader),
    _dataOffset(soundBank._dataOffset),
    _wemData(soundBank._wemData),
    _wemObjects(soundBank._wemObjects)
{

}

SoundBank::~SoundBank()
{
    if (_bankFile.is_open())
    {
        _bankFile.close();
    }
}

size_t SoundBank::Swap(size_t value)
{
#ifdef __GNUC__
    return __builtin_bswap32(value);
#elif _MSC_VER
    return _byteswap_ulong(value);
#endif
}

bool SoundBank::ParseSections()
{
    if (std::filesystem::exists(_filePath))
    {
        _bankFile.open(_filePath, std::ios::binary);

        if (_bankFile.is_open())
        {
            std::cout << "Parsing sections..." << std::endl;

            Section currentSection;
            //ObjSection objSection; // Unimplemented.
            //SfxSection sfxSection; // Unimplemented.
            DIDX currentData;
            HIRC currentObject;

            while (_bankFile.read(reinterpret_cast<char*>(&currentSection), sizeof(currentSection)))
            {
                const size_t sectionPos = _bankFile.tellg();

                if (Settings::Swap)
                {
                    currentSection.Length = Swap(currentSection.Length);
                }

                if (strncmp(currentSection.Id, "BKHD", _countof(currentSection.Id)) == 0)
                {
                    if (Settings::Info)
                    {
                        std::cout << "Bank Header (BKHD)" << std::endl;
                    }

                    _bankFile.read(reinterpret_cast<char*>(&_bankHeader), sizeof(_bankHeader));
                    _bankFile.seekg((currentSection.Length - sizeof(_bankHeader)), std::ios_base::cur);

                    if (Settings::Info)
                    {
                        std::cout << "Header Version: " << _bankHeader.Version << std::endl;
                        std::cout << "Header Id: " << _bankHeader.Id << std::endl << std::endl;
                    }
                }
                else if (strncmp(currentSection.Id, "DIDX", _countof(currentSection.Id)) == 0)
                {
                    if (Settings::Info)
                    {
                        std::cout << "Data Index (DIDX)" << std::endl;
                    }

                    for (uint32_t i = 0; i < currentSection.Length; i += sizeof(currentData))
                    {
                        _bankFile.read(reinterpret_cast<char*>(&currentData), sizeof(currentData));
                        _wemData.push_back(currentData);

                        if (Settings::Info)
                        {
                            std::cout << "Index Id: " << currentData.Id << std::endl;
                            std::cout << "Index Offset: " << currentData.Offset << std::endl;
                            std::cout << "Index Length: " << currentData.Length << std::endl << std::endl;
                        }
                    }
                }
                else if (strncmp(currentSection.Id, "DATA", _countof(currentSection.Id)) == 0)
                {
                    if (Settings::Info)
                    {
                        std::cout << "Data (DATA)" << std::endl;
                    }

                    _dataOffset = _bankFile.tellg();

                    if (Settings::Info)
                    {
                        std::cout << "Data Offset: " << _dataOffset << std::endl << std::endl;
                    }
                }
                else if (strncmp(currentSection.Id, "HIRC", _countof(currentSection.Id)) == 0) // Unfinished!
                {
                    //_bankFile.read(reinterpret_cast<char*>(&objSection), sizeof(objSection));
                    //_bankFile.seekg((currentSection.Length - sizeof(objSection)), std::ios_base::cur);

                    //std::cout << "Object Count: " << ObjSection.Count << std::endl;
                    //std::cout << "Section Length: " << ObjSection.Length << std::endl;

                    //for (uint32_t i = 0; i < currentSection.Length; i += sizeof(currentObject))
                    //{
                    //    _bankFile.read(reinterpret_cast<char*>(&currentObject), sizeof(currentObject));
                    //    _wemObjects.emplace_back(currentObject);
                    //}
                }
                else if (strncmp(currentSection.Id, "STID", _countof(currentSection.Id)) == 0) // Unfinished!
                {
                    //_bankFile.read(reinterpret_cast<char*>(&sfxSection), sizeof(sfxSection));
                    //_bankFile.seekg((currentSection.Length - sizeof(sfxSection)), std::ios_base::cur);

                    //_bankFile.read(reinterpret_cast<char*>(&soundSection), sizeof(soundSection));
                    //_bankFile.seekg((sectionPos - sizeof(currentSection)), std::ios_base::cur);
                }

                _bankFile.seekg(sectionPos + currentSection.Length);
            }

            _bankFile.close();
            std::cout << "Found " << std::to_string(_wemData.size()) << " Wwise Encoded Media (.wem) files." << std::endl;
            //std::cout << "Found " << std::to_string(_wemObjects.size()) << " Wwise Objects." << std::endl << std::endl;
            return !_wemData.empty();
        }
        else
        {
            std::cout << "Failed to open file \"" << _filePath.filename().replace_extension("").string() << "\", cannot parse sections!" << std::endl;
            return false;
        }
    }
    else
    {
        std::cout << "File \"" << _filePath.string() << "\" does not exist, cannot parse sections!" << std::endl;
        return false;
    }

    return false;
}

void SoundBank::ExtractFiles()
{
    if (!_wemData.empty())
    {
        std::cout << "Extracting files..." << std::endl;

        size_t fileCount = 1;
        _bankFile.open(_filePath, std::ios::binary);

        if (_bankFile.is_open())
        {
            for (DIDX& data : _wemData)
            {
                std::filesystem::path wemDirectory = _filePath.parent_path();

                if (!Settings::NoDir)
                {
                    std::filesystem::path directoryName = _filePath.filename().replace_extension("");
                    std::filesystem::path directory = _filePath.replace_filename(directoryName);
                    std::filesystem::create_directory(directory);
                    wemDirectory = directory;
                }

                std::filesystem::path wemfileName = wemDirectory;

                if (Settings::Index)
                {
                    wemfileName = wemfileName.append(std::to_string(data.Id)).replace_extension(".wem");
                }
                else
                {
                    std::stringstream ss;
                    ss << _filePath.filename().replace_extension("").string() << "_" << std::setfill('0') << std::setw(4) << std::right << fileCount;
                    wemfileName = wemfileName.append(ss.str()).replace_extension(".wem");
                }

                std::ofstream wemFile(wemfileName, std::ios::binary);

                if (Settings::Swap)
                {
                    data.Length = Swap(data.Length);
                    data.Offset = Swap(data.Offset);
                }

                if (wemFile.is_open())
                {
                    uint8_t* wemData = new uint8_t[data.Length];
                    _bankFile.seekg(_dataOffset + data.Offset);
                    _bankFile.read(reinterpret_cast<char*>(wemData), data.Length);
                    wemFile.write(reinterpret_cast<char*>(wemData), data.Length);
                    wemFile.close();
                    delete[] wemData;
                }

                fileCount++;

                if (Settings::Info)
                {
                    std::cout << "Extracted file: \"" << wemfileName.filename().u8string() << "\"." << std::endl;
                }
            }

            _bankFile.close();
        }
        else
        {
            std::cout << "Failed to open file \"" << _filePath.filename().replace_extension("").string() << "\", cannot export data!" << std::endl;
        }

        std::cout << "Extracted " << std::to_string(fileCount - 1) << " file(s)!" << std::endl;
    }
}

void SoundBank::PrintHeader()
{
    std::cout << "Header Information:" << std::endl;
    std::cout << "Version: " << std::to_string(_bankHeader.Version) << std::endl;
    std::cout << "Id: " << std::to_string(_bankHeader.Id) << std::endl << std::endl;
}

void SoundBank::PrintObjects()
{
    for (const HIRC& object : _wemObjects)
    {
        std::cout << "Type: " << object.GetTypeStr() << std::endl;
        std::cout << "Length: " << std::to_string(object.Length) << std::endl;
        std::cout << "Id: " << std::to_string(object.Id) << std::endl << std::endl;
    }
}

SoundBank& SoundBank::operator=(const SoundBank& soundBank)
{
    _filePath = soundBank._filePath;
    _bankHeader = soundBank._bankHeader;
    _dataOffset = soundBank._dataOffset;
    _wemData = soundBank._wemData;
    _wemObjects = soundBank._wemObjects;
    return *this;
}

bool ArugmentExists(const std::string& argumentToFind, const char* arguments[])
{
    int32_t index = 0;

    while (arguments[index])
    {
        if (argumentToFind.find(arguments[index]) != std::string::npos)
        {
            return true;
        }

        index++;
    }

    return false;
}

int32_t main(int32_t argumentCount, const char* arguments[])
{
    std::cout << "Wwise *.BNK File Extractor (ItsBranK Fork)" << std::endl << std::endl;

    if (argumentCount < 2)
    {
        std::cout << "Usage: BankExtractor filename.bnk [/swap] [/nodir] [/index]" << std::endl;
        std::cout << "\t/swap - Swap byte order (use it for unpacking \"Army of Two\")." << std::endl;
        std::cout << "\t/nodir - Create no additional directory for the *.wem files." << std::endl;
        std::cout << "\t/index - Writes the internal bnk id for the extracted *.wem file names." << std::endl;
        std::cout << "\t/info - Prints extra/detailed info to the console while extracting files." << std::endl;  
        return EXIT_SUCCESS;
    }

    Settings::Swap = ArugmentExists("/swap", arguments);
    Settings::NoDir = ArugmentExists("/nodir", arguments);
    Settings::Index = ArugmentExists("/index", arguments);
    Settings::Info = ArugmentExists("/info", arguments);

    std::filesystem::path _bankFile(arguments[1]);

    if (std::filesystem::exists(_bankFile))
    {
        SoundBank soundBank(_bankFile);

        if (soundBank.ParseSections())
        {
            soundBank.ExtractFiles();
        }
    }
    else
    {
        std::cout << "File does not exist, cannot extract contents!" << std::endl;
    }

    return EXIT_SUCCESS;
}
