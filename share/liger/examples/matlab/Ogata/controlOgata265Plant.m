%#<TIGON_MATLAB_FUNC_HEADER_START>
%# {
%#  "functionProperties": {
%#      "name": "A-4-8",
%#      "description": "K.Ogata example problem A-4-8 pg.265"
%#  },
%#  "input": {
%#      "name": ["Kp", "Ki"],
%#      "description": ["Kp", "Ki"],
%#      "type": ["real", "real"],
%#      "unit": ["", ""],
%#      "lbounds": [0.0001, 0.0001],
%#      "ubounds": [10.0, 10.0]
%#  },
%#  "output": {
%#      "name": ["CloseLoopStability", "CloseLoopGainMargin", "CloseLoopPhaseMargin", "RiseTime", "PeakTime", "Overshoot", "Undershoot", "SettlingTime", "SteadyStateError"],
%#      "description": ["CloseLoopStability", "CloseLoopGainMargin", "CloseLoopPhaseMargin", "RiseTime", "PeakTime", "Overshoot", "Undershoot", "SettlingTime", "SteadyStateError"],
%#      "type": ["real", "real", "real", "real", "real", "real", "real", "real", "real"],
%#      "unit": ["", "", "", "", "", "", "", "", ""]
%#  }
%# }
%#<TIGON_MATLAB_FUNC_HEADER_END>
%
% Return objectives for PI control of plant given in Ogata p265.
%
% All suitable for minimisation.
%
% 1: Closed-loop stability
% 2: - Gain margin
% 3. - Phase margin
% 4. Rise time
% 5. Peak time
% 6. Overshoot
% 7. Undershoot
% 8. Settling time
% 9. Steady-state error
%

function varargout = controlOgata265Plant( varargin )
% ----------------------------------------------------------
x = cell2mat(varargin);
y = ogata_function(x);
% ----------------------------------------------------------
if nargout == 1 
    varargout{1} = y;
else
    varargout = num2cell(y);
end
end

function f = ogata_function(x)

n=size(x,1);
Kp=x(:,1);
Ki=x(:,2);
f = NaN * ones(n,9);


warning off
for i = 1:n
  % Closed-loop system.
  clNum=[(1-exp(-1))*(Kp(i)+Ki(i)) ...
          -(Kp(i)/(Kp(i)+Ki(i)))*(1-exp(-1))*(Kp(i)+Ki(i))];
  
  clDen=[ 1 ...
         -(1+exp(-1)) ...
          exp(-1) ...
          (1-exp(-1))*(Kp(i)+Ki(i)) ...
         -(Kp(i)/(Kp(i)+Ki(i)))*(1-exp(-1))*(Kp(i)+Ki(i))];
 
      
  % Get stability measure.
  sPoles = roots(clDen);
  clStable = max(abs(sPoles));
      
  clTF = tf(clNum, clDen, 1.0);
  
  [clGM, clPM, wcGP, wcPM] = margin(clTF);
  
  % Do a unit step response.
  timeData = [0:1:100];
  outputData = dstep(clNum, clDen, timeData);
  
  % Collect results where possible (stable).
  if clStable < 1
    riseTime = getRiseTime(timeData, outputData, outputData(end));
    settleTime = getSettlingTime(timeData, outputData, 0.02, ...
						outputData(end));
  else
    riseTime = getRiseTime(timeData, outputData, 1.0);
    settleTime = getSettlingTime(timeData, outputData, 0.02, 1.0);
  end
  [overshoot, overTime, undershoot, underTime] = getShoots(timeData, outputData);
  ssError = getSSError(outputData);
  
  
  % Assign to output variable.
  if clStable < 1 
      f(i,1) = clStable;
  else
      f(i,1) = 10e+9;
  end
  if clGM == inf
      f(i,2) = 10e+9;
  else
      f(i,2) = -clGM;
  end
  f(i,3) = -clPM;
  f(i,4) = riseTime;
  f(i,5) = overTime;
  f(i,6) = overshoot;
  f(i,7) = undershoot;
  if settleTime == inf 
      f(i,8) = 10e+9;
  else
      f(i,8) = settleTime;
  end
  f(i,9) = ssError;
end
warning on
end

