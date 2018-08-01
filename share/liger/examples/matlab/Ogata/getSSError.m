%
% getSSError.m
%
% Returns the absolute steady-state error.
% NB: Just compares final value provided with unity.
%
% ssError = getSSError(oData) 
%
% Inputs: oData    - response values
% Output: ssError  - steady-state error (as a percentage of unit step)
%
function ssError = getSSError(oData);

ssError = abs(oData(end)-1);