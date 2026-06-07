# CMakeList.txt: CMake-Projekt für "GFXEngineGameTemplate". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#

# Fügen Sie der ausführbaren Datei dieses Projekts eine Quelle hinzu.
add_executable (${PROJECT_NAME} "${PROJECT_NAME}.cpp" "MyGame.h" "MyGame.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

# TODO: Fügen Sie bei Bedarf Tests hinzu, und installieren Sie Ziele.

# link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE GFXEngine
    PRIVATE LibGFX
    PRIVATE glm::glm
    PRIVATE imgui
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE GFXEditor
)

# include directories
target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${GFXEngine_SOURCE_DIR}
        ${GFXEditor_SOURCE_DIR}
)

# compile features
target_compile_features(${PROJECT_NAME} 
    PUBLIC cxx_std_20
)

# copy shaders to output directory after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${GFXEngine_SOURCE_DIR}/assets/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)