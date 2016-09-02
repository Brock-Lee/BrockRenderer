#pragma once

//! Row-column 4x4 Matrix.
/*!
  * (a00, a01, a02, a03)
  * (a10, a11, a12, a13)
  * (a20, a21, a22, a23)
  * (a30, a31, a32, a33)
  */
template <typename T>
class _TMat4
{
public:
    //! x[row][col].
    T m[4][4];

    //! Default ctor.
    /*! Default identity matrix. */
    _TMat4();
    //! Array ctor.
    _TMat4(const T* a);
    //! Component ctor.
    _TMat4(T a00, T a01, T a02, T a03,
        T a10, T a11, T a12, T a13,
        T a20, T a21, T a22, T a23,
        T a30, T a31, T a32, T a33);
	T* operator []( int iIdx)
	{
		return m[iIdx];
	}
	_TMat4(const _TVector3<T>& x, const _TVector3<T> &y, const _TVector3<T> &z, const _TVector3<T>&pos);
    void Scale(T s);
	void Zero();
	void Identity();
	static _TMat4<T> GetRotateX(T rad);
	static _TMat4<T> GetRotateY(T rad);
	static _TMat4<T> GetRotateZ(T rad);
    _TMat4<T> operator*  (const _TMat4 &a);
    friend _TVector3<T> operator* (const _TVector3<T> &v);
	friend _TVector4<T> operator* (const _TVector4<T> &v);
};



template <typename T>
_TMat4<T> _TMat4<T>::GetRotateZ( T angle )
{
	_TMat4<T> M;

	float cosinus = cos(angle);
	float sinus = sin(angle);

	M.m[0][0] = cosinus;
	M.m[0][1] = sinus;
	M.m[1][0] = -sinus;
	M.m[1][1] = cosinus;

	return M;
}

template <typename T>
_TMat4<T> _TMat4<T>::GetRotateY( T angle )
{
	_TMat4<T> M;

	float cosinus = cos(angle);
	float sinus =  sin(angle);

	M.m[0][0] = cosinus;
	M.m[0][2] = -sinus;
	M.m[2][0] = sinus;
	M.m[2][2] = cosinus;

	return M;
}

template <typename T>
_TMat4<T> _TMat4<T>::GetRotateX( T angle )
{
	_TMat4<T> M;
	T cosinus = cos(angle);
	T sinus = sin(angle);

	M.m[1][1] = cosinus;
	M.m[1][2] = sinus;
	M.m[2][1] = -sinus;
	M.m[2][2] = cosinus;

	return M;
}



template <typename T>
_TMat4<T>::_TMat4()
{
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			m[i][j] =  ((i==j)?1:0);
}

template <typename T>
_TMat4<T>::_TMat4( T a00, T a01, T a02, T a03, T a10, T a11, T a12, T a13, T a20, T a21, T a22, T a23, T a30, T a31, T a32, T a33 )
{
	m[0][0] = a00;m[0][1] = a01;m[0][2] = a02;m[0][3] = a03;
	m[1][0] = a10;m[1][1] = a11;m[1][2] = a12;m[1][3] = a13;
	m[2][0] = a20;m[2][1] = a21;m[2][2] = a22;m[2][3] = a23;
	m[3][0] = a30;m[3][1] = a31;m[3][2] = a32;m[3][3] = a33;
}

template <typename T>
_TMat4<T>::_TMat4( const T* a )
{
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			m[i][j] =  a[i*4 +j];
}



template <typename T>
_TMat4<T>::_TMat4( const _TVector3<T>& x, const _TVector3<T> &y, const _TVector3<T> &z, const _TVector3<T>&pos )
{
	m[0][0] = x.x;m[0][1] = y.x;m[0][2] = z.x;m[0][3] = 0;
	m[1][0] = x.y;m[1][1] = y.y;m[1][2] = z.y;m[1][3] = 0;
	m[2][0] = x.z;m[2][1] = y.z;m[2][2] = z.z;m[2][3] = 0;
	m[3][0] = -x.Dot(pos);m[3][1] = -y.Dot(pos);m[3][2] = -z.Dot(pos);m[3][3] = 1;
}



template <typename T>
_TMat4<T> _TMat4<T>::operator*( const _TMat4 &b )
{
	_TMat4<T> res;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			res.m[i][j] = m[i][0] * b.m[0][j] +
				m[i][1] * b.m[1][j] +
				m[i][2] * b.m[2][j] +
				m[i][3] * b.m[3][j];
		}
	}

	return res;
}

template <typename T>
void _TMat4<T>::Scale( T s )
{
	for(int i=0; i<3;i++)
		m[i][i] *= s;
}


template <typename T>
_TVector4<T> operator*( const _TVector4<T> &v , const _TMat4<T> & m)
{
	_TVector4<T> res;  // result 4-vector

	res[0] = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
	res[1] = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
	res[2] = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
	res[3] = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];

	return res;
}

template <typename T>
_TVector3<T> operator*( const _TVector3<T> &b , const _TMat4<T> & m)
{
	T res[4];  // result 4-vector

	res[0] = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + /*1.0 * */m.m[3][0];
	res[1] = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + /*1.0 * */m.m[3][1];
	res[2] = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + /*1.0 * */m.m[3][2];
	res[3] = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + /*1.0 * */m.m[3][3];

	if (fabs(res[3] - 1.0) < 0.00000001/*std::numeric_limits<float>::epsilon()*/)   // equals to 1.0
	{
		return _TVector3<T>(res[0], res[1], res[2]);
	}
	else
	{
		return _TVector3<T>(res[0] / res[3], res[1] / res[3], res[2] / res[3]);
	}
}


template <typename T>
void _TMat4<T>::Zero()
{
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			m[i][j] = 0 ;
}

template <typename T>
void _TMat4<T>::Identity()
{
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			m[i][j] =  ((i==j)?1:0);
}

typedef _TMat4<float> mat4;
typedef _TMat4<double> mat4d;