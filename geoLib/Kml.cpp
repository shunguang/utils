#include "imint_kml.h"

using namespace std;
using namespace imint;

Imint_KML::Imint_KML()
	: fid(0)
{
}

Imint_KML::~Imint_KML()
{
}

void Imint_KML::createKml( const std::string  &fpath, const std::string &headName)
{
	if (fid) {
		closeKml();
	}

	fid = fopen(fpath.c_str(), "w");
	writeHead( headName );
}

void Imint_KML::closeKml()
{
	writeTail();
	fclose(fid);
	fid = 0;
}


void Imint_KML::writeHead(const string &name )
{
	fprintf(fid, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	fprintf(fid, "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");
	fprintf(fid, "<Document>\n");
	fprintf(fid, "<name>%s</name>\n", name.c_str());
	fprintf(fid, "\n");
}

void Imint_KML::writeTail()
{
	fprintf(fid, "</Document>\n");
	fprintf(fid, "</kml>\n");
	fclose(fid);
}

void Imint_KML::writeLineStyle(const std::string &styleId, const std::string &colorCode, int width, int labelVisibility)
{
	fprintf(fid, "<Style id=\"%s\">\n", styleId.c_str());
	fprintf(fid, "<LineStyle>\n");
	fprintf(fid, "<color>%s</color>\n", colorCode);
	fprintf(fid, "<width>%d</width>\n", width);
	fprintf(fid, "<gx:labelVisibility> %d </gx:labelVisibility>\n", labelVisibility);
	fprintf(fid, "</LineStyle>\n");
	fprintf(fid, "</Style>\n");
	fprintf(fid, "\n");
}

void Imint_KML::writeIconPtStyle(const std::string &styleId, const std::string &iconUrl, float scale)
{
	fprintf(fid, "<Style id=\"%s\">\n", styleId);
	fprintf(fid, "<IconStyle>\n");
	fprintf(fid, "<scale>%f</scale>\n", scale);
	fprintf(fid, "<Icon>\n");
	fprintf(fid, "<href>%s</href>\n", iconUrl);
	fprintf(fid, "</Icon>\n");
	fprintf(fid, "</IconStyle>\n");
	fprintf(fid, "<LabelStyle>\n");
	fprintf(fid, "<scale>0.7</scale>\n");
	fprintf(fid, "</LabelStyle>\n");
	fprintf(fid, "</Style>\n");
	fprintf(fid, "\n");
}

void Imint_KML::writeLine(const std::vector<imint::GeoPt_LLH_f32> &vLine, const std::string &styleId, const std::string &name)
{
	int bExtrude = 1;
	int bTessellate = 1;

	fprintf(fid, "<Placemark>\n");
	fprintf(fid, "<name>%s</name>\n", name.c_str());
	fprintf(fid, "<styleUrl>#%s</styleUrl>\n", styleId.c_str());

	fprintf(fid, "<LineString>\n");
	fprintf(fid, "<extrude>%d</extrude>\n", bExtrude);
	fprintf(fid, "<tessellate>%d</tessellate>\n", bTessellate);
	fprintf(fid, "<coordinates>\n");
	for each (const GeoPt_LLH_f32 var in vLine) {
		fprintf(fid, "\t%.7f,%.7f,0\n", var.lon_deg, var.lat_deg);
	}
	fprintf(fid, "</coordinates>\n");
	fprintf(fid, "</LineString>\n");
	fprintf(fid, "</Placemark>\n");
	fprintf(fid, "\n");
}

void Imint_KML::writeIconPoints(const std::vector<imint::GeoPt_LLH_f32> &vPts, const std::string &styleId, const std::string &name, const std::string &description)
{
	for each (const GeoPt_LLH_f32 var in vPts) {
		fprintf(fid, "<Placemark>\n");
		if (!name.empty()) {
			fprintf(fid, "<name>%s</name>\n", name.c_str());
		}

		if (!description.empty()) {
			fprintf(fid, "<description>\n");
			fprintf(fid, "%s\n", description.c_str());
			fprintf(fid, "</description>\n");
		}
		//fprintf(fid, "<styleUrl>#%s</styleUrl>\n", styleId);
		fprintf(fid, "<Point>\n");
		fprintf(fid, "<coordinates>%.7f, %.7f</coordinates>\n", var.lon_deg, var.lat_deg);
		fprintf(fid, "</Point>\n");
		fprintf(fid, "</Placemark>\n");
	}
	fprintf(fid, "\n");
}