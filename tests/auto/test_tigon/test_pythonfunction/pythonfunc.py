##<TIGON_PYTHON_FUNC_HEADER_START>
## {
## 	"functionProperties": {
## 		"name": "Test Function",
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

def pythonfunc(X):
	#print('---- Enrer pythonfunc')
	y1 = 0.0
	y2 = 1.0

	#print("*size of X: " + str(len(X)))
	cout = 0
	for x in X:
		#print(" X[" + str(cout) + "]"+ str(x))
		y1 += x
		y2 *= x 
		cout += 1
	Y = [y1, y2]
	#print(Y)
	#print('---- Exit pythonfunc\n')

	return Y