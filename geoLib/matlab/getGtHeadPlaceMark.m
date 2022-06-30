function vStr = getGtHeadPlaceMark( mmsi, latlon )
%for trajectory header
%
vStr = cell(8,1);
vStr{1} = '<Placemark>\n';
%vStr{2} = ['<name> mmsi', num2str(mmsi), '</name>\n'];
vStr{2} = ['<name></name>\n'];
vStr{3} = ['<description>', 'mmsi statistics are: ' , num2str(rand), '</description>\n'];
vStr{4} = ['<styleUrl> #GtHeader_Style </styleUrl>\n'];
vStr{5} = '<Point>\n';
vStr{6} = [sprintf( '<coordinates>%.7f,%.7f,0</coordinates>', latlon(2), latlon(1) ), '\n'];
vStr{7} = '</Point>\n';
vStr{8} = '</Placemark>\n';
end
