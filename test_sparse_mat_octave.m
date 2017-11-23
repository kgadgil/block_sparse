clc;
clear all;
close all;

%%matrix A
%A=[1,2;3,4];
%%disp(A);
%n = 4;
%s = diag (sparse (randn (1,n)), -1);
%s1=spdiags(s);
%%disp(s1);
%
%b = sprand(3,3,1);
%%disp(b);
%
%m = speye(4,4);
%%disp(m1);
%
%S = sparse(double(rand(3,3) < 1/2));
%%disp(S);
%A = full(S);
%%disp(A);

%example sparse matrices
r = 9;    %r*c matrix formed
c = 9;    
n = 1;  %distribution of nnz
ri = ci = d = [];
  for j = 1:c
    ri = [ri; randperm(r,n)'];
    ci = [ci; j*ones(n,1)];
    d = [d; rand(n,1)];
  endfor
  s = sparse (ri, ci, d, r, c);
  disp(s);
  disp("ri array");
  disp(ri);
  disp("ci array");
  disp(ci);
  disp("d array");
  disp(d);