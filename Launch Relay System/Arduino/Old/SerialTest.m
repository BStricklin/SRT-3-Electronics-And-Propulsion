%ARDUINO Serial Comm attempt
clc;
clear;

a1 = serial('/dev/tty.wchusbserial1410','BaudRate',9600);
a1.Terminator = 10;
fopen(a1);
pause(0.3);

%Say hello first
fprintf(a1, 'HELLO\n'); 

%Wait for a result to be calculated and sent
response = fgetl(a1);
%Make Sure response says "Handshake"
if (strcmpi(response,'HS'))
    fprintf('\nHandshake Passed.\n');
else
    fprintf('\nHandshake Failed.');
end


%Go ahead and close serial port
fclose(a1);
delete(a1);
