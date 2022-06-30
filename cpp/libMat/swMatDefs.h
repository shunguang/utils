#ifndef SW_MAT_DEFS_H
#define SW_MAT_DEFS_H

#ifdef _WINDOWS
//------- win32 dll ------------
# if _USRDLL
#	ifdef SWMAT_EXPORTS
#		define SWMAT_EXPORT __declspec( dllexport )
#	else
#		define SWMAT_EXPORT __declspec( dllimport )
#	endif
# else
#	define SWMAT_EXPORT
# endif
//------ win32 static lib ------
#else
#	define SWMAT_EXPORT
#endif

#define SWF_MAT_EPS		(1e-7f)
#define SWF_MAT_REALMIN (1e-38f)
#define SWF_MAT_REALMAX (1e+38f)

#define SWD_MAT_EPS		(1e-15)
#define SWD_MAT_REALMIN (1e-308)
#define SWD_MAT_REALMAX (1e+308)


#define SW_MAT_HALF_PI       (1.57079632679490)
#define SW_MAT_PI            (3.14159265358979)
#define SW_MAT_ONEandHALF_PI (4.71238898038469)
#define SW_MAT_TWO_PI        (6.28318530717959)
#define SW_MAT_D2R           (0.01745329251994)
#define SW_MAT_R2D           (57.29577951308232)

#define SW_MAT_ROUND(x)		( (int) floor( (x) + 0.500 ) )
#define SW_MAT_NAN				( sqrt(-1.0) )
#define SW_MAT_ISNAN(x)		( (x) != (x) )

#if _DEBUG
	#define SW_MAT_BOUNDS_CHECK	1
#else
	#define SW_MAT_BOUNDS_CHECK	1
#endif

#define SW_MAT_MIN(a,b)  (((b) < (a)) ? (b) : (a));
#define SW_MAT_MAX(a,b)  (((b) < (a)) ? (a) : (b));
#endif
