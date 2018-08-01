%
% getRiseTime.m
%
% Calculate the rise time for a unit step input.
%
% Inputs: tData    - simulation time values
%         oData    - corresponding response values
%         finalVal - the reference value to rise to
%         lVal     - lower bound for rise time (default is 0.1)
%         uVal     - upper bound for rise time (default is 0.9)
% Output: the rise time
%
function riseTime = getRiseTime(tData, oData, finalVal, lVal, uVal);

badvals = 0;
if nargin == 5
  if lVal < 0 | lVal > 1 | uVal < lVal | uVal > 1
    disp('Rise time bounds are unacceptable. Using defaults.');
    badvals = 1;
  end
end

if nargin < 5 | badvals == 1
  % Set defaults.
  lVal = 0.1;
  uVal = 0.9;
end
  
% Express bounds in response format.
lVal = lVal * finalVal;
uVal = uVal * finalVal;

% Find the time to reach upper limit.
indexUp = min(find(oData > uVal));

if ~isempty(indexUp)
  
  % Linearly interpolate for greater accuracy.
  if indexUp ~= 1
    timeUp = tData(indexUp-1) + (tData(indexUp) - tData(indexUp-1)) * ...
	     (uVal - oData(indexUp-1)) / (oData(indexUp) - oData(indexUp-1)); 
  else
    timeUp = tData(indexUp);
  end
  
  % Find the time to reach the lower bound.
  indexDown = min(find(oData > lVal));
  
  % NB: This index will exist, if we passed the test for the upper bound.
  
  if indexDown ~= 1
    timeDown = tData(indexDown-1) + (tData(indexDown) - tData(indexDown-1)) * ...
	     (lVal - oData(indexDown-1)) / (oData(indexDown) - oData(indexDown-1));
  else
    timeDown = tData(indexDown);
  end
  
  % Calculate rise-time.
  riseTime = timeUp - timeDown;
  
else
  disp('Response failed to reach upper rise time bound.');
  riseTime = Inf;
end