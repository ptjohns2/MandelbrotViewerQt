#ifndef MANDELBROT_HPP
#define MANDELBROT_HPP

#include <vector>
using std::vector;

#include <QColor>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QResizeEvent>

#define DEFAULT_VIEW_WIDTH          400
#define DEFAULT_VIEW_HEIGHT         400
#define DEFAULT_MANDEL_ORIGIN_X     -0.75
#define DEFAULT_MANDEL_ORIGIN_Y     0.0
#define DEFAULT_MANDEL_PIXEL_DELTA  0.0035
#define DEFAULT_MAX_ITERATIONS      10000
#define DEFAULT_ZOOM_MULTIPLIER     2.0

typedef long double precisionFloat;

class MandelPoint{
    public:
        MandelPoint()
            :x(0.0), y(0.0){}
        MandelPoint(precisionFloat x, precisionFloat y)
            :x(x), y(y){}
        precisionFloat x, y;
};
class MandelLocation{
    public:
        MandelLocation()
            :origin(MandelPoint()), pixelDelta(0.0){}
        MandelLocation(MandelPoint origin, precisionFloat pixelDelta)
            :origin(origin), pixelDelta(pixelDelta){}
        MandelPoint origin;
        precisionFloat pixelDelta;
};

class ViewPoint{
    public:
        ViewPoint()
            :x(0), y(0){}
        ViewPoint(uint x, uint y)
            :x(x), y(y){}
        uint x, y;
};
class ViewParameters{
    public:
        ViewParameters()
            :width(0), height(0), origin(ViewPoint()){}
        ViewParameters(uint width, uint height, ViewPoint origin)
            :width(width), height(height), origin(origin){}
        uint width, height;
        ViewPoint origin;
};


class Mandelbrot : public QWidget{
    Q_OBJECT

    public:
        Mandelbrot(QWidget *parent = NULL);
        void initPointers();
        void init();
        ~Mandelbrot();
        void deinit();

        void setViewParameters(uint viewWidth, uint viewHeight);
        void setMandelLocation(MandelLocation location);
        void setMaxIterations(uint maxIterations);
        void setZoomMultiplier(precisionFloat zoomMultiplier);

        static MandelPoint transformViewPointToMandelPoint(ViewPoint point, ViewParameters viewParameters, MandelLocation mandelLocation);

        MandelLocation zoomMandelLocationIn(MandelLocation location);
        MandelLocation zoomMandelLocationOut(MandelLocation location);

        static uint calculateNumMandelbrotEscapeIterations(MandelPoint point);
        static QColor calculateIterationValueColor(uint i);
        static QColor calculateMandelPointColor(MandelPoint point);

        static void mapMandelLocationToIterationValues(MandelLocation mandelLocation, ViewParameters viewParameters, vector<vector<uint>> &iterationValues);
        void mapIterationValuesToPixmap(vector<vector<uint>> const &iterationValues);

    signals:
        void signalZoom();
        void signalResize();

    protected:
        void paintEvent(QPaintEvent *event);
        void paintPixmap(QPixmap *pixmap);

        void resizeEvent(QResizeEvent *event);

        void mousePressEvent(QMouseEvent *event);
        QMouseEvent *latestQMouseEvent;

    public slots:
        void slotZoomEvent();
        void slotResizeEvent();

    private:
        QPixmap *pixmap;
        vector<vector<uint>> iterationValues;
        bool iterationValuesAreValid;

        ViewParameters viewParameters;
        MandelLocation mandelLocation;

        static uint maxIterations;
        static precisionFloat zoomMultiplier;


};

#endif // MANDELBROT_HPP
















