#include "mandelbrot.hpp"

#include <QBrush>

#define PROGRAM_NAME    "MandelbrotLight"


Mandelbrot::Mandelbrot(QWidget *parent)
    :   QWidget(parent)
{
    latestQMouseEvent = NULL;

    viewWidth = viewHeight = 400;
    this->resize(viewWidth, viewHeight);
    recalculateViewParameters();

    pixmap = new QPixmap(viewWidth, viewHeight);
    pixMapIsValid = false;


    mandelOriginX = -0.75;
    mandelOriginY = 0.0;
    mandelPixelOffset = 0.0035;

    maxIterations = 10000;

    connect(this, SIGNAL(signalZoom()), this, SLOT(slotZoom()));
    connect(this, SIGNAL(signalResize()), this, SLOT(slotResize()));

}
Mandelbrot::~Mandelbrot(){
    delete pixmap;
}


int Mandelbrot::calculateNumMandelbrotEscapeIterations(long double x0, long double y0){
    long double x = 0.0;
    long double y = 0.0;
    int i = 0;
    while(x*x + y*y < 2*2 && i < maxIterations){
        long double xtmp = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = xtmp;
        i++;
    }

    return i;
}

QColor Mandelbrot::calculateIterationColor(int i){
    /*
    QColor color;
    if(i==maxIterations){
        return QColor(0, 0, 0);
    }
    #define ITERATION_MOD 7
    int rgb[ITERATION_MOD][3] = {
        {0, 0, 255}, {0, 255, 0}, {0, 255, 255}, {255, 0, 0}, {255, 0, 255}, {255, 255, 0}, {255, 255, 255}
    };
    int indicator = i % ITERATION_MOD;
    return QColor(rgb[indicator][0], rgb[indicator][1], rgb[indicator][2]);
    */


    if(i == maxIterations){return QColor(0, 0, 0);}
    int val = (i*6) % 256;
    return QColor(0, val, val);
}

QColor Mandelbrot::calculateMandelPointColor(long double a, long double b){
    return calculateIterationColor(calculateNumMandelbrotEscapeIterations(a, b));
}

QColor Mandelbrot::calculateViewPointColor(int x, int y){
    long double mandelA, mandelB;
    mandelA = (long double)(x - viewOriginX) * mandelPixelOffset + mandelOriginX;
    mandelB = (long double)(viewOriginY - y) * mandelPixelOffset + mandelOriginY;
    return calculateMandelPointColor(mandelA, mandelB);
}






void Mandelbrot::paintEvent(QPaintEvent *event){
    this->setWindowTitle(tr("Rendering..."));
    if(!pixMapIsValid){
        validatePixmap();
    }
    QPainter painter(this);
    painter.drawPixmap(0, 0, viewWidth, viewHeight, *pixmap);
    this->setWindowTitle(tr(PROGRAM_NAME));
}

void Mandelbrot::validatePixmap(){
    QPainter painter(pixmap);
    for(int i=0; i<viewWidth; i++){
        for(int j=0; j<viewHeight; j++){
            painter.setPen(calculateViewPointColor(i, j));
            painter.drawPoint(i, j);
        }
    }
    pixMapIsValid = true;
}

void Mandelbrot::resizeEvent(QResizeEvent *event){
    emit slotResize();
}

void Mandelbrot::recalculateViewParameters(){
    viewOriginX = viewWidth / 2;
    viewOriginY = viewHeight / 2;
}

void Mandelbrot::mousePressEvent(QMouseEvent *event){
    this->latestQMouseEvent = event;
    emit slotZoom();
}



void Mandelbrot::slotZoom(){
    //recalculating mandelPixelOffset
    #define ZOOM_MULTIPLIER .5
    if(latestQMouseEvent->button() == Qt::RightButton){
        this->mandelPixelOffset /= ZOOM_MULTIPLIER;
       // this->maxIterations *= ZOOM_MULTIPLIER;
    }else if(latestQMouseEvent->button() == Qt::LeftButton){
        this->mandelPixelOffset *= ZOOM_MULTIPLIER;
       // this->maxIterations /= ZOOM_MULTIPLIER;
    }

    //recalculating mandelOrigin
    int xpos = latestQMouseEvent->x();
    int ypos = latestQMouseEvent->y();
    int dxViewOrigin = viewOriginX - xpos;
    int dyViewOrigin = viewOriginY - ypos;
    this->mandelOriginX = (long double)(xpos - viewOriginX) * mandelPixelOffset + mandelOriginX;
    //this->mandelOriginX += (long double)mandelPixelOffset * dxViewOrigin;
    this->mandelOriginY = (long double)(viewOriginY - ypos) * mandelPixelOffset + mandelOriginY;
    //this->mandelOriginY += (long double)mandelPixelOffset * dyViewOrigin;

    pixMapIsValid = false;
    this->update();
}

void Mandelbrot::slotResize(){
    this->viewWidth = this->frameSize().width();
    this->viewHeight = this->frameSize().height();
    recalculateViewParameters();
    this->pixMapIsValid = false;
    delete pixmap;
    pixmap = new QPixmap(viewWidth, viewHeight);
    this->update();
}


















