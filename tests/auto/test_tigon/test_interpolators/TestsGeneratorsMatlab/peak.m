function [cdf,pdf,z,psiN] = peak(width,mean,N)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Returns a distribution function with a single peak %
% Over the range z \in [0 1].                       %
% Probability at z=0 and z=1 is 0.                  %
% WIDTH of the distribution in the range [0.1 1]    %
% MEAN controls the position of the peak:            %
% For MEAN=0 the peak starts at z=0, for MEAN=1      %
% the peak ends at z=1.                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if nargin < 2
    mean = 0.5;
end
if nargin < 1
    width = 1;
end
dz = 1/N;
z = (0:dz:1)';
shift = mean*pi;
N = 1/width;
psiN = zeros(size(z));
Lz = z(end);
for n = 1:max(30,N*3)
    cNn = ( N^n * exp(-N) / factorial(n) )^0.5;
    psi_n = eigenFunction(z,n,Lz);
    psiN = psiN + cNn * exp(-1i*shift*(n+0.5)) * psi_n;
end
psiN = psiN / (psiN' * psiN * dz)^0.5;
pdf = conj(psiN) .* psiN;
cdf = cumtrapz(z,pdf);
% account for rounding errors
pdf = pdf/cdf(end);
cdf = cdf/cdf(end);
