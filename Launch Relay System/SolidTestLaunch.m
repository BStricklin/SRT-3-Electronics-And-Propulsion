endPrgrm = 0;
cmdInput = 0;
megaData = 0;

obj1 = instrfind('Type', 'serial', 'Port', 'COM4', 'Tag', '');

% Create the serial port object if it does not exist
% otherwise use the object that was found.
if isempty(obj1)
    obj1 = serial('COM4');
else
    fclose(obj1);
    obj1 = obj1(1)
end

% Connect to instrument object, obj1.
fopen(obj1);

% Configure instrument object, obj1.
set(obj1, 'Terminator', {13,13});
% Configure instrument object, obj1.
set(obj1, 'Timeout', 10.0);

% Communicating with instrument object, obj1.
fprintf(obj1, 'This is how to write');
data1 = fscanf(obj1);
fprintf(obj1, 'This is how to write again');

while (~endPrgrm)
    
    Disp('Enter Command');
    
    megaData = fscanf(obj1);
    
end
        

% Disconnect all objects.
fclose(obj1);

% Clean up all objects.
delete(obj1);
solidTestGUI()