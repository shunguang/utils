function vStr = getTrkHeadPlaceMark( trkId, latlon )
%for trajectory header
%
vStr = cell(8,1);
vStr{1} = '<Placemark>\n';
vStr{2} = ['<name> T', num2str(trkId), '</name>\n'];
vStr{3} = ['<description>', 'trk statistics are: ' , num2str(rand), '</description>\n'];
vStr{4} = ['<styleUrl> #TrkHeader_Style </styleUrl>\n'];
vStr{5} = '<Point>\n';
vStr{6} = [sprintf( '<coordinates>%.7f,%.7f,0</coordinates>', latlon(2), latlon(1) ), '\n'];
vStr{7} = '</Point>\n';
vStr{8} = '</Placemark>\n';
end
