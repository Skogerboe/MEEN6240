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
Itest = zeros(100);
type = 0;

has_quit = false;
% menu loop
while ~has_quit
    fprintf('PIC32 MOTOR DRIVER INTERFACE\n\n');
    % display the menu options; this list will grow
    fprintf([ ...
        '     d: Get Encoder Counts (Degrees)           x: Integer SUM\n' ...
        '     c: Get Encoder Counts                     e: Reset Encoder Counts\n' ...
        '     a: Get ADC Ticks (0-1023)                 b: Get Current Value (mA)\n' ...
        '     r: Get Current State of Motor             f: Set PWM (-100 to 100)\n' ...
        '     p: Stop Motor                             g: Set Current Loop Gains\n' ...
        '     h: Get Current Loop Gains                 k: Test Current Loop Gains\n' ...
        '     i: Set Position Loop Gains                j: Get Position Loop Gains\n' ...
        '     l: Step to Angle (Test P-Loop Gains)      m: Set Up Step Trajectory\n' ...
        '     n: Set up Cubic Trajectory                o: Execute Trajectory\n' ...
        '                               q: Quit\n                                       ']);
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
            read_plot_matrix(mySerial);
        case 'i'
            Pgain2 = input('Enter P Gain: ');
            Igain2 = input('Enter I gain: ');
            Dgain2 = input('Enter D gain: ');
            PIDgain1 = [Pgain2; Igain2; Dgain2];
            fprintf(mySerial, "%f %f %f\n", PIDgain1);
            ack_j = fscanf(mySerial, '%d');
            if ack_j == 1
                fprintf('Position Control Mode Gains Set.\n');
            else
                fprintf('Error - Position Control Mode Gains Not Set.\n');
            end
        case 'j'
            PIDgainj = fscanf(mySerial, '%f %f %f');
            Pgainj = PIDgainj(1);
            Igainj = PIDgainj(2);
            Dgainj = PIDgainj(3);
            fprintf('P Gain: %f\n', Pgainj);
            fprintf('I Gain: %f\n', Igainj);
            fprintf('D Gain: %f\n', Dgainj);
        case 'l'
            Angleref = input('Enter a desired angle: ');
            fprintf(mySerial, "%d\n", Angleref);
        case 'm'
            mSamples = input('Enter number of steps to track: ');
            trajectory = zeros(mSamples, 2);
            type = 0;   %Step
            for i = 1:1:mSamples
                trajectory(i, 1) = input('Insert Time of step: ');
                trajectory(i, 2) = input('Insert Position of step: ');
            end
            position_m = genRef(trajectory, 'step');
            fprintf(mySerial,'%d\n',length(position_m));
            for i=1:length(position_m)
                fprintf(mySerial,'%f\n',position_m(i));
            end
            m_ack=fscanf(mySerial,'%d');
            if m_ack == 1
                fprintf('Data Recieved\n');
            else
                fprintf('Data Not Received\n');
            end
        case 'n'
            nSamples = input('Enter number of steps to track: ');
            trajectory = zeros(nSamples, 2);
            type = 1;   %Cubic
            for i = 1:1:nSamples
                trajectory(i, 1) = input('Insert Time of step: ');
                trajectory(i, 2) = input('Insert Position of step: ');
            end
            position_n = genRef(trajectory, 'cubic');
            fprintf(mySerial,'%d\n',length(position_n));
            for i=1:length(position_n)
                fprintf(mySerial,'%f\n',position_n(i));
            end
            plot(position_n);
            n_ack=fscanf(mySerial,'%d');
            if n_ack == 1
                fprintf('Data Recieved\n');
            else
                fprintf('Data Not Received\n');
            end
        case 'o'
            read_plot_matrix_position(mySerial, type);
        case 'q'
            has_quit = true;             % exit client
        otherwise
            fprintf('Invalid Selection %c\n', selection);
    end
end

end
