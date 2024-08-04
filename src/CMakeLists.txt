set(Sources
        Core/Util/types.h
        Core/Util/config.h
        Core/Util/errors.h
        Core/Storage/entity.cpp
        Core/Storage/entity.h
        Core/Storage/attribute.cpp
        Core/Storage/attribute.h
        Core/Storage/IO_Manager/io_manager.cpp
        Core/Storage/IO_Manager/io_manager.h
        Core/Storage/database.cpp
        Core/Storage/database.h
        Core/Storage/Page/slotted_page.cpp
        Core/Storage/Page/slotted_page.h
        Core/Storage/Page/data_page.cpp
        Core/Storage/Page/data_page.h
        Core/Storage/Page/free_page.cpp
        Core/Storage/Page/free_page.h
        Core/Storage/LRUCache/lru_cache.h
        Core/Storage/LRUCache/linked_list.h
)

add_library(source_libs ${Sources})

add_executable(Csql main.cpp)