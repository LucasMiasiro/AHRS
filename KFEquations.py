from sympy import *

x, v = symbols('x,v')
a = symbols('a')
xmeas, vmeas = symbols('x_{meas},v_{meas}')
dt, g = symbols('dt,g')
Q0, Q1 = symbols('Q_0, Q_1')
R0, R1 = symbols('R_0, R_1')

# Predict Step
statep = Matrix([x, v])

A = Matrix([
            [1, dt],
            [0, 1]
            ])

B = Matrix([
            [(dt**2)*g/2],
            [dt]
            ])

statepriori = A*statep + B*a

Pp = MatrixSymbol('Pp', 2, 2)

Q = Matrix([
            [Q0, 0],
            [0, Q1]
            ])

R = Matrix([
            [R0, 0],
            [0, R1]
            ])

Ppriori = A*Matrix(Pp)*transpose(A)
Ppriori = simplify(Ppriori) + Q