function vStr = getTrajPathPlaceMark( vLatLon, colorCode, lineWidth )
%%
%vLatLon,       2 x n, each column is [lat,lon]';
%colorCode,     aabbggrr, string
%%

vStr = cell(15,1);

%%
%for trajectory itsself
%
vStr{1} = '<Placemark>\n';
vStr{2} = '<Snippet maxLines="0"> </Snippet>\n';
vStr{3} = '<Style>\n';
vStr{4} = '<IconStyle>\n';
vStr{5} = ['<color>', colorCode, '</color>\n'];
vStr{6} = '</IconStyle>\n';
vStr{7} = '<LineStyle>\n';
vStr{8} = ['<color>', colorCode, '</color>\n'];
vStr{9} = ['<width>', num2str(lineWidth), '</width>\n'];
vStr{10} = '</LineStyle>\n';
vStr{11} = '</Style>\n';
vStr{12} = '<LineString>\n';
%%
%----------------createTrajPlaceMarks
s1 = '<coordinates>';
n = size( vLatLon,2 );
z = 0;
for i=1:n
    s1 = sprintf('%s %.7f,%.7f,%d ', s1, vLatLon(2,i), vLatLon(1,i), z );
end
s1 = sprintf('%s </coordinates>',s1);
%--------------
%%
vStr{13} = [s1, '\n'];
vStr{14} = '</LineString>\n';
vStr{15} = '</Placemark>\n';
end


