classdef Kml
    %UNTITLED3 Summary of this class goes here
    %   Detailed explanation goes here
    properties
    end
    
    methods (Static)
        function writeKmlHead( fid, name)
            fprintf(fid, '<?xml version="1.0" encoding="utf-8"?>\n');
            fprintf(fid, '<kml xmlns="http://www.opengis.net/kml/2.2">\n');
            fprintf(fid, '<Document>\n');
            fprintf(fid, '<name>%s</name>\n', name);
            fprintf(fid, '\n');
        end
        
        function writeKmlTail( fid )
            fprintf(fid, '</Document>\n');
            fprintf(fid, '</kml>\n');
        end
        
        function writeLineStyle( fid, styleId, colorCode, width, labelVisibility )
            fprintf( fid, '<Style id="%s">\n', styleId );
            fprintf( fid, '<LineStyle>\n');
            fprintf( fid, '<color>%s</color>\n', colorCode);
            fprintf( fid, '<width>%d</width>\n', width);
            fprintf( fid, '<gx:labelVisibility> %d </gx:labelVisibility>\n', labelVisibility );
            fprintf( fid, '</LineStyle>\n');
            fprintf( fid, '</Style>\n');
            fprintf(fid, '\n');
        end
        
        function writeIconStyle( fid, styleId, iconUrl, scale )
            fprintf( fid, '<Style id="%s">\n', styleId);
            fprintf( fid, '<IconStyle>\n');
            fprintf( fid, '<scale>%f</scale>\n', scale);
            fprintf( fid, '<Icon>\n');
            fprintf( fid, '<href>%s</href>\n', iconUrl);
            fprintf( fid, '</Icon>\n');
            fprintf( fid, '</IconStyle>\n');
            fprintf( fid, '<LabelStyle>\n');
            fprintf( fid, '<scale>0.7</scale>\n');
            fprintf( fid, '</LabelStyle>\n');
            fprintf( fid, '</Style>\n');
            fprintf(fid, '\n');
        end
        
        function writeIconPoint( fid, latlon, styleId, name, description )
            fprintf(fid, '<Placemark>\n');
            
            if ~isempty( name )
                fprintf(fid, '<name>%s</name>\n', name);
            end
            if ~isempty( description )
                fprintf(fid, '<description>\n' );
                if iscell( description )
                    for i=1:numel( description )
                        fprintf(fid, '%s\n', description{i} );
                    end
                else
                    fprintf(fid, '%s\n', description );
                end
                fprintf(fid, '</description>\n');
            end
            
            fprintf(fid, '<styleUrl>#%s</styleUrl>\n', styleId );
            fprintf(fid, '<Point>\n');
            fprintf(fid, '<coordinates>%.7f, %.7f</coordinates>\n', latlon(2), latlon(1));
            fprintf(fid, '</Point>\n');
            fprintf(fid, '</Placemark>\n');
            fprintf(fid, '\n');
        end
        
        function writeLine1( fid, vLatLon, styleId, name, bExtrude, bTessellate )
            %vLatLon,       2 x n, each column is [lat,lon]';
            
            fprintf(fid, '<Placemark>\n');
            if ~isempty( name )
                fprintf(fid, '<name>%s</name>\n', name );
            end
            fprintf(fid, '<styleUrl>#%s</styleUrl>\n', styleId );
            
            fprintf(fid, '<LineString>\n');
            fprintf(fid, '<extrude>%d</extrude>\n', bExtrude);
            fprintf(fid, '<tessellate>%d</tessellate>\n', bTessellate);
            fprintf(fid, '<coordinates>\n');
            n = size( vLatLon,2 );
            for i=1:n
               fprintf(fid, '\t%.7f,%.7f,0\n', vLatLon(2,i), vLatLon(1,i) );
            end
            fprintf(fid, '</coordinates>\n');
            fprintf(fid, '</LineString>\n');
            fprintf(fid, '</Placemark>\n');
            fprintf(fid, '\n');
        end
    end
end

