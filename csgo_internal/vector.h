#include <iostream>
#include <math.h>
#include <stdlib.h>

#pragma once

#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

#define FastSqrt(x)			(sqrt)(x)

#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.

#define M_PHI		1.61803398874989484820 // golden ratio

// NJS: Inlined to prevent floats from being autopromoted to doubles, as with the old system.
#ifndef RAD2DEG
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif

#define DECL_ALIGN(x) __declspec(align(x))
#define ALIGN16 DECL_ALIGN(16)
#define ALIGN16_POST DECL_ALIGN(16)
#define VALVE_RAND_MAX 0x7fff
#define VectorExpand(v) (v).x, (v).y, (v).z

struct vec2 { float x, y; };
class vec3
{
public:
    vec3(void)
    {
        Invalidate();
    }
    vec3(float X, float Y, float Z)
    {
        x = X;
        y = Y;
        z = Z;
    }
    vec3(const float* clr)
    {
        x = clr[0];
        y = clr[1];
        z = clr[2];
    }

    void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
    {
        x = ix; y = iy; z = iz;
    }
    bool IsValid() const
    {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }
    void Invalidate()
    {
        x = y = z = std::numeric_limits<float>::infinity();
    }

    float& operator[](int i)
    {
        return ((float*)this)[i];
    }
    float operator[](int i) const
    {
        return ((float*)this)[i];
    }

    void Zero()
    {
        x = y = z = 0.0f;
    }

    bool operator==(const vec3& src) const
    {
        return (src.x == x) && (src.y == y) && (src.z == z);
    }
    bool operator!=(const vec3& src) const
    {
        return (src.x != x) || (src.y != y) || (src.z != z);
    }

    vec3& operator+=(const vec3& v)
    {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    vec3& operator-=(const vec3& v)
    {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    vec3& operator*=(float fl)
    {
        x *= fl;
        y *= fl;
        z *= fl;
        return *this;
    }
    vec3& operator*=(const vec3& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
    vec3& operator/=(const vec3& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }
    vec3& operator+=(float fl)
    {
        x += fl;
        y += fl;
        z += fl;
        return *this;
    }
    vec3& operator/=(float fl)
    {
        x /= fl;
        y /= fl;
        z /= fl;
        return *this;
    }
    vec3& operator-=(float fl)
    {
        x -= fl;
        y -= fl;
        z -= fl;
        return *this;
    }

    void NormalizeInPlace()
    {
        *this = Normalized();
    }
    vec3 Normalized() const
    {
        vec3 res = *this;
        float l = res.Length();
        if (l != 0.0f) {
            res /= l;
        }
        else {
            res.x = res.y = res.z = 0.0f;
        }
        return res;
    }

    void Normalize() {
        while (y < -180) { y += 360; }
        while (y > 180) { y -= 360; }
        if (x > 89) { x = 89; }
        if (x < -89) { x = -89; }
    }

    float DistTo(const vec3& vOther) const
    {
        vec3 delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.Length();
    }
    float DistToSqr(const vec3& vOther) const
    {
        vec3 delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.LengthSqr();
    }
    float Dot(const vec3& vOther) const
    {
        return (x * vOther.x + y * vOther.y + z * vOther.z);
    }
    float Length() const
    {
        return sqrt(x * x + y * y + z * z);
    }
    float LengthSqr(void) const
    {
        return (x * x + y * y + z * z);
    }
    float Length2D() const
    {
        return sqrt(x * x + y * y);
    }

    vec3& operator=(const vec3& vOther)
    {
        x = vOther.x; y = vOther.y; z = vOther.z;
        return *this;
    }

    vec3 operator-(void) const
    {
        return vec3(-x, -y, -z);
    }
    vec3 operator+(const vec3& v) const
    {
        return vec3(x + v.x, y + v.y, z + v.z);
    }
    vec3 operator-(const vec3& v) const
    {
        return vec3(x - v.x, y - v.y, z - v.z);
    }
    vec3 operator*(float fl) const
    {
        return vec3(x * fl, y * fl, z * fl);
    }
    vec3 operator*(const vec3& v) const
    {
        return vec3(x * v.x, y * v.y, z * v.z);
    }
    vec3 operator/(float fl) const
    {
        return vec3(x / fl, y / fl, z / fl);
    }
    vec3 operator/(const vec3& v) const
    {
        return vec3(x / v.x, y / v.y, z / v.z);
    }

    float x, y, z;
};

inline vec3 operator*(float lhs, const vec3& rhs)
{
    return rhs * lhs;
}
inline vec3 operator/(float lhs, const vec3& rhs)
{
    return rhs / lhs;
}
struct vec4 { float x, y, z, w; };

class ALIGN16 VectorAligned : public vec3
{
public:
    inline VectorAligned(void) {};

    inline VectorAligned(vec3 X, vec3 Y, vec3 Z)
    {
        //Init(X, Y, Z);
    }

    explicit VectorAligned(const vec3& vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
    }

    VectorAligned& operator=(const vec3& vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
        return *this;
    }

    float w;	// this space is used anyway
} ALIGN16_POST;

class QAngleByValue;
class QAngle
{
public:
    // Members
    float x, y, z;

    // Construction/destruction
    QAngle(void);
    QAngle(float X, float Y, float Z);
    //      QAngle(RadianEuler const &angles);      // evil auto type promotion!!!

    // Allow pass-by-value
    operator QAngleByValue& () { return *((QAngleByValue*)(this)); }
    operator const QAngleByValue& () const { return *((const QAngleByValue*)(this)); }

    // Initialization
    void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f);
    void Random(float minVal, float maxVal);

    // Got any nasty NAN's?
    bool IsValid() const;
    void Invalidate();

    // array access...
    float operator[](int i) const;
    float& operator[](int i);

    // Base address...
    float* Base();
    float const* Base() const;

    // equality
    bool operator==(const QAngle& v) const;
    bool operator!=(const QAngle& v) const;

    // arithmetic operations
    QAngle& operator+=(const QAngle& v);
    QAngle& operator-=(const QAngle& v);
    QAngle& operator*=(float s);
    QAngle& operator/=(float s);

    // Get the vector's magnitude.
    float   Length() const;
    float   LengthSqr() const;

    // negate the QAngle components
    //void  Negate();

    // No assignment operators either...
    QAngle& operator=(const QAngle& src);

#ifndef VECTOR_NO_SLOW_OPERATIONS
    // copy constructors

    // arithmetic operations
    QAngle  operator-(void) const;

    QAngle  operator+(const QAngle& v) const;
    QAngle  operator-(const QAngle& v) const;
    QAngle  operator*(float fl) const;
    QAngle  operator/(float fl) const;
#else

private:
    // No copy constructors allowed if we're in optimal mode
    QAngle(const QAngle& vOther);

#endif
};

//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------
inline QAngle::QAngle(void)
{
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
    // Initialize to NAN to catch errors
    x = y = z = VEC_T_NAN;
#endif
#endif
}

inline QAngle::QAngle(float X, float Y, float Z)
{
    x = X; y = Y; z = Z;
    CHECK_VALID(*this);
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------
inline void QAngle::Init(float ix, float iy, float iz)
{
    x = ix; y = iy; z = iz;
    CHECK_VALID(*this);
}

inline void QAngle::Random(float minVal, float maxVal)
{
    x = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    y = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    z = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    CHECK_VALID(*this);
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------
inline QAngle& QAngle::operator=(const QAngle& vOther)
{
    CHECK_VALID(vOther);
    x = vOther.x; y = vOther.y; z = vOther.z;
    return *this;
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------
inline bool QAngle::operator==(const QAngle& src) const
{
    CHECK_VALID(src);
    CHECK_VALID(*this);
    return (src.x == x) && (src.y == y) && (src.z == z);
}

inline bool QAngle::operator!=(const QAngle& src) const
{
    CHECK_VALID(src);
    CHECK_VALID(*this);
    return (src.x != x) || (src.y != y) || (src.z != z);
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------
inline QAngle& QAngle::operator+=(const QAngle& v)
{
    CHECK_VALID(*this);
    CHECK_VALID(v);
    x += v.x; y += v.y; z += v.z;
    return *this;
}

inline QAngle& QAngle::operator-=(const QAngle& v)
{
    CHECK_VALID(*this);
    CHECK_VALID(v);
    x -= v.x; y -= v.y; z -= v.z;
    return *this;
}

inline QAngle& QAngle::operator*=(float fl)
{
    x *= fl;
    y *= fl;
    z *= fl;
    CHECK_VALID(*this);
    return *this;
}

inline QAngle& QAngle::operator/=(float fl)
{
    Assert(fl != 0.0f);
    float oofl = 1.0f / fl;
    x *= oofl;
    y *= oofl;
    z *= oofl;
    CHECK_VALID(*this);
    return *this;
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
inline float* QAngle::Base()
{
    return (float*)this;
}

inline float const* QAngle::Base() const
{
    return (float const*)this;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline float& QAngle::operator[](int i)
{
    Assert((i >= 0) && (i < 3));
    return ((float*)this)[i];
}

inline float QAngle::operator[](int i) const
{
    Assert((i >= 0) && (i < 3));
    return ((float*)this)[i];
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
inline float QAngle::Length() const
{
    CHECK_VALID(*this);
    return (float)FastSqrt(LengthSqr());
}


inline float QAngle::LengthSqr() const
{
    CHECK_VALID(*this);
    return x * x + y * y + z * z;
}


//-----------------------------------------------------------------------------
// arithmetic operations (SLOW!!)
//-----------------------------------------------------------------------------
#ifndef VECTOR_NO_SLOW_OPERATIONS

inline QAngle QAngle::operator-(void) const
{
    return QAngle(-x, -y, -z);
}

inline QAngle QAngle::operator+(const QAngle& v) const
{
    QAngle res;
    res.x = x + v.x;
    res.y = y + v.y;
    res.z = z + v.z;
    return res;
}

inline QAngle QAngle::operator-(const QAngle& v) const
{
    QAngle res;
    res.x = x - v.x;
    res.y = y - v.y;
    res.z = z - v.z;
    return res;
}

inline QAngle QAngle::operator*(float fl) const
{
    QAngle res;
    res.x = x * fl;
    res.y = y * fl;
    res.z = z * fl;
    return res;
}

inline QAngle QAngle::operator/(float fl) const
{
    QAngle res;
    res.x = x / fl;
    res.y = y / fl;
    res.z = z / fl;
    return res;
}

inline QAngle operator*(float fl, const QAngle& v)
{
    return v * fl;
}

#endif // VECTOR_NO_SLOW_OPERATIONS


//QANGLE SUBTRAC
inline void QAngleSubtract(const QAngle& a, const QAngle& b, QAngle& c)
{
    CHECK_VALID(a);
    CHECK_VALID(b);
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
}

//QANGLEADD
inline void QAngleAdd(const QAngle& a, const QAngle& b, QAngle& c)
{
    CHECK_VALID(a);
    CHECK_VALID(b);
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
}

struct matrix3x4_t
{
    matrix3x4_t() {}
    matrix3x4_t(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23)
    {
        m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
        m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
        m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
    }

    float* operator[](int i) { Assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
    const float* operator[](int i) const { Assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
    float* Base() { return &m_flMatVal[0][0]; }
    const float* Base() const { return &m_flMatVal[0][0]; }

    float m_flMatVal[3][4];
};