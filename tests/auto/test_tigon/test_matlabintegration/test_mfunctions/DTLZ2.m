%#<TIGON_MATLAB_FUNC_HEADER_START>
%# {
%# 	"functionProperties": {
%# 		"name": "DTLZ2",
%# 		"description": "DTLZ2 Matlab Version"
%# 	},
%# 	"input": {
%# 		"name": ["x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12"],
%# 		"description": ["x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12"],
%# 		"type": ["real", "real", "real", "real", "real", "real", "real", "real", "real", "real", "real", "real"],
%# 		"unit": ["", "", "", "", "", "", "", "", "", "", "", ""],
%#      "lbounds": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
%#      "ubounds": [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]
%# 	},
%# 	"output": {
%# 		"name": ["y1", "y2", "y3"],
%# 		"description": ["y1", "y2", "y3"],
%# 		"type": ["real", "real", "real"],
%# 		"unit": ["", "", ""]
%# 	}
%# }
%#<TIGON_MATLAB_FUNC_HEADER_END>

function varargout = DTLZ2( varargin )
% DTLZ2 with 12 decision variables and 3 objectives
% ----------------------------------------------------------
% Define Evaluation function here
% Edit the following block to define your evaluation functions
% ----------------------------------------------------------
x = cell2mat(varargin);
y = dtlz2_func(x, 3, 12);

% ----------------------------------------------------------
% End of editing.
% ----------------------------------------------------------
disp(nargout)
if nargout == 1 
    varargout{1} = y;
else
    varargout = num2cell(y);
end

end

function f = dtlz2_func(x, M, N)
% f = dtlz2_func(x, M, N)
%
% Inputs: x      - Candidate solutions
%         M      - Number of objectives
%         N      - Number of decision variables
%
% Output: f      - Objective Values
%
% Minimization problem
% Constraints: Decision variables in [0,1]
% Optimal solution: xm = [0.5...0.5], Sum(fn) = 0.5 hyperplane.

if(M>N)
    error('Cannot have more objectives than decision variables.');
end

k = N-M+1;
nind = size(x,1);

% Set-up the output matrix.
f = NaN*ones(nind, M);

% Compute 'g' functional.
xm = x(:,M:N)';
if k > 0
    g = sum((xm - 0.5).^2);
else
    g = 0;
end

% Compute the objectives.
opg = 1+g';
f(:,1) = prod(cos(pi/2*x(:,1:M-1)),2).*opg;
for n = 2:M-1
   f(:,n) = prod(cos(pi/2*x(:,1:M-n)),2).*sin(pi/2*x(:,M-n+1)).*opg;
end
f(:,M) = sin(pi/2*x(:,1)).*opg;

end
