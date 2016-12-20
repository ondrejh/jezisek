close all;
clear all;

hperiod = 0.5;
tablen = 30;
dt = hperiod/(tablen-1);
hamplitude = 15;

t = [];

t = [0:dt:hperiod];
p = cos(pi/hperiod*t)*-hamplitude+hamplitude;

figure()
plot(t,p,'x')

f = fopen('../sketch_dec19a/cos.h','w')
s = sprintf('#define TABLEN %d\nconst int ampl = %d;\n',tablen,hamplitude*2);
fputs(f,s)
s = sprintf('const uint8_t cosT[%d] = {',length(t));
fputs(f,s)
for b = p(1:length(t)-1)
  v = round(b);
  s = sprintf('%d,',v);
  fputs(f,s);
endfor
s = sprintf('%d};\n',round(p(length(t))));
fputs(f,s)
fclose(f)
