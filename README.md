# Fibonacci
This is a personal project around the fibonacci sequence. I wanted to see how far I could optimize the calculation of this sequence and what is needed to represent absurdly large values in c. For this I created a datatype called uintx_t. It contains an array of uint8's which represent the value in Base 10.

## Recursive
The recursive implementation of the fibonacci sequence. Max calculatable value is F(93) since it uses 64 bit unsigned integers. This is extremely slow as it doesn't reuse already calculated values.

## Dynamic
This is an implementation which uses dynamic programming. An intermediate value gets calculated and saved in an array. When this value needs to be used again, it's queried from the array.

## Dynamic large
Virtually the same as the dynamic implementation except it uses uintx_t's as datatype. This means that the biggest representable value is as long as you want it to be. It starts getting slow around F(40.000).

## Logarithmic large
UNIMPLEMENTED. This implementation uses fast matrix multiplication and a cool matrix identity which states that:

$$\begin{pmatrix}1 & 1\\\ 1 & 0\end{pmatrix}^2 = \begin{pmatrix}F(n+1) & F(n)\\\ F(n) & F(n-1)\end{pmatrix}$$
