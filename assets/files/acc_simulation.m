% Numerical Methods Final Project: Danny Radke, Justin Song, Nick Magnani
% PARAMETERS
scenario = 1; % Choose 1, 2, or 3
%Conversion Constants
mph_to_fts = 5280/3600; %converts mph to ft/s
% Simulation parameters
dt = 0.05; % time step in seconds
t_max = 60; % total simulation time in seconds
steps = t_max / dt;
time = (0:steps-1) * dt;
% PID constants
Kp = 0.1;
Kd = 0.7;
% Follower Car Unchanged Parameters
fol_initial_pos = 0; % ft
target_gap = 180; % ft
% Lead Vehicle Unchanged Parameters
lead_initial_pos = 400; % ft
% Changing Parameters for required scenarios:
switch scenario
    case 1 % Constant speed lead car
        lead_vel = ones(1, steps) * 55 * mph_to_fts;
        fol_initial_vel = 45; % mph
        fol_target_vel = 65; % mph
    case 2 % Sudden braking
        lead_vel = ones(1, steps) * 55 * mph_to_fts;
        brake_time = 10; % seconds
        brake_idx = round(brake_time / dt);
        brake_rate = -15; % ft/s² (reasonable heavy braking)
        for i = brake_idx:steps-1
            lead_vel(i+1) = max(lead_vel(i) + brake_rate * dt, 0);
        end
        fol_initial_vel = 45; %mph
        fol_target_vel = 55; %mph
    case 3 % Sinusoidal speed between 50 and 70 mph (reasonable highway speeds)
        lead_vel = 60 + 10 * sin(2 * pi * time / 30); % period = 30 sec
        lead_vel = lead_vel * mph_to_fts; % Convert to ft/s
        fol_initial_vel = 65; %mph
        fol_target_vel = 75; %mph
    otherwise
        error('Invalid scenario selected.');
end
% ADAPTIVE ALGORITHM
%Velocity from mph to ft/s
fol_initial_vel= fol_initial_vel*mph_to_fts;
fol_target_vel= fol_target_vel*mph_to_fts;
% Vector Initialization
lead_pos = zeros(1, steps);
lead_pos(1) = lead_initial_pos;
fol_pos = zeros(1, steps);
fol_pos(1) = fol_initial_pos;
fol_vel = zeros(1, steps);
fol_vel(1) = fol_initial_vel;
gap = zeros(1, steps);
% Simulation loop
for i = 2:steps
    % Lead car
    lead_pos(i) = lead_pos(i-1) + lead_vel(i-1) * dt;
    % Calculate gap and error
    gap(i-1) = lead_pos(i-1) - fol_pos(i-1);
    error = gap(i-1) - target_gap;
    if i == 2
        prev_error = error;
    end
    d_error = (error - prev_error) / dt;
    % PID acceleration
    accel = Kp * error + Kd * d_error;
    prev_error = error;
    % Clamp acceleration to realistic values (+10 ft/s², -15 ft/s²)
    accel = max(min(accel, 10), -15);
    % Euler for follower velocity
    fol_vel(i) = fol_vel(i-1) + accel * dt;
    % Ensure follower speed doesn't exceed the target speed or go below
    % zero
    fol_vel(i) = max(0, min(fol_vel(i), fol_target_vel));
    % Euler for follower position
    fol_pos(i) = fol_pos(i-1) + fol_vel(i-1) * dt;
end
gap(steps) = lead_pos(steps) - fol_pos(steps);
% PLOTTING RESULTS
% Convert speeds from ft/s to mph for readability in plots
lead_speed_mph = lead_vel * 3600 / 5280;
fol_speed_mph = fol_vel * 3600 / 5280;
fol_target_vel_mph = fol_target_vel * 3600 / 5280;
% PLOT 1: Lead Car Speed
figure;
plot(time, lead_speed_mph, 'r-', 'LineWidth', 2);
xlabel('Time (s)');
ylabel('Speed (mph)');
title('Lead Car Speed vs. Time');
ylim([0 80]);
grid on;
% PLOT 2: Follower Car Speed
figure;
plot(time, fol_speed_mph, 'b-', 'LineWidth', 2);
yline(fol_target_vel_mph, '-r', 'Target Speed');
xlabel('Time (s)');
ylabel('Speed (mph)');
title('Follower Car Speed vs. Time');
ylim([0 80]);
grid on;
% PLOT 3: Gap Between Cars
figure;
plot(time, gap, 'k-', 'LineWidth', 2);
yline(target_gap, '-r', 'Target Gap');
xlabel('Time (s)');
ylabel('Gap (ft)');
title('Gap Between Cars vs. Time');
ylim([0 450]);
grid on;
