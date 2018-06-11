##<TIGON_PYTHON_FUNC_HEADER_START>
## {
##  "functionProperties": {
##      "name": "DTLZ1",
##      "description": "DTLZ1 Python Version"
##  },
##  "input": {
##      "name": ["x1", "x2", "x3", "x4", "x5"],
##      "description": ["x1", "x2", "x3", "x4", "x5"],
##      "type": ["real", "real", "real", "real", "real"],
##      "unit": ["", "", "", "", ""],
##      "lbounds": [0.0, 0.0, 0.0, 0.0, 0.0],
##      "ubounds": [1.0, 1.0, 1.0, 1.0, 1.0]
##  },
##  "output": {
##      "name": ["y1", "y2"],
##      "description": ["y1", "y2"],
##      "type": ["real", "real"],
##      "unit": ["", ""]
##  }
## }
##<TIGON_PYTHON_FUNC_HEADER_END>

from math import cos, pi
def DTLZ1Batch(X):
    Y = []
    for x in X:
        Y.append(dtlz1_func(x))

    return Y

def dtlz1_func(X):
    n = len(X)
    M = 2
    k = n - M +1;

    g = 0.0
    for i in range(n-k, n):
        g += (X[i] - 0.5)*(X[i] - 0.5) - cos(20.0 * pi * (X[i] - 0.5))
    g = 100.0 * (k + g)

    Y = M*[(1.0 + g) * 0.5]
    for i in range(0, M):
        aux = M - (i+1)
        for j in range(0, aux):
            Y[i] *= X[j]
        if i != 0:
            Y[i] *= (1 - X[aux])

    return Y
if __name__ == "__main__":
    X = [[0.25, 0.5, 0.5, 0.5, 0.5]]
    print(DTLZ1Batch(X))
