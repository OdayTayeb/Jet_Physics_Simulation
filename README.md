# Jet_Physics_Simulation

This Simulation is about mid-size business jet airplane that called Learjet 70/75.

# The Forces

1- Gravity Force : its direction is always to the center of earth.
W = m*g where: 
m is Jet Mass
g is gravity Acceleration

2- Lift Force : its direction is always perpendicular on Jet plane.
L=1/2 * Cl * rho * v * v * A where: 
Cl is Lift coefficient
rho is air density 
v is Jet velocity 
A is Wing Area

3- Drag Force : its direction is always opposite of Velocity.
D=1/2 * C * rho * v * v * A where:
C is Drag coefficient
rho is air density 
v is Jet velocity 
A is Wing Area

4- Thrust Force : its direction always with Jet body.
FT=𝑚̇ 0*(Ve-V0) where:
𝑚̇ 0 is mass flow rate 
𝑚̇ 0 =rho*V0*A 
rho is air density
V0 is velocity of incoming air
A is Fan area 
Ve is velocity of outflow air

# Banking turn

When the pilot turns the wing right or left, Lift force will rotate around the body of the jet.

To do that we can use Rotation Matrix of rotating a vector around another vector (so we will rotate Lift vector around Thrust vector which is parallel to Jet’s body)

Rotation Matrix:
![image](https://user-images.githubusercontent.com/92798033/190854942-35862f7e-a7a3-4ff6-848b-598703d51ec5.png)
