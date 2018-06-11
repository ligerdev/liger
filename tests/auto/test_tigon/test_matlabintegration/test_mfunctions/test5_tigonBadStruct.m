function [out1] = test5_tigonBadStruct( varargin )
%test2_matlabTigonWrapper A test function for the interface between Matlab
%and Liger/Tigon

if nargin == 0 && nargout == 1
    %Tigon interface section.
    %Informs Tigon of input and output properties for this function.
    
    Inputs = struct('name', {'intIn', 'realIn', 'realIn', 'intIn'}, ...
                    'description', {'Integer', 'Real', 'Real', 'Integer'}, ...
                    'type', {'int', 'real', 'real', 'int'}, ...
                    'unit', {'mm', 'mm', 'n/a', 'none'}, ...
                    'upperBound', {1000 , 30.6, 1, Inf}, ...  
                    'lowerBound', {-100, -20, 0, -Inf});
    Outputs = struct('description', {'Integer', 'Real', 'Real', 'Integer'}, ... %Missing 'name' field
                     'type', {'int', 'real', 'real', 'int'}, ...
                     'unit', {'mm', 'mm', 'n/a', 'none'});
    out1 = struct('input', Inputs, 'output', Outputs);
else    
    in1 = varargin{1};
    out1 = in1;
end

end