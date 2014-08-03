#include "mandelbrot.hpp"

#include <QBrush>


Mandelbrot::Mandelbrot(QWidget *parent)
    :   QWidget(parent)
{
    initPointers();
    init();

    connect(this, SIGNAL(signalZoom()), this, SLOT(slotZoomEvent()));
    //connect(this, SIGNAL(signalResize()), this, SLOT(slotResizeEvent()));

}
uint Mandelbrot::maxIterations = DEFAULT_MAX_ITERATIONS;
precisionFloat Mandelbrot::zoomMultiplier = DEFAULT_ZOOM_MULTIPLIER;
void Mandelbrot::initPointers(){
    image = NULL;
}

void Mandelbrot::init(){
    this->iterationValuesAreValid = false;

    setViewParameters(DEFAULT_VIEW_WIDTH, DEFAULT_VIEW_HEIGHT);
    this->resize(viewParameters.width, viewParameters.height);
    setMandelLocation(MandelLocation(MandelPoint(DEFAULT_MANDEL_ORIGIN_X, DEFAULT_MANDEL_ORIGIN_Y), DEFAULT_MANDEL_PIXEL_DELTA));
    setMaxIterations(DEFAULT_MAX_ITERATIONS);
    setZoomMultiplier(DEFAULT_ZOOM_MULTIPLIER);
}

Mandelbrot::~Mandelbrot(){
    deinit();
}
void Mandelbrot::deinit(){
    delete image;
    initPointers();
}


void Mandelbrot::setViewParameters(uint viewWidth, uint viewHeight){
    viewParameters.width = viewWidth;
    viewParameters.height = viewHeight;
    viewParameters.origin.x = viewWidth / 2;
    viewParameters.origin.y = viewHeight / 2;

    delete image;
    image = new QImage(viewWidth, viewHeight, QImage::Format_RGB888);

    iterationValues.resize(viewWidth);
    for(int i=0; i<viewWidth; i++){
        iterationValues[i].resize(viewHeight);
    }
    iterationValuesAreValid = false;
}
void Mandelbrot::setMandelLocation(MandelLocation location){
    this->mandelLocation = location;
}
void Mandelbrot::setMaxIterations(uint maxIterations){
    this->maxIterations = maxIterations;
}
void Mandelbrot::setZoomMultiplier(precisionFloat zoomMultiplier){
    this->zoomMultiplier = zoomMultiplier;
}


MandelPoint Mandelbrot::transformViewPointToMandelPoint(ViewPoint point, ViewParameters viewParameters, MandelLocation mandelLocation){

    int xViewDiff = point.x - viewParameters.origin.x;
    int yViewDiff = viewParameters.origin.y - point.y;
    precisionFloat scaledX = xViewDiff * mandelLocation.pixelDelta;
    precisionFloat scaledY = yViewDiff * mandelLocation.pixelDelta;
    precisionFloat translatedX = scaledX + (precisionFloat)mandelLocation.origin.x;
    precisionFloat translatedY = scaledY + (precisionFloat)mandelLocation.origin.y;
    return MandelPoint(translatedX, translatedY);

/*
    MandelPoint mandelPoint;
    mandelPoint.x = (precisionFloat)(point.x - viewParameters.origin.x) * mandelLocation.pixelDelta + (precisionFloat)mandelLocation.origin.x;
    mandelPoint.y = (precisionFloat)(viewParameters.origin.y - point.y) * mandelLocation.pixelDelta + (precisionFloat)mandelLocation.origin.y;
    return mandelPoint;
   */
}


uint Mandelbrot::calculateNumMandelbrotEscapeIterations(MandelPoint point){
    precisionFloat x = 0.0;
    precisionFloat y = 0.0;
    int i = 0;
    while(x*x + y*y < 2*2 && i < maxIterations){
        precisionFloat xtmp = x*x - y*y + point.x;
        y = 2*x*y + point.y;
        x = xtmp;
        i++;
    }
    return i;
}
QColor Mandelbrot::calculateIterationValueColor(uint i){
    if(i == 1){
        return QColor(255, 0, 0);
    }
    if(i == maxIterations){return QColor(0, 0, 0);}
    int val = (i*8) % 256;
    if(val > 200){
        return QColor(val - 150, 0, val - 150);
    }
    return QColor(0, val, 0);
}
QColor Mandelbrot::calculateMandelPointColor(MandelPoint point){
    return calculateIterationValueColor(calculateNumMandelbrotEscapeIterations(point));
}


void Mandelbrot::mapMandelLocationToIterationValues(MandelLocation mandelLocation, ViewParameters viewParameters, vector<vector<uint>> &iterationValues){
    for(int i=0; i<viewParameters.width; i++){
        for(int j=0; j<viewParameters.height; j++){
            MandelPoint mandelPoint = transformViewPointToMandelPoint(ViewPoint(i, j), viewParameters, mandelLocation);
            uint iterations = calculateNumMandelbrotEscapeIterations(mandelPoint);
            iterationValues[i][j] = iterations;
        }
    }
    int asdkfjdsf = 0;
}
void Mandelbrot::mapIterationValuesToQImage(vector<vector<uint>> const &iterationValues){
    for(int i=0; i<viewParameters.width; i++){
        for(int j=0; j<viewParameters.height; j++){
            image->setPixel(i, j, calculateIterationValueColor(iterationValues[i][j]).rgb());
        }
    }
}



void Mandelbrot::paintEvent(QPaintEvent *event){
    paintImage(this->image);
}
void Mandelbrot::paintImage(QImage *image){
    if(!iterationValuesAreValid){
        mapMandelLocationToIterationValues(mandelLocation, viewParameters, iterationValues);
        iterationValuesAreValid = true;
    }
    mapIterationValuesToQImage(iterationValues);

    QPainter painter(this);
    //painter.drawPixmap(0, 0, viewParameters.width, viewParameters.height, *pixmap);

    //painter.drawImage(0, 0, viewParameters.width, viewParameters.height, *image);
    painter.drawImage(0, 0, *image);
    //painter.drawImage(viewParameters.width, viewParameters.height, *image);
}



void Mandelbrot::resizeEvent(QResizeEvent *event){
    emit slotResizeEvent();
}


void Mandelbrot::mousePressEvent(QMouseEvent *event){
    this->latestQMouseEvent = event;
    emit slotZoomEvent();
}



void Mandelbrot::slotZoomEvent(){
    //recalculating mandelPixelDelta
    if(latestQMouseEvent->button() == Qt::RightButton){
        mandelLocation.pixelDelta *= zoomMultiplier;
    }else if(latestQMouseEvent->button() == Qt::LeftButton){
        mandelLocation.pixelDelta /= zoomMultiplier;
    }

    //recalculating mandelOrigin
    ViewPoint point(latestQMouseEvent->x(), latestQMouseEvent->y());
    MandelLocation newMandelLocation(transformViewPointToMandelPoint(point, viewParameters, mandelLocation), mandelLocation.pixelDelta);
    setMandelLocation(newMandelLocation);

    iterationValuesAreValid = false;
    this->update();
}


void Mandelbrot::slotResizeEvent(){
    setViewParameters(this->frameSize().width(), this->frameSize().height());
    this->iterationValuesAreValid = false;
    this->update();
}


















