#pragma once

#include "Ammo.h"
#include "Drill.h"
#include "Gun.h"
#include "Window.h"

#include <LAS/json.h>
#include <LAS/HelperFunctions.h>
#include <LAS/Logging.h>

#include <filesystem>
#include <memory>

namespace ShooterCentral{

    class FileIO {
    public:
        struct SubDirectories{
            std::string parentDir;
            std::string ammoDir;
            std::string drillDir;
            std::string eventDir;
            std::string gunDir;
        };

        FileIO(LAS::Logging::LoggerPtr setLogger);
        ~FileIO();

        std::string     getSaveDirectory() const;
        bool            setSaveDirectory(std::string path);

        bool            setupSubDirectories(std::string parentDir="");
    private:
        std::string             saveDirectory;
        FileIO::SubDirectories  subDirectories;
        LAS::Logging::LoggerPtr logger;
    };

    using FileIOPtr = std::shared_ptr<FileIO>;
}

// Need to make JSON read and writes for each object and write to framework list
// FileIO::readAmmo(map&) to add each ammo part