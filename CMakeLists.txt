# CMakeList.txt: CMake-Projektdatei der obersten Ebene. Führen Sie hier die globale Konfiguration aus,
# und schließen Sie Unterprojekte ein.
#
cmake_minimum_required (VERSION 3.10)

set(PROJECT_NAME "GFXEngineGameTemplate")
set(GFXENGINE_TAG "dev")


# Aktivieren Sie Hot Reload für MSVC-Compiler, sofern unterstützt.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (${PROJECT_NAME})


include(FetchContent)

FetchContent_Declare(
  GFXEngine
  GIT_REPOSITORY https://github.com/Andy16823/GFXEngine.git
  GIT_TAG ${GFXENGINE_TAG}
)

# Herunterladen und als Unterprojekt integrieren (falls das Repo ein CMake-Projekt enthält)
FetchContent_MakeAvailable(GFXEngine)


# Schließen Sie Unterprojekte ein.
add_subdirectory (${PROJECT_NAME})
