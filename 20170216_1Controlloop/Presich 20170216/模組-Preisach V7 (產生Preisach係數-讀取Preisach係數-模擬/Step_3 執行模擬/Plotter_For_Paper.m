%---------------------------------------Plot Recursive Curve
figure;
plot(t,u);
xlabel('t (sec)'); ylabel('Operating  Voltage * 0.1 (V)');
legend('u * 0.1 (V)')


%---------------------------------------Plot Uh Real and Sim
figure;
plot(u(6000:end),Uh_Real(6000:end)-Uh_Real(6000),'b',u(6000:end),Uh_sim_Prsh(6000:end)-Uh_sim_Prsh(6000)-0.02,'r');
xlabel('u*0.1 (V)'); ylabel('Uh*0.1 (V)'); ylim([-1 1])
legend('Uh Real','Uh Simulation')