#ifndef MANDELBROT_HPP
#define MANDELBROT_HPP

#include <vector>

#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QPainter>
#include <QResizeEvent>

/*
http://stackoverflow.com/questions/5960074/qimage-in-a-qgraphics-scene
*/


class Mandelbrot : public QWidget{
    Q_OBJECT

    public:
        Mandelbrot(QWidget *parent = NULL);
        ~Mandelbrot();

        int calculateNumMandelbrotEscapeIterations(long double x0, long double y0);
        QColor calculateIterationColor(int i);
        QColor calculateMandelPointColor(long double a, long double b);
        QColor calculateViewPointColor(int x, int y);

    signals:
        void signalZoom();
        void signalResize();

    protected:
        void paintEvent(QPaintEvent *event);
        /**/ void validatePixmap();

        void resizeEvent(QResizeEvent *event);
        /**/ void recalculateViewParameters();

        void mousePressEvent(QMouseEvent *event);
        QMouseEvent *latestQMouseEvent;

    public slots:
        void slotZoom();
        void slotResize();





    private:
        QPixmap *pixmap;
        bool pixMapIsValid;


        int viewWidth, viewHeight;

        int viewOriginX, viewOriginY;

        long double mandelOriginX, mandelOriginY;
        long double mandelPixelOffset;

        int maxIterations;

};

#endif // MANDELBROT_HPP
















