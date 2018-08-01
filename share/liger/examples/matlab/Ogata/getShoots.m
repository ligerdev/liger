%
% getPeakInfo.m
%
% Returns the time at which the response peaks.
%
% [overshoot overTime undershoot underTime] = getShoots(tData, oData) 
%
% Inputs: tData      - simulation time values
%         oData      - corresponding response values
% Output: overshoot  - the maximum overshoot
%         overTime   - the time when the overshoot occurred
%         undershoot - the maximum undershoot following the overshoot
%         underTime  - the time when this occurred
%
% Note: This function won't give sensible answers for unstable systems
%       and may give incorrect answers if step response data isn't sufficient
%
function [overshoot, overTime, undershoot, underTime] = getShoots(tData, oData);

% OVERSHOOT
% Find the maximum value in the response data.
[overshoot, indexMax] = max(oData);

% Get the corresponding time.
overTime = tData(indexMax);

% Express the peak value as a [0 1] percentage
if overshoot > 1
  overshoot = overshoot - 1;
else
  overshoot = 0;
end

% UNDERSHOOT
% Check that we've got some data left to play with.
if overTime == tData(end)
  undershoot = 0;
  underTime = Inf;
else
  [undershoot, indexMin] = min(oData(indexMax+1:end));
  underTime = tData(indexMin+indexMax);
  if undershoot < 1
    undershoot = 1 - undershoot;
  else
    undershoot = 0;
    underTime = Inf;
  end
end
