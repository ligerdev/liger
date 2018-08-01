%
% getPeakInfo.m
%
% Returns the time at which the response peaks.
%
% [peakTime, peakVal] = getPeakInfo(tData, oData) 
%
% Inputs: tData    - simulation time values
%         oData    - corresponding response values
% Output: peakTime - the peak time
%         peakVal  - the peak value (as a percentage of unit step)
%
function [peakTime, peakVal] = getPeakInfo(tData, oData);

% Find the maximum value in the response data.
[peakVal, indexMax] = max(oData);

% Get the corresponding time.
peakTime = tData(indexMax);

% Express the peak value as a [0 1] percentage
if peakVal > 1
  peakVal = peakVal - 1;
else
  peakVal = 0;
end