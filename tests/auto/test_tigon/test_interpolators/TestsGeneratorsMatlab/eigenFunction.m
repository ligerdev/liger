function psi = eigenFunction(z,n,Lz)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Returns a base function over the interval [0 Lz]. %
% The function is a sine wave with n peaks, where   %
% psi(0) = 0 and psi(Lz) = 0.                       %
% sum(psi^2 dz) = 1.                                %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
An = (2/Lz)^0.5;
psi = An*sin(pi*n*z/Lz);