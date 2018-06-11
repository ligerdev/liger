function [cdf,pdf,z] = uniform(start,finish,N)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Returns an even distribution between [START FINISH]%
% Over the range z \in [0 1].                        %
% If START < 0 or FINISH >  1, they are truncated    %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if nargin < 2 || finish > 1 || finish <= start
    finish = 1;
end
if nargin < 1 || start < 0 || start >=1
    start = 0;
end
dz = 1/N; 
z = (0:dz:1)';
pdf = zeros(size(z));
pdf(z>=start & z<= finish) = 1/(finish-start);
cdf = cumtrapz(z,pdf);
cdf = cdf / cdf(end);
