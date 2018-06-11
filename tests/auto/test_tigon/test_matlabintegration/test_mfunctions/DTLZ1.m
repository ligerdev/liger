%#<TIGON_MATLAB_FUNC_HEADER_START>
%# {
%# 	"functionProperties": {
%# 		"name": "DTLZ1",
%# 		"description": "DTLZ1 Matlab Version"
%# 	},
%# 	"input": {
%# 		"name": ["x1", "x2", "x3", "x4", "x5"],
%# 		"description": ["x1", "x2", "x3", "x4", "x5"],
%# 		"type": ["real", "real", "real", "real", "real"],
%# 		"unit": ["", "", "", "", ""],
%#      "lbounds": [0.0, 0.0, 0.0, 0.0, 0.0],
%#      "ubounds": [1.0, 1.0, 1.0, 1.0, 1.0]
%# 	},
%# 	"output": {
%# 		"name": ["y1", "y2"],
%# 		"description": ["y1", "y2"],
%# 		"type": ["real", "real"],
%# 		"unit": ["", ""]
%# 	}
%# }
%#<TIGON_MATLAB_FUNC_HEADER_END>

function varargout = DTLZ1( varargin )
% DTLZ1 with 5 decision variables and 2 objectives
% ----------------------------------------------------------
% Define Evaluation function here
% Edit the following block to define your evaluation functions
% ----------------------------------------------------------
x = cell2mat(varargin);
y = dtlz1_func(x, 2, 5);

% ----------------------------------------------------------
% End of editing.
% ----------------------------------------------------------
if nargout == 1 
    varargout{1} = y;
else
    varargout = num2cell(y);
end

end

function f = dtlz1_func(x, M, N)
% f = dtlz1_func(x, M, N)
%
% Inputs: x      - Candidate solutions
%         M      - Number of objectives
%         N      - Number of decision variables
%
% Output: f      - Objective Values
%
% Minimization problem
% Constraints: Decision variables in [0.0,1]
% Optimal solution: xm = [0.5...0.5], Sum(fn) = 0.5 hyperplane.

if(M>N)
    error('Cannot have more objectives than decision variables.');
end

k = N-M+1;
nind = size(x,1);

% Set-up the output matrix.
f = NaN*ones(nind, M);

% Compute 'g' functional.
if k > 0
    g = 100*(k + sum((x(:,M:N) - 0.5).^2 - cos(20*pi*(x(:,M:N)-0.5)), 2));
else
    g = 0;
end
% Compute the objectives.
opg = 1+g;
f(:,1) = 0.5*prod(x(:,1:M-1), 2).*opg;
for n = 2:M-1
    f(:,n) = 0.5*prod(x(:,1:M-n), 2).*(1 - x(:,M-n+1)).*opg;
end
f(:,M) = 0.5*(1 - x(:,1)).*opg;
end