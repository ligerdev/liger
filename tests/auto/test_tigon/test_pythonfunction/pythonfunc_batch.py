##<TIGON_PYTHON_FUNC_HEADER_START>
## {
##  "functionProperties": {
##      "name": "Test Function",
##      "description": "This is a test function for unit testing"
##  },
##  "input": {
##      "name": ["x1", "x2", "x3", "x4", "x5"],
##      "description": ["x1", "x2", "x3", "x4", "x5"],
##      "type": ["real", "real", "real", "real", "real"],
##      "unit": ["", "", "", "", ""],
##      "lbounds": [0.0, 1.0, 2.0, 3.0, 4.0],
##      "ubounds": [1.0, 2.0, 3.0, 4.0, 5.0]
##  },
##  "output": {
##      "name": ["y1", "y2"],
##      "description": ["y1", "y2"],
##      "type": ["real", "real"],
##      "unit": ["", ""]
##  }
## }
##<TIGON_PYTHON_FUNC_HEADER_END>

def pythonfunc_batch(X):
    #print('---- Enrer pythonfunc')
    Y = [[None , None]]*len(X)
    
    counter = 0
    for x in X:
        y1 = 0.0
        y2 = 1.0
        
        for elm in x:
            y1 += elm
            y2 *= elm 
        Y[counter] = [y1, y2]
        counter += 1

    return Y
