#include "SineWavePlot.hpp"
#include <QApplication>
#include <QMainWindow>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("DSP Library - Signal Viewer");
    window.setCentralWidget(new dsp::gui::SineWavePlot());
    window.resize(900, 500);
    window.show();

    return app.exec();
}
