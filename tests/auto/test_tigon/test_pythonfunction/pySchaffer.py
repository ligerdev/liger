##<TIGON_PYTHON_FUNC_HEADER_START>
## {
## 	"functionProperties": {
## 		"name": "Schaffer",
## 		"description": "Modified Schaffer N. 1 test problem without constrain for unit testing"
## 	},
## 	"input": {
## 		"name": ["x1", "x2"],
## 		"description": ["x1", "x2"],
## 		"type": ["real", "real"],
## 		"unit": ["", ""],
##      "lbounds": [-10.0, -10.0],
##      "ubounds": [10.0, 10.0]
## 	},
## 	"output": {
## 		"name": ["y1", "y2"],
## 		"description": ["y1", "y2"],
## 		"type": ["real", "real"],
## 		"unit": ["", ""]
## 	}
## }
##<TIGON_PYTHON_FUNC_HEADER_END>

def pySchaffer(X):
	x = X[0];
	y = X[1];

	f1 = x*x
	f2 = (x-2)*(x-2)
	Y = [f1, f2]

	return Y
