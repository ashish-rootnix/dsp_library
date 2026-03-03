find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport)

# GUI Application
set(CMAKE_AUTOMOC ON)

# QCustomPlot as a library
add_library(qcustomplot
    third_party/qcustomplot/qcustomplot.cpp
)

target_include_directories(qcustomplot SYSTEM PUBLIC third_party/qcustomplot)
target_link_libraries(qcustomplot PUBLIC Qt6::Widgets Qt6::PrintSupport)

# Silence warnings in third-party code — not our problem to fix
target_compile_options(qcustomplot PRIVATE -w)

add_executable(dsp_gui
    main.cpp
    SineWavePlot.cpp
)

target_link_libraries(dsp_gui
    PRIVATE
        dsp_core
        qcustomplot
        Qt6::Widgets
)

# Use DT_RPATH (not DT_RUNPATH) so system libs are found before anything in
# LD_LIBRARY_PATH, preventing snap's bundled libpthread from being picked up.
set_target_properties(dsp_gui PROPERTIES
    BUILD_RPATH "/usr/lib/x86_64-linux-gnu;/lib/x86_64-linux-gnu"
)
target_link_options(dsp_gui PRIVATE -Wl,--disable-new-dtags)
