classdef TrkPool < TrajPool
    %--------------------------------------------------------------------
    %
    % \file        TrajPoolTrk.m
    % \brief       a class to manage the track trajectories
    %
    % \date        01/31/2015
    
    % \author      Shunguang.Wu@jhuapl.edu
    %--------------------------------------------------------------------
    properties ( Access = public )
        %data defined in $TrajPool$
        vDelTrkIds;
        nDelSize;
        
        %params
        coastingThd;
        vSensors;
        vColorCodes;      %n x 8 cell, each one is a code with [aabbggrr] 
        maxTimeSpan4EachTrk;
    end
    
    methods
        function obj = TrkPool( vSensors, vColorCode, maxTimeSpan4EachTrk, coastingThd )
            obj@TrajPool(10);
            
            obj.vDelTrkIds = nan(100,1);
            obj.nDelSize = 0;
            
            obj.vSensors  = vSensors;
            obj.vColorCodes = vColorCode;
            obj.maxTimeSpan4EachTrk = maxTimeSpan4EachTrk;
            obj.coastingThd = coastingThd;
        end

        function obj = addNewPts( obj, x )
            %----------------------------------------------------------------
            %Track History:		x
            %x.TrackID    Track ID number
            %x.Age		Time since last detection (min)
            %x.ColorId	
            %x.SensorID	Radar ID: 0 = X-Fix, 1 = DUCK, 2 = LISL, 3 = TIKI
            %x.Updates	N x 3 matrix, where the 3 columns are: [ Time, Latitude, Longitude ]
            %
            %Only new Track Updates are sent.
            %If  x.Age == -1	Suspend drawing the track (erase, but keep in memory)
            %If  x.Age == -2	Drop the track – it will not be shown again
            %If  x.Age > 5		Show label NNNNNcAA with the track,
            %					Where NNNNN is the track ID; AA is the age And show a square for the current location
            %Else				Show just NNNNN as the label
            %				    And show a circle for the current location
            %----------------------------------------------------------------
            trkId = x.TrackID;
            cellIdx = obj.findCellIdx( trkId );
            m = size(x.Updates,1);
            y = [x.Updates, zeros(m,1)];
            
            if isempty( cellIdx ) %add new trk in cell
                n = obj.nSize + 1;
                prop = TrkProp( trkId, x.SensorId, x.ColorId, x.Age );
                stat = TrkStatistics(0);
                obj.vTrajs{n} =  Trk(prop, stat, y', obj.maxTimeSpan4EachTrk );
                obj.idxTab(n, :) = [n,trkId];
                obj.nSize = n;
            else  %update current trk in cell
                if x.Age == -2  %remove from pull
                    obj = obj.deleteTraj( cellIdx );
                    obj.nDelSize = obj.nDelSize + 1;
                    obj.vDelTrkIds( obj.nDelSize ) = trkId;
                else
                    obj.vTrajs{cellIdx} =  obj.vTrajs{cellIdx}.add( y' );
                    obj.vTrajs{cellIdx}.prop.age = x.Age;
                end
            end
        end
        
        %overwritten all trks in pool
        function obj = addKfTrks( obj, sensorId, curTrks, refLat, refLon, isIncludePredictedPts )
            %curTrks, the trks from KF
            %(refLat, refLon), the (lat, lon) of reference radar
           wgs84InKm   = wgs84Ellipsoid('km');
            nColors = numel( obj.vColorCodes );
            nTrks = length( curTrks );
            idxCell = obj.nSize;   
            for i=1:nTrks
                %plot estimated trajectory
                if ~(curTrks{i}.isShowGui)
                    continue;
                end
                trkId = curTrks{i}.id;
                u = curTrks{i}.trajQ.getTraj2( isIncludePredictedPts );  %4 x n, each col is [t,x,y,flag]'
                n = size(u,2);
                if n < 1
                    continue;
                end
                colorId = mod(trkId, nColors);
                if colorId==0
                    colorId = nColors;
                end
                age = 10;
                
                prop = TrkProp( trkId, sensorId, colorId, age );
                stat = TrkStatistics(0); 
                [lat, lon, ~] = enu2geodetic( u(2,:), u(3,:), zeros(1,n), refLat, refLon, 0, wgs84InKm );
                u(2,:) = lat;
                u(3,:) = lon;
                
                idxCell = idxCell + 1;
                obj.vTrajs{idxCell} =  Trk(prop, stat, u, obj.maxTimeSpan4EachTrk );
                obj.idxTab(idxCell, :) = [idxCell,trkId];
            end  %for i=1:nTrks
            obj.nSize = idxCell;
        end
        
        
        function writeToKml( obj, tmpFile, finalFile, trkHeadIconScale )
            if obj.nSize<1
                return;
            end
            
            bExtrude = 1;
            bTessellate = 1;

            fid = fopen( tmpFile, 'w' );
            if fid == -1
                warning( 'TrkTrajPool.writeToKml() - cannot open file: %s', tmpFile );
                return;
            end
               
            Kml.writeKmlHead( fid, 'Coda Tracks' );
            
            %define trk head styles
            n = numel( obj.vSensors );
            vTrkHeadStyIds = cell(n,1);
            vTrkHeadCoastStyIds = cell(n,1);
            for i=1:n
                styId = [ 'TrkHead4Sensor_Sty', num2str(i)];
                Kml.writeIconStyle( fid, styId, obj.vSensors{i}.trkHeadIconUrl, trkHeadIconScale );
                vTrkHeadStyIds{i}       = styId;
            end
            for i=1:n
                styId = [ 'TrkHeadCoast4Sensor_Sty', num2str(i)];
                Kml.writeIconStyle( fid, styId, obj.vSensors{i}.trkHeadCoastIconUrl, trkHeadIconScale );
                vTrkHeadCoastStyIds{i}  = styId;
            end

            %define line style
            nColors = numel( obj.vColorCodes );
            vTrkTrajStyIds = cell(nColors,1);
            labelVisibility = 0;
            for i=1:nColors
                styId = ['TrkTraj_Sty', num2str(i)];
                Kml.writeLineStyle( fid, styId, obj.vColorCodes{i}, 3, labelVisibility )
                vTrkTrajStyIds{i} = styId;
            end
 
            
            %write trk trajectory and trk head for each trk
            for i=1:obj.nSize  %loop for each trk
               vLatLon = obj.vTrajs{i}.getTraj();  %2 x nPts
               nPts = size( vLatLon, 2);
               if nPts<1
                   continue;
               end
               prop = obj.vTrajs{i}.prop;
               if prop.age == -1 %Suspend drawing the track 
                   continue;
               end
               
               stat = obj.vTrajs{i}.stat;
               headLoc = vLatLon(:,end);
               Kml.writeLine1( fid, vLatLon, vTrkTrajStyIds{prop.colorId}, ['traj',num2str( prop.trkId )], bExtrude, bTessellate );
               if prop.age>obj.coastingThd
                   styId = vTrkHeadCoastStyIds{ prop.sensorId };
                   name = sprintf( '%05dc%02d', prop.trkId, ceil(prop.age) );
               else
                   styId = vTrkHeadStyIds{ prop.sensorId };
                   name = sprintf( '%05d', prop.trkId );
               end
               dscp = stat.toString();
               Kml.writeIconPoint( fid, headLoc, styId, name, dscp);
            end
            Kml.writeKmlTail( fid );
            fclose( fid);

            %update network linked file
           [status,msg]=copyfile( tmpFile, finalFile );
           if status==0
               warning( 'TrkTrajPool.writeToKml(): cannot copy file %s -> %s, msg: %s', tmpFile, finalFile, msg );
           end
        end

        function code = getColorCode( obj, trkId )
            n = size( obj.rgbTab, 1);
            i = mod(trkId, n);
            if i==0
                i = n;
            end
            [rr,gg,bb] = rgbconv( obj.rgbTab(i, :) );
            aa ='7F'; %50 percent opacity 
            %aa ='FF'; %no transparency
            code = [aa,bb,gg,rr];
        end
        
    end
end %classdef
