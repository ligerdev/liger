function [cdf,pdf,z] = linearProbability(start,finish,direction,N)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Returns a triangular increasing distribution between %
% [START FINISH] over the range z \in [0 1].           %
% If START < 0 or FINISH >  1, they are truncated      %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if nargin == 3
    if ~strcmp(direction,'increase') && ~strcmp(direction,'decrease')
        error('DIRECTION must be either ''increase'' of ''decrease''');
    end
elseif nargin < 3
    direction = 'increase';
end
if nargin < 2 || finish > 1 || finish <= start
    finish = 1;
end
if nargin < 1 || start < 0 || start >=1
    start = 0;
end
dz = 1/N; 
z = (0:dz:1)';
pdf = zeros(size(z));
range = z>=start & z<= finish;
if strcmp(direction,'increase')
    pdf(range) = (z(range) - start)*2 / (finish-start)^2;
else
    pdf(range) = (finish - z(range))*2 / (finish-start)^2;
end
cdf = cumtrapz(z,pdf);
cdf = cdf / cdf(end);
