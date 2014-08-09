#include "MandelbrotWidget.hpp"

#include <cassert>
#include <math.h>
#include <thread>

#include <QBrush>


MandelbrotWidget::MandelbrotWidget(QWidget *parent)
    :   QWidget(parent)
{
    initPointers();
    init();

    connect(this, SIGNAL(signalZoom()), this, SLOT(slotZoomEvent()));
    connect(this, SIGNAL(signalResize()), this, SLOT(slotResizeEvent()));
    connect(this, SIGNAL(signalChangeColor()), this, SLOT(slotChangeColorEvent()));
}
uint MandelbrotWidget::maxIterations = DEFAULT_MAX_ITERATIONS;
precisionFloat MandelbrotWidget::zoomMultiplier = DEFAULT_ZOOM_MULTIPLIER;
dwellValue MandelbrotWidget::DWELL_VALUE_IN_SET = std::numeric_limits<dwellValue>::max();
void MandelbrotWidget::initPointers(){
    image = NULL;
    dwellValues = NULL;
}
void MandelbrotWidget::init(){
    this->dwellValuesAreValid = false;
    currentColor = COLOR_101;

    setViewParameters(DEFAULT_VIEW_WIDTH, DEFAULT_VIEW_HEIGHT);
    this->resize(viewParameters.width, viewParameters.height);
    setMandelLocation(MandelLocation(MandelPoint(DEFAULT_MANDEL_ORIGIN_X, DEFAULT_MANDEL_ORIGIN_Y), DEFAULT_MANDEL_PIXEL_DELTA));
    setMaxIterations(DEFAULT_MAX_ITERATIONS);
    setZoomMultiplier(DEFAULT_ZOOM_MULTIPLIER);
}
MandelbrotWidget::~MandelbrotWidget(){
    deinit();
}
void MandelbrotWidget::deinit(){
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


void MandelbrotWidget::setViewParameters(uint viewWidth, uint viewHeight){
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
void MandelbrotWidget::setMandelLocation(MandelLocation location){this->mandelLocation = location;}
void MandelbrotWidget::setMaxIterations(uint maxIterations){this->maxIterations = maxIterations;}
void MandelbrotWidget::setZoomMultiplier(precisionFloat zoomMultiplier){this->zoomMultiplier = zoomMultiplier;}

MandelPoint MandelbrotWidget::transformViewPointToMandelPoint(ViewPoint point, ViewParameters viewParameters, MandelLocation mandelLocation){
    int xViewDiff = point.x - viewParameters.origin.x;
    int yViewDiff = viewParameters.origin.y - point.y;
    precisionFloat scaledX = xViewDiff * mandelLocation.pixelDelta;
    precisionFloat scaledY = yViewDiff * mandelLocation.pixelDelta;
    precisionFloat translatedX = scaledX + (precisionFloat)mandelLocation.origin.x;
    precisionFloat translatedY = scaledY + (precisionFloat)mandelLocation.origin.y;
    return MandelPoint(translatedX, translatedY); 
}


dwellValue MandelbrotWidget::calculateMandelPointDwellValue(MandelPoint point){
    precisionFloat x = 0.0;
    precisionFloat y = 0.0;
    int i = 0;
    while(x*x + y*y < 2*2 && i < maxIterations){
        precisionFloat xtmp = x*x - y*y + point.x;
        y = 2*x*y + point.y;
        x = xtmp;
        i++;
    }
    //Floating point modifications below!
    precisionFloat xtmp = x*x - y*y + point.x;
    y = 2*x*y + point.y;
    x = xtmp;
    //i++;
    xtmp = x*x - y*y + point.x;
    y = 2*x*y + point.y;
    x = xtmp;
    //i++;
    i+=2;   //combine both i++ into i+=2
    //^
    
    if(i>=maxIterations){
        return DWELL_VALUE_IN_SET;   
    }
    
    precisionFloat distance = sqrt(x*x+y*y);
    precisionFloat dwellValue = (precisionFloat)i - (log(log(distance)) / log(2.0));
    return dwellValue;
}

QColor MandelbrotWidget::calculateDwellValueColor(dwellValue value){
    if(value == DWELL_VALUE_IN_SET || value > maxIterations){return DEFAULT_QCOLOR_IN_SET;}
    value = fmod(value, 256.0);
    //return QColor(value, 0, value);
    switch(currentColor){
        case COLOR_001:
            return QColor(0, 0, value);
            break;
        case COLOR_010:
            return QColor(0, value, 0);
            break;
        case COLOR_011:
            return QColor(0, value, value);
            break;
        case COLOR_100:
            return QColor(value, 0, 0);
            break;
        case COLOR_101:
            return QColor(value, 0, value);
            break;
        case COLOR_110:
            return QColor(value, value, 0);
            break;
        case COLOR_111:
            return QColor(value, value, value);
            break;
        default:
            return QColor(value, 0, value);
            break;
    }
    /*
    value = fmod(value, 100.0);
    float ratio = value / 100.0;
    return mixColors(ratio, QColor(0, 0, 0), QColor(255, 0, 255));
    */
}


QColor MandelbrotWidget::mixColors(float ratio, QColor const &lower, QColor const &higher){
    return QColor(
                    lower.red() + (ratio * (float)(higher.red() - lower.red())),
                    lower.green() + (ratio * (float)(higher.green() - lower.green())),
                    lower.blue() + (ratio * (float)(higher.blue() - lower.blue()))
                  );
}


void MandelbrotWidget::mapMandelLocationSegmentToDwellValues(MandelLocation mandelLocation, ViewParameters viewParameters, dwellValue **dwellValues, uint startLine, uint endLine){
    for(int i=startLine; i<endLine; i++){
        for(int j=0; j<viewParameters.height; j++){
            MandelPoint mandelPoint = transformViewPointToMandelPoint(ViewPoint(i, j), viewParameters, mandelLocation);
            dwellValue value = calculateMandelPointDwellValue(mandelPoint);
            dwellValues[i][j] = value;
        }
    }
}

void MandelbrotWidget::mapMandelLocationToDwellValues(MandelLocation mandelLocation, ViewParameters viewParameters, dwellValue **dwellValues){
    vector<std::thread*> threads;
    for(int i=0; i<MAX_WORKER_THREADS; i++){
        uint startLine = i*(viewParameters.width / MAX_WORKER_THREADS);
        uint endLine = (i+1)*(viewParameters.width / MAX_WORKER_THREADS);
        std::thread *thread = new std::thread(mapMandelLocationSegmentToDwellValues, mandelLocation, viewParameters, dwellValues, startLine, endLine);
        threads.push_back(thread);
    }
    for(int i=0; i<threads.size(); i++){
        threads[i]->join();
        delete threads[i];
    }
}

void MandelbrotWidget::mapDwellValuesToQImage(dwellValue **dwellValues){
    for(int i=0; i<viewParameters.width; i++){
        for(int j=0; j<viewParameters.height; j++){
            image->setPixel(i, j, calculateDwellValueColor(dwellValues[i][j]).rgb());
        }
    }
}



void MandelbrotWidget::paintEvent(QPaintEvent *event){
    paintImage(this->image);
}
void MandelbrotWidget::paintImage(QImage *image){
    if(!dwellValuesAreValid){
        //threaded version
        setWindowTitle(tr("Rendering..."));
        mapMandelLocationToDwellValues(mandelLocation, viewParameters, dwellValues);
        dwellValuesAreValid = true;
        setWindowTitle(DEFAULT_WINDOW_TITLE);
    }
    setWindowTitle(tr("Drawing..."));
    mapDwellValuesToQImage(dwellValues);
    setWindowTitle(DEFAULT_WINDOW_TITLE);
    QPainter painter(this);
    painter.drawImage(0, 0, *image);
}



void MandelbrotWidget::resizeEvent(QResizeEvent *event){
    emit signalResize();
}


void MandelbrotWidget::mousePressEvent(QMouseEvent *event){
    this->latestQMouseEvent = event;
    emit signalZoom();
}

void MandelbrotWidget::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Left){
        latestColorChangeEventIsForward = false;
        emit signalChangeColor();
    }else if(event->key() == Qt::Key_Right){
        latestColorChangeEventIsForward = true;
        emit signalChangeColor();
    }
}



void MandelbrotWidget::slotZoomEvent(){
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


void MandelbrotWidget::slotResizeEvent(){
    setViewParameters(this->frameSize().width(), this->frameSize().height());
    dwellValuesAreValid = false;
    this->update();
}


void MandelbrotWidget::slotChangeColorEvent(){
    if(latestColorChangeEventIsForward){
        currentColor = (COLOR_CODE)((int)currentColor + 1);
        if(currentColor == COLOR_END){
            currentColor = (COLOR_CODE)((int)COLOR_START + 1);
        }
    }else{
        currentColor = (COLOR_CODE)((int)currentColor - 1);
        if(currentColor == COLOR_START){
            currentColor = (COLOR_CODE)((int)COLOR_END - 1);
        }
    }
    this->update();
}















