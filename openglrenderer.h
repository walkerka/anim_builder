#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H
#include <string>
#include <vector>
#include <map>
#include <math.h>

#define MAX_BLUR_RADIUS 2000
class BlurProgram;


class Vector2
{
public:
    float x;
    float y;
public:
    Vector2():x(0), y(0)
    {
    }

    Vector2(float x, float y):x(x), y(y)
    {
    }

    bool operator == (const Vector2& v) const
    {
        return x == v.x && y == v.y;
    }

    bool operator != (const Vector2& v) const
    {
        return x != v.x || y != v.y;
    }

    bool operator < (const Vector2& v) const
    {
        return x < v.x && y < v.y;
    }

    bool operator > (const Vector2& v) const
    {
        return x > v.x && y > v.y;
    }

    bool operator <= (const Vector2& v) const
    {
        return x <= v.x && y <= v.y;
    }

    bool operator >= (const Vector2& v) const
    {
        return x >= v.x && y >= v.y;
    }

    Vector2 operator * (float s) const
    {
        return Vector2(x * s, y * s);
    }

    Vector2 operator / (float s) const
    {
        return Vector2(x / s, y / s);
    }

    Vector2 operator - () const
    {
        return Vector2(-x, -y);
    }

    Vector2 operator + (const Vector2& v) const
    {
        return Vector2(x + v.x, y + v.y);
    }

    Vector2 operator - (const Vector2& v) const
    {
        return Vector2(x - v.x, y - v.y);
    }

    Vector2 operator * (const Vector2& v) const
    {
        return Vector2(x * v.x, y * v.y);
    }

    Vector2 operator / (const Vector2& v) const
    {
        return Vector2(x / v.x, y / v.y);
    }

    Vector2& operator *= (float s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    Vector2& operator /= (float s)
    {
        x /= s;
        y /= s;
        return *this;
    }

    Vector2& operator += (const Vector2& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2& operator -= (const Vector2& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    float Length() const
    {
        return sqrtf(x * x + y * y);
    }

    float LengthSq() const
    {
        return x * x + y * y;
    }

    float DistanceTo(const Vector2& v) const
    {
        float dx = x - v.x;
        float dy = y - v.y;
        return sqrtf(dx * dx + dy * dy);
    }

    float DistanceToSq(const Vector2& v) const
    {
        float dx = x - v.x;
        float dy = y - v.y;
        return dx * dx + dy * dy;
    }

    float Normalise()
    {
        float length = sqrtf(x * x + y * y);
        if (length > 0)
        {
            float invLength = 1.0f / length;
            x *= invLength;
            y *= invLength;
        }
        return length;
    }

    Vector2 GetNormalized() const
    {
        Vector2 n(x, y);
        n.Normalise();
        return n;
    }

    float Dot(const Vector2& v) const
    {
        return x * v.x + y * v.y;
    }

    Vector2 GetPerpendicular() const
    {
        return Vector2(-y, x);
    }

    bool IsPerpendicular(const Vector2& v, float epsilon = 0.0001f) const
    {
        return fabs(GetNormalized().Dot(v.GetNormalized())) < epsilon;
    }

    bool IsParellel(const Vector2& v, float epsilon = 0.0001f) const
    {
        float d = GetNormalized().Dot(v.GetNormalized());
        return 1.0f - fabs(d) < epsilon;
    }

    bool IsEqual(const Vector2& v, float epsilon = 0.0001f) const
    {
        return fabs(v.x - x) < epsilon && fabs(v.y - y) < epsilon;
    }

    bool IsZero(float epsilon = 0.0001f) const
    {
        return fabs(x) < epsilon && fabs(y) < epsilon;
    }

    Vector2 Lerp(const Vector2& v, float t) const
    {
        float invT = 1.0f - t;
        return Vector2(x * invT + v.x * t, y * invT + v.y * t);
    }

    Vector2 Lerp(const Vector2& v, Vector2 t) const
    {
        return Vector2(x * (1.0f - t.x) + v.x * t.x, y * (1.0f - t.y) + v.y * t.y);
    }

    void Set(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    void Truncate(float x0, float y0, float x1, float y1)
    {
        if (x < x0)
        {
            x = x0;
        }
        else if (x > x1)
        {
            x = x1;
        }

        if (y < y0)
        {
            y = y0;
        }
        else if (y > y1)
        {
            y = y1;
        }
    }

    void Truncate(const Vector2& low, const Vector2& high)
    {
        Truncate(low, high);
    }
};

class Vector3
{
public:
    float x;
    float y;
    float z;
public:
    Vector3():x(0), y(0), z(0)
    {
    }

    Vector3(float x, float y, float z):x(x), y(y), z(z)
    {
    }

    bool operator == (const Vector3& v) const
    {
        return x == v.x && y == v.y && z == v.z;
    }

    bool operator != (const Vector3& v) const
    {
        return x != v.x || y != v.y || z != v.z;
    }

    bool operator < (const Vector3& v) const
    {
        return x < v.x && y < v.y && z < v.z;
    }

    bool operator > (const Vector3& v) const
    {
        return x > v.x && y > v.y && z > v.z;
    }

    bool operator <= (const Vector3& v) const
    {
        return x <= v.x && y <= v.y && z <= v.z;
    }

    bool operator >= (const Vector3& v) const
    {
        return x >= v.x && y >= v.y && z >= v.z;
    }

    Vector3 operator * (float s) const
    {
        return Vector3(x * s, y * s, z * s);
    }

    Vector3 operator / (float s) const
    {
        return Vector3(x / s, y / s, z / s);
    }

    Vector3 operator - () const
    {
        return Vector3(-x, -y, -z);
    }

    Vector3 operator + (const Vector3& v) const
    {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator - (const Vector3& v) const
    {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator * (const Vector3& v) const
    {
        return Vector3(x * v.x, y * v.y, z * v.z);
    }

    Vector3 operator / (const Vector3& v) const
    {
        return Vector3(x / v.x, y / v.y, z / v.z);
    }

    Vector3& operator *= (float s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Vector3& operator /= (float s)
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    Vector3& operator += (const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3& operator -= (const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    float Length() const
    {
        return sqrtf(x * x + y * y + z * z);
    }

    float LengthSq() const
    {
        return x * x + y * y + z * z;
    }

    float DistanceTo(const Vector3& v) const
    {
        float dx = x - v.x;
        float dy = y - v.y;
        float dz = z - v.z;
        return sqrtf(dx * dx + dy * dy + dz * dz);
    }

    float DistanceToSq(const Vector3& v) const
    {
        float dx = x - v.x;
        float dy = y - v.y;
        float dz = z - v.z;
        return dx * dx + dy * dy + dz * dz;
    }

    float Normalise()
    {
        float length = sqrtf(x * x + y * y + z * z);
        if (length > 0)
        {
            float invLength = 1.0f / length;
            x *= invLength;
            y *= invLength;
            z *= invLength;
        }
        return length;
    }

    Vector3 GetNormalized() const
    {
        Vector3 n(x, y, z);
        n.Normalise();
        return n;
    }

    float Dot(const Vector3& v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3 Cross(const Vector3& v) const
    {
        return Vector3(
              y * v.z - z * v.y
            , z * v.x - x * v.z
            , x * v.y - y * v.x);
    }


    bool IsPerpendicular(const Vector3& v, float epsilon = 0.0001f) const
    {
        return fabs(GetNormalized().Dot(v.GetNormalized())) < epsilon;
    }

    bool IsParellel(const Vector3& v, float epsilon = 0.0001f) const
    {
        float d = GetNormalized().Dot(v.GetNormalized());
        return 1.0f - fabs(d) < epsilon;
    }

    bool IsEqual(const Vector3& v, float epsilon = 0.0001f) const
    {
        return fabs(v.x - x) < epsilon && fabs(v.y - y) < epsilon && fabs(v.z - z) < epsilon;
    }

    bool IsZero(float epsilon = 0.0001f) const
    {
        return fabs(x) < epsilon && fabs(y) < epsilon && fabs(z) < epsilon;
    }

    Vector3 Lerp(const Vector3& v, float t) const
    {
        float invT = 1.0f - t;
        return Vector3(x * invT + v.x * t, y * invT + v.y * t, z * invT + v.z * t);
    }

    void Set(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

class Matrix4
{
public:
    // column 0
    float m00;
    float m10;
    float m20;
    float m30;
    // column 1
    float m01;
    float m11;
    float m21;
    float m31;
    // column 2
    float m02;
    float m12;
    float m22;
    float m32;
    // column 3
    float m03;
    float m13;
    float m23;
    float m33;

public:
    Matrix4(void)
    {
        Identity();
    }

    ~Matrix4(void)
    {
    }

    Matrix4(const float* m, bool isColumnMajor)
    {
        if (isColumnMajor)
        {
            m00 = m[0];m01 = m[4];m02 = m[8];m03 = m[12];
            m10 = m[1];m11 = m[5];m12 = m[9];m13 = m[13];
            m20 = m[2];m21 = m[6];m22 = m[10];m23 = m[14];
            m30 = m[3];m31 = m[7];m32 = m[11];m33 = m[15];
        }
        else
        {
            m00 = m[0];m01 = m[1];m02 = m[2];m03 = m[3];
            m10 = m[4];m11 = m[5];m12 = m[6];m13 = m[7];
            m20 = m[8];m21 = m[9];m22 = m[10];m23 = m[11];
            m30 = m[12];m31 = m[13];m32 = m[14];m33 = m[15];
        }
    }

    Matrix4(const Matrix4& m)
        :m00(m.m00), m10(m.m10), m20(m.m20), m30(m.m30)
        ,m01(m.m01), m11(m.m11), m21(m.m21), m31(m.m31)
        ,m02(m.m02), m12(m.m12), m22(m.m22), m32(m.m32)
        ,m03(m.m03), m13(m.m13), m23(m.m23), m33(m.m33)
    {
    }

    Matrix4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
        )
        :m00(m00), m10(m10), m20(m20), m30(m30)
        ,m01(m01), m11(m11), m21(m21), m31(m31)
        ,m02(m02), m12(m12), m22(m22), m32(m32)
        ,m03(m03), m13(m13), m23(m23), m33(m33)
    {
    }

    void CopyTo(float* buf, bool isColumnMajor = true) const
    {
        if (isColumnMajor)
        {
            buf[0] = m00; buf[1] = m10; buf[2] = m20; buf[3] = m30;
            buf[4] = m01; buf[5] = m11; buf[6] = m21; buf[7] = m31;
            buf[8] = m02; buf[9] = m12; buf[10] = m22; buf[11] = m32;
            buf[12] = m03; buf[13] = m13; buf[14] = m23; buf[15] = m33;
        }
        else
        {
            buf[0] = m00; buf[4] = m10; buf[8] = m20; buf[12] = m30;
            buf[1] = m01; buf[5] = m11; buf[9] = m21; buf[13] = m31;
            buf[2] = m02; buf[6] = m12; buf[10] = m22; buf[14] = m32;
            buf[3] = m03; buf[7] = m13; buf[11] = m23; buf[15] = m33;
        }
    }

    Matrix4 operator * (const Matrix4& m) const
    {
        return Matrix4(
            // row 0
            m00 * m.m00 + m01 * m.m10 + m02 * m.m20 + m03 * m.m30,
            m00 * m.m01 + m01 * m.m11 + m02 * m.m21 + m03 * m.m31,
            m00 * m.m02 + m01 * m.m12 + m02 * m.m22 + m03 * m.m32,
            m00 * m.m03 + m01 * m.m13 + m02 * m.m23 + m03 * m.m33,
            // row 1
            m10 * m.m00 + m11 * m.m10 + m12 * m.m20 + m13 * m.m30,
            m10 * m.m01 + m11 * m.m11 + m12 * m.m21 + m13 * m.m31,
            m10 * m.m02 + m11 * m.m12 + m12 * m.m22 + m13 * m.m32,
            m10 * m.m03 + m11 * m.m13 + m12 * m.m23 + m13 * m.m33,
            // row 2
            m20 * m.m00 + m21 * m.m10 + m22 * m.m20 + m23 * m.m30,
            m20 * m.m01 + m21 * m.m11 + m22 * m.m21 + m23 * m.m31,
            m20 * m.m02 + m21 * m.m12 + m22 * m.m22 + m23 * m.m32,
            m20 * m.m03 + m21 * m.m13 + m22 * m.m23 + m23 * m.m33,
            // row 3
            m30 * m.m00 + m31 * m.m10 + m32 * m.m20 + m33 * m.m30,
            m30 * m.m01 + m31 * m.m11 + m32 * m.m21 + m33 * m.m31,
            m30 * m.m02 + m31 * m.m12 + m32 * m.m22 + m33 * m.m32,
            m30 * m.m03 + m31 * m.m13 + m32 * m.m23 + m33 * m.m33
            );
    }

    Vector3 operator * (const Vector3& v) const
    {
        float wInv = 1.0f / (v.x * m03 + v.y * m13 + v.z * m23 + m33);
        return Vector3
            ((v.x * m00 + v.y * m10 + v.z * m20 + m30) * wInv
            ,(v.x * m01 + v.y * m11 + v.z * m21 + m31) * wInv
            ,(v.x * m02 + v.y * m12 + v.z * m22 + m32) * wInv);
    }

    Matrix4 Inverse() const
    {
        float d =
            (m00 * m11 - m01 * m10) * (m22 * m33 - m23 * m32) -
            (m00 * m12 - m02 * m10) * (m21 * m33 - m23 * m31) +
            (m00 * m13 - m03 * m10) * (m21 * m32 - m22 * m31) +
            (m01 * m12 - m02 * m11) * (m20 * m33 - m23 * m30) -
            (m01 * m13 - m03 * m11) * (m20 * m32 - m22 * m30) +
            (m02 * m13 - m03 * m12) * (m20 * m31 - m21 * m30);

        d = 1.0f / d;

        return Matrix4(
            d * (m11 * (m22 * m33 - m23 * m32) + m12 * (m23 * m31 - m21 * m33) + m13 * (m21 * m32 - m22 * m31)),
            d * (m21 * (m02 * m33 - m03 * m32) + m22 * (m03 * m31 - m01 * m33) + m23 * (m01 * m32 - m02 * m31)),
            d * (m31 * (m02 * m13 - m03 * m12) + m32 * (m03 * m11 - m01 * m13) + m33 * (m01 * m12 - m02 * m11)),
            d * (m01 * (m13 * m22 - m12 * m23) + m02 * (m11 * m23 - m13 * m21) + m03 * (m12 * m21 - m11 * m22)),

            d * (m12 * (m20 * m33 - m23 * m30) + m13 * (m22 * m30 - m20 * m32) + m10 * (m23 * m32 - m22 * m33)),
            d * (m22 * (m00 * m33 - m03 * m30) + m23 * (m02 * m30 - m00 * m32) + m20 * (m03 * m32 - m02 * m33)),
            d * (m32 * (m00 * m13 - m03 * m10) + m33 * (m02 * m10 - m00 * m12) + m30 * (m03 * m12 - m02 * m13)),
            d * (m02 * (m13 * m20 - m10 * m23) + m03 * (m10 * m22 - m12 * m20) + m00 * (m12 * m23 - m13 * m22)),

            d * (m13 * (m20 * m31 - m21 * m30) + m10 * (m21 * m33 - m23 * m31) + m11 * (m23 * m30 - m20 * m33)),
            d * (m23 * (m00 * m31 - m01 * m30) + m20 * (m01 * m33 - m03 * m31) + m21 * (m03 * m30 - m00 * m33)),
            d * (m33 * (m00 * m11 - m01 * m10) + m30 * (m01 * m13 - m03 * m11) + m31 * (m03 * m10 - m00 * m13)),
            d * (m03 * (m11 * m20 - m10 * m21) + m00 * (m13 * m21 - m11 * m23) + m01 * (m10 * m23 - m13 * m20)),

            d * (m10 * (m22 * m31 - m21 * m32) + m11 * (m20 * m32 - m22 * m30) + m12 * (m21 * m30 - m20 * m31)),
            d * (m20 * (m02 * m31 - m01 * m32) + m21 * (m00 * m32 - m02 * m30) + m22 * (m01 * m30 - m00 * m31)),
            d * (m30 * (m02 * m11 - m01 * m12) + m31 * (m00 * m12 - m02 * m10) + m32 * (m01 * m10 - m00 * m11)),
            d * (m00 * (m11 * m22 - m12 * m21) + m01 * (m12 * m20 - m10 * m22) + m02 * (m10 * m21 - m11 * m20))
        );
    }

    void Identity()
    {
        m00 = 1.0f; m01 = 0.0f; m02 = 0.0f; m03 = 0.0f;
        m10 = 0.0f; m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
        m20 = 0.0f; m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
        m30 = 0.0f; m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
    }

    Vector3 GetXAxis() const
    {
        return Vector3(m00, m10, m20);
    }

    Vector3 GetYAxis() const
    {
        return Vector3(m01, m11, m21);
    }

    Vector3 GetZAxis() const
    {
        return Vector3(m02, m12, m22);
    }

    Vector3 GetTranslate() const
    {
        return Vector3(m03, m13, m23);
    }

    void Translate(float x, float y, float z)
    {
        const Matrix4& m = BuildTranslate(x, y, z) * *this;
        *this = m;
    }

    void Scale(float x, float y, float z)
    {
        const Matrix4& m = BuildScale(x, y, z) * *this;
        *this = m;
    }

    void Rotate(float radians, float x, float y, float z)
    {
        const Matrix4& m = BuildRotate(radians, x, y, z) * *this;
        *this = m;
    }

    void RotateAt(float radians, float x, float y, float z, float posx, float posy, float posz)
    {
        const Matrix4& m = BuildRotateAt(radians, x, y, z, posx, posy, posz) * *this;
        *this = m;
    }

    void LocalTranslateX(float delta)
    {
        const Vector3& d = GetXAxis().GetNormalized() * delta;
        m03 += d.x;
        m13 += d.y;
        m23 += d.z;
    }

    void LocalTranslateY(float delta)
    {
        const Vector3& d = GetYAxis().GetNormalized() * delta;
        m03 += d.x;
        m13 += d.y;
        m23 += d.z;
    }

    void LocalTranslateZ(float delta)
    {
        const Vector3& d = GetZAxis().GetNormalized() * delta;
        m03 += d.x;
        m13 += d.y;
        m23 += d.z;
    }

    void LocalRotate(float deltaRadians, float x, float y, float z)
    {
        float tx = m03;
        float ty = m13;
        float tz = m23;
        Translate(-tx, -ty, -tz);
        Rotate(deltaRadians, x, y, z);
        Translate(tx, ty, tz);
    }

    static Matrix4 BuildTranslate(float x, float y, float z)
    {
        return Matrix4(
            1.0f, 0.0f, 0.0f, x,
            0.0f, 1.0f, 0.0f, y,
            0.0f, 0.0f, 1.0f, z,
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    static Matrix4 BuildScale(float sx, float sy, float sz)
    {
        return Matrix4(
              sx, 0.0f, 0.0f, 0.0f,
            0.0f,   sy, 0.0f, 0.0f,
            0.0f, 0.0f,   sz, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    static Matrix4 BuildRotate(float radians, float x, float y, float z)
    {
        float cosa = cosf(radians);
        float sina = sinf(radians);
        float mc = 1.0f - cosa;
        float xyc = x * y * mc;
        float yzc = y * z * mc;
        float xzc = x * z * mc;
        float xxc = x * x * mc;
        float yyc = y * y * mc;
        float zzc = z * z * mc;
        float xs = x * sina;
        float ys = y * sina;
        float zs = z * sina;

        return Matrix4(
            xxc + cosa,  xyc - zs,    xzc + ys,   0.0f,
            xyc + zs,    yyc + cosa,  yzc - xs,   0.0f,
            xzc - ys,    yzc + xs,    zzc + cosa, 0.0f,
            0.0f,        0.0f,        0.0f,       1.0f);
    }

    static Matrix4 BuildRotateAt(float radians, float x, float y, float z, float posx, float posy, float posz)
    {
        return BuildTranslate(posx, posy, posz) * BuildRotate(radians, x, y, z) * BuildTranslate(-posx, -posy, -posz);
    }

    static Matrix4 BuildFrustum(float left, float right, float bottom, float top, float nearZ, float farZ)
    {
        float dx = 1.0f / (right - left);
        float dy = 1.0f / (top - bottom);
        float dz = 1.0f / (farZ - nearZ);
        float n2 = nearZ * 2;
        return Matrix4(
            n2 * dx, 0,        (right + left) * dx,  0,
            0,       n2 * dy,  (top + bottom) * dy,  0,
            0,       0,       -(farZ + nearZ) * dz, -n2 * farZ * dz,
            0,       0,       -1,                   0
            );
    }

    static Matrix4 BuildPerspective(float fovy, float aspect, float nearZ, float farZ)
    {
        float top = tanf(fovy * 0.5f * 3.1415926f / 180.0f) * nearZ;
        float right = top * aspect;
        return BuildFrustum(-top, top, -right, right, nearZ, farZ);
    }

    static Matrix4 BuildOrtho(float left, float right, float bottom, float top, float nearZ, float farZ)
    {
        float dx = 1.0f / (right - left);
        float dy = 1.0f / (top - bottom);
        float dz = 1.0f / (farZ - nearZ);
        return Matrix4(
            2 * dx,      0,       0, -(right + left) * dx,
                 0, 2 * dy,       0, -(top + bottom) * dy,
                 0,      0, -2 * dz, -(farZ + nearZ) * dz,
                 0,      0,       0,                    1
            );
    }
};


class GLTexture
{
    friend class GLRenderer;
    friend class GLRenderTarget;
    friend class GLShaderProgram;
public:
    ~GLTexture();
    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }
    void ReadTexture(int* data);
    void WriteTexture(int* data, int w, int h, bool useMipmap);

private:
    GLTexture(int width, int height, int* data, bool topDownOrder, int colorSize, bool magSmooth, bool repeat, bool useMipmap);

    int mWidth;
    int mHeight;
    int mTextureId;
};

class GLRenderTarget
{
    friend class GLRenderer;
public:
    ~GLRenderTarget();
    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }
    void Blit(GLRenderTarget* src, bool smooth);
    GLTexture* GetTexture() { return mTexture; }

private:
    GLRenderTarget(GLTexture* texture);

    int mWidth;
    int mHeight;
    int mFrameBufferId;
    GLTexture* mTexture;
};

class GLShaderProgram
{
    friend class GLRenderer;
public:
    GLShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
    ~GLShaderProgram();

    void SetAttribute(const char* name, int index, int numComponents, float* attr);
    void SetTexture(const char* name, int index, GLTexture* texture);
    void SetColor(const char* name, float r, float g, float b, float a);
    void SetMatrix(const char* name, float* mat);
    void SetFloat4(const char* name, float* v);
    void SetFloat3(const char* name, float* v);
    void SetFloat2(const char* name, float* v);
    void SetFloat(const char* name, float v);
    void SetFloat4Array(const char* name, float* v, int count);
    void SetFloat3Array(const char* name, float* v, int count);
    void SetFloat2Array(const char* name, float* v, int count);
    void SetFloatArray(const char* name, float* v, int count);
private:
    int mProgramId;
};


class GLUniformValue
{
    struct TextureValue
    {
        GLTexture* texture;
        int index;
    };

    union Value
    {
        float value;
        float vec2[2];
        float vec3[3];
        float vec4[4];
        float mat4[16];
        TextureValue texture;
    };

    enum ValueType
    {
        ValueTypeNone,
        ValueTypeTexture,
        ValueTypeFloat,
        ValueTypeFloat2,
        ValueTypeFloat3,
        ValueTypeFloat4,
        ValueTypeMatrix4
    };

public:
    GLUniformValue();
    GLUniformValue(int index, GLTexture* texture);
    GLUniformValue(float x, float y, float z, float w);
    GLUniformValue(float x, float y, float z);
    GLUniformValue(float x, float y);
    GLUniformValue(float v);
    GLUniformValue(const float* mat);
    ~GLUniformValue();

    void Apply(const char* name, GLShaderProgram* program);

private:
    ValueType mType;
    Value mValue;
};

class GLShaderState
{
    typedef std::map<std::string, GLUniformValue> ValueMap;
public:
    GLShaderState();
    ~GLShaderState();
    void SetValue(const char* name, const GLUniformValue& value);
    void Apply(GLShaderProgram* program);

private:
    ValueMap mValues;
};

class GLAttribute
{
    friend class GLMesh;
    friend class GLRenderer;
public:
    GLAttribute(int components, int count);
    ~GLAttribute();

    void SetValue(int index, float value);
    void SetValue(int index, float x, float y);
    void SetValue(int index, float x, float y, float z);
    void SetValue(int index, float x, float y, float z, float w);

private:
    int mComponets;
    int mCount;
    float* mData;
};

class GLMesh
{
    friend class GLRenderer;
    typedef std::map<std::string, GLAttribute*> AttributeMap;
public:

    GLMesh(int vertexCount, int indexCount);
    ~GLMesh();

    GLAttribute* AddAttribute(const char* name, int components);
    GLAttribute* GetAttribute(const char* name);
    void SetIndexValue(int index, unsigned int value);
    void Apply(GLShaderProgram* program);

private:
    int mVertexCount;
    int mIndexCount;
    AttributeMap mAttributes;
    unsigned int* mIndices;
};

class GLShape
{
    friend class GLRenderer;
public:
    GLShape();
    ~GLShape();

    void Build();
    void Clear();
    void SetColor(float r, float g, float b, float a);
    GLMesh* GetMesh() { return mMesh; }

    void AddRect(float x, float y, float width, float height);
    void AddCircle(float x, float y, float r);
    void AddLine(float x0, float y0, float x1, float y1, float w = 1.0f);
    void AddLineV(float x0, float y0, float w0, float x1, float y1, float w1);

private:
    std::vector<float> mVertices;
    std::vector<unsigned int> mIndices;
    GLMesh* mMesh;
    float mColor[4];
};

class GLRenderer
{
public:
    GLRenderer();
    ~GLRenderer();

    GLTexture* CreateTexture(int width, int height, int* data, bool topDownOrder);
    GLRenderTarget* CreateTarget(int width, int height, int* data, bool topDownOrder);
    GLMesh* CreateCube(float center[3], float size[3]);
    GLShape* CreateShape();

    void UseProgram(GLShaderProgram* program);
    void SetRenderTarget(GLRenderTarget* target);
    void SetScreenSize(int width, int height);
    void Clear(GLRenderTarget* target, float r, float g, float b, float a);
    void BlendSource(GLRenderTarget* target, GLTexture* src, float* mvp, float r, float g, float b, float a);
    void BlendSourceOver(GLRenderTarget* target, GLTexture* dst, GLTexture* src, float* mvp, float r, float g, float b, float a);
    void BlendMultipy(GLRenderTarget* target, GLTexture* dst, GLTexture* src, float* mvp, float r, float g, float b, float a);
    void Blur(GLRenderTarget* target, GLRenderTarget* targetTemp, float* mvp, GLTexture* src, int radius);
    void Mask(GLRenderTarget* target, GLTexture* src, float* mvp, GLTexture* colorMask, GLTexture* alphaMask, float alpha);


//    void ChangeColor(int r, int g, int b, int a);
//    void DrawImage(int texture, int x, int y);
//    void DrawVLine(float x0, float y0, float w0, float x1, float y1, float w1);
//    void DrawCircle(float x, float y, float r);

    void DrawMesh(GLMesh* mesh, GLShaderProgram* program, GLShaderState* state);
    void DrawShape(GLShape* shape, float* mvp);
private:
    int mWidth;
    int mHeight;
    GLTexture* mDefaultTexture;
    GLShaderProgram* mShapeProgram;
    GLShaderProgram* mBlendSourceProgram;
    GLShaderProgram* mBlendSourceOverProgram;
    GLShaderProgram* mBlendMultiplyProgram;
    GLShaderProgram* mMaskProgram;
    BlurProgram* mBlurPrograms[MAX_BLUR_RADIUS];
};

#endif // OPENGLRENDERER_H
