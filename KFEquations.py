from sympy import *

# x, y, z, vx, vy, vz = symbols('x,y,z,v_x,v_y,v_z')
xp, yp, zp, vxp, vyp, vzp = \
            symbols('x_{prev},y_{prev},z_{prev},v_{xprev},v_{yprev},v_{zprev}')
dt, g = symbols('dt,g')
Q1, Q2, Q3, Q4, Q5, Q6 = symbols('Q_1, Q_2, Q_3, Q_4, Q_5, Q_6')
R1, R2, R3, R4, R5, R6 = symbols('R_1, R_2, R_3, R_4, R_5, R_6')

# Predict Step
statep = Matrix([xp, yp, zp, vxp, vyp, vzp])

A = Matrix([
            [1, 0, 0, dt, 0, 0],
            [0, 1, 0, 0, dt, 0],
            [0, 0, 1, 0, 0, dt],
            [0, 0, 0, (dt**2)*g/2, 0, 0],
            [0, 0, 0, 0, (dt**2)*g/2, 0],
            [0, 0, 0, 0, 0, (dt**2)*g/2]
             ])

state = A*statep

Pp = MatrixSymbol('Pp', 6, 6)

Q = Matrix([
            [Q1, 0, 0, 0, 0, 0],
            [0, Q2, 0, 0, 0, 0],
            [0, 0, Q3, 0, 0, 0],
            [0, 0, 0, Q4, 0, 0],
            [0, 0, 0, 0, Q5, 0],
            [0, 0, 0, 0, 0, Q6]
             ])

R = Matrix([
            [R1, 0, 0, 0, 0, 0],
            [0, R2, 0, 0, 0, 0],
            [0, 0, R3, 0, 0, 0],
            [0, 0, 0, R4, 0, 0],
            [0, 0, 0, 0, R5, 0],
            [0, 0, 0, 0, 0, R6]
             ])

Ppriori = A*Matrix(Pp)*transpose(A)
Ppriori = simplify(Ppriori) + Q

# Update Step

#Assuming H is Identity
H = Matrix([
            [1, 0, 0, 0, 0, 0],
            [0, 1, 0, 0, 0, 0],
            [0, 0, 1, 0, 0, 0],
            [0, 0, 0, 1, 0, 0],
            [0, 0, 0, 0, 1, 0],
            [0, 0, 0, 0, 0, 1]
             ])

H0 = transpose(Matrix(H[0:6]))
K0 = Ppriori*transpose(H0)*(H0*Ppriori*transpose(H0) + R).inv()


state
Ppriori
# K