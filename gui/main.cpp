#include "SineWaveViewer.hpp"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    dsp::gui::SineWaveViewer viewer;
    viewer.show();

    return app.exec();
}
