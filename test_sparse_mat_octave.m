clc;
clear all;
close all;

%matrix A
A=[1,2;3,4];
%disp(A);
n = 4;
s = diag (sparse (randn (1,n)), -1);
s1=spdiags(s);
%disp(s1);

b = sprand(3,3,1);
%disp(b);

m = speye(4,4);
%disp(m1);

S = sparse(double(rand(3,3) < 1/2));
disp(S);
A = full(S);
disp(A);
