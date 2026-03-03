#ifndef DSP_GUI_SINE_WAVE_PLOT_HPP
#define DSP_GUI_SINE_WAVE_PLOT_HPP

#include <qcustomplot.h>
#include "../include/dsp/dsp/dsp/SignalGenerator.hpp"
#include "../include/dsp/dsp/dsp/NyquistValidator.hpp"
#include "../include/dsp/core/Signal.hpp"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

namespace dsp::gui {

    class SineWavePlot : public QWidget
    {   
            Q_OBJECT
        public:
            explicit SineWavePlot(QWidget* parent = nullptr) : QWidget(parent)
            {
                auto* layout = new QVBoxLayout(this);

                // Title
                auto* label = new QLabel("DSP Library - Sine Wave Plot", this);
                label -> setAlignment(Qt::AlignCenter);
                label -> setStyleSheet("font-size: 16px; font-weight: bold; padding: 8px");
                layout -> addWidget(label);

                // Plot widget
                plot_ = new QCustomPlot(this);
                layout->addWidget(plot_);

                generate_and_plot();
            }

        private:
            QCustomPlot* plot_;

            void generate_and_plot()
            {
                const double frequency   = 100.0;   // 100 Hz sine
                const double sample_rate = 8000.0;  // 8000 Hz sample rate
                const double duration    = 0.05;    // 50 ms — 5 full cycles visible
                const double nyquist     = sample_rate / 2.0;

                // Generate the signal using your DSP library
                auto signal = dsp::SignalGenerator::sine<double>(frequency, sample_rate, duration);

                QVector<double> x_time(static_cast<int>(signal.size()));
                QVector<double> y_amplitude(static_cast<int>(signal.size()));

                for (std::size_t i = 0; i < signal.size(); ++i) {
                    const auto idx = static_cast<int>(i);
                    x_time[idx]      = static_cast<double>(i) / sample_rate * 1000.0;  // ms
                    y_amplitude[idx] = signal[i];
                }

                // ── Time domain plot ──
                plot_->addGraph();
                plot_->graph(0)->setData(x_time, y_amplitude);
                plot_->graph(0)->setPen(QPen(QColor(41, 128, 185), 2));  // blue
                plot_->graph(0)->setName(QString("Sine %1 Hz @ %2 Hz SR")
                        .arg(frequency)
                        .arg(sample_rate));

                // ── Nyquist info as text on plot ──
                auto* text_item = new QCPItemText(plot_);
                text_item->setPositionAlignment(Qt::AlignTop | Qt::AlignRight);
                text_item->position->setType(QCPItemPosition::ptAxisRectRatio);
                text_item->position->setCoords(0.98, 0.1);
                text_item->setText(
                    QString("f = %1 Hz\nFs = %2 Hz\nNyquist = %3 Hz\nStatus: %4")
                        .arg(frequency)
                        .arg(sample_rate)
                        .arg(nyquist)
                        .arg(frequency < nyquist ? "VALID" : "ALIASED")
                );

                text_item->setFont(QFont("Monospace", 10));
                text_item->setPadding(QMargins(8, 8, 8, 8));
                text_item->setBrush(QBrush(QColor(255, 255, 255, 200)));
                text_item->setPen(QPen(QColor(150, 150, 150)));

                // Axis labels
                plot_->xAxis->setLabel("Time (ms)");
                plot_->yAxis->setLabel("Amplitude");
                plot_->xAxis->setRange(0, duration * 1000.0);
                plot_->yAxis->setRange(-1.2, 1.2);

                // Legend
                plot_->legend->setVisible(true);
                plot_->legend->setFont(QFont("Arial", 9));

                plot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
                plot_->replot();  
            }
    };

} // namespace dsp::gui
#endif // DSP_GUI_SINE_WAVE_PLOT_HPP
