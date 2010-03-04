/** \file
\brief Tree View of scene
\author Josiah Manson

Modified: May 23, 2006
changed constructors to compile with gcc 4.0

Modified: July 07, 2005
added constructor to vect4<T>
**/

#pragma once

#ifndef __vect_h_
#define __vect_h_

//#pragma warning (disable: 4786 4244 4305)

#include <assert.h>
#include <cmath>

//**********************************************************//
//******************definition of classes*******************//
//**********************************************************//

/// 2D vector
template <class T>
class vect2
{
public:
	T v[2];

	vect2<T>() {}
	vect2<T>(const vect2<T>& obj);
	vect2<T>(const T x, const T y) {v[0] = x; v[1] = y;}
	~vect2<T>() {}

	// misc
	vect2<T> rotate(const T rad) const;

	// math
	inline void add(const vect2<T>);
	inline void subtract(const vect2<T>);
	inline void multiply(const T);
	inline void divide(const T);
	inline T dot(const vect2<T>) const;
	inline void cross(const vect2<T>);

	void negative();
	T length() const;
	T length2() const;
	void unit();

	T comp(const vect2<T>) const;
	vect2<T> proj(const vect2<T>) const;

	// operators
	inline T &operator[](const int); // return index
	inline void operator()(const T, const T); // assign Ts

	inline vect2<T> operator+(const vect2<T>) const; // add
	inline vect2<T> operator-(const vect2<T>) const; // subtract
	inline vect2<T> operator*(const T) const; // multiply
	inline vect2<T> operator/(const T) const; // divide
	inline T operator*(const vect2<T>) const; // dot product
	inline vect2<T> operator%(const vect2<T>) const; // cross product	

	void operator+=(const vect2<T> val) { add(val); }
	void operator-=(const vect2<T> val) { subtract(val); }
	void operator*=(const T val) { multiply(val); }
	void operator/=(const T val) { divide(val); }
	void operator%=(const vect2<T> val) { cross(val); }

	inline vect2<T> operator-() const; // negation
	inline T operator!() const; // absolute value, length
	inline vect2<T> operator~() const; // unit vector

	bool operator==(const vect2<T> val) const 
	{
		if (v[0] == val.v[0] && v[1] == val.v[1])
			return true;
		return false;
	}
	bool operator!=(const vect2<T> val) const 
	{
		return !(*this == val);
	}
};


//=========================================================//

/// 3D vector
template <class T>
class vect3
{
public:
	T v[3];

	vect3<T>() {}
	vect3<T>(const vect3<T>& obj);
	vect3<T>(const T x, const T y, const T z) {v[0] = x; v[1] = y; v[2] = z;}
	~vect3<T>() {}

	// functions
	inline void add(const vect3<T>);
	inline void subtract(const vect3<T>);
	inline void multiply(const T);
	inline void divide(const T);
	inline T dot(const vect3<T>) const;
	inline void cross(const vect3<T>);

	inline void negative();
	inline T length() const;
	inline T length2() const;
	inline void unit();

	T comp(const vect3<T>) const;
	vect3<T> proj(const vect3<T>) const;

	// operators
	inline T &operator[](const int); // return index
	inline void operator()(const T, const T, const T); // assign Ts

	inline vect3<T> operator+(const vect3<T>) const; // add
	inline vect3<T> operator-(const vect3<T>) const; // subtract
	inline vect3<T> operator*(const T) const; // multiply
	inline vect3<T> operator/(const T) const; // divide
	inline T operator*(const vect3<T>) const; // dot product
	inline vect3<T> operator%(const vect3<T>) const; // cross product	

	void operator+=(const vect3<T> val) { add(val); }
	void operator-=(const vect3<T> val) { subtract(val); }
	void operator*=(const T val) { multiply(val); }
	void operator/=(const T val) { divide(val); }
	void operator%=(const vect3<T> val) { cross(val); }

	inline vect3<T> operator-() const; // negation
	inline T operator!() const; // absolute value, length
	inline vect3<T> operator~() const; // unit vector

	bool operator==(vect3<T> val) const
	{
		if (v[0] == val.v[0] && v[1] == val.v[1] && v[2] == val.v[2])
			return true;
		return false;
	}
	bool operator!=(const vect3<T> val) const 
	{
		return !(*this == val);
	}
};

//=========================================================//

/// 4D vector
template <class T>
class vect4
{
public:
	T v[4];

	vect4<T>() {}
	vect4<T>(const vect4<T>& obj);
	vect4<T>(const T x, const T y, const T z, const T w) {v[0] = x; v[1] = y; v[2] = z; v[3] = w;}
	~vect4<T>() {}

	// functions
	inline void add(const vect4<T>);
	inline void subtract(const vect4<T>);
	inline void multiply(const T);
	inline void divide(const T);

	inline void negative();

	// operators
	inline T &operator[](const int); // return index
	inline void operator()(const T, const T, const T, const T); // assign Ts

	inline vect4<T> operator+(const vect4<T>) const; // add
	inline vect4<T> operator-(const vect4<T>) const; // subtract
	inline vect4<T> operator*(const T) const; // multiply
	inline vect4<T> operator/(const T) const; // divide

	void operator+=(const vect4<T> val) { add(val); }
	void operator-=(const vect4<T> val) { subtract(val); }
	void operator*=(const T val) { multiply(val); }
	void operator/=(const T val) { divide(val); }

	inline vect4<T> operator-() const; // negation

	bool operator==(const vect4<T> val) const
	{
		if (v[0] == val.v[0] && v[1] == val.v[1] && v[2] == val.v[2] && v[3] == val.v[3])
			return true;
		return false;
	}
	bool operator!=(const vect4<T> val) const 
	{
		return !(*this == val);
	}
};

//**********************************************************//
//***********************typedefs***************************//
//**********************************************************//

typedef vect2<float> vect2f;
typedef vect2<double> vect2d;

typedef vect3<float> vect3f;
typedef vect3<double> vect3d;

typedef vect4<float> vect4f;
typedef vect4<double> vect4d;

#ifndef WIN32
#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long
#endif

typedef vect2<__int8> vect2b;
typedef vect2<__int16> vect2s;
typedef vect2<__int32> vect2i;
typedef vect2<__int64> vect2l;
typedef vect2<unsigned __int8> vect2ub;
typedef vect2<unsigned __int16> vect2us;
typedef vect2<unsigned __int32> vect2ui;
typedef vect2<unsigned __int64> vect2ul;

typedef vect3<__int8> vect3b;
typedef vect3<__int16> vect3s;
typedef vect3<__int32> vect3i;
typedef vect3<__int64> vect3l;
typedef vect3<unsigned __int8> vect3ub;
typedef vect3<unsigned __int16> vect3us;
typedef vect3<unsigned __int32> vect3ui;
typedef vect3<unsigned __int64> vect3ul;

typedef vect4<__int8> vect4b;
typedef vect4<__int16> vect4s;
typedef vect4<__int32> vect4i;
typedef vect4<__int64> vect4l;
typedef vect4<unsigned __int8> vect4ub;
typedef vect4<unsigned __int16> vect4us;
typedef vect4<unsigned __int32> vect4ui;
typedef vect4<unsigned __int64> vect4ul;


//**********************************************************//
//*******************template functions*********************//
//**********************************************************//

template <class A, class B>
void vec2Copy(A& a, const B& b)
{
	a.v[0] = b.v[0];
	a.v[1] = b.v[1];
}
template <class A, class B>
void vec3Copy(A& a, const B& b)
{
	a.v[0] = b.v[0];
	a.v[1] = b.v[1];
	a.v[2] = b.v[2];
}
template <class A, class B>
void vec4Copy(A& a, const B& b)
{
	a.v[0] = b.v[0];
	a.v[1] = b.v[1];
	a.v[2] = b.v[2];
	a.v[3] = b.v[3];
}


template <class A, class B>
void vec2mult(A a, const B& b)
{
	a.v[0] *= b.v[0];
	a.v[1] *= b.v[1];

	return a;
}
template <class A, class B>
void vec3mult(A a, const B& b)
{
	a.v[0] *= b.v[0];
	a.v[1] *= b.v[1];
	a.v[2] *= b.v[2];

	return a;
}
template <class A, class B>
void vec4mult(A a, const B& b)
{
	a.v[0] *= b.v[0];
	a.v[1] *= b.v[1];
	a.v[2] *= b.v[2];
	a.v[3] *= b.v[3];

	return a;
}


template <class A, class B>
void vec2div(A a, const B& b)
{
	a.v[0] /= b.v[0];
	a.v[1] /= b.v[1];

	return a;
}
template <class A, class B>
void vec3div(A a, const B& b)
{
	a.v[0] /= b.v[0];
	a.v[1] /= b.v[1];
	a.v[2] /= b.v[2];

	return a;
}
template <class A, class B>
void vec4div(A a, const B& b)
{
	a.v[0] /= b.v[0];
	a.v[1] /= b.v[1];
	a.v[2] /= b.v[2];
	a.v[3] /= b.v[3];

	return a;
}


template <class A, class B>
void vec2add(A a, const B& b)
{
	a.v[0] += b.v[0];
	a.v[1] += b.v[1];

	return a;
}
template <class A, class B>
void vec3add(A a, const B& b)
{
	a.v[0] += b.v[0];
	a.v[1] += b.v[1];
	a.v[2] += b.v[2];

	return a;
}
template <class A, class B>
void vec4add(A a, const B& b)
{
	a.v[0] += b.v[0];
	a.v[1] += b.v[1];
	a.v[2] += b.v[2];
	a.v[3] += b.v[3];

	return a;
}


template <class A, class B>
void vec2sub(A a, const B& b)
{
	a.v[0] -= b.v[0];
	a.v[1] -= b.v[1];

	return a;
}
template <class A, class B>
void vec3sub(A a, const B& b)
{
	a.v[0] -= b.v[0];
	a.v[1] -= b.v[1];
	a.v[2] -= b.v[2];

	return a;
}
template <class A, class B>
void vec4sub(A a, const B& b)
{
	a.v[0] -= b.v[0];
	a.v[1] -= b.v[1];
	a.v[2] -= b.v[2];
	a.v[3] -= b.v[3];

	return a;
}

//**********************************************************//
//*********************conversion functions*********************//
//**********************************************************//

template<class T, class U>
vect2<T> convert(vect2<U> p, T type)
{
	vect2<T> q((T)p[0], (T)p[1]);
	return q;
}

template<class T, class U>
vect3<T> convert(vect3<U> p, T type)
{
	vect3<T> q((T)p[0], (T)p[1], (T)p[2]);
	return q;
}

template<class T, class U>
vect4<T> convert(vect4<U> p, T type)
{
	vect4<T> q((T)p[0], (T)p[1], (T)p[2], (T)p[3]);
	return q;
}


//**********************************************************//
//*********************vect2<T> functions*********************//
//**********************************************************//

template <class T>
vect2<T>::vect2(const vect2<T>& obj)
{
	v[0] = obj.v[0];
	v[1] = obj.v[1];
}

//---------------------------------------------------------//

template <class T>
vect2<T> vect2<T>::rotate(const T rad) const
{
	const T s = (double)sin(rad), c = (double)cos(rad);
	vect2<T> ret;

	ret.v[0] = c * v[0] - s * v[1];
	ret.v[1] = s * v[0] + c * v[1];

	return ret;
}

//---------------------------------------------------------//

template <class T>
void vect2<T>::add(const vect2<T> add)
{
	v[0] = v[0] + add.v[0];
	v[1] = v[1] + add.v[1];
}

//---------------------------------------------------------//

template <class T>
void vect2<T>::subtract(const vect2<T> sub)
{
	v[0] = v[0] - sub.v[0];
	v[1] = v[1] - sub.v[1];
}

//---------------------------------------------------------//

template <class T>
void vect2<T>::multiply(const T mult)
{
	v[0] = v[0] * mult;
	v[1] = v[1] * mult;
}

//---------------------------------------------------------//

template <class T>
void vect2<T>::divide(const T div)
{
	v[0] = v[0] / div;
	v[1] = v[1] / div;
}

//---------------------------------------------------------//

template <class T>
T vect2<T>::dot(const vect2<T> dot) const
{
	return v[0] * dot.v[0] + v[1] * dot.v[1];
}

//---------------------------------------------------------//

template <class T>
void vect2<T>::negative()
{
	v[0] = -v[0];
	v[1] = -v[1];
}

//---------------------------------------------------------//

template <class T>
T vect2<T>::length() const
{
	return (double) sqrt(v[0] * v[0] + v[1] * v[1]);
}

template <class T>
T vect2<T>::length2() const
{
	return v[0] * v[0] + v[1] * v[1];
}

//---------------------------------------------------------//

template <class T>
void vect2<T>::unit()
{
	const T len = T(1.0 / (double) sqrt(v[0] * v[0] + v[1] * v[1]));

	v[0] *= len;
	v[1] *= len;
}

//---------------------------------------------------------//

template <class T>
T vect2<T>::comp(const vect2<T> obj) const
{
	return (*this * obj) / !*this;
}

//---------------------------------------------------------//

template <class T>
vect2<T> vect2<T>::proj(const vect2<T> obj) const
{
	const vect2<T> a = *this, b = obj;
	return ((a * b) / (a * a)) * a;
}

//=========================================================//

template <class T>
T &vect2<T>::operator[](const int i)
{
	return v[i];
}

//---------------------------------------------------------//

template <class T>
void vect2<T>::operator()(const T x, const T y)
{
	v[0] = x;
	v[1] = y;
}

//---------------------------------------------------------//

template <class T>
vect2<T> vect2<T>::operator+(const vect2<T> add) const
{
	vect2<T> result;
	result.v[0] = v[0] + add.v[0];
	result.v[1] = v[1] + add.v[1];
	return result;
}

//---------------------------------------------------------//

template <class T>
vect2<T> vect2<T>::operator-(const vect2<T> add) const
{
	vect2<T> result;
	result.v[0] = v[0] - add.v[0];
	result.v[1] = v[1] - add.v[1];
	return result;
}

//---------------------------------------------------------//

template <class T>
vect2<T> vect2<T>::operator*(const T mult) const
{
	vect2<T> result;
	result.v[0] = v[0] * mult;
	result.v[1] = v[1] * mult;
	return result;
}

//---------------------------------------------------------//

template <class T>
vect2<T> vect2<T>::operator/(const T div) const
{
	vect2<T> result;
	result.v[0] = v[0] / div;
	result.v[1] = v[1] / div;
	return result;
}

//---------------------------------------------------------//

template <class T>
T vect2<T>::operator*(const vect2<T> dot) const
{
	T result;
	result = v[0] * dot.v[0] + v[1] * dot.v[1];
	return result;
}

//---------------------------------------------------------//

template <class T>
vect2<T> vect2<T>::operator-() const
{
	vect2<T> result;
	result.v[0] = -v[0];
	result.v[1] = -v[1];
	return result;
}

//---------------------------------------------------------//

template <class T>
T vect2<T>::operator!() const
{
	T dist = sqrt(v[0] * v[0] + v[1] * v[1]);
	return dist;
}

//---------------------------------------------------------//

template <class T>
vect2<T> vect2<T>::operator~() const
{
	vect2<T> result;
	const T len = 1 / (double) sqrt(v[0] * v[0] + v[1] * v[1]);

	result.v[0] = v[0] * len;
	result.v[1] = v[1] * len;

	return result;
}

//**********************************************************//
//*********************vect3<T> functions*********************//
//**********************************************************//

template <class T>
vect3<T>::vect3(const vect3<T>& obj)
{
	v[0] = obj.v[0];
	v[1] = obj.v[1];
	v[2] = obj.v[2];
}

//---------------------------------------------------------//

template <class T>
void vect3<T>::add(const vect3<T> add)
{
	v[0] = v[0] + add.v[0];
	v[1] = v[1] + add.v[1];
	v[2] = v[2] + add.v[2];
}

//---------------------------------------------------------//

template <class T>
void vect3<T>::subtract(const vect3<T> sub)
{
	v[0] = v[0] - sub.v[0];
	v[1] = v[1] - sub.v[1];
	v[2] = v[2] - sub.v[2];
}

//---------------------------------------------------------//

template <class T>
void vect3<T>::multiply(const T mult)
{
	v[0] = v[0] * mult;
	v[1] = v[1] * mult;
	v[2] = v[2] * mult;
}

//---------------------------------------------------------//

template <class T>
void vect3<T>::divide(const T div)
{
	v[0] = v[0] / div;
	v[1] = v[1] / div;
	v[2] = v[2] / div;
}

//---------------------------------------------------------//

template <class T>
T vect3<T>::dot(const vect3<T> dot) const
{
	return v[0] * dot.v[0] + v[1] * dot.v[1] + v[2] * dot.v[2];
}

//---------------------------------------------------------//

template <class T>
void vect3<T>::cross(const vect3<T> cross)
{
	vect3<T> result;
	result.v[0] = v[1] * cross.v[2] - v[2] * cross.v[1];
	result.v[1] = v[2] * cross.v[0] - v[0] * cross.v[2];
	result.v[2] = v[0] * cross.v[1] - v[1] * cross.v[0];
	*this = result;
}

//---------------------------------------------------------//

template <class T>
void vect3<T>::negative()
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

//---------------------------------------------------------//

template <class T>
T vect3<T>::length() const
{
	return (double) sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

template <class T>
T vect3<T>::length2() const
{
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

//---------------------------------------------------------//

template <class T>
void vect3<T>::unit()
{
	T len = 1 / (double) sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	v[0] *= len;
	v[1] *= len;
	v[2] *= len;
}

//---------------------------------------------------------//

template <class T>
T vect3<T>::comp(const vect3<T> obj) const
{
	return (*this * obj) / !*this;
}

//---------------------------------------------------------//

template <class T>
vect3<T> vect3<T>::proj(const vect3<T> obj) const
{
	vect3<T> a = *this, b = obj;
	return ((a * b) / (a * a)) * a;
}

//=========================================================//

template <class T>
T &vect3<T>::operator[](const int i)
{
	return v[i];
}

//---------------------------------------------------------//

template <class T>
void vect3<T>::operator()(const T x, const T y, const T z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

//---------------------------------------------------------//

template <class T>
vect3<T> vect3<T>::operator+(const vect3<T> add) const
{
	vect3<T> result;
	result.v[0] = v[0] + add.v[0];
	result.v[1] = v[1] + add.v[1];
	result.v[2] = v[2] + add.v[2];
	return result;
}

//---------------------------------------------------------//

template <class T>
vect3<T> vect3<T>::operator-(const vect3<T> add) const
{
	vect3<T> result;
	result.v[0] = v[0] - add.v[0];
	result.v[1] = v[1] - add.v[1];
	result.v[2] = v[2] - add.v[2];
	return result;
}

//---------------------------------------------------------//

template <class T>
vect3<T> vect3<T>::operator*(const T mult) const
{
	vect3<T> result;
	result.v[0] = v[0] * mult;
	result.v[1] = v[1] * mult;
	result.v[2] = v[2] * mult;
	return result;
}

//---------------------------------------------------------//

template <class T>
vect3<T> vect3<T>::operator/(const T div) const
{
	vect3<T> result;
	result.v[0] = v[0] / div;
	result.v[1] = v[1] / div;
	result.v[2] = v[2] / div;
	return result;
}

//---------------------------------------------------------//

template <class T>
T vect3<T>::operator*(const vect3<T> dot) const
{
	T result;
	result = v[0] * dot.v[0] + v[1] * dot.v[1] + v[2] * dot.v[2];
	return result;
}

//---------------------------------------------------------//

template <class T>
vect3<T> vect3<T>::operator%(const vect3<T> cross) const
{
	vect3<T> result;
	result.v[0] = v[1] * cross.v[2] - v[2] * cross.v[1];
	result.v[1] = v[2] * cross.v[0] - v[0] * cross.v[2];
	result.v[2] = v[0] * cross.v[1] - v[1] * cross.v[0];
	return result;
}

//---------------------------------------------------------//

template <class T>
vect3<T> vect3<T>::operator-() const
{
	vect3<T> result;
	result.v[0] = -v[0];
	result.v[1] = -v[1];
	result.v[2] = -v[2];
	return result;
}

//---------------------------------------------------------//

template <class T>
T vect3<T>::operator!() const
{
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

//---------------------------------------------------------//

template <class T>
vect3<T> vect3<T>::operator~() const
{
	vect3<T> result;
	T len = 1 / (double) sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	result.v[0] = v[0] * len;
	result.v[1] = v[1] * len;
	result.v[2] = v[2] * len;
	return result;
}

//**********************************************************//
//*********************vect4<T> functions*********************//
//**********************************************************//

template <class T>
vect4<T>::vect4(const vect4<T>& obj)
{
	v[0] = obj.v[0];
	v[1] = obj.v[1];
	v[2] = obj.v[2];
	v[3] = obj.v[3];
}

//---------------------------------------------------------//

template <class T>
void vect4<T>::add(const vect4<T> add)
{
	v[0] = v[0] + add.v[0];
	v[1] = v[1] + add.v[1];
	v[2] = v[2] + add.v[2];
	v[3] = v[3] + add.v[3];
}

//---------------------------------------------------------//

template <class T>
void vect4<T>::subtract(const vect4<T> sub)
{
	v[0] = v[0] - sub.v[0];
	v[1] = v[1] - sub.v[1];
	v[2] = v[2] - sub.v[2];
	v[3] = v[3] - sub.v[3];
}

//---------------------------------------------------------//

template <class T>
void vect4<T>::multiply(const T mult)
{
	v[0] = v[0] * mult;
	v[1] = v[1] * mult;
	v[2] = v[2] * mult;
	v[3] = v[3] * mult;
}

//---------------------------------------------------------//

template <class T>
void vect4<T>::divide(const T div)
{
	v[0] = v[0] / div;
	v[1] = v[1] / div;
	v[2] = v[2] / div;
	v[3] = v[3] / div;
}

//---------------------------------------------------------//

template <class T>
void vect4<T>::negative()
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
	v[3] = -v[3];
}

//=========================================================//

template <class T>
T &vect4<T>::operator[](const int i)
{
	return v[i];
}

//---------------------------------------------------------//

template <class T>
void vect4<T>::operator()(const T x, const T y, const T z, const T w)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
	v[3] = w;
}

//---------------------------------------------------------//

template <class T>
vect4<T> vect4<T>::operator+(const vect4<T> add) const
{
	vect4<T> result;
	result.v[0] = v[0] + add.v[0];
	result.v[1] = v[1] + add.v[1];
	result.v[2] = v[2] + add.v[2];
	result.v[3] = v[3] + add.v[3];
	return result;
}

//---------------------------------------------------------//

template <class T>
vect4<T> vect4<T>::operator-(const vect4<T> add) const
{
	vect4<T> result;
	result.v[0] = v[0] - add.v[0];
	result.v[1] = v[1] - add.v[1];
	result.v[2] = v[2] - add.v[2];
	result.v[3] = v[3] - add.v[3];
	return result;
}

//---------------------------------------------------------//

template <class T>
vect4<T> vect4<T>::operator*(const T mult) const
{
	vect4<T> result;
	result.v[0] = v[0] * mult;
	result.v[1] = v[1] * mult;
	result.v[2] = v[2] * mult;
	result.v[3] = v[3] * mult;
	return result;
}

//---------------------------------------------------------//

template <class T>
vect4<T> vect4<T>::operator/(const T div) const
{
	vect4<T> result;
	result.v[0] = v[0] / div;
	result.v[1] = v[1] / div;
	result.v[2] = v[2] / div;
	result.v[3] = v[3] / div;
	return result;
}

//---------------------------------------------------------//

template <class T>
vect4<T> vect4<T>::operator-() const
{
	vect4<T> result;
	result.v[0] = -v[0];
	result.v[1] = -v[1];
	result.v[2] = -v[2];
	result.v[3] = -v[3];
	return result;
}

#endif // __vect_h_
