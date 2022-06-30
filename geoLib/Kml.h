// TestDriver.cpp : Defines the entry point for the console application.
//

#ifndef __IMINT_KML_H__
#define __IMINT_KML_H__

#include <iostream>
#include <vector>
#include <string>

#include "geoConversion.h"
namespace imint {
	class Imint_KML {
	public:
		Imint_KML();
		~Imint_KML();
		void createKml( const std::string  &fpath, const std::string &headName);
		void closeKml();

		void writeIconPtStyle(const std::string &styleId, const std::string &iconUrl, float scale);
		void writeLineStyle(const std::string &styleId, const std::string &colorCode, int width, int labelVisibility);

		void writeLine(const std::vector<imint::GeoPt_LLH_f32> &vLine, const std::string &styleId, const std::string &name);
		void writeIconPoints(const std::vector<imint::GeoPt_LLH_f32> &v, const std::string &styleId, const std::string &name, const std::string &description);
	protected:
		void writeHead(const std::string &name);
		void writeTail();
	protected:
		FILE *fid;
	};
}

#endif
