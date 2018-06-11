function [ varargout ] = MatlabTigonWrapperExample( varargin )
%A wrapper around matlab commands to allow Tigon to utilise Matlab functions
% This wrapper provides an interface for Tigon to utilise Matlab functions. When called with no
% arguments and a single output, the funciton returns a structure of struct arrays detailing the
% required inputs and outputs for the function.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Copyright (C) 2012-2016 The University of Sheffield (www.sheffield.ac.uk)
%
% This file is part of Liger.
%
% GNU Lesser General Public License Usage
% This file may be used under the terms of the GNU Lesser General
% Public License version 2.1 as published by the Free Software
% Foundation and appearing in the file LICENSE.LGPL included in the
% packaging of this file.  Please review the following information to
% ensure the GNU Lesser General Public License version 2.1 requirements
% will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if nargin == 0 && nargout == 1
    %Property structure is defined here
    %Informs Tigon of input and output properties for this function.

    %Property struct must have three fields: "funcPrpts", "input" and "output"
    %Both "input" and "output" must be struct arrays where each element is a seperate variable. The order of the struct array 
    %must be the same as the order expected by the input and provided by the output.
    %The structures in the array must contain the following fields:
    %    name (STRING):        This is the name given to the variable in Tigon.
    %    description (STRING): A description for what the variable represents.
    %    type (STRING):        The type of value being passed. Can be "int", "real", "binary" or "complex".
    %    unit (STRING):        The scientific units of measurement for the variable. If not available use "none" or "n/a".
    %    Optional field only for "input":
    %    upperBound (DOUBLE):  The maximum allowed input value for the variable.
    %    lowerBound (DOUBLE):  The lowest allowed input value for the variable.
    %
    %Both "upperBound" and "lowerBound" can have an +-infinte value indicating no bounds on the input. Although MATLAB supports
    %positive and negative infinity, Tigon does not. An infintie value in the upperBound field will be converted to the maximum
    %possible value supported by Tigon. A positive or negative value in the lowerBound field will be converted to the lowest
    %supported value in Tigon. "upperBound" and "lowerBound" fileds can be omitted but if present in variable they must be present
    %for all variables. 
    

    funcPrpts = struct('name', 'ExampleFunc', ...
                       'description', 'Example Matlab Version');

    Inputs = struct('name', {'intIn', 'realIn', 'boolIn', 'complexIn'}, ...
                    'description', {'Integer', 'Real', 'Boolean', 'Complex'}, ...
                    'type', {'int', 'real', 'binary', 'complex'}, ...
                    'unit', {'mm', 'm', 'n/a', 'none'}, ...
                    'upperBound', {100 , 50, 1, Inf}, ...  
                    'lowerBound', {-100, -50, 0, -Inf});

    Outputs = struct('name', {'intOut', 'realOut', 'boolOut', 'complexOut'}, ...
                     'description', {'Integer', 'Real', 'Boolean', 'Complex'}, ...
                     'type', {'int', 'real', 'binary', 'complex'}, ...
                     'unit', {'mm', 'mm', 'n/a', 'none'});

    varargout{1} = struct('functionProperties', funcPrpts, 'input', Inputs, 'output', Outputs);
else    
    % Do any work here. 
    varargout{1} = varargin{1};

end

end

