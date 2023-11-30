#include <string>
#include <vector>
using namespace std;

class Transformation
{
public:
    string type;

    float angle;
    float time;
    float timeNow;
    bool align;

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
    bool getAlign(void);
    void setAlign(bool _align);

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

    Transformation(){};
    Transformation(string _type, float _angle, float _time, float _timeNow, bool _align,
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
    Transformation(const Transformation &trans)
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

inline void Transformation::setType(string _type)
{
    type = _type;
}

inline string Transformation::getType(void)
{
    return type;
}

inline void Transformation::setAngle(float _angle)
{
    angle = _angle;
}

inline float Transformation::getAngle(void)
{
    return angle;
}

inline void Transformation::setTime(float _time)
{
    time = _time;
}

inline float Transformation::getTime(void)
{
    return time;
}

inline void Transformation::setTimeNow(float _timeNow)
{
    timeNow = _timeNow;
}

inline float Transformation::getTimeNow(void)
{
    return timeNow;
}

inline void Transformation::setAlign(bool _align)
{
    align = _align;
}

inline bool Transformation::getAlign(void)
{
    return align;
}

inline void Transformation::setVarX(float _var_x)
{
    var_x = _var_x;
}

inline float Transformation::getVarX(void)
{
    return var_x;
}

inline void Transformation::setVarY(float _var_y)
{
    var_y = _var_y;
}

inline float Transformation::getVarY(void)
{
    return var_y;
}

inline void Transformation::setVarZ(float _var_z)
{
    var_z = _var_z;
}

inline float Transformation::getVarZ(void)
{
    return var_z;
}

inline void Transformation::setPrevY(vector<float> _prev_y) {
    prev_y = _prev_y;
}

inline vector<float> Transformation::getPrevY(void) {
    return prev_y;
}

inline void Transformation::setCatmullPoints(vector<vector<float>> _catmullPoints)
{
    catmullPoints = _catmullPoints;
}

inline vector<vector<float>> Transformation::getCatmullPoints(void)
{
    return catmullPoints;
}