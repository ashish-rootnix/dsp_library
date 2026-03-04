#ifndef DSP_GUI_SINE_WAVE_VIEWER_HPP
#define DSP_GUI_SINE_WAVE_VIEWER_HPP

#include <qcustomplot.h>
#include "../include/dsp/core/Signal.hpp"
#include "../include/dsp/dsp/dsp/SignalGenerator.hpp"

#include <QWidget>      // Base class for all UI objects
#include <QSlider>      // Draggable slider widget
#include <QLabel>       // Static text display
#include <QVBoxLayout>  // Vertical box layout manager
#include <QHBoxLayout>  // Horizontal box layout manager
#include <cmath>

namespace dsp::gui {
    /**
     * Q-Object Macreo 
     * It does nothing at compile time - its a marker. Qt's Meta-Object Compiler
     * MOC scans header files for Q-OBJECT, then generate a hidden .moc file containing
     * 1) Signal emission functions (The body of your signal declarations)
     * 2) A string table mapping signal/slot names to function pointers
     * 3) The staticMetaObject used by connect() for deispatch
     * 
     * Rules:
     * Must apper in private section of any class using signals/slots
     * class must inherit Q_Object directly /indirectly
     * Adter adding Q-OBJECT to a class, recompile the 
     * One Q-OBJECT per class.
     */

     class SineWaveViewer : public QWidget
     {  
            Q_OBJECT // MOC Marker -> without this connect() calls silently fails
        public:
        /**
         * Parent parameter feeds into QWidget's parent-chile tree
         * nullptr = top level window 
         * (no parent, must be deleted manually or cleaned up on app exists)
         */
        explicit SineWaveViewer(QWidget* parent = nullptr);

        // No destructor neede - Dt's parent-child ownership handles all chile widgets
        // QCustomPlot, QSlider, QLabel all parented to this

        /** 
         * SIGNALS
         * Declares only function body. MOC generates the body.
         * Signals are emitted by this class to notify anyone listening.
         * signals: is Qt keyword
         * emit is Qt macro
         * emit sginalFunction() just calls signalFunction()
         * MOC iterate the connection list and invoke all slots by emit
        */
       signals:
        void frequencyChanged(double newFreq);

        /**
         * SLOTS
         * Slots are normal member functions that can be connected to signals
         * They can also called directly as functions
         * The slots: keyword tells MOC to register then in the meta-object table
         * Slots has same visisbility rules as C++
         */
        // QSlider::valueChanged(int) -> this slot
        private slots:
            void onSliderValueChanged(int value);

            /**
             * Widget ownership herem not smart pointer
             * Qt's parent-child trees is the ownership model
             * QT MOC takes care of delete dynamic memory clean up
             * 
             * Qt Widget owned by parent -> raw pointer
             */
            
        private:
            QCustomPlot* plot_;     // Plot widget -> owned by layout -> owned by this
            QSlider* freqSlider_;    // Frquency control owned by this layout
            QLabel* freqLabel_;      // Shows frequency value - owned by this layout
            
            double sample_rate_;
            double duration_;
            double current_freq_;

            // Regenerate sine wave data and replots
            void regenerateSignal();
     };
    
} // namespace dsp::gui
#endif // DSP_GUI_SINE_WAVE_VIEWER_HPP
