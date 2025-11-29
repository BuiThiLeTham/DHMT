if(WIN32)
    add_library(glad
        include/glad/glad.h
        include/glad/glad_wgl.h
        src/glad.c
        src/glad_wgl.c
    )
elseif(APPLE)
    add_library(glad
        include/glad/glad.h
        src/glad.c
    )
elseif(UNIX)
    add_library(glad
        include/glad/glad.h
        include/glad/glad_glx.h
        src/glad.c
        src/glad_glx.c
    )
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

target_include_directories(glad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
