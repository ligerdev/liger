%#<TIGON_MATLAB_FUNC_HEADER_START>
%# {
%#  "functionProperties": {
%#      "name": "PI control problem",
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
%#      "name": ["CloseLoopStability", "GainMargin", "PhaseMarginMin", "PhaseMarginMax", "RiseTime", "PeakTime", "Overshoot", "Undershoot", "SettlingTime", "SteadyStateError"],
%#      "description": ["CloseLoopStability", "GainMargin", "PhaseMarginMin", "PhaseMarginMax", "RiseTime", "PeakTime", "Overshoot", "Undershoot", "SettlingTime", "SteadyStateError"],
%#      "type": ["real", "real", "real", "real", "real", "real", "real", "real", "real", "real"],
%#      "unit": ["", "", "", "", "", "", "", "", "", ""]
%#  }
%# }
%#<TIGON_MATLAB_FUNC_HEADER_END>

%
% Simulation model for a PI controller.
%
% Z = evaluateControlSystem(X);
%
% Input:  X - a sampling plan
%             (one design per row, one design variable per column)
% Output: Z - performance evaluations
%             (one design per row, one criterion per column;
%              criteria are...
%              1: maximum closed-loop pole magnitude
%              2: gain margin
%              3: phase margin (minimization)
%              4: phase margin (maximization)
%              5: 10-90% rise time
%              6. peak time
%              7. overshoot (% points)
%              8. undershoot (% points)
%              9. 2% settling time
%             10. steady-state error (% points))
%

function varargout = evaluateControlSystem( varargin )
% ----------------------------------------------------------
X = cell2mat(varargin);
y = ogata_function(X);
% ----------------------------------------------------------
if nargout == 1 
    varargout{1} = y;
else
    varargout = num2cell(y);
end
end

function Z = ogata_function(X);

[noInds, noVar] = size(X);
Z = NaN * ones(noInds,10);

warning off

for ind = 1:noInds
 
    % Extract the candidate controller gains.
  kp=X(ind,1);
  ki=X(ind,2);
    
  % Form the open-loop transfer function.
  olNum = [(kp+ki)*(1-exp(-1)) ...
           -kp*(1-exp(-1))];
  
  olDen = [1 ...
           -1-exp(-1) ...
           exp(-1) ...
           0 ...
           0];
  
  % Form the closed-loop transfer function.
  clNum=[(1-exp(-1))*(kp+ki) ...
         -(kp/(kp+ki))*(1-exp(-1))*(kp+ki)];
  clDen=[1 ...
         -(1+exp(-1)) ...
         exp(-1) ...
         (1-exp(-1))*(kp+ki) ...
	 -(kp/(kp+ki))*(1-exp(-1))*(kp+ki)];
 
  % Get stability measure.
  sPoles = roots(clDen);
  clStable = max(abs(sPoles));
      
  olTF = tf(olNum, olDen, 1.0);
  
  [gainMargin, phaseMargin, wcGP, wcPM] = margin(olTF);
  
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
  Z(ind,1) = clStable;
  Z(ind,2) = min(20*log10(gainMargin),100);
  Z(ind,3) = min(phaseMargin,180);
  Z(ind,4) = min(phaseMargin,180);
  Z(ind,5) = min(riseTime,1000);
  Z(ind,6) = min(overTime,1000);
  Z(ind,7) = min(100*overshoot,1000);
  Z(ind,8) = min(100*undershoot,1000);
  Z(ind,9) = min(settleTime,1000);
  Z(ind,10) = min(100*ssError,1000);

  warning on

end
end

