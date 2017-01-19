// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __ROBOLIB_CORE__
#define __ROBOLIB_CORE__

#error MathCore algorithms no yet released, read note below
// Temporary removed implementation of algorithms
// File will be updated after filan release of library

#define MAX_QR_ALGORITHM_ITERATIONS 1000 // maksymalna liczba iteracji algorytmu QR

#include <math.h>
#include <limits.h>

namespace roboLib
{
const double pi = 3.1415926535897932;		// liczba pi
const double gravity = 9.8105;				// przyspieszenie ziemskie w Krakowie [m/s^2]
const double rhoAir = 1.168;				// gestosc powietrza w warukach normalnych [kg/m^3]
const double rEarth = 6371000;				// przyblizony promien Ziemi [m]
const double absoluteZeroCelsius = -273.15;	// temperatura zera bezwzglednego w skali Celsiusza
const double R = 29.2746;					// stala gazowa
const double tau = 6.5 / 1000.0;			// gradnient temperaturowy atmosfery [K/m]
const double normTemperature = 288.15;		// normal temperature for sea level [K]
const double normPressure = 1013.25;		// normal preassure for sea level [hPa]

template <typename _Tp> inline _Tp toRad(const _Tp deg); // przeksztalcenie kata ze stopni na radiany
template <typename _Tp> inline _Tp toDeg(const _Tp rad); // przeksztalcenie kata z radianow na stopnie

template <typename _Tp> inline _Tp toKelvin(const _Tp celsius); // przeksztalcenie stopni Celsiusza na Kelvina
template <typename _Tp> inline _Tp toCelcius(const _Tp kelvin); // przeksztalcenie stopni Kelvin na Celsiusza

template <typename _Tp> inline _Tp minmaxVal(const _Tp min, const _Tp max, const _Tp val); // ustalenie wartosci w przedziale [min,max]
template <typename _Tp> inline _Tp minmaxVal(const _Tp minmaxAbs, const _Tp val);

template <typename _Tp> unsigned short valToShort(const _Tp min, const _Tp max, const _Tp val); // konwersja z floata do unsigned shorta na zadanym zakresie [min,max]
template <typename _Tp> _Tp shortToVal(const _Tp min, const _Tp max, const unsigned short val); // konwersja z unsigned shortaf do loata na zadanym zakresie [min,max]
}


// ================== ZDEFINIOWANE KLASY ==================
template <class _Tp> class Vect2D; // wektor 2-elementowy (x,y)
template <class _Tp> class Vect3D; // wektor 3-elemntowy (x,y,z)
template <class _Tp> class Vect4D; // wektor 4-elementowy (kwaternion)
template <class _Tp> class Mat2D; // macierz 2x2 
template <class _Tp> class Mat3D; // macierz 3x3 (rotacji 3D)
template <class _Tp> class Mat4D; // macierz 4x4 

template <class _Tp> class Vector; // dynamicznie alokowany wektor dowolnego rozmiaru
template <class _Tp> class Matrix; // dynamicznie alokowana macierz kwadratowa dowolnego rozmiaru
// ========================================================

// konwencja katow RPY !!!
// roll (x), pitch (y), yaw (z) - 321


// ==================== PODSTAWOWE TYPY ===================
// Vect2D
typedef Vect2D <short> Vect2Ds;
typedef Vect2D <float> Vect2Df;
typedef Vect2D <double> Vect2Dd;
typedef Vect2D <long double> Vect2Ddd;
// Vect3D
typedef Vect3D <short> Vect3Ds;
typedef Vect3D <float> Vect3Df;
typedef Vect3D <double> Vect3Dd;
typedef Vect3D <long double> Vect3Ddd;
// Vect4D
typedef Vect4D <short> Vect4Ds;
typedef Vect4D <float> Vect4Df;
typedef Vect4D <double> Vect4Dd;
typedef Vect4D <long double> Vect4Ddd;
// Mat2D
typedef Mat2D <float> Mat2Df;
typedef Mat2D <double> Mat2Dd;
typedef Mat2D <long double> Mat2Ddd;
// Mat3D
typedef Mat3D <float> Mat3Df;
typedef Mat3D <double> Mat3Dd;
typedef Mat3D <long double> Mat3Ddd;
// Mat4D
typedef Mat4D <float> Mat4Df;
typedef Mat4D <double> Mat4Dd;
typedef Mat4D <long double> Mat4Ddd;

// Vector
typedef Vector <short> Vectors;
typedef Vector <float> Vectorf;
typedef Vector <double> Vectord;
typedef Vector <long double> Vectordd;
// Matrix
typedef Matrix <float> Matrixf;
typedef Matrix <double> Matrixd;
typedef Matrix <long double> Matrixdd;
// ========================================================


template <class _Tp> class Vect2D // wektor 2-elemntowy (x,y)
{
public:
	// wsporzedne
	// zastosowanie: wspolrzedne kartezjanskie [x; y], wspolrzedne geograficzne [lat; lon]
	_Tp x, y;

	// konstruktory
	inline Vect2D(void);
	inline Vect2D(_Tp _x, _Tp _y);
	explicit Vect2D(const _Tp* vectTab);
	template <typename _Type> inline Vect2D(const Vect2D <_Type> &v);

	Vect2D(const Vector<_Tp>& vect);

	// operatory dostepu (UWAGA! elementy numerowane w stylu Matlaba - od 1!)
	inline _Tp& operator () (const unsigned row);
	inline const _Tp& operator () (const unsigned row) const; // tylko odczyt

	inline unsigned size() const; // rozmiar wektora

	// operatory logiczne
	template <typename _Type> inline bool operator == (const Vect2D <_Type> &v) const; // rownosc
	template <typename _Type> inline bool operator != (const Vect2D <_Type> &v) const; // nierownosc


	// operatory arytmetyczne
	template <typename _Type> inline Vect2D <_Tp> operator + (const _Type a) const; // suma
	template <typename _Type> inline Vect2D <_Tp> operator + (const Vect2D <_Type> &v) const;

	template <typename _Type> inline Vect2D <_Tp> operator - (const _Type a) const; // rozniaca
	template <typename _Type> inline Vect2D <_Tp> operator - (const Vect2D <_Type> &v) const;

	template <typename _Type> inline Vect2D <_Tp> operator * (const _Type a) const; // monozenie
	template <typename _Type> inline Vect2D <_Tp> operator * (const Vect2D <_Type> &v) const;

	template <typename _Type> inline Vect2D <_Tp> operator / (const _Type a) const; // dzielenie
	template <typename _Type> inline Vect2D <_Tp> operator / (const Vect2D <_Type> &v) const;


	// operacje analityczne
	template <typename _Type> _Tp getDistance(const Vect2D <_Type> &v) const; // wyznaczenie odleglosci miedzy koncami wektorow w ukladzie kartezjanskim
	template <typename _Type> _Tp getGeoDistance(const Vect2D <_Type> &geoPoint) const; // wyznaczenie odleglosci miedzy koncami wektorow w ukladzie wspolrzednych geograficznych (po sferze ziemskiej)

	inline _Tp getNorm(void) const; // obliczanie normalnej wektora
	Vect2D <_Tp> getNormal(void) const; // obliczanie wektora normalnego
	Vect2D <_Tp> getSqrt(void) const; // wyznaczenie pierwiastka kwadratowego wektora

	template <typename _Type> Vect2D <_Tp> toCartesian(const Vect2D<_Type>& origin) const;
	template <typename _Type> Vect2D <_Tp> toGeographic(const Vect2D<_Type>& origin) const;
};

template <class _Tp> class Vect3D // wektor 3-elemntowy (x,y,z)
{
public:
	// wsporzedne
	_Tp x, y, z;

	// konstruktory
	inline Vect3D(void);
	inline Vect3D(_Tp _x, _Tp _y, _Tp _z);
	explicit Vect3D(const _Tp* vectTab);
	template <typename _Type> inline Vect3D(const Vect3D <_Type> &v);
	template <typename _Type> inline Vect3D(const Vect2D <_Tp> &v, const _Type z);

	Vect3D(const Vector<_Tp>& vect);

	// operatory dostepu (UWAGA! elementy numerowane w stylu Matlaba - od 1!)
	inline _Tp& operator () (const unsigned row);
	inline const _Tp& operator () (const unsigned row) const; // tylko odczyt

	Vect2D<_Tp> getVect2D(void) const; // wektora 2D z wspolrzednych x i y wektora 3D

	inline unsigned size() const; // rozmiar wektora


	// operatory logiczne
	template <typename _Type> inline bool operator == (const Vect3D <_Type> &v) const; // rownosc
	template <typename _Type> inline bool operator != (const Vect3D <_Type> &v) const; // nierownosc


	// operatory arytmetyczne
	template <typename _Type> inline Vect3D <_Tp> operator + (const _Type a) const; // suma
	template <typename _Type> inline Vect3D <_Tp> operator + (const Vect3D <_Type> &v) const;

	template <typename _Type> inline Vect3D <_Tp> operator - (const _Type a) const; // rozniaca
	template <typename _Type> inline Vect3D <_Tp> operator - (const Vect3D <_Type> &v) const;

	template <typename _Type> inline Vect3D <_Tp> operator * (const _Type a) const; // monozenie
	template <typename _Type> inline Vect3D <_Tp> operator * (const Vect3D <_Type> &v) const;

	template <typename _Type> inline Vect3D <_Tp> operator / (const _Type a) const; // dzielenie
	template <typename _Type> inline Vect3D <_Tp> operator / (const Vect3D <_Type> &v) const;


	// operacje analityczne
	const _Tp& max(void) const; // maksymalna wartosc wektora
	const _Tp& min(void) const; // minimalna wartosc wektora
	const _Tp& max(unsigned& row) const; // maksymalna wartosc wektora dla odpowiedniego wiersza
	const _Tp& min(unsigned& row) const; // minimalna wartosc wektora dla odpowiedniego wiersza

	template <typename _Type> inline _Tp dist(const Vect3D <_Type> &v) const; // odleglosci pomiedzy koncami wektorow

	inline _Tp getNorm(void) const; // obliczanie normalnej wektora
	Vect3D <_Tp> getNormal(void) const; // obliczanie wektora normalnego
	Vect3D <_Tp> getSqrt(void) const; // wyznaczenie pierwiastka kwadratowego wektora

	template <typename _Type> Vect3D <_Tp> toCartesian(const Vect3D<_Type>& origin) const;
	template <typename _Type> Vect3D <_Tp> toGeographic(const Vect3D<_Type>& origin) const;

	template <typename _Type> _Tp getDot(const Vect3D <_Type> &v) const; // iloczyn skalarny
	template <typename _Type> Vect3D <_Tp> getCross(const Vect3D <_Type> &v) const; // iloczyn wektorowy
	template <typename _Type> Vect3D <_Tp> getProj(const Vect3D <_Type>& v) const; // projekcja wektora na wektor v

	Mat3D <_Tp> getCrossProductMatrix(void) const; // macierz krzyzowa wektora

	Vect4D <_Tp> getQuaternion(void) const; // obliczanie kawaternionu
	Mat3D <_Tp> getRotationMatrix(void) const; // obliczanie macierzy kosinusow kierunkowych

	Vect3D <_Tp> getRotedX(const _Tp theta) const; // obliczenie zrotowanego wektora wzgledem osi X o kat theta
	Vect3D <_Tp> getRotedY(const _Tp theta) const; // obliczenie zrotowanego wektora wzgledem osi Y o kat theta
	Vect3D <_Tp> getRotedZ(const _Tp theta) const; // obliczenie zrotowanego wektora wzgledem osi Z o kat theta

	template <typename _Type> Vect3D <_Tp> getRoted(const Vect3D <_Type> &rpy) const; // obliczenie zrotowanego wektora o wektor katow Eulera
	template <typename _Type> Vect3D <_Tp> getRoted(const Vect4D <_Type> &quat) const; // obliczenie zrotowanego wektora o kwaternion
	template <typename _Type> Vect3D <_Tp> getRoted(const Mat3D <_Type> &dcm) const; // obliczenie zrotowanego wektora o macierz kosinusow kierunkowych

	template <typename _Type> Vect3D <_Tp> getRotedTrans(const Vect3D <_Type> &rpy) const; // obliczenie zrotowanego wektora o odwrotny wektor katow Eulera
	template <typename _Type> Vect3D <_Tp> getRotedTrans(const Vect4D <_Type> &quat) const; // obliczenie zrotowanego wektora o odwrotny kwaternion
	template <typename _Type> Vect3D <_Tp> getRotedTrans(const Mat3D <_Type> &dcm) const; // obliczenie zrotowanego wektora o odwrotna macierz kosinusow kierunkowych

	// data validation
	bool isNormal(void) const; // check if data is correct 

	// metody statyczne
	static Vect3D <_Tp> ones(void); // wektor jedynkowy
	static Vect3D <_Tp> zeros(void); // wektor zerowy
	static Vect3D <_Tp> gravity(void); // wektor przyspieszenia ziemskiego
	static Vect3D <_Tp> toDeg(const Vect3D<_Tp> &rpyRad); // przeliczenia na stopnie
	static Vect3D <_Tp> toRad(const Vect3D<_Tp> &rpyDeg); // przeliczenie na radiany

	template <typename _Type> static Vect3D <_Tp> eulerFromDCM(const Mat3D<_Type> &R); // konstrukcja wektora katow rpy z macierzy kosinusow kierunkowych
	template <typename _Type> static Vect3D <_Tp> eulerFromQuat(const Vect4D<_Type> &q); // konstrukcja wektora katow rpy z kwaternionu
};

template <class _Tp> class Vect4D // wektor 4 - elementowy(kwaternion)
{
public:
	_Tp a, b, c, d; // skladowe wektora

	// konstruktory
	inline Vect4D(void);
	inline Vect4D(_Tp _a, _Tp _b, _Tp _c, _Tp _d);
	explicit Vect4D(const _Tp* vectTab);
	template <typename _Type> inline Vect4D(const Vect4D <_Type> &v);

	// operatory dostepu (UWAGA! elementy numerowane w stylu Matlaba - od 1!)
	inline _Tp& operator () (const unsigned row);
	inline const _Tp& operator () (const unsigned row) const; // tylko odczyt

	inline unsigned size() const; // rozmiar wektora


	// operatory logiczne
	template <typename _Type> inline bool operator == (const Vect4D <_Type> &v) const; // rownosc
	template <typename _Type> inline bool operator != (const Vect4D <_Type> &v) const; // nierownosc


	// operatory arytmetyczne
	template <typename _Type> inline Vect4D <_Tp> operator + (const _Type w) const; // suma
	template <typename _Type> inline Vect4D <_Tp> operator + (const Vect4D <_Type> &v) const;

	template <typename _Type> inline Vect4D <_Tp> operator - (const _Type w) const; // rozniaca
	template <typename _Type> inline Vect4D <_Tp> operator - (const Vect4D <_Type> &v) const;

	template <typename _Type> inline Vect4D <_Tp> operator * (const _Type w) const; // monozenie
	template <typename _Type> inline Vect4D <_Tp> operator * (const Vect4D <_Type> &v) const;

	template <typename _Type> inline Vect4D <_Tp> operator / (const _Type w) const; // dzielenie
	template <typename _Type> inline Vect4D <_Tp> operator / (const Vect4D <_Type> &v) const;


	// operacje analityczne
	const _Tp& max(void) const; // maksymalna wartosc wektora
	const _Tp& min(void) const; // minimalna wartosc wektora
	const _Tp& max(unsigned& row) const; // maksymalna wartosc wektora dla odpowiedniego wiersza
	const _Tp& min(unsigned& row) const; // minimalna wartosc wektora dla odpowiedniego wiersza

	template <typename _Type> _Tp inline dist(Vect4D <_Type>& v) const; // odleglosci pomiedzy koncami wektorow

	inline _Tp getNorm(void) const; // obliczanie normalnej wektora
	Vect4D <_Tp> getNormal(void) const; // obliczanie wektora normalnego
	Vect4D <_Tp> getSqrt(void) const; // wyznaczenie pierwiastka kwadratowego wektora

	template <typename _Type> _Tp getDot(const Vect3D <_Type>& v) const; // iloczyn skalarny
	template <typename _Type> Vect4D <_Tp> getCross(const Vect4D <_Type>& q) const; // iloczyn wektorowy kwaternionu
	template <typename _Type> Vect4D <_Tp> getProj(const Vect4D <_Type>& v) const; // projekcja wektora na wektor v

	Mat4D <_Tp> getCrossProductMatrix(void) const; // macierz krzyzowa wektora

	Vect3D <_Tp> getEulerAngles(void) const; // obliczanie katow rotacji Eulera
	Mat3D <_Tp> getDcm(void) const; // obliczanie macierzy kosinusow kierunkowych

	Mat3D <_Tp> getRotedX(const _Tp theta) const; // obliczenie zrotowanego kwaternionu wzgledem osi X o kat theta
	Mat3D <_Tp> getRotedY(const _Tp theta) const; // obliczenie zrotowanego kwaternionu wzgledem osi Y o kat theta
	Mat3D <_Tp> getRotedZ(const _Tp theta) const; // obliczenie zrotowanego kwaternionu wzgledem osi Z o kat theta

	template <typename _Type> Vect4D <_Tp> getRoted(const Vect3D <_Type> &rpy) const; // obliczenie zrotowanego kwaternionu o wektor katow Eulera
	template <typename _Type> Vect4D <_Tp> getRoted(const Vect4D <_Type> &quat) const; // obliczenie zrotowanego kwaternionu o kwaternion
	template <typename _Type> Vect4D <_Tp> getRoted(const Mat3D <_Type> &dcm) const; // obliczenie zrotowanego kwaternionu o macierz kosinusow kierunkowych

	template <typename _Type> Vect4D <_Tp> getRotedTrans(const Vect3D <_Type> &rpy) const; // obliczenie zrotowanego kwaternionu o odwrotny wektor katow Eulera
	template <typename _Type> Vect4D <_Tp> getRotedTrans(const Vect4D <_Type> &quat) const; // obliczenie zrotowanego kwaternionu o odwrotny kwaternion
	template <typename _Type> Vect4D <_Tp> getRotedTrans(const Mat3D <_Type> &dcm) const; // obliczenie zrotowanego kwaternionu o odwrotna macierz kosinusow kierunkowych


	// metody statyczne
	static Vect4D <_Tp> ones(void); // wektor jedynkowy
	static Vect4D <_Tp> zeros(void); // wektor zerowy

	template <typename _Type> static Vect4D <_Tp> quatFromDCM(const Mat3D<_Type> &R);
	template <typename _Type> static Vect4D <_Tp> quatFromEuler(const Vect3D<_Type> &rpy);
};


template <class _Tp> class Mat2D // macierz 2x2 (rotacji 1d)
{
public:
	_Tp mat[4];

	// konstruktory
	inline Mat2D(void); // defoultowy (macierz jednostkowa)
	inline Mat2D(const _Tp m0, const _Tp m1, const _Tp m2, const _Tp m3);
	template <typename _Type> inline Mat2D(const Mat2D <_Type> &m);


	// operatory dostepu (UWAGA! elementy numerowane w stylu Matlaba - od 1!)
	inline _Tp& operator () (const unsigned row, const unsigned column);
	inline const _Tp& operator () (const unsigned row, const unsigned column) const; // tylko odczyt

	inline Vect2D <_Tp> col(const unsigned column) const;
	inline Vect2D <_Tp> row(const unsigned row) const;
	inline Vect2D <_Tp> diag(void) const;

	inline unsigned size() const; // rozmiar wektora

	const _Tp* begin(void) const; // wskaznik na poczatek tablicy macierzy
	const _Tp* end(void) const; // wskaznik na koniec tablicy macierzy


	// operatory logiczne
	template <typename _Type> inline bool operator == (const Mat2D <_Type> &m) const; // rownosc
	template <typename _Type> inline bool operator != (const Mat2D <_Type> &m) const; // nierownosc


	// operatory arytmetyczne
	template <typename _Type> Mat2D <_Tp> operator * (const _Type a) const; // mnozenie razy stala
	template <typename _Type> Vect2D <_Tp> operator * (const Vect2D <_Type> &v) const; // mnozenie razy wektor
	template <typename _Type> Mat2D <_Tp> operator * (const Mat2D <_Type> &m) const; // mnozenie razy macierz

	template <typename _Type> Vect2D <_Tp> transMul(const Vect2D <_Type> &v) const; // mnozenie transponowanej macierzy razy wektor
	template <typename _Type> Mat2D <_Tp> transMul(const Mat2D <_Type> &m) const; // mnozenie transponowanej macierzy razy macierz

	// operacje analityczne
	_Tp getDet(void) const; // wyznacznik macierzy
	_Tp getTrace(void) const; // slad macierzy
};

template <class _Tp> class Mat3D // macierz 3x3 (rotacji 3d)
{
public:
	_Tp mat[9]; // macierz 3x3 (9-cio elementowa) 

	// konstruktory
	inline Mat3D(void); // defoultowy (macierz jednostkowa)
	inline Mat3D(const _Tp m0, const _Tp m1, const _Tp m2, const _Tp m3, const _Tp m4, const _Tp m5, const _Tp m6, const _Tp m7, const _Tp m8);
	template <typename _Type> inline Mat3D(const Mat3D <_Type> &m);

	Mat3D(const Matrix<_Tp>& mat);


	// operatory dostepu (UWAGA! elementy numerowane w stylu Matlaba - od 1!)
	inline _Tp& operator () (const unsigned row, const unsigned column);
	inline const _Tp& operator () (const unsigned row, const unsigned column) const; // tylko odczyt

	inline Vect3D <_Tp> col(const unsigned column) const;
	inline Vect3D <_Tp> row(const unsigned row) const;
	inline Vect3D <_Tp> diag(void) const;

	inline unsigned size() const; // rozmiar wektora

	const _Tp* begin(void) const; // wskaznik na poczatek tablicy macierzy
	const _Tp* end(void) const; // wskaznik na koniec tablicy macierzy


	// operatory logiczne
	template <typename _Type> inline bool operator == (const Mat3D <_Type> &m) const; // rownosc
	template <typename _Type> inline bool operator != (const Mat3D <_Type> &m) const; // nierownosc


	// operatory arytmetyczne
	template <typename _Type> Mat3D <_Tp> operator + (const _Type a) const; // dodawanie stalej
	template <typename _Type> Mat3D <_Tp> operator + (const Mat3D <_Type> &m) const; // dodawanie macierzy

	template <typename _Type> Mat3D <_Tp> operator - (const _Type a) const; // odejmowanie stalej
	template <typename _Type> Mat3D <_Tp> operator - (const Mat3D <_Type> &m) const; // odejmowanie macierzy

	template <typename _Type> Mat3D <_Tp> operator * (const _Type a) const; // mnozenie razy stala
	template <typename _Type> Vect3D <_Tp> operator * (const Vect3D <_Type> &v) const; // mnozenie razy wektor
	template <typename _Type> Mat3D <_Tp> operator * (const Mat3D <_Type> &m) const; // mnozenie razy macierz

	template <typename _Type> Vect3D <_Tp> transMul(const Vect3D <_Type> &v) const; // mnozenie transponowanej macierzy razy wektor
	template <typename _Type> Mat3D <_Tp> transMul(const Mat3D <_Type> &m) const; // mnozenie transponowanej macierzy razy macierz

	template <typename _Type> inline Mat3D <_Tp> operator / (const _Type a) const; // dzielenie


	// operacje analityczne
	_Tp getDet(void) const; // wyznacznik macierzy
	_Tp getTrace(void) const; // slad macierzy

	Mat3D <_Tp> getTrans(void) const; // transpozycja macierzy
	Mat3D <_Tp> getInv(void) const; // odwracanie macierzy
	Mat3D <_Tp> getNormal(void) const; // normalizacja macierzy
	Mat3D <_Tp> getQR(Mat3D<_Tp>& R) const; // dekompozycja QR
	Mat3D <_Tp> getEigens(Vect3D <_Tp>& eValues) const; // wyznaczenie wektorow i wartosci wlasnych macierzy
	Mat3D <_Tp> getSqrt(void) const; // wyznaczenie pierwiastka kwadratowego macierzy

	Mat3D <_Tp> getRotedX(const _Tp theta) const; // obliczenie zrotowanej macierzy wzgledem osi X o kat theta
	Mat3D <_Tp> getRotedY(const _Tp theta) const; // obliczenie zrotowanej macierzy wzgledem osi Y o kat theta
	Mat3D <_Tp> getRotedZ(const _Tp theta) const; // obliczenie zrotowanej macierzy wzgledem osi Z o kat theta

	template <typename _Type> Mat3D <_Tp> getRoted(const Vect3D <_Type> &rpy) const; // obliczenie zrotowanej macierzy o wektor katow Eulera
	template <typename _Type> Mat3D <_Tp> getRoted(const Vect4D <_Type> &quat) const; // obliczenie zrotowanej macierzy o kwaternion
	template <typename _Type> Mat3D <_Tp> getRoted(const Mat3D <_Type> &dcm) const; // obliczenie zrotowanej macierzy o macierz kosinusow kierunkowych

	template <typename _Type> Mat3D <_Tp> getRotedTrans(const Vect3D <_Type> &rpy) const; // obliczenie zrotowanej macierzy o odwrotny wektor katow Eulera
	template <typename _Type> Mat3D <_Tp> getRotedTrans(const Vect4D <_Type> &quat) const; // obliczenie zrotowanej macierzy o odwrotny kwaternion
	template <typename _Type> Mat3D <_Tp> getRotedTrans(const Mat3D <_Type> &dcm) const; // obliczenie zrotowanej macierzy o odwrotna macierz kosinusow kierunkowych

	Vect3D <_Tp> getEulerAngles(void) const; // obliczenie wektora katow PRY 
	Vect4D <_Tp> getQuaternion(void) const; // obliczenie kwaternionu
	Mat2D <_Tp> getFlatDcm(void) const; // obliczanie macierzy rotacji dla ukladu plaskiego (osie XY, tylko yaw)

	// data validation
	bool isNormal(void) const; // check if data is correct

	// metody statyczne
	static Mat3D <_Tp> eye(void); // macierz jednostkowa
	static Mat3D <_Tp> ones(void); // macierz jedynkowa
	static Mat3D <_Tp> zeros(void); // macierz zerowa
	template <typename _Type> static Mat3D <_Tp> diag(const _Type c); // macierz diagonalna ze stalej
	template <typename _Type> static Mat3D <_Tp> diag(const Vect3D<_Type>& v); // macierz diagonalna z wekora
	template <typename _Type> static Mat3D <_Tp> vectAsRows(const Vect3D <_Type> &v1, const Vect3D <_Type> &v2, const Vect3D <_Type> &v3); // macierz z wektorow na wierszach
	template <typename _Type> static Mat3D <_Tp> vectAsCols(const Vect3D <_Type> &v1, const Vect3D <_Type> &v2, const Vect3D <_Type> &v3); // macierz z wektorow na kolumnach

	template <typename _Type> static Mat3D <_Tp> DCMFromQuat(const Mat4D<_Type> &q); // macierz kosinusow kierunkowych z kwaternionu
	template <typename _Type> static Mat3D <_Tp> DCMFromEuler(const Vect3D<_Type> &rpy); // macierz kosinusow kierunkowych z katow eulera
};

template <class _Tp> class Mat4D // macierz 4x4
{
public:
	_Tp mat[16]; // elementy macierzy

	// konstruktory
	inline Mat4D(void); // defoultowy (macierz jednostkowa)
	inline Mat4D(const _Tp m0, const _Tp m1, const _Tp m2, const _Tp m3, const _Tp m4, const _Tp m5, const _Tp m6, const _Tp m7, const _Tp m8, const _Tp m9, const _Tp m10, const _Tp m11, const _Tp m12, const _Tp m13, const _Tp m14, const _Tp m15);
	template <typename _Type> inline Mat4D(const Mat4D <_Type> &m);

	Mat4D(const Matrix<_Tp>& mat);

	// operatory dostepu (UWAGA! elementy numerowane w stylu Matlaba - od 1!)
	inline _Tp& operator () (const unsigned row, const unsigned column);
	inline const _Tp& operator () (const unsigned row, const unsigned column) const; // tylko odczyt

	inline Vect4D <_Tp> col(const unsigned column) const;
	inline Vect4D <_Tp> row(const unsigned row) const;
	inline Vect4D <_Tp> diag(void) const;

	inline unsigned size() const; // rozmiar wektora

	const _Tp* begin(void) const; // wskaznik na poczatek tablicy macierzy
	const _Tp* end(void) const; // wskaznik na koniec tablicy macierzy


	// operatory logiczne
	template <typename _Type> inline bool operator == (const Mat4D <_Type> &m) const; // rownosc
	template <typename _Type> inline bool operator != (const Mat4D <_Type> &m) const; // nierownosc


	// operatory arytmetyczne
	template <typename _Type> Mat4D <_Tp> operator + (const _Type a) const; // dodawanie stalej
	template <typename _Type> Mat4D <_Tp> operator + (const Mat4D <_Type> &m) const; // dodawanie macierzy

	template <typename _Type> Mat4D <_Tp> operator - (const _Type a) const; // odejmowanie stalej
	template <typename _Type> Mat4D <_Tp> operator - (const Mat4D <_Type> &m) const; // odejmowanie macierzy

	template <typename _Type> Mat4D <_Tp> operator * (const _Type a) const; // mnozenie razy stala
	template <typename _Type> Vect4D <_Tp> operator * (const Vect4D <_Type> &v) const; // mnozenie razy wektor
	template <typename _Type> Mat4D <_Tp> operator * (const Mat4D <_Type> &m) const; // mnozenie razy macierz

	template <typename _Type> Vect4D <_Tp> transMul(const Vect4D <_Type> &v) const; // mnozenie transponowanej macierzy razy wektor
	template <typename _Type> Mat4D <_Tp> transMul(const Mat4D <_Type> &m) const; // mnozenie transponowanej macierzy razy macierz

	template <typename _Type> inline Mat4D <_Tp> operator / (const _Type a) const; // dzielenie


	// operacje analityczne
	_Tp getDet(void) const; // wyznacznik macierzy
	_Tp getTrace(void) const;// slad macierzy

	Mat4D <_Tp> getTrans(void) const; // transpozycja macierzy
	Mat4D <_Tp> getInv(void) const; // odwracanie macierzy
	Mat4D <_Tp> getQR(Mat4D<_Tp>& R) const; // dekompozycja QR
	Mat4D <_Tp> getEigens(Vect4D <_Tp>& eValues) const; // wyznaczenie wektorow i wartosci wlasnych macierzy
	Mat4D <_Tp> getSqrt(void) const; // wyznaczenie pierwiastka kwadratowego macierzy


	// metody statyczne
	static Mat4D <_Tp> eye(void); // macierz jednostkowa
	static Mat4D <_Tp> ones(void); // macierz jedynkowa
	static Mat4D <_Tp> zeros(void); // macierz zerowa
	template <typename _Type> static Mat4D <_Tp> diag(const _Type c); // macierz diagonalna ze stalej
	template <typename _Type> static Mat4D <_Tp> diag(const Vect4D<_Type>& v); // macierz diagonalna z wekora
	template <typename _Type> static Mat4D <_Tp> vectAsRows(const Vect4D <_Type> &v1, const Vect4D <_Type> &v2, const Vect4D <_Type> &v3, const Vect4D <_Type> &v4); // macierz z wektorow na wierszach
	template <typename _Type> static Mat4D <_Tp> vectAsCols(const Vect4D <_Type> &v1, const Vect4D <_Type> &v2, const Vect4D <_Type> &v3, const Vect4D <_Type> &v4); // macierz z wektorow na kolumnach
};


template <class _Tp> class Vector
{
	unsigned dataSize;
	_Tp* data;

public:
	Vector(const Vector<_Tp>& v);
	Vector(const unsigned _dataSize);

	Vector(const Vect2D<_Tp>& v);
	Vector(const Vect3D<_Tp>& v);
	Vector(const Vect4D<_Tp>& v);

	~Vector(void);

	// operatory dostepu (UWAGA! elementy numerowane w stylu Matlaba - od 1!)
	_Tp& operator () (const unsigned row);
	_Tp operator () (const unsigned row) const; // tylko odczyt

	unsigned size() const;

	void copyVector(const Vector&  source);

	// operatory arytmetyczne
	template <typename _Type> void operator = (const Vector <_Type>& v) const; // przyrownanie

	template <typename _Type> Vector <_Tp> operator + (const _Type a) const; // dodawanie stalej
	template <typename _Type> Vector <_Tp> operator + (const Vector <_Type>& v) const; // dodawanie wektora


	// metody anlityczne
	void mul(const Matrix<_Tp>& left, const Vector<_Tp>& right);


	// metody stateczne
	static Vector<_Tp> zeros(const unsigned dataSize); // wektor zerowy
	static Vector<_Tp> ones(const unsigned dataSize); // wektor jedynkowy
};

template <class _Tp> class Matrix
{
	unsigned dataSize;
	_Tp* data;

public:
	Matrix(const Matrix<_Tp>& m);
	Matrix(const unsigned _dataSize);

	Matrix(const Mat2D<_Tp>& m);
	Matrix(const Mat3D<_Tp>& m);
	Matrix(const Mat4D<_Tp>& m);

	~Matrix(void);

	// operatory dostepu (UWAGA! elementy numerowane w stylu Matlaba - od 1!)
	_Tp& operator () (const unsigned row, const unsigned column);
	_Tp operator () (const unsigned row, const unsigned column) const;

	unsigned size() const;

	void copyMatrix(const Matrix&  source);

	// metody analityczne
	void mul(const Matrix& left, const Matrix& right);

	void invert();

	// metody statyczne
	static Matrix <_Tp> eye(const unsigned dataSize); // macierz jednostkowa
	static Matrix <_Tp> ones(const unsigned dataSize); // macierz jedynkowa
	static Matrix <_Tp> zeros(const unsigned dataSize); // macierz zerowa
	template <typename _Type> static Matrix <_Tp> diag(const _Type c, const unsigned dataSize); // macierz diagonalna ze stalej
	template <typename _Type> static Matrix <_Tp> diag(const Vector<_Type>& v); // macierz diagonalna z wekora
};

#endif // __ROBOLIB_CORE__
