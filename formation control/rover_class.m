classdef rover_class < handle
    properties
        tcp_port = [];
        id = -1;
        current_coor = [0,0,0];
        target = [0,0,0];
    end
    
    methods
        function connect2rover(obj)
            % initialise the tcp/ip port
            obj.tcp_port = tcpip('0.0.0.0',1234,'NetworkRole', 'server');
            % listen to the connection
            fopen(obj.tcp_port);
        end
        
        function askID(obj)
            fwrite(obj.tcp_port, 4, 'uint8'); % ask for ID
            obj.id = fread(obj.tcp_port,1,'uint8');
            fprintf("The ID of this rover is: %i\n", obj.id);
        end
        
        function setInitialCoor(obj,x,y,phi)
            obj.current_coor = [x,y,phi];
            fwrite(obj.tcp_port, 2, 'uint8'); % notify the rover that we are sending initial coordinate
            [byte1,byte2,byte3,byte4,byte5,byte6] = coor2byte(x,y,phi);

            fwrite(obj.tcp_port, [byte1,byte2,byte3,byte4,byte5,byte6],'uint8');
        end
        
        function sendTarget(obj,x,y,phi)
            obj.target = [x,y,phi];
            fwrite(obj.tcp_port, 0, 'uint8'); % notify the rover that we are sending the target
            [byte1,byte2,byte3,byte4,byte5,byte6] = coor2byte(x,y,phi);

            fwrite(obj.tcp_port, [byte1,byte2,byte3,byte4,byte5,byte6],'uint8');
        end
        
        function getCoordinate(obj)
            fwrite(obj.tcp_port, 1, 'uint8'); % notify the rover that we are getting the current coordinate
            byte_array = fread(obj.tcp_port, 8, 'uint8');
            [obj.current_coor(1), obj.current_coor(2), obj.current_coor(3)] = byte2coor(byte_array(1),byte_array(2),byte_array(3),byte_array(4),byte_array(5),byte_array(6));
        end
        
    end
    
end

function [x, y, phi] = byte2coor(x_high,x_low,y_high,y_low,phi_high,phi_low)
   x = ((-1)^bitshift(x_high,-7)) * (bitor(x_low, bitshift(bitand(x_high,127),8))); %((-1)**(x_high>>7)) * ((x_low) | (((x_high&0x7f)<<8)))
   y = ((-1)^bitshift(y_high,-7)) * (bitor(y_low, bitshift(bitand(y_high,127),8)));
   phi = ((-1)^bitshift(phi_high,-7)) * (bitor(phi_low, bitshift(bitand(phi_high,127),8)));
end

function [x_high,x_low,y_high,y_low,phi_high,phi_low] = coor2byte(x,y,phi)
    if x >= 0
        x_high = bitand(bitshift(x,-8),255);
        x_low = bitand(x,255);
    else
        x = x*-1;
        x_high = bitand(bitshift(x,-8),255);
        x_high = bitor(x_high,128);
        x_low = bitand(x, 255);
    end
    
    if y >= 0
        y_high = bitand(bitshift(y,-8),255);
        y_low = bitand(y,255);
    else
        y = y*-1;
        y_high = bitand(bitshift(y,-8),255);
        y_high = bitor(y_high,128);
        y_low = bitand(y, 255);
    end
    
    if phi >= 0
        phi_high = bitand(bitshift(phi,-8),255);
        phi_low = bitand(phi,255);
    else
        phi = phi*-1;
        phi_high = bitand(bitshift(phi,-8),255);
        phi_high = bitor(phi_high,128);
        phi_low = bitand(phi, 255);
    end
    
end
