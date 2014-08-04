#include "mandelbrot.hpp"

#include <cassert>
#include <math.h>
#include <thread>

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
    dwellValues = NULL;
}

void Mandelbrot::init(){
    this->dwellValuesAreValid = false;

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
    if(dwellValues != NULL){
        for(int i=0; i<viewParameters.width; i++){
            delete dwellValues[i];
        }
    }
    delete dwellValues;
    dwellValues = NULL;
    
    delete image;
    image = NULL;
    
    initPointers();
}


void Mandelbrot::setViewParameters(uint viewWidth, uint viewHeight){
    deinit();
    viewParameters.width = viewWidth;
    viewParameters.height = viewHeight;
    viewParameters.origin.x = viewWidth / 2;
    viewParameters.origin.y = viewHeight / 2;

    image = new QImage(viewWidth, viewHeight, QImage::Format_RGB888);

    dwellValues = new dwellValue*[viewWidth];
    for(int i=0; i<viewWidth; i++){
        dwellValues[i] = new dwellValue[viewHeight];
    }
    dwellValuesAreValid = false;
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


uint Mandelbrot::calculateMandelPointIterationCount(MandelPoint point){
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
dwellValue Mandelbrot::calculateMandelPointDwellValue(MandelPoint point){
    precisionFloat x = 0.0;
    precisionFloat y = 0.0;
    int i = 0;
    while(x*x + y*y < 2*2 && i < maxIterations){
        precisionFloat xtmp = x*x - y*y + point.x;
        y = 2*x*y + point.y;
        x = xtmp;
        i++;
    }
    //return i;
    //Floating point modifications below!
    
    //v removes wrinkles
    //
    precisionFloat xtmp = x*x - y*y + point.x;
    y = 2*x*y + point.y;
    x = xtmp;
    //i++;
    xtmp = x*x - y*y + point.x;
    y = 2*x*y + point.y;
    x = xtmp;
    //i++;
    i+=2;   //combine i++, i++
    //
    
    if(i>=maxIterations){
        return DWELL_VALUE_IN_SET;   
    }
    precisionFloat distance = sqrt(x*x+y*y);
    precisionFloat dwellValue = (precisionFloat)i - (log(log(distance)) / log(2.0));
    return dwellValue;
}

QColor Mandelbrot::calculateDwellValueColor(dwellValue value){
    /*
    if(i == maxIterations){return DEFAULT_QCOLOR_IN_SET;}
    uint val = (i*8) % 256;
    return QColor(val, 0, val);
    */
    
    QColor color;
    if(value==DWELL_VALUE_IN_SET){
        return QColor(0, 0, 0);
    }
    /*
    #define ITERATION_MOD 7
    int rgb[ITERATION_MOD][3] = {
        {0, 0, 255}, {0, 255, 0}, {0, 255, 255}, {255, 0, 0}, {255, 0, 255}, {255, 255, 0}, {255, 255, 255}
    };
    */
    #define ITERATION_MOD 2
    int rgb[ITERATION_MOD][3] = {
        {255, 0, 255}, {0, 0, 0}
    };
    
    int indicator = (int)value % ITERATION_MOD;
    double jnk;
    float ratio = modf(value, &jnk);
    return mixColors(ratio, 
                     QColor(rgb[indicator][0], rgb[indicator][1], rgb[indicator][2]),
                     QColor(rgb[(indicator+1)%ITERATION_MOD][0], rgb[(indicator+1)%ITERATION_MOD][1], rgb[(indicator+1)%ITERATION_MOD][2])
            );
}
QColor Mandelbrot::calculateIterationCountColor(uint value){
    if(value == maxIterations){return DEFAULT_QCOLOR_IN_SET;}
    value = (value*8) % 256;
    return QColor(value, 0, value);
}


QColor Mandelbrot::mixColors(float ratio, QColor const &lower, QColor const &higher){
    if(!(ratio <= 1.0 && ratio >= 0.0)){
     //int asdfad = 21;   
    }
   // assert(ratio <= 1.0 && ratio >= 0.0);
    
    //differentials of each color
    int dr = higher.red() - lower.red();
    int dg = higher.green() - lower.green();
    int db = higher.blue() - lower.blue();
    //fractional differentials of each color (how far towards higher is lower based on ratio?)
    float fdr = ratio * (float)dr;
    float fdg = ratio * (float)dg;
    float fdb = ratio * (float)db;
    //fractional differentials added to lower color
    return QColor(lower.red() + fdr, lower.green() + fdg, lower.blue() + fdb);
    
}


void Mandelbrot::mapMandelLocationSegmentToDwellValues(MandelLocation mandelLocation, ViewParameters viewParameters, dwellValue **dwellValues, uint startLine, uint endLine){
    for(int i=startLine; i<endLine; i++){
        for(int j=0; j<viewParameters.height; j++){
            MandelPoint mandelPoint = transformViewPointToMandelPoint(ViewPoint(i, j), viewParameters, mandelLocation);
            dwellValue value = calculateMandelPointDwellValue(mandelPoint);
            //dwellValue value = calculateMandelPointIterationCount(mandelPoint);
            dwellValues[i][j] = value;
        }
    }
}

void Mandelbrot::mapMandelLocationToDwellValues(MandelLocation mandelLocation, ViewParameters viewParameters, dwellValue **dwellValues){
    vector<std::thread*> threads;
    for(int i=0; i<DEFAULT_MAX_NUM_WORKERS_THREADS; i++){
        uint startLine = i*(viewParameters.width / DEFAULT_MAX_NUM_WORKERS_THREADS);
        uint endLine = (i+1)*(viewParameters.width / DEFAULT_MAX_NUM_WORKERS_THREADS);
        std::thread *thread = new std::thread(mapMandelLocationSegmentToDwellValues, mandelLocation, viewParameters, dwellValues, startLine, endLine);
        threads.push_back(thread);
    }
    for(int i=0; i<threads.size(); i++){
        threads[i]->join();
        delete threads[i];
    }
}

void Mandelbrot::mapDwellValuesToQImage(dwellValue **dwellValues){
    for(int i=0; i<viewParameters.width; i++){
        for(int j=0; j<viewParameters.height; j++){
            image->setPixel(i, j, calculateDwellValueColor(dwellValues[i][j]).rgb());
        }
    }
}



void Mandelbrot::paintEvent(QPaintEvent *event){
    paintImage(this->image);
}
void Mandelbrot::paintImage(QImage *image){
    if(!dwellValuesAreValid){
        //threaded version
        mapMandelLocationToDwellValues(mandelLocation, viewParameters, dwellValues);
        dwellValuesAreValid = true;
    }
    mapDwellValuesToQImage(dwellValues);

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
    ViewPoint point(latestQMouseEvent->x(), latestQMouseEvent->y());

    //ZOOM AND CENTER = CALCULATE pixelDelta FIRST THEN CENTER
    //ZOOM FIXED = CALCULATE CENTER FIRST THEN pixelDelta
    //left click = ZOOM FIXED
    if(latestQMouseEvent->button() == Qt::LeftButton){
        //recalculating mandelPixelDelta
        mandelLocation.pixelDelta /= zoomMultiplier;
        //recalculating mandelOrigin
        MandelLocation newMandelLocation(transformViewPointToMandelPoint(point, viewParameters, mandelLocation), mandelLocation.pixelDelta);
        setMandelLocation(newMandelLocation);
    //right click = ZOOM AND CENTER
    }else if(latestQMouseEvent->button() == Qt::RightButton){
        //recalculating mandelOrigin
        MandelLocation newMandelLocation(transformViewPointToMandelPoint(point, viewParameters, mandelLocation), mandelLocation.pixelDelta);
        setMandelLocation(newMandelLocation);
        //recalculating mandelPixelDelta
        mandelLocation.pixelDelta *= zoomMultiplier;
    }


    dwellValuesAreValid = false;
    this->update();
}


void Mandelbrot::slotResizeEvent(){
    setViewParameters(this->frameSize().width(), this->frameSize().height());
    this->update();
}


















