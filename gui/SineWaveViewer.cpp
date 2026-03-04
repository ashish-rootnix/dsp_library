#include "SineWaveViewer.hpp"

using dsp::gui::SineWaveViewer;
/**
 * Paint cycle
 * 1) QSlider::valueChanged(int) signal emitted
 * 2) Qt event loop dispatches to connected slot: onSliderValueChanged()
 * 3) Slot calls regenerateSignal() -> sets new data on QCustomPlot
 * 4) regenerateSignal() calls replot() -> QCustomPlot queues internal repaint
 * 5) Event loop processes paint event -> QCustomPlot::paintEvent() draws pixels
 * 
 * Never call paintEvent() explicitlly. Event loop calls paintEvent() 
 */

SineWaveViewer::SineWaveViewer(QWidget* parent)
: QWidget(parent)   // Enter the ownership tree
, sample_rate_(16000.0)  // 1KHz
, duration_(1)       // 10ms window
, current_freq_(100)    // Starts at 100Hz
{
    // Step 1: Create Widget
    // 'this' as parent -> QT owns them. Layout will manage positioning
    // addWidget() will reparent them to the layout's parent automatically
    plot_ = new QCustomPlot(this);
    freqSlider_ = new QSlider(Qt::Horizontal, this);
    freqLabel_ = new QLabel(this);

    // Step 2: Configure Slider
    // QSlider marks in integers. We want frquency range 20-2000 Hz
    // Map: SLider int value in frewquncy HZ
    freqSlider_->setRange(0, 2000);    // Map 1Hz to 2000Hz
    freqSlider_->setValue(static_cast<int>(current_freq_));
    freqSlider_->setTickPosition(QSlider::TicksBelow);
    freqSlider_->setTickInterval(100);

    freqLabel_->setText(QString("Frequency: %1 Hz").arg(current_freq_, 0, 'f', 1));
    freqLabel_->setAlignment(Qt::AlignCenter);

    // Step 3: Connect Signal -> Slot
    // connect(sender, &SenderClass::signalName, receiver, &ReceiverClass::slotName)
    connect(freqSlider_, &QSlider::valueChanged, this, &SineWaveViewer::onSliderValueChanged);

    // Step 4: Layout
    // QVBoxLayout stacks widgets vertically. 
    // When we call addWidget(), the layout takes ownership of widget positioning
    // setLayout() assigns this layout to 'this' widget
    // The layout becomes a child of 'this' -> auto-deleted
    auto* sliderRow = new QHBoxLayout; // Horizontal: label + slider
    sliderRow->addWidget(freqLabel_);
    sliderRow->addWidget(freqSlider_, /*stretch*/1); // SLider gets remainin space

    auto* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(plot_, /*stretch*/1);
    mainLayout->addLayout(sliderRow); // Slider row at bottom

    setLayout(mainLayout);

    // Step 5: Configure Plot axes
    plot_->addGraph();
    plot_->xAxis->setLabel("Time (s)");
    plot_->yAxis->setLabel("Amplitude");
    plot_->yAxis->setRange(-1.2, 1.2);
    plot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // Mouse drag/zoom

    plot_->graph(0)->setPen(QPen(QColor(40, 120, 200), 2));

    // Step 6: Initial draw
    regenerateSignal();

    setWindowTitle("DSP Signal Viewer - Frequency Control");
    resize(900, 600);
}

/**
 * This functio nis called by event loop when QSlider emits valueChanged
 * -> OS sends mouse event to Qt event loop
 * -> Event loop dispatches to QSlider
 * -> QSlider updates its value, emits valueChanged(int)
 * -> Qt's meta-object dispatches calls to slots
 * -> We update state and request repaint
 */
void SineWaveViewer::onSliderValueChanged(int value)
{
    current_freq_ = static_cast<double>(value);

    /*if(sample_rate_ <= 2.0 * current_freq_)
        sample_rate_ = 4.0 *current_freq_;*/

    freqLabel_->setText(QString(QString("Frequency: %1 Hz").arg(current_freq_, 0, 'f', 1)));

    regenerateSignal();

    // Emit our own signal - anyone listening to frequencyChanged() get notified
    // Right now nobody is connected
    
    emit frequencyChanged(current_freq_);
}

void SineWaveViewer::regenerateSignal()
{
    auto signal = dsp::SignalGenerator::sine<double>(current_freq_, sample_rate_, duration_);

    const int numSamples = static_cast<int>(signal.size());
    QVector<double> xData(numSamples);
    QVector<double> yData(numSamples);

    for(std::size_t i = 0; i < signal.size(); ++i)
    {
        const auto idx = static_cast<int>(i);
        double t = static_cast<double>(i) / sample_rate_ * 1000.0;
        xData[idx] = t;
        yData[idx] = signal[i];
    }

    plot_->graph(0)->setData(xData, yData);

    plot_->xAxis->setRange(0, duration_);

    // replot() — THIS TRIGGERS THE PAINT CYCLe
    //   1. QCustomPlot marks itself as "needs repaint"
    //   2. Internally calls QWidget::update() (or paints immediately depending
    //      on replot mode)
    //   3. The event loop picks up the paint event
    //   4. QCustomPlot::paintEvent() is called
    //   5. A QPainter is created, draws axes, ticks, grid, data points
    //   6. QPainter is destroyed — painting done
    plot_->replot();
}
