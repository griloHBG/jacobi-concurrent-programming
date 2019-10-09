import numpy as np

A = np.array([[4, 2, 1], [1, 3, 1], [2, 3, 6]])

B = np.array([[7], [-8], [6]])

print(A)

A_star = np.zeros(A.shape)
B_star = np.zeros(B.shape)

for i, lin in enumerate(A):
    A_star[i] = (lin/lin[i])
    A_star[i][i] = 0
    B_star[i] = B[i]/lin[i]

print(A_star)
print(B_star)

x = B_star.copy()

print(x)

x_new = np.zeros((3, 1))

x_new[0] =            -0*x[0] -A_star[0][1]*x[1] -A_star[0][2]*x[2] +B_star[0]
x_new[1] = -A_star[1][0]*x[0]            -0*x[1] -A_star[1][2]*x[2] +B_star[1]
x_new[2] = -A_star[2][0]*x[0] -A_star[2][1]*x[1]            -0*x[2] +B_star[2]

print(x_new)

print(- np.matmul(A_star, x_new))