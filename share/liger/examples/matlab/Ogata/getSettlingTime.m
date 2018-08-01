%
% getSettlingTime.m
%
% Returns the settling time.
%
% settlingTime = getSettlingTime(tData, oData, band, finalVal) 
%
% Inputs: tData    - time values
%         oData    - corresponding response values
%         band     - percentage to settle within, expressed in [0 1]
%         finalVal - the centre of the band
% Output: ssError  - steady-state error (as a percentage of unit step)
%
function settlingTime = getSettlingTime(tData, oData, band, finalVal);

sizeData = max(size(oData));
  
% Calculate the limits we need to settle within in oData terms.
aboveVal = finalVal * (1+band);
belowVal = finalVal * (1-band);

% Search for the final time we're above and below these limits.
lastAbove = max(find(oData>=aboveVal));
lastBelow = max(find(oData<=belowVal));

% Find the latest one of these and perform interpolation.
lastOutside = max([lastAbove lastBelow]);
if lastOutside == lastBelow
  interpVal = belowVal;
else
  interpVal = aboveVal;
end

if lastOutside == sizeData
  % We didn't get convergence within the data provided.
  settlingTime = Inf;
  warning('Response did not settle within time provided.');
else
  settlingTime = tData(lastOutside) + (tData(lastOutside+1) - tData(lastOutside)) ...
      * (interpVal - oData(lastOutside)) / (oData(lastOutside+1) - oData(lastOutside));
end