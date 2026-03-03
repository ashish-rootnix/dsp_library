# Core library 
add_library(dsp_core INTERFACE)

target_include_directories(dsp_core
        INTERFACE
            $(PROJECT_SOURCE_DIR)/include
)
# Initial phases only header files were in this folder structure so INTERFACE is used
# Later *.cpp files added so changed to STATIC library
