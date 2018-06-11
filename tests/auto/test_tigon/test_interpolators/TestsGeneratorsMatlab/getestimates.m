function [x,y,xu,yu,xql,yql,xqs,yqs] = getestimates(xs,xe,N,NU,fname,p1)
x = linspace(xs,xe,N);
y = eval( [fname '(' num2str(p1) '.*x)'] );
xu= linspace(xs,xe,NU);
yu = eval( [fname '(' num2str(p1) '.*xu)'] );

% Linear Interpolation
xql = linspace(xs,xe,NU);
yql = interp1(x,y,xql,'linear');


% Cubic Spline Interpolation
xqs = xql;
yqs = interp1(x,y,xqs,'spline');
end

