##<TIGON_PYTHON_FUNC_HEADER_START>
## {
## 	"functionProperties": {
## 		"name": "Test Import Function",
## 		"description": "This is a test function for unit testing"
## 	},
## 	"input": {
## 		"name": ["x1", "x2", "x3", "x4", "x5"],
## 		"description": ["x1", "x2", "x3", "x4", "x5"],
## 		"type": ["real", "real", "real", "real", "real"],
## 		"unit": ["", "", "", "", ""],
##      "lbounds": [0.0, 1.0, 2.0, 3.0, 4.0],
##      "ubounds": [1.0, 2.0, 3.0, 4.0, 5.0]
## 	},
## 	"output": {
## 		"name": ["y1", "y2"],
## 		"description": ["y1", "y2"],
## 		"type": ["real", "real"],
## 		"unit": ["", ""]
## 	}
## }
##<TIGON_PYTHON_FUNC_HEADER_END>

from datetime import datetime
from external_module import test_func
def pythonImport(X):
        # Just test if it runs
	time_string = str(datetime.now())

	Y = test_func(X)

	return Y
