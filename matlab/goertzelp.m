function [r]= goertzelp( d , N)
M = size(d)(2);
h=d;

v1(1:M+2)=zeros;
v2(1:M+2)=zeros;
v21(1:M+2)=zeros;
v22(1:M+2)=zeros;
x(1:M+1)=zeros;
y=[];
ttt(1:N) = zeros;

for k=1:1;
	
	b2=1;
	b1=-(2*cos(2*pi*k/N));
	b0=1;
	a2=1;
	a1=-(cos(2*pi*k/N)-j*sin(2*pi*k/N));
	a0=0;
	D = zeros(2,M);
	A = [0 1;-1 -b1];
	for i=1:M
		D(:,M+1-i) = A^(i-1) * [0;1];
	end;
	
	
	t = cputime;

	for tn=1:10

	v1(1)=0;
	v2(1)=0;
	x(1:M+1)=zeros;
	x(M-N+1:M) = d(1:N)';


	for nn=1:M
		
		v1(nn+1) = v2(nn);
		v2(nn+1) = (1/b2)*(x(nn)-b1*v2(nn)-b0*v1(nn));
		v21(nn+1) = dot( D(1,M+1-nn:M) , x(1:nn));
		v22(nn+1) = dot( D(2,M+1-nn:M) , x(1:nn));
	end
	%v1(M+1) = dot( D(1,1:M) , x(1:M));
	%v2(M+1) = dot( D(2,1:M) , x(1:M));
	%b1
	%v1
	%v2
	%v21
	%v22

	v1(M+2) = v2(M+1);
	v2(M+2) = (1/b2)*(x(M+1)-b1*v2(M+1)-b0*v1(M+1));
	temp = a2*v2(M+2)+a1*v2(M+1)+a0*v1(M+1);
	y(k+1) = temp;

	end

	ttt(k+1) = cputime - t;
end
r=y;
sum(ttt) / 10
