%#<TIGON_MATLAB_FUNC_HEADER_START>
%# {
%# 	"functionProperties": {
%# 		"name": "TestHeader",
%# 		"description": "Matlab Header Test function"
%# 	},
%# 	"input": {
%# 		"name": ["a", "b"],
%# 		"description": ["a value", "another value"],
%# 		"type": ["real", "real"],
%# 		"unit": ["mm", "mm"],
%# 		"lbounds": [0.0, 0.0],
%# 		"ubounds": [1.0, 1.0]
%# 	},
%# 	"output": {
%# 		"name": ["out1", "out2"],
%# 		"description": ["The first output", "The second output"],
%# 		"type": ["real", "real"],
%# 		"unit": ["mm", "mm"]
%# 	}
%# }
%#<TIGON_MATLAB_FUNC_HEADER_END>

function varargout = test_properties_parsing( varargin )
%test1_matlabTigonWrapper A test function for the interface between Matlab
%and Liger/Tigon
    varargout{2} = varargin{1} + 1;
    varargout{1} = varargin{1} + 2;
end