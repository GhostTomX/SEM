clear all;
%close all; clc

% THIS Code can only work with ALL-use plotter (Control Results Plotter is
% not match

SAVE_dscp = 'Multi_Step_H2.8_t15_Clmb7_F_ord8_30Hz';
Ref = load([SAVE_dscp,'.txt']); 

% 先創一個空名字
Name_Pmt_Set = [];
Pre_name = [];
other=['Double_splited_KpKi_Adjust'];

% DTAT LOAD 讀取Reference  
r = Ref (2:end);

clear Ref;

% DTAT LOAD 量到的訊號跟輸入電壓
Measurement_Data = load('Measurement_Data.txt');
%Measurement_Data = load('1.03_0.95_0.022_0.0062.txt');
y      = Measurement_Data(:,1);
u      = Measurement_Data(:,2);
ve     = Measurement_Data(:,3);
u_ctrl = Measurement_Data(:,4);
vol    = Measurement_Data(:,5);
y0     = Measurement_Data(:,6);
vol0   = Measurement_Data(:,7);
p   = Measurement_Data(:,8);
%vin   = Measurement_Data(:,9);
clear Measurement_Data;

% DTAT LOAD 使用那些控制模組              
OuterLoop_Setting = load('OuterLoop_Setting.txt');   % Kp / Ki / Fil_alpha
Kp = OuterLoop_Setting(1);
Ki = OuterLoop_Setting(2);
% DTAT LOAD 是否Unstable / 是否紀錄loop time
Stb_Tclc_Use = load('Stability_and_TimeClc.txt');   
Stability = Stb_Tclc_Use(1);

%-------------------------------------Error
    Error = y - r ;
    Err_Std = std(Error(1500:end))*1000 ;

    Ts = 1/5000;
    t = Ts:Ts:length(y)*Ts;

%-------------------------------------Find Real Uh
    
    Qx = 84*ve;                      
    Temx = 4.9492;
    Up = ( Qx - Temx * y ) / 3.6 ; %一?台?容1.8 ??台 珨跺怢1.8 謗跺怢
    Up = Up / 20;                 % piezo 有兩個X軸同時推，除2 ； 還原成沒有壓電放大器之前的，除10 (放大器放大10倍 ，除10才是電腦放出的電壓值)
    Uh_Real = u - Up - ve/10;
    clear Qx ;

    
%-------------------------------------Un-Stability Plot 
if (Stability==1)
    Azzzzz = 'Stable';
    
else
    Azzzzz = 'Unstable !!!!!!!!!!!!!!!!!'
    figure; plot(UstbPts,u(UstbPts),UstbPts,u_ctrl(UstbPts),UstbPts,y(UstbPts),UstbPts,ve(UstbPts)); title('Unstable Points & Data'); xlabel('Data Point');
    legend('u','u_ctrl','y','ve')
end
clear Azzzzz;


deg = asind(  ((y0-y)*1000)/(13*1000000) );

%-------------------------------------Ctrl Result Simple Plot 

plot(t , r*1000, t , y*1000,t,y0*1000,t,(y-y0)*1000);
ylabel('Displacement (nm)');
xlabel('time (sec)');
legend('r','y','y0','y-y0') ;

figure;
plot(t , r*1000, t , y*1000,t,Error*1000);
ylabel('Displacement (nm)');
xlabel('time (sec)');
title(['y-r Error Mean = ',num2str(mean(Error)*1000),' nm   y-rError Std = ',num2str(std(Error)*1000),' nm']);
legend('r','y','Error') ;
%legend(['Error Std = ',num2str(std(Error(1500:end))*1000),' nm']) ;

figure;
plot(t,deg);
title(['deg Mean = ',num2str(mean(deg)*1000),' nm   deg Std = ',num2str(std(deg)*1000),' nm']);
ylabel('Degree');


% figure;
% plot(Up+0.1*ve,u_ctrl);



ErrStd_name = ['_Std',num2str(std(Error)*1000)] ;
name_pre=[datestr(now,22),ErrStd_name,other,]
name=[name_pre,'_',SAVE_dscp,'_','Kp',num2str(Kp),'Ki',num2str(Ki), '.mat'] ;

save(name) ;



