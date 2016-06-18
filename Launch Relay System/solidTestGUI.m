function solidTestGUI()

%by Tommy Arrington
%GUI controller for solid motor tests - has ability for arm/disarm,
%continuity check, and ignition

clc; clear;
global state gObjects;

port = 0;   %com port for xbee/arduino
countdownLength = 10;   %default countdown length in seconds

%Appearance variables and structs with state information
grey = [0.8,0.8,0.8];   %RGB color definition
fs = 20;    %font size
cdL = abs(countdownLength) * -100; %countdown length, negative, in sec/100
state = struct('continuity',3, 'arm',1, 'launching',0, 'lTime',cdL,...
    'cdLength',cdL);

%{
NOTE ON "CONTINUITY" CODE
1-test failed
2-test passed
3-initial (no test done yet)

NOTE ON "ARM" CODE-
1-disarmed
3-armed
(2-armed at box, not used in Solid Test GUI)

NOTE ON "LAUNCHING" CODE
0-not launching
1-counting down (pre launch)
2-counting up (post launch)\
3-aborting

NOTE ON "lTime" CODE
-initially: (negative) # of seconds to countdown *1000;
-then: space in countdown (-) or countup (+)
%}
    
%Timer to run countdown/up
timeH = timer('Period',0.05, 'ExecutionMode','FixedRate',...
    'TimerFcn',@launchTick);

%Figure and axes
%Intended for square aspect ratio
fh = figure('Visible', 'off', 'Toolbar', 'none', 'Resize', 'off',...
    'Position', [150,150,600,600]);% 'DeleteFcn', @endSerial);
set(fh, 'Name', 'TAMU SRT-Solid Motor Launch Controller', 'NumberTitle',...
    'off', 'MenuBar', 'none');
set(fh, 'Color', grey);
ah = gca;
set(ah, 'Units', 'normalized', 'Position', [0,0,1,1], 'Visible', 'off');

%Menu bar
menuH = uimenu('Label', 'Settings');
uimenu(menuH, 'Label', 'Countdown', 'Callback', @countdownMenu);

%Create panels for arming, cont. check, igntion, and logo
p1 = uipanel(fh, 'BackgroundColor',grey, 'Position', [0,0.5,0.5,0.5]); %arm
p2 = uipanel(fh, 'BackgroundColor',grey, 'Position', [0.5,0.5,0.5,0.5]); %cont. check
p3 = uipanel(fh, 'BackgroundColor',grey, 'Position', [0,0.1,1,0.4]); %ingition
p4 = uipanel(fh, 'BackgroundColor',grey, 'Position', [0,0,1,0.1]); %logo

%Logo Panel
uicontrol(p4, 'Style','text', 'Units','normalized','String',...
    'SOUNDING ROCKETRY TEAM','Position',[0,0.66,1,0.37], 'BackgroundColor',...
    grey,'FontSize',(fs-8),'FontWeight','bold');
uicontrol(p4, 'Style','text', 'Units','normalized','String',...
    'Texas A&M University','Position',[0,0.33,1,0.33], 'BackgroundColor',...
    grey,'FontSize',(fs-8));
uicontrol(p4, 'Style','text', 'Units','normalized','String',...
    '2016','Position',[0,0,1,0.33], 'BackgroundColor',...
    grey,'FontSize',(fs-8));

%Arming Panel
uicontrol(p1, 'Style','text', 'Units','normalized','String','Status:',...
    'Position',[0,0.75,1,0.15], 'BackgroundColor',grey,'FontSize',fs);
t1 = uicontrol(p1, 'Style','text', 'Units','normalized','Position',...
    [0,0.6,1,0.15], 'BackgroundColor',grey);
b1 = uicontrol(p1, 'Style','pushbutton', 'Units','normalized','Position',...
    [0.1,0.05,0.8,0.45],'FontSize',fs,'Callback',@armButtonF);

%Continuity Check Panel
uicontrol(p2, 'Style','text', 'Units','normalized','String',...
    'Igniter Continuity','Position',[0,0.75,1,0.15], 'BackgroundColor',...
    grey,'FontSize',fs);
t2 = uicontrol(p2, 'Style','text', 'Units','normalized','Position',...
    [0.3,0.5,0.4,0.1],'FontSize',fs);
b2 = uicontrol(p2, 'Style','pushbutton', 'Units','normalized','String','Test',...
    'Position',[0.35,0.25,0.3,0.15],'FontSize',fs,...
    'Callback',@contButtonF);

%Ignition Panel
b3 = uicontrol(p3, 'Style','pushbutton', 'Units','normalized',...
    'Position',[0.15,0.2,0.7,0.7], 'FontWeight','bold',...
    'Callback',@igniterButtonF);
t3 = uicontrol(p3, 'Style','text', 'Units','normalized',...
    'Position',[0,0,1,0.15], 'BackgroundColor',grey, 'FontSize',fs);

%Struct with all changeable objects' handles
gObjects = struct('armTxt',t1, 'armButton',b1, 'contTxt',t2,...
    'contButton',b2, 'igniterButton',b3, 'countdownTxt',t3, 'grey',grey,...
    'fs',fs, 'timeH',timeH, 'menuH',menuH);

%Call redraw to set with initial state, make everything visible
reDraw();
set(fh, 'Visible', 'on');

%Start connection with serial port
% startSerial(port);

end

%reDraw Function - changes certain GUI elements based on state variables
function reDraw()
    global state gObjects;
    
    %Set appearance from armed state
    switch state.arm
        case 1  %disarmed
            set(gObjects.armTxt, 'String','Disarmed', 'Foregroundcolor','k',...
                'FontWeight','normal', 'FontSize',gObjects.fs);
            set(gObjects.armButton, 'String','Arm', 'Backgroundcolor','y');
            set(gObjects.igniterButton, 'enable','off', 'Backgroundcolor',...
                (gObjects.grey-[0.4,0.4,0.4]), 'String',...
                'Initiate Launch Sequence', 'FontSize',(gObjects.fs+5));
            state.launching = 0;    %prevent launch when disarmed
            %if the timer is running, stop it. Reset time
            if (strcmpi( get(gObjects.timeH,'Running'), 'on'))
                stop(gObjects.timeH);
            end
            state.lTime = state.cdLength;
            %Enable use of menu bar (for settings)
            set(gObjects.menuH, 'enable','on');
        case 3  %armed
            set(gObjects.armTxt, 'String','ARMED', 'Foregroundcolor','r',...
                'FontWeight','bold', 'FontSize',(gObjects.fs+8));
            %If launch/abort occured, button reads "Disarm and Reset",
            %else: "Disarm"
            if (state.launching == 2 || state.launching == 3)
                set(gObjects.armButton, 'String','Disarm and Reset');
            else
                set(gObjects.armButton, 'String','Disarm');
            end
            set(gObjects.armButton, 'Backgroundcolor','w');
            set(gObjects.igniterButton, 'enable','on');
            %Disable use of menu bar (cannot change settings when armed)
            set(gObjects.menuH, 'enable','off');
    end
    
    %Set appearance from continuity state
    switch state.continuity
        case 1  %failed
            set(gObjects.contTxt, 'String','Fail', 'Backgroundcolor','r');
        case 2  %passed
            set(gObjects.contTxt, 'String','Pass', 'Backgroundcolor','g');
        otherwise   %initial-nothing
            set(gObjects.contTxt, 'String','', 'Backgroundcolor',gObjects.grey);
    end
    
    
    %Print countdown time as formatted string
    cdTemp = sprintf('T %+06.2f seconds', (state.lTime/100));
    set(gObjects.countdownTxt, 'String',cdTemp);
    
    %Set appearance from Launch/Abort State if disarmed
    if (state.arm == 3)
        switch state.launching
            case 0 %not launching
                set(gObjects.igniterButton, 'String','Initiate Launch Sequence',...
                    'FontSize',(gObjects.fs+5), 'ForegroundColor','k',...
                    'BackgroundColor','r');
            case 1  %counting down
                set(gObjects.igniterButton, 'String','ABORT', 'FontSize',...
                    (gObjects.fs+20), 'ForegroundColor','w');
                %lock out arm button and cont. test button
                set(gObjects.armButton, 'enable','off');
                set(gObjects.contButton, 'enable','off');
            case 2  %counting up - launched
                set(gObjects.igniterButton, 'String','Launched', 'FontSize',...
                    (gObjects.fs+20), 'ForegroundColor','k', 'BackgroundColor',...
                    'g');
                %Re-enable continutity check and arm buttons
                set(gObjects.armButton, 'enable','on');
                set(gObjects.contButton, 'enable','on');
            case 3  %aborted
                set(gObjects.igniterButton, 'String', 'Launch Aborted', 'FontSize',...
                    (gObjects.fs+14), 'ForegroundColor','k');
                %Re-enable continutity check and arm buttons, stop
                %countdown
                set(gObjects.armButton, 'enable','on');
                set(gObjects.contButton, 'enable','on');
                stop(gObjects.timeH);
        end
    end

            

end

%Callback functions for menu bar
function countdownMenu(hObject, callbackdata)
    global state;
    
    %use an input dlg to get countdown length
    cdL = inputdlg('Countdown Length (seconds)', 'Countdown Settings',...
        [1 40], {num2str(abs(state.cdLength/100))});
    if (~isempty(cdL) && ~isnan(str2double(cdL))) %make sure cancel was not selected 
        state.cdLength = abs(str2double(cdL)) * -100;
    end
    
    reDraw();
end

%Callback functions for buttons
function armButtonF(hObject, callbackdata)
    global state;
    
    %Arm/disarm based on current state
    if (state.arm == 1) %disarmed - arm box
        if (sendArm() == 1)
            state.arm = 3;
        end
        
    else    %armed - disarm box
        if (sendDisarm() == 1)
            state.arm = 1;  
        end
    end
    reDraw();   %update graphics
end

function contButtonF(hObject, callbackdata)
    global state;
    
    state.continuity = continuityCheck();   %software test case
    reDraw();   %update graphics
end

function igniterButtonF (hObject, callbackdata)
    global gObjects state;
    
    %Switch to determine how to set state (ignite or abort)
    ls = state.launching;   %local launch state var.
    switch ls
        case 0  %start countdown
            state.launching = 1;
            start(gObjects.timeH);
        case 1 %abort-button was trigged in countdown
            abortFcn();
            state.launching = 3;
    end
    
    reDraw();
end

%Callback function for timer
function launchTick(hObject, callbackdata)
    global state;
    
    %Add 1 to lTime increment (add 5 if pd of 0.05 is used
    state.lTime = state.lTime + 5;
    
    %If lTime=0, call ignition command and if successful, switch to countup
    if (state.lTime==0 && state.arm==3 && state.launching==1)
        if (sendIgnite() == 1)
            state.launching = 2;
        end
    end
    
    reDraw();

end

%Function to ABORT
function abortFcn()
    %global state;
    
    %***ADD ANY OTHER ABORT COMMANDS HERE***

end





%FUNCTIONS FOR SERIAL COMMUNICATION

%Initiate communication with serial port
%-takes in (port), the port ID of xbee/Arduino connection
%-return [success]: 1 for good communication test, else 0
%-creates global var. sObj: the handle for the serial connection
function success = startSerial(port)
    %{
    global sObj;
    
    sObj = serial(port);
    fopen(sObj);
    %}
    success = 1;
end

%Send message to ARM
%-return [success]: 1 for successful transmission, else 0
%sObj is global handle for serial connection
function success = sendArm()
    %global sObj;
    
    success = 1;
end

%Send message to DISARM
%-return [success]: 1 for successful transmission, else 0
%sObj is global handle for serial connection
function success = sendDisarm()
    %global sObj;
    
    success = 1;
end

%Send message to TEST CONTINUITY
%-return [result]: 1 for fail, 2 for pass, 0 for bad transmit/receive
%sObj is global handle for serial connection
function result = continuityCheck()
    %global sObj;
    
    result = 2;
end

%Send message to IGNITE
%-return [success]: 1 for successful transmission, else 0
%sObj is global handle for serial connection
function success = sendIgnite()
    %global sObj;
    
    success = 1;
end


%End serial connection automatically when window closes
%-(callback function for figure)
function endSerial(hObject, callbackdata)
    %global sObj;
    %fclose(sObj);
end