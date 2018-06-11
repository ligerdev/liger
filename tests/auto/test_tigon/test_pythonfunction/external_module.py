def test_func(X):
    y1 = 0.0
    y2 = 1.0
    for x in X:
        y1 += x
        y2 *= x

    Y = [y1, y2]
    return Y
