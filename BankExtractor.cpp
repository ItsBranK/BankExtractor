#include "BankExtractor.hpp"

namespace Settings
{
    bool Swap = false;
    bool NoDir = false;
    bool Index = false;
    bool Info = false;
}

Section::Section() : Length(0) {}

Section::~Section() {}

ObjectSection::ObjectSection() : Count(0) {}

ObjectSection::~ObjectSection() {}

BKHD::BKHD() : Version(0), Id(0) {}

BKHD::~BKHD() {}

DIDX::DIDX() : Id(0), Offset(0), Length(0) {}

DIDX::~DIDX() {}

HIRC::HIRC() : Type(WwiseObjects::Unknown), Length(0), Id(0) {}

HIRC::~HIRC() {}

std::string HIRC::GetObjectName() const
{
    std::string returnName;

    switch (static_cast<WwiseObjects>(Type))
    {
    case WwiseObjects::Settings:
        returnName = "Settings";
        break;
    case WwiseObjects::SoundEffects:
        returnName = "Sound SFX/Sound Voice";
        break;
    case WwiseObjects::EventAction:
        returnName = "Event Action";
        break;
    case WwiseObjects::Event:
        returnName = "Event";
        break;
    case WwiseObjects::SequenceContainer:
        returnName = "Sequence Container";
        break;
    case WwiseObjects::SwitchContainer:
        returnName = "Switch Container";
        break;
    case WwiseObjects::ActorMixer:
        returnName = "Actor Mixer";
        break;
    case WwiseObjects::AudioBus:
        returnName = "Audio Bus";
        break;
    case WwiseObjects::BlendContainer:
        returnName = "Blend Container";
        break;
    case WwiseObjects::MusicSegment:
        returnName = "Music Segment";
        break;
    case WwiseObjects::MusicTrack:
        returnName = "Music Track";
        break;
    case WwiseObjects::MusicSwitchContainer:
        returnName = "Music Switch Container";
        break;
    case WwiseObjects::MusicPlaylistContainer:
        returnName = "Music Playlist Container";
        break;
    case WwiseObjects::Attenuation:
        returnName = "Attenuation";
        break;
    case WwiseObjects::DialogueEvent:
        returnName = "Dialogue Event";
        break;
    case WwiseObjects::MotionBus:
        returnName = "Motion Bus";
        break;
    case WwiseObjects::MotionFx:
        returnName = "Motion FX";
        break;
    case WwiseObjects::Effect:
        returnName = "Effect";
        break;
    case WwiseObjects::AuxiliaryBus:
        returnName = "Auxiliary Bus";
        break;
    default:
        returnName = "Unknown";
        break;
    };

    return returnName;
}

STID::STID() : SoundId(0), NameLength(0) {}

STID::~STID() {}

STID_2::STID_2() { memset(Name, 0, sizeof(Name));}

STID_2::~STID_2() {}

SoundBank::SoundBank(const std::filesystem::path& filePath) : FilePath(std::filesystem::absolute(filePath)), FileName(filePath.filename().replace_extension("").u8string()), DataOffset(0)
{
    if (std::filesystem::exists(FilePath))
    {
        BankFile.open(FilePath, std::ios::binary | std::ios::in);
    }
}

SoundBank::~SoundBank()
{
    BankFile.close();
}

size_t SoundBank::FileCount() const
{
    return WemData.size();
}

size_t SoundBank::ObjectCount() const
{
    return WwiseObjects.size();
}

void SoundBank::PhraseSections()
{
    if (BankFile.is_open())
    {
        std::cout << "Phrasing sections..." << std::endl;

        Section currentSection;
        ObjectSection objectSection;
        SoundSection soundSection;
        DIDX currentData;
        HIRC currentObject;

        while (BankFile.read(reinterpret_cast<char*>(&currentSection), sizeof(currentSection)))
        {
            size_t sectionPos = BankFile.tellg();

            if (Settings::Swap)
            {
                currentSection.Length = _byteswap_ulong(currentSection.Length);
            }

            if (strncmp(currentSection.Id, "BKHD", 4) == 0)
            {
                if (Settings::Info)
                {
                    std::cout << "Bank Header (BKHD)" << std::endl;
                }

                BankFile.seekg(sectionPos - sizeof(currentSection));
                BankFile.read(reinterpret_cast<char*>(&BankHeader), sizeof(BankHeader));

                if (Settings::Info)
                {
                    std::cout << "Header Version: " << BankHeader.Version << std::endl;
                    std::cout << "Header Id: " << BankHeader.Id << std::endl << std::endl;
                }
            }
            else if (strncmp(currentSection.Id, "DIDX", 4) == 0)
            {
                if (Settings::Info)
                {
                    std::cout << "Data Index (DIDX)" << std::endl;
                }

                for (size_t i = 0; i < currentSection.Length; i += sizeof(currentData))
                {
                    BankFile.read(reinterpret_cast<char*>(&currentData), sizeof(currentData));
                    WemData.emplace_back(currentData);

                    if (Settings::Info)
                    {
                        std::cout << "Index Id: " << currentData.Id << std::endl;
                        std::cout << "Index Offset: " << currentData.Offset << std::endl;
                        std::cout << "Index Length: " << currentData.Length << std::endl << std::endl;
                    }
                }
            }
            else if (strncmp(currentSection.Id, "DATA", 4) == 0)
            {
                if (Settings::Info)
                {
                    std::cout << "Data (DATA)" << std::endl;
                }

                DataOffset = BankFile.tellg();

                if (Settings::Info)
                {
                    std::cout << "Data Offset: " << DataOffset << std::endl << std::endl;
                }
            }
            else if (strncmp(currentSection.Id, "HIRC", 4) == 0) // UNFINISHED!
            {
                //BankFile.seekg(sectionPos - sizeof(currentSection));
                //BankFile.read(reinterpret_cast<char*>(&objectSection), sizeof(objectSection));

                //std::cout << "Object Count: " << objectSection.Count << std::endl;
                //std::cout << "Section Length: " << objectSection.Length << std::endl;

                //for (size_t i = 0; i < currentSection.Length; i += sizeof(currentObject))
                //{
                //    BankFile.read(reinterpret_cast<char*>(&currentObject), sizeof(currentObject));
                //    WwiseObjects.emplace_back(currentObject);
                //}
            }
            else if (strncmp(currentSection.Id, "STID", 4) == 0) // UNFINISHED!
            {
                //BankFile.seekg(sectionPos - sizeof(currentSection));
                //BankFile.read(reinterpret_cast<char*>(&soundSection), sizeof(soundSection));
            }

            BankFile.seekg(sectionPos + currentSection.Length);
        }

        BankFile.clear();

        std::cout << "Found " << std::to_string(FileCount()) << " Wwise Encoded Media (.wem) files." << std::endl;
        //std::cout << "Found " << std::to_string(ObjectCount()) << " Wwise Objects." << std::endl << std::endl;
    }
    else
    {
        std::cout << "Failed to open file \"" << FileName << "\", cannot phrase sections!" << std::endl;
    }
}

void SoundBank::ExtractFiles()
{
    if (FileCount() > 0)
    {
        std::cout << "Extracting files..." << std::endl;

        size_t fileCount = 1;

        for (DIDX& data : WemData)
        {
            std::filesystem::path wemDirectory = FilePath.parent_path();

            if (!Settings::NoDir)
            {
                std::filesystem::path directoryName = FilePath.filename().replace_extension("");
                std::filesystem::path directory = FilePath.replace_filename(directoryName);
                std::filesystem::create_directory(directory);
                wemDirectory = directory;
            }

            std::filesystem::path wemfilename = wemDirectory;

            if (Settings::Index)
            {
                wemfilename = wemfilename.append(std::to_string(data.Id)).replace_extension(".wem");
            }
            else
            {
                std::stringstream ss;
                ss << FileName << "_" << std::setfill('0') << std::setw(4) << std::right << fileCount;
                wemfilename = wemfilename.append(ss.str()).replace_extension(".wem");
            }

            std::fstream wemFile(wemfilename, std::ios::out | std::ios::binary);

            if (Settings::Swap)
            {
                data.Length = _byteswap_ulong(data.Length);
                data.Offset = _byteswap_ulong(data.Offset);
            }

            if (wemFile.is_open())
            {
                std::vector<char> wemData(data.Length);
                BankFile.seekg(DataOffset + data.Offset);
                BankFile.read(static_cast<char*>(wemData.data()), data.Length);
                wemFile.write(static_cast<char*>(wemData.data()), data.Length);
            }

            fileCount++;

            if (Settings::Info)
            {
                std::cout << "Extracted file: \"" << wemfilename.filename().u8string() << "\"." << std::endl;
            }
        }

        std::cout << "Extracted " << std::to_string(fileCount - 1) << " file(s)!" << std::endl;
    }
}

void SoundBank::PrintHeader()
{
    std::cout << "Header Information:" << std::endl;
    std::cout << "Version: " << std::to_string(BankHeader.Version) << std::endl;
    std::cout << "Id: " << std::to_string(BankHeader.Id) << std::endl << std::endl;
}

void SoundBank::PrintObjects()
{
    for (HIRC& object : WwiseObjects)
    {
        std::cout << "Type: " << object.GetObjectName() << std::endl;
        std::cout << "Length: " << std::to_string(object.Length) << std::endl;
        std::cout << "Id: " << std::to_string(object.Id) << std::endl << std::endl;
    }
}

bool ArugmentExists(const std::string& argumentToFind, const char* arguments[])
{
    int32_t index = 0;

    while (arguments[index] != NULL)
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

    std::filesystem::path bankFile(arguments[1]);

    if (std::filesystem::exists(bankFile))
    {
        SoundBank soundBank(bankFile);
        soundBank.PhraseSections();

        if (soundBank.FileCount() > 0)
        {
            soundBank.ExtractFiles();
        }
        else
        {
            std::cout << "Could not find any Wwise Encoded Media (.wem) files to extract!" << std::endl;
        }
    }
    else
    {
        std::cout << "File does not exist, cannot extract contents!" << std::endl;
    }

    return EXIT_SUCCESS;
}