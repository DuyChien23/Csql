//
// Created by chiendd on 21/07/2024.
//

#ifndef CONSTANT_H
#define CONSTANT_H

namespace Configs {
    const std::string databaseMetadataFileName = "metadata.bin";
    const uint32_t storageUnitSize = 4096;
    const uint32_t storageFreeSpaceSize = storageUnitSize / 16;
    const uint32_t maxSlots = 3;
    const size_t cacheMaxSize = 100000;
    const std::string databaseDictionaryName = "Databases";
    const std::string entityFileExtension = ".csql";

    const std::string defaultServerAddress = "127.0.0.1";
    const unsigned short defaultServerPort = 8080;
    const int maxBacklog = 3;
}

#endif //CONSTANT_H
