function [out1] = test3_tigonBadStruct( varargin )
%test2_matlabTigonWrapper A test function for the interface between Matlab
%and Liger/Tigon

if nargin == 0 && nargout == 1
    %Tigon interface section.
    %Informs Tigon of input and output properties for this function.
    out1 = [1:10];
else    
    in1 = varargin{1};
    out1 = in1;

end

end