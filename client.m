function client(port)
%   provides a menu for accessing PIC32 motor control functions
%
%   client(port)
%
%   Input Arguments:
%       port - the name of the com port.  This should be the same as what
%               you use in screen or putty in quotes ' '
%
%   Example:
%       client('/dev/ttyUSB0') (Linux/Mac)
%       client('COM3') (PC)
%
%   For convenience, you may want to change this so that the port is hardcoded.
   
% Opening COM connection
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

fprintf('Opening port %s....\n',port);

% settings for opening the serial port. baud rate 230400, hardware flow control
% wait up to 120 seconds for data before timing out
mySerial = serial(port, 'BaudRate', 230400, 'FlowControl', 'hardware','Timeout',120); 
% opens serial connection
fopen(mySerial);
% closes serial port when function exits
clean = onCleanup(@()fclose(mySerial));                                 

has_quit = false;
% menu loop
while ~has_quit
    fprintf('PIC32 MOTOR DRIVER INTERFACE\n\n');
    % display the menu options; this list will grow
    fprintf('     d: Get Encoder Counts (Degrees)      x: Integer SUM      c: Get Encoder Counts     e: Reset Encoder Counts    a: Get ADC Ticks (0-1023)    b: Get Current Value (mA)     q: Quit\n');
    % read the user's choice
    selection = input('\nENTER COMMAND: ', 's');
     
    % send the command to the PIC32
    fprintf(mySerial,'%c\n',selection);
    
    % take the appropriate action
    switch selection
        case 'd'                         % encoder counts in degrees
            n = fscanf(mySerial,'%d');   
            fprintf('Motor Degrees from Zero: %d degrees\n',n); 
        case 'x'
            n1 = input('Enter first integer to sum: ');
            n2 = input('Enter second integer to sum: ');
            nums = [n1; n2];
            fprintf(mySerial, '%d %d\n',nums);
            n = fscanf(mySerial,'%d');
            fprintf('SUM: %d\n', n);
        case 'c'
            %%fprintf(mySerial, 'c');
            enc = fscanf(mySerial, '%d');
            fprintf('Encoder Count: %d\n', enc);
        case 'e'
            %%fprintf(mySerial, 'e');
            enc_reset = fscanf(mySerial, '%d');
            fprintf('Encoder Count Reset to: %d\n', enc_reset);
        case 'a'
            adc_ticks = fscanf(mySerial, '%d');
            fprintf('ADC ticks from 0-1023 is: %d\n', adc_ticks);
        case 'b'
            adc_ma = fscanf(mySerial, '%d');
            fprintf('ADC result in mA is: %d mA\n', adc_ma);
        case 'r'
            r = fscanf(mySerial, '%d');
            if r == 0
                mode = 'IDLE';
            elseif r == 1
                mode = 'PWM';
            elseif r == 2
                mode = 'ITEST';
            elseif r == 3
                mode = 'HOLD';
            elseif r == 4
                mode = 'TRACK';
            end
            fprintf('System is in %s mode\n', mode);
        case 'f'
            duty = input('Enter a Duty cycle from -100 to 100 (%): ');
            fprintf(mySerial, '%d\n', duty);
            f = fscanf(mySerial, '%d');
            fprintf('PWM freq: %d\n', f);
        case 'p'
            fprintf('Motor Stopping\n');
        case 't'
            test = fscanf(mySerial, '%d');
            fprintf('%d\n', test);
        case 'g'
            Pgain1 = input('Enter P Gain: ');
            Igain1 = input('Enter I Gain: ');
            PIgain = [Pgain1; Igain1];
            fprintf(mySerial, '%f %f\n', PIgain);
            ack_g = fscanf(mySerial, '%d');
            if ack_g == 1
                fprintf('Current Mode Gains Set.\n');
            else
                fprintf('Error - Current Mode Gains Not Set.\n');
            end
        case 'h'
            PIgainh = fscanf(mySerial, '%f %f');
            Pgainh = PIgainh(1);
            Igainh = PIgainh(2);
            fprintf('P Gain: %f\n', Pgainh);
            fprintf('I Gain: %f\n', Igainh);
        case 'k'

        case 'q'
            has_quit = true;             % exit client
        otherwise
            fprintf('Invalid Selection %c\n', selection);
    end
end

end
