
#include "mandelbrot.hpp"

#include <QApplication>
#include <time.h>


int main(int argc, char** args){
    srand(time(NULL));

    QApplication app(argc, args);

    MandelbrotWidget mandelbrot = MandelbrotWidget();
    QColor mixedColor = mandelbrot.mixColors(0.5, QColor(255, 0, 255), QColor(0, 255, 0));
    mandelbrot.raise();
    mandelbrot.show();

    return app.exec();
}
















































