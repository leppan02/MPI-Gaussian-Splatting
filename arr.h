#include <iostream>
#include <valarray>
using namespace std;

template <int D, class T>
struct Arr
{
    std::valarray<T> data;

    // Assignment
    Arr<D, T> operator=(const Arr<D, T> &o)
    {
        for (int i = 0; i < D; i++)
            this->data[i] = o.data[i];
        return *this;
    }
    Arr<D, T> operator=(const T &o)
    {
        for (int i = 0; i < D; i++)
            this->data[i] = o;
        return *this;
    }

    // Constructor
    Arr<D, T>() {}
    Arr<D, T>(T o)
    {
        for (int i = 0; i < D; i++)
            data[i] = 0;
    }
    Arr<D, T>(const Arr<D, T> &o)
    {
        for (int i = 0; i < D; i++)
            data[i] = o.data[i];
    }
    //////////////////////////////////
    // In place
    Arr<D, T> operator+=(const Arr<D, T> &o)
    {
        for (int i = 0; i < D; i++)
            this->data[i] += o.data[i];
        return *this;
    }
    Arr<D, T> operator*=(const Arr<D, T> &o)
    {
        for (int i = 0; i < D; i++)
            this->data[i] *= o.data[i];
        return *this;
    }
    Arr<D, T> operator-=(const Arr<D, T> &o)
    {
        for (int i = 0; i < D; i++)
            this->data[i] -= o.data[i];
        return *this;
    }
    Arr<D, T> operator/=(const Arr<D, T> &o)
    {
        for (int i = 0; i < D; i++)
            this->data[i] /= o.data[i];
        return *this;
    }

    Arr<D, T> operator+=(const T &o)
    {
        for (int i = 0; i < D; i++)
            this->data[i] += o;
        return *this;
    }
    Arr<D, T> operator*=(const T &o)
    {
        for (int i = 0; i < D; i++)
            this->data[i] *= o;
        return *this;
    }
    Arr<D, T> operator-=(const T &o)
    {
        for (int i = 0; i < D; i++)
            this->data[i] -= o;
        return *this;
    }
    Arr<D, T> operator/=(const T &o)
    {
        for (int i = 0; i < D; i++)
            this->data[i] /= o;
        return *this;
    }

    /////////////////////////////
    // Not in place
    Arr<D, T> operator+(const Arr<D, T> &o) const
    {
        Arr<D, T> res(*this);
        res += o;
        return res;
    }
    Arr<D, T> operator*(const Arr<D, T> &o) const
    {
        Arr<D, T> res(*this);
        res *= o;
        return res;
    }
    Arr<D, T> operator-(const Arr<D, T> &o) const
    {
        Arr<D, T> res(*this);
        res -= o;
        return res;
    }
    Arr<D, T> operator/(const Arr<D, T> &o) const
    {
        Arr<D, T> res(*this);
        res /= o;
        return res;
    }

    Arr<D, T> operator+(const T &o) const
    {
        Arr<D, T> res(*this);
        res += o;
        return res;
    }
    Arr<D, T> operator*(const T &o) const
    {
        Arr<D, T> res(*this);
        res *= o;
        return res;
    }
    Arr<D, T> operator-(const T &o) const
    {
        Arr<D, T> res(*this);
        res -= o;
        return res;
    }
    Arr<D, T> operator/(const T &o) const
    {
        Arr<D, T> res(*this);
        res /= o;
        return res;
    }

    inline T &x() const{
        return data[0];
    }
    inline T &y() const{
        return data[1];
    }
    inline T &z() const{
        return data[2];
    }
    inline T &r() const{
        return data[0];
    }
    inline T &g() const{
        return data[1];
    }
    inline T &b() const{
        return data[2];
    }
    inline T &a() const{
        return data[3];
    }

    void clip(T mn, T mx)
    {
        for (int i = 0; i < D; i++)
            data[i] = min(mx, max(mn, data[i]));
    }

};

template <int D, class T>
ostream &operator<<(ostream &os, const Arr<D, T> &arr)
{
    os << "Arr(" << D << ") = [";
    for (int i = 0; i < D; i++)
    {
        if (i != 0)
            os << "; ";
        os << arr.data[i];
    }
    os << "]";
    return os;
}

template <int D, class T>
Arr<D, T> operator+(const T &o, const Arr<D, T> &arr)
{
    return arr + o;
}
template <int D, class T>
Arr<D, T> operator*(const T &o, const Arr<D, T> &arr)
{
    return arr * o;
}
template <int D, class T>
Arr<D, T> operator-(const T &o, const Arr<D, T> &arr)
{
    return (-1) * arr + o;
}
template <int D, class T>
Arr<D, T> operator/(const T &o, const Arr<D, T> &arr)
{
    Arr<D, T> one(1);
    return (one / arr) * o;
}