/**
 * @file   Roi.h
 * @author Shunguang Wu
 * @date   02/08/2017
 * @brief: float point roi 
 * 
*------------------------------------------------------------------------------
*/
#ifndef _ROI_H_
#define _ROI_H_

#include "DataTypes.h"
#include "UtilDefs.h"

//
//(xo,yo): upper-left corner of the roi,
//(w, h): width and height of the roi.
//(xb,yb): bottom-right corner of the roi, with xb = xo + w - 1 and yb = yo + h - 1.
//

namespace pnt {
class UTIL_EXPORT Roi{
	public:
		float m_xo, m_yo, m_w, m_h, m_xb, m_yb;  

		inline static int myround(const float &x)
		{
			return (int)((x > 0.0) ? floor(x + 0.5) : ceil(x - 0.5));
		}

	public:
		Roi();
		Roi( const int xo, const int yo, const int w, const int h );
		Roi( const float xo, const float yo, const float w, const float h  );
		Roi( const Roi &p );

		Roi& operator =( const Roi &p);

		float	getCenterX() const	{ return  0.5f*(m_xo + m_xb); }
		float	getCenterY() const	{ return  0.5f*(m_yo + m_yb); }
		float   getSize() const		{ return  m_w * m_h; }
		void    getIntParams(int &xo, int &yo, int &xb, int &yb, int &w, int &h, const int imgW=-1, const int imgH=-1) const;
		Roi     getRoi(const int L) const;

		int		getInt_xc() const   { return  myround( 0.5f*(m_xo + m_xb ) ); }
		int		getInt_yc() const   { return  myround( 0.5f*(m_yo + m_yb ) ); }
		int		getInt_xo() const   { return  myround( m_xo ); }
		int		getInt_yo() const   { return  myround( m_yo ); }
		int		getInt_xb() const   { return  myround( m_xb ); }
		int		getInt_yb() const	{ return  myround( m_yb ); }
		int		getInt_w() const	{ return  myround( m_w  ); }
		int		getInt_h() const	{ return  myround( m_h  ); }

		bool	invalid() const						{ return ( m_w<=0 || m_h<=0 ); }
		bool    sameSize(int w, int h) const		{ return ( w == getInt_w() && h == getInt_h() ); }
		bool	sameCenter(int xc, int yc) const	{ return (xc == getInt_xc() && yc == getInt_yc()); }

		void	setRoi( const int &xo, const int &yo, const int &w, const int &h );
		void	setRoi( const float &xo, const float &yo, const float &w, const float &h  );
		void    setByCenter( const float &xc, const float &yc, const float &w, const float &h);
		void	setByIntCenter( const int &xc, const int &yc, const int &w, const int &h);

		void	print ( const char *str = NULL ) const;

		//force roi inside the image via change the roi size
		bool	trimSize( const int imgW, const int imgH );
		//force roi inside the image via change the roi location, may change roi size
		bool	trimLocation( const int imgW, const int imgH );
		//force roi inside the image via change the roi location, w/o  change roi size
		bool	trimLocation_wo_chg_sz( const int imgW, const int imgH );
		
		void	oneLevelUp();
		void	oneLevelDown();
		void    oneLevelUpOrDown( const int &step, const float &newW, const float &newH );
		void    forceSizeOdd();
		void    forceSizeOdd2();

		void	move2L( const int &step ) { m_xo -=step; m_xb -= step; }
		void	move2R( const int &step ) { m_xo +=step; m_xb += step; }
		void	move2U( const int &step ) { m_yo -=step; m_yb -= step; }
		void	move2D( const int &step ) { m_yo +=step; m_yb += step; }
		void    fromString(const std::string &s);
		std::string   toString();
		std::string   toString2();

		inline cv::Rect toCvRect() const{
			cv::Rect rect( myround(m_xo), myround(m_yo), myround(m_w), myround(m_h) );
			return rect;
		}
};

bool UTIL_EXPORT isRoiOutOfImg( const Roi &roi, const int &frmImgW, const int &frmImgH );
bool UTIL_EXPORT isRoi1InRoi2( const Roi &roi1, const Roi &roi2 );
}	//end namespace util
#endif

