# -*- coding: utf-8 -*-
"""
Created on Mon Dec 05 15:58:30 2016

@author: Dogan
"""

import picos as pic
import cvxopt as cvx

#generate data
A = [   cvx.sparse([[1 ,2 ,0 ],
                    [2 ,0 ,0 ]]),
        cvx.sparse([[0 ,2 ,2 ]]),
        cvx.sparse([[0 ,2 ,-1],
                    [-1,0 ,2 ],
                    [0 ,1 ,0 ]])
    ]
K = cvx.sparse([[1 ,1 ,1 ],
                [1 ,-5,-5]])

#size of the data
s = len(A)
m = A[0].size[0]
l = [ Ai.size[1] for Ai in A ]
r = K.size[1]

#creates a problem and the optimization variables
prob = pic.Problem()
mu = prob.add_variable('mu',s)
Z  = [prob.add_variable('Z[' + str(i) + ']', (l[i],r))
      for i in range(s)]

#convert the constants into params of the problem
A = pic.new_param('A',A)
K = pic.new_param('K',K)

#add the constraints
prob.add_constraint( pic.sum([ A[i]*Z[i] for i in range(s)], #summands
                            'i',                            #name of the index
                            '[s]'                           #set to which the index belongs
                           ) == K
                   )
prob.add_list_of_constraints( [ abs(Z[i]) < mu[i] for i in range(s)], #constraints
                              'i',                                    #index of the constraints
                              '[s]'                                   #set to which the index belongs
                            )

#sets the objective
prob.set_objective('min', 1 | mu ) # scalar product of the vector of all ones with mu

#display the problem
print prob

#call to the solver cvxopt
sol = prob.solve(solver='cvxopt', verbose = 0)

#show the value of the optimal variable
print '\n  mu ='
print mu

#show the dual variable of the equality constraint
print'\nThe optimal dual variable of the'
print prob.get_constraint(0)
print 'is :'
print prob.get_constraint(0).dual