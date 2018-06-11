%#<TIGON_MATLAB_FUNC_HEADER_START>
%# {
%# 	"functionProperties": {
%# 		"name": "TestWrapper2",
%# 		"description": "Matlab Wrapper 2"
%# 	},
%# 	"input": {
%# 		"name": ["intIn", "realIn"],
%# 		"description": ["Integer", "Real"],
%# 		"type": ["int", "real"],
%# 		"unit": ["mm", "mm"],
%# 		"ubounds": [1000, 30.6],
%# 		"lbounds": [-100, -20]
%# 	},
%# 	"output": {
%# 		"name": ["intOut", "realOut"],
%# 		"description": ["Integer", "Real"],
%# 		"type": ["int", "real"],
%# 		"unit": ["mm", "mm"]
%# 	}
%# }
%#<TIGON_MATLAB_FUNC_HEADER_END>
function varargout = test2_matlabTigonWrapper( varargin )
%test2_matlabTigonWrapper A test function for the interface between Matlab
%and Liger/Tigon
in1 = varargin{1};
in2 = varargin{2};

varargout{1} = in1;
varargout{2} = in2;
end

