#ifndef MandelbrotWidget_HPP
#define MandelbrotWidget_HPP

#include "types.hpp"

#include <limits>
#include <vector>
using std::vector;

#include <QColor>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QResizeEvent>

#define DEFAULT_VIEW_WIDTH          (300)
#define DEFAULT_VIEW_HEIGHT         (300)

#define DEFAULT_MANDEL_ORIGIN_X     (-0.75)
#define DEFAULT_MANDEL_ORIGIN_Y     (0.0)
#define DEFAULT_MANDEL_PIXEL_DELTA  (0.0035)

#define DEFAULT_MAX_ITERATIONS      (10000)
#define DEFAULT_ZOOM_MULTIPLIER     (2.0)

#define DEFAULT_QCOLOR_IN_SET       (QColor(0, 0, 0))
#define DWELL_VALUE_IN_SET          (std::numeric_limits<dwellValue>::max())

#define MAX_WORKER_THREADS      (100)



class MandelbrotWidget : public QWidget{
    Q_OBJECT

    public:
        MandelbrotWidget(QWidget *parent = NULL);
        void initPointers();
        void init();
        ~MandelbrotWidget();
        void deinit();

        void setViewParameters(uint viewWidth, uint viewHeight);
        void setMandelLocation(MandelLocation location);
        void setMaxIterations(uint maxIterations);
        void setZoomMultiplier(precisionFloat zoomMultiplier);

        static MandelPoint transformViewPointToMandelPoint(ViewPoint point, ViewParameters viewParameters, MandelLocation mandelLocation);

        
        static dwellValue calculateMandelPointDwellValue(MandelPoint point);
        
        static QColor calculateDwellValueColor(dwellValue value);
        
        static QColor mixColors(float ratio, QColor const &lower, QColor const &higher);

        static void mapMandelLocationSegmentToDwellValues(MandelLocation mandelLocation, ViewParameters viewParameters, dwellValue **iterationValues, uint startLine, uint endLine);
        static void mapMandelLocationToDwellValues(MandelLocation mandelLocation, ViewParameters viewParameters, dwellValue **iterationValues);
        void mapDwellValuesToQImage(dwellValue  **iterationValues);

    signals:
        void signalZoom();
        void signalResize();

    protected:
        void paintEvent(QPaintEvent *event);
        void paintImage(QImage *image);

        void resizeEvent(QResizeEvent *event);

        void mousePressEvent(QMouseEvent *event);
        QMouseEvent *latestQMouseEvent;

    public slots:
        void slotZoomEvent();
        void slotResizeEvent();

    private:
        QImage *image;
        dwellValue  **dwellValues;
        bool dwellValuesAreValid;

        ViewParameters viewParameters;
        MandelLocation mandelLocation;

        static uint maxIterations;
        static precisionFloat zoomMultiplier;


};

#endif // MandelbrotWidget_HPP
















