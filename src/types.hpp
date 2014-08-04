#ifndef TYPES_HPP
#define TYPES_HPP

#endif // TYPES_HPP

typedef long double precisionFloat;
typedef float dwellValue;
typedef unsigned int uint;

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
