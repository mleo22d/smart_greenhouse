%% ARDUINO A MATLAB
% En el código de Arduino, mandar al serial las variables de interés
% separadas por comas.
% Reemplaza 'COM5' con el nombre del puerto donde está conectado tu Arduino
s = serialport("COM5", 9600)
fopen(s);
lines = animatedline('Marker', 'o', 'MarkerSize', 6); % D Creo la variable lines que va a graficar en esa figura
f1=figure(1); % D Declaro la Figura
clf; % Abre una nueva figura
f1.Position = [0 50 1600 700];
lines(2) = animatedline('Marker', 'o', 'MarkerSize', 4,'Color', 'g');
lines(3) = animatedline('Marker', 'o', 'MarkerSize', 2,'Color', 'r');
%plot(sim_time,dist_mm,'--b','LineWidth', 1.5); hold on;
xlabel('Tiempo [s]');
ylabel('Temperatura [C]');
title('Temperatura en Tiempo Real desde ARDUINO');
legend('Temperatura [C]','Setpoint [C]','Location','northwest');
axis([0 500 25 55]);
k = 1;
tiempo=[];
temperatura=[];
setpoint=[];
while true
% Read a line of data from the serial port
data = readline(s);
% Split the data into columns based on a delimiter (e.g., a comma)
columns = strsplit(data, ',');
var = zeros(1,3);
for i = 1:3
var(1,i) = [str2double(columns(i))];
end
tiempo=[tiempo;var(1,1)]
temperatura=[temperatura;var(1,2)]
setpoint=[setpoint;var(1,3)]
for i = 1:2
x = var(k,1);
y = var(k,i+1);
% Add a new point to the animated line
addpoints(lines(i+1), x, y); %D Llamo en el loop al lines correspondiente y dibujo
% Update the plot
drawnow expose;
end
end
%%
clear variables;
clear s;
clf;
