/*
 *------------------------------------------------------------------------
 * SwMat.h - partial emulation of matrix computation in MatLab for T 
 *            matrices
 * No  any Corporation or client funds were used to develop this code. 
 *
 * $Id: SwMat.h,v 1.3 2011/06/23 18:41:20 swu Exp $
 * Copyright (c) 2005 Shunguang Wu
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL THE AUTHOR OR DISTRIBUTOR BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 *-------------------------------------------------------------------------
 */

//2d matrix, data stored row by row wise
//
#ifndef SW_MAT_H
#define SW_MAT_H 1

#include <math.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "swMatDefs.h"

using namespace std;
namespace app {

	template <typename Type>
	class SwMat;

	template<class T>
	class SWMAT_EXPORT SwMat {
		//non-member friend functions
		friend ostream& operator<<(ostream& os, SwMat& x);
		friend SwMat   operator+(const T scale, const SwMat& x); // 5+x
		friend SwMat   operator-(const T scale, const SwMat& x); // 5-x
		friend SwMat   operator*(const T scale, const SwMat& x); // 5*x
		friend SwMat   operator/(const T scale, const SwMat& x); // 5/x

	public:
		SwMat(const uint32_t rows = 0, const uint32_t cols = 0)
			: m_rows(rows),	m_cols(cols),	m_size(rows* cols)
		{
			if (m_size > 0)
				creatBuf();
		}

		SwMat(const uint32_t rows, const uint32_t cols, const T initValue)
			: m_rows(rows), m_cols(cols), m_size(rows* cols)
		{
				creatBuf();
				setData(initValue);
		}

		//deep copy
		SwMat(const uint32_t rows, const uint32_t cols, const T* buf, const uint32_t bufLength)
			: m_rows(rows), m_cols(cols), m_size(rows* cols)
		{
			if (bufLength < m_size) {
				errMsg("Warning! swMat::swMat(): the input size not match the buffer size.");
			}

			creatBuf();
			setData(buf, m_size);
		}

		SwMat(const SwMat& x)
			: m_rows(x.rows()), 	m_cols(x.cols()),	m_size(x.size())
		{
				creatBuf();
				memcpy(m_buf, x.m_buf, x.size());
		}

		~SwMat()
		{
			deleteBuf();
		}

		uint32_t cols() const { return m_cols; }
		T* data()							{	return m_buf;	}
		const T* data() const { return m_buf; }

		//asign the data of matrix to a double point, the caller take the responsbility of assign memoery to p
		void  data(T** p) const
		{
			for (uint32_t i = 0; i < m_rows; i++)
				*(p + i) = &m_buf[i * m_cols];
		}

		// delete coloumn c
		void  delCol(const uint32_t c)
		{
			delCols(c, c);
		}

		// delete coloumns from c1 to c2 
		void  delCols(const uint32_t c1, const uint32_t c2, const uint32_t skip = 1)
		{
#if SW_MAT_BOUNDS_CHECK
			if (c2 >= m_cols)
				errMsg("swdMat :: delCols(), c2 is out of range!");

			if (c1 < 0)
				errMsg("swdMat :: delCols(), c1 is out of range!");
#endif
			if (c1 == 0 && c2 == (m_cols - 1) && skip == 1) {
				resize(0, 0);
				return;
			}

			SwMat<T> x0(*this); //copy the current matrix

			uint32_t deletedCols = (c2 - c1) / skip + 1;
			uint32_t delIdSize = deletedCols * m_rows;
			uint32_t* delIdBuf = new uint32_t[delIdSize];

			uint32_t i, j, k = 0;
			for (i = 0; i < x0.rows(); i++) {
				uint32_t rr = i * m_cols;
				for (j = c1; j <= c2; j += skip)
					delIdBuf[k++] = rr + j;
			}
			//create the matrix after deleting
			uint32_t newCols = m_cols - deletedCols;
			resize(m_rows, newCols);

			//put the filtered data in m_buf[]
			delElementsFromBuf(x0.m_buf, x0.size(), delIdBuf, delIdSize, m_buf);

			delete[] delIdBuf;
		}

		// delete row r
		void  delRow(const uint32_t r) {
				delRows( r, r);
		}

		// delete rows from c1 to c2 
		void  delRows(const uint32_t r1, const uint32_t r2, const uint32_t skip = 1)
		{
#if SW_MAT_BOUNDS_CHECK
			if (r1 > r2)
				errMsg("swdMat :: delRows(), r1 must be smaller than or equal r2!");
			if (r1 < 0)
				errMsg("swdMat :: delRows(), r1 is out of range!");
#endif
			if (r1 == 0 && r2 == (m_rows - 1) && skip == 1) //try to delete all
			{
				resize(0, 0);
				return;
			}
			if (r1 >= m_rows) return;

			uint32_t rr2 = (r2 >= m_rows) ? (m_rows - 1) : r2;

			SwMat<T> x0(*this); //copy the current matrix

			uint32_t deletedRows = (rr2 - r1) / skip + 1;
			uint32_t delIdSize = deletedRows * m_cols;
			uint32_t* delIdBuf = new uint32_t[delIdSize];

			uint32_t i, j, k = 0;
			for (i = r1; i <= rr2; i += skip)
				for (j = 0; j < m_cols; j++)
					delIdBuf[k++] = i * m_cols + j;

			//create the matrix after deleting
			uint32_t newRows = m_rows - deletedRows;
			resize(newRows, m_cols);

			//put the filtered data in m_buf[]
			delElementsFromBuf(x0.m_buf, x0.size(), delIdBuf, delIdSize, m_buf);

			delete[] delIdBuf;
		}

		//keep the original values at the original place, but extened number of rows
		//and the garbage are in the extended places
		void extendRows(const uint32_t numNewRows);

		SwMat getCol(const uint32_t c) const;    // get a column
		SwMat getCols(const uint32_t cBeg, const uint32_t cEnd, const uint32_t nSkip = 1) const;
		SwMat getDiagonal() const;						// get the diagnal
		SwMat getRow(const uint32_t r) const;	// get a row
		T*    getRowAddress(const uint32_t& r);
		const T* getRowAddress2(const uint32_t& r) const;

		SwMat getRows(const uint32_t rBeg, const uint32_t rEnd, const uint32_t nSkip = 1) const;
		SwMat getSlice(const uint32_t r0, const uint32_t c0,           //top left position
			const uint32_t rowL, const uint32_t colL) const;     // row and col length 
		SwMat getSubCol(const uint32_t colNum, const uint32_t r0, const uint32_t rowL) const;
		SwMat getSubRow(const uint32_t rowNum, const uint32_t c0, const uint32_t colL) const;

		void  insertCol(const uint32_t c, const T val);     // insert a column with same val
		void  insertCol(const uint32_t c, const T* buf);    // insert a column with data from buf[]
		void  insertCol(const uint32_t c, const SwMat& x);      // insert a column with data from matrix x
		void  insertRow(const uint32_t r, const T val);     // insert a row with same data value
		void  insertRow(const uint32_t r, const T* buf);    // insert a row with data from buf[]
		void  insertRow(const uint32_t r, const SwMat& x);      // insert a row with data from matrix x

		void  print(const char* str = " ") const;
		void  print(const char* fmt, const char* str) const;

		void  cprint(const char* str = " ") const;

		SwMat& randomize();       // randomize the matrix
		//change size of the matrix, 
		void resize(const uint32_t newRows, const uint32_t newCols);
		void resize(const uint32_t newRows, const uint32_t newCols, const T val);
		// keep the value, and size, but chnage raws and cols, and the data are stored row by row
		void reshape(const uint32_t newRows, const uint32_t newCols);
		uint32_t rows() const;  //get # of rows

		void  setData(const T val) {
			for (uint32_t k = 0; k < m_size; k++)
				m_buf[k] = initValue;
		}

		void  setData(const T* buf, const uint32_t bufSize);

		void  setCol(const uint32_t c, const T val);   // set the col elements as val
		void  setCol(const uint32_t c, const T* buf, const uint32_t bufSize);  // set the col elements as vals stored in buf[] 
		void  setCol(const uint32_t c, const SwMat& x);   // set the col elements as vals stored in vector x 
		void  setSubCol(const uint32_t c, const uint32_t r0, const uint32_t r1, const T val);
		void  setSubCol(const uint32_t c, const uint32_t r0, const uint32_t r1, const SwMat& x);
		

		void  setDiagonal(const T val);  // set the diagnoal elements as val
		void  setDiagonal(const SwMat& x);  // set the diagnoal elements as vals stored in vector x 
		void  setDiagonal(const T* buf, const uint32_t bufSize);  // set the diagnoal elements as vals stored in vector x 
		void  setRow(const uint32_t& r, const T& val);  // set the row elements as val
		void  setRow(const uint32_t& r, const SwMat& x, const bool& isByRow = true);  // set the row elements as vals stored in vector x 
		void  setRow(const uint32_t& r, const T* pSrc);
		void  setRows(const uint32_t r0, const uint32_t r1, const SwMat& x);  // set the rows as vals stored in vector x 
		void  setSubRow(const uint32_t r, const uint32_t c0, const uint32_t c1, const T val);
		void  setSubRow(const uint32_t r, const uint32_t c0, const uint32_t c1, const SwMat& x);

		void  setSlice(const uint32_t r0, const uint32_t c0, //top left position
			const uint32_t rowL, const uint32_t colL,    // row and col length
			const T val);
		void  setSlice(const uint32_t r0, const uint32_t c0, //top left position
			const uint32_t rowL, const uint32_t colL,        // row and col length
			const SwMat& x);

		uint32_t size() const; //get the size # 
		T trace() const;       // get trace
		SwMat  transpose() const;  // get transpose
		void appendToFile(FILE* fp, char* fmtStr, char* msgStr = NULL, bool isPrintSize = true) const;
		void writeToFile(const char* fileName, const char* fmt = NULL) const;
		void writeToFile(const char* fileName, const vector<string>& titleLines, const char* fmt = NULL) const;
		void writeToFile2(const std::string& path, const std::string& fname_wo_ext, const std::string& fmt, const int& fn, const int& idx = -1);

		void readFromFile(const uint32_t nCols, const char* fileName, const int nMaxLines = -1);
		void readFromFile(const char* fileName, const uint32_t numOfLinesSkiped, const uint32_t nCols, const long nMaxLines = -1);
		void readFromFile2(const std::string& path, const std::string& fname_wo_ext, const int& fn, const int& idx = -1);

		//public operator overloads
		SwMat& operator =(const SwMat& x); //assignment matrix x to this
		SwMat& operator =(const T d);  //assignment d to all elements of this

		bool   operator==(const SwMat& x) const; //compare equal
		bool   operator!=(const SwMat& x) const; //compare not equal
		T      operator()(const uint32_t i, const uint32_t j) const;   // get a(i,j)
		T&     operator()(const uint32_t i, const uint32_t j);         // get a(i,j)
		T      operator()(const uint32_t k) const;
		T&     operator()(const uint32_t k);

		// in the following explaination. we assume $a$ is the matrix at the left hand side
		SwMat operator -() const;                   //by add minus sign for each element
		SwMat operator +(const SwMat& x) const;     // a+x  add by element
		SwMat operator +(const T scale) const; // a+scale add scale to every element
		SwMat operator -(const SwMat& x) const;     // a-x  minus by element
		SwMat operator -(const T scale) const; // a-scale  minus ascale to every element
		SwMat operator *(const SwMat& x) const;     // a*x  times by element
		SwMat operator *(const T scale) const; // a*scale  times scale to every element
		SwMat operator /(const SwMat& x) const;     // a/x  dividion by element
		SwMat operator /(const T scale) const; // a/scale  divided every element by the scale
		SwMat operator %(const SwMat& x) const;     // a%x  matrix prodcution

		SwMat& operator +=(const SwMat& x);     //a +=x (a=a+x), by element
		SwMat& operator +=(const T scale); //a+=scale, matrix plus a scale
		SwMat& operator -=(const SwMat& x);     //a -= x, by element
		SwMat& operator -=(const T scale); //a -= scale, matrix minus a scale
		SwMat& operator *=(const SwMat& x);     //a *=x, by element
		SwMat& operator *=(const T scale); //a *= scale, amatrix times a scale
		SwMat& operator /=(const SwMat& x);     //a /= x, by element
		SwMat& operator /=(const T scale); //a /= scale, by element

	private:
		void creatBuf()
		{
			if (m_size > 0) {
				deleteBuf();
				m_buf = new T[m_size];
				if (!m_buf)
					errMsg("swdMat::creatBuf(): cannot allocate memory!");
			}
			else {
				m_size = 0;
				m_rows = 0;
				m_cols = 0;
				m_buf = nullptr;
			}
		}

		void deleteBuf()
		{
			if (m_buf) {
				delete[] m_buf;
				m_buf = nullptr;
			}
		}

		void deepCopy(T* buf, const uint32_t size) const
		{
#if SW_MAT_BOUNDS_CHECK
			if (buf == nullptr)
				errMsg("Error: swdMat::deepCopy() -- buf is NULL!");

			if (size != m_size)
				errMsg("Error: swdMat::deepCopy() -- size not match the m_size!");
#endif
			memcpy( buf, m_buf, m_size);
		}
		//-----------------------------------------------------------------------------
		//input params
		//  oldBuf[],original data buffer
		//  oldSize, the elements of oldBuf[]
		//  deletedIdBuf[], all the elements in oldBuf[] with the IDs stored in deletedIdBuf[] will be deleted
		//                  these elements are stored in the order of from smallest to largest
		//  deletedIdBufSize, the elements of deletedIdBuf[]
		//output
		//  newBufp[], the reminder elements oldBuf[] still keep their original order
		//-----------------------------------------------------------------------------
		void delElementsFromBuf(const T* oldBuf, const uint32_t oldSize,
			const uint32_t* deletedIdBuf, const uint32_t deletedIdBufSize, T* newBuf)
		{
			bool isFound;
			uint32_t i, j, k = 0;
			uint32_t n0 = 0;  //start search index in deletedIdBuf[]
			for (i = 0; i < oldSize; i++) {
				//find index i in deletedIdBuf
				isFound = false;
				for (j = n0; j < deletedIdBufSize; j++) {
					if (i == deletedIdBuf[j]) {
						isFound = true;
						n0 = j + 1;
						break;
					}
				}
				if (!isFound)
					newBuf[k++] = oldBuf[i];
			}
		}

		void errMsg(const char* msgStr) const
		{
			cerr << msgStr << endl;
			throw runtime_error(msgStr);
		}

		void warningMsg(const char* msgStr) const
		{
			cerr << msgStr << endl;
		}

		uint32_t m_rows{ 0 };  //rows
		uint32_t m_cols{ 0 };  //cols
		uint32_t m_size{ 0 };  //m_rows * m_cols
		T*       m_buf{ nullptr };
	};

	typedef  SwMat<double>	Mat_f64;
	typedef  SwMat<float>	Mat_f32;
	typedef  SwMat<uint8_t>  Mat_u8;
	typedef  SwMat<uint16_t> Mat_u16;
	typedef  SwMat<uint32_t> Mat_u32;
	typedef  SwMat<int8_t>   Mat_i8;
	typedef  SwMat<int16_t>  Mat_i16;
	typedef  SwMat<int32_t>  Mat_i32;
}
#endif

