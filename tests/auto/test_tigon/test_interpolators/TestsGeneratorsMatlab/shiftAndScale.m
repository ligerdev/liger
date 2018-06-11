function [PDF,zNew] = shiftAndScale(unitPDF,lowerBound,upperBound,z)
if nargin < 4
    z = linspace(0,1,size(unitPDF,1))';
elseif length(z) ~= size(unitPDF,1)
    error('Domain and probability functions must have the same dimentions');
end
if nargin < 3
    lowerBound = 0;
end
if upperBound <= lowerBound
    error('Upper bound must be larger than Lower bound');
end
rangeOld = z(end)-z(1);
rangeNew = upperBound - lowerBound;

PDF = unitPDF * rangeOld / rangeNew;
zNew = lowerBound + ( (z-z(1))*(upperBound-lowerBound) ) / (z(end)-z(1));