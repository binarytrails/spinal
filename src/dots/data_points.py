#!/usr/bin/python

#! @file
#! @author Vsevolod (Seva) Ivanov
#!

def eq_spaced_points(n, p):
    """
        Computes equally spaced point inside [-1, 1] range.

        n - number of points
        p - padding

        Return points
    """
    points = list()
    point = -1.0
    step = (2.0 - p * 2.0) / n

    for i in range(n):
        point += step
        points.append(point)

    return points

y_points = eq_spaced_points(5, 0.1)

for y in y_points:
    print("0 %f 0" % y)
