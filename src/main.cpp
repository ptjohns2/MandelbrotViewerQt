
#include "mandelbrot.hpp"

#include <QApplication>
#include <time.h>


int main(int argc, char** args){
    srand(time(NULL));

    QApplication app(argc, args);

    Mandelbrot mandelbrot = Mandelbrot();
    mandelbrot.raise();
    mandelbrot.show();

    return app.exec();
}
















































