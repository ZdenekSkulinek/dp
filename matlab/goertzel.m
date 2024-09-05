function [r]= goertzel( d )
N = 16;
h=d;

v1(1:N+2)=zeros;
v2(1:N+2)=zeros;
x(1:N+1)=zeros;
y=[];

for k=0:15;
	
	b2=1;
	b1=-(2*cos(2*pi*k/N))
	b0=1;
	a2=1;
	a1=-(cos(2*pi*k/N)-j*sin(2*pi*k/N));
	a0=0;
	
	v1(1)=0;
	v2(1)=0;
	x(1:N) = h';

	for nn=1:N
		
		v1(nn+1) = v2(nn);
		v2(nn+1) = (1/b2)*(x(nn)-b1*v2(nn)-b0*v1(nn));
	end

	v1(N+2) = v2(N+1)
	v2(N+2) = (1/b2)*(x(N+1)-b1*v2(N+1)-b0*v1(N+1))
	temp = a2*v2(N+2)+a1*v2(N+1)+a0*v1(N+1);
	y = [y temp];
end
r=y;
