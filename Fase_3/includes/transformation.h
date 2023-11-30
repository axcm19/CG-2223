#include <string>
#include <vector>
using namespace std;

class transformation
{
public:
    string type;

    float angle;
    float time;
    float timeNow;
    string align;

    float var_x;
    float var_y;
    float var_z;

    vector<float> prev_y;

    vector<vector<float>> catmullPoints;

public:
    string getType(void);
    void setType(string _type);

    float getAngle(void);
    void setAngle(float _angle);
    float getTime(void);
    void setTime(float _time);
    float getTimeNow(void);
    void setTimeNow(float _timeNow);
    string getAlign(void);
    void setAlign(string _align);

    float getVarX(void);
    void setVarX(float _var_x);
    float getVarY(void);
    void setVarY(float _var_y);
    float getVarZ(void);
    void setVarZ(float _var_z);

    vector<float> getPrevY(void);
    void setPrevY(vector<float> prev_y);

    vector<vector<float>> getCatmullPoints(void);
    void setCatmullPoints(vector<vector<float>> _catmullPoints);

    transformation(){};
    transformation(string _type, float _angle, float _time, float _timeNow, float _align,
                   float _var_x, float _var_y, float _var_z, vector<float> _prev_y,
                   vector<vector<float>> _catmullPoints)
    {
        type = _type;

        angle = _angle;
        time = _time;
        timeNow = _timeNow;
        align = _align;

        var_x = _var_x;
        var_y = _var_y;
        var_z = _var_z;

        prev_y = _prev_y;

        catmullPoints = _catmullPoints;
    }
    transformation(const transformation &trans)
    {
        type = trans.type;

        angle = trans.angle;
        time = trans.time;
        timeNow = trans.timeNow;
        align = trans.align;

        var_x = trans.var_x;
        var_y = trans.var_y;
        var_z = trans.var_z;

        prev_y = trans.prev_y;

        catmullPoints = trans.catmullPoints;
    }
};

inline void transformation::setType(string _type)
{
    type = _type;
}

inline string transformation::getType(void)
{
    return type;
}

inline void transformation::setAngle(float _angle)
{
    angle = _angle;
}

inline float transformation::getAngle(void)
{
    return angle;
}

inline void transformation::setTime(float _time)
{
    time = _time;
}

inline float transformation::getTime(void)
{
    return time;
}

inline void transformation::setTimeNow(float _timeNow)
{
    timeNow = _timeNow;
}

inline float transformation::getTimeNow(void)
{
    return timeNow;
}

inline void transformation::setAlign(string _align)
{
    align = _align;
}

inline string transformation::getAlign(void)
{
    return align;
}

inline void transformation::setVarX(float _var_x)
{
    var_x = _var_x;
}

inline float transformation::getVarX(void)
{
    return var_x;
}

inline void transformation::setVarY(float _var_y)
{
    var_y = _var_y;
}

inline float transformation::getVarY(void)
{
    return var_y;
}

inline void transformation::setVarZ(float _var_z)
{
    var_z = _var_z;
}

inline float transformation::getVarZ(void)
{
    return var_z;
}

inline void transformation::setPrevY(vector<float> _prev_y) {
    prev_y = _prev_y;
}

inline vector<float> transformation::getPrevY(void) {
    return prev_y;
}

inline void transformation::setCatmullPoints(vector<vector<float>> _catmullPoints)
{
    catmullPoints = _catmullPoints;
}

inline vector<vector<float>> transformation::getCatmullPoints(void)
{
    return catmullPoints;
}