
#include "MandelbrotWidget.hpp"

#include <QApplication>
#include <time.h>


int main(int argc, char** args){
    srand(time(NULL));

    QApplication app(argc, args);

    MandelbrotWidget mandelbrot = MandelbrotWidget();
    mandelbrot.raise();
    mandelbrot.show();

    return app.exec();
}
















































