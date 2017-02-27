clear all;
%close all; clc

% THIS Code can only work with ALL-use plotter (Control Results Plotter is
% not match

SAVE_dscp = 'zig ';
Ref = load('Ref_1Sine_60Hz_prezero_0_precon_0.txt'); 

%是否有zpetc
name_ZPTEC = 0;

%選擇檔名要顯示的 Outer Loop 參數
name_Kp = 1;
name_Ki = 1;
name_F_a = 1;

%選擇檔名要顯示的 Middle Loop 參數
name_cf = 1;
name_Amp = 0;

%選擇檔名要顯示的 Inner Loop Uh_Prev 參數
name_f_ord = 0;
name_f_a = 0;

%選擇檔名要顯示的 Inner Loop A(z) 參數
name_KpA = 0;
name_KiA = 0;

%選擇檔名要顯示的 Inner Loop Prsh 參數
name_incld = 1;
name_Pflt_a = 1;

name_PAmp = 1;

% 先創一個空名字
Name_Pmt_Set = [];
Pre_name = [];

if (name_ZPTEC==1)
    Pre_name = ['_ZPTEC'];
end

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
Module_Usage = load('Module_Usage.txt');                % Outer_use, Middle_use, Uh_prv_use, A_use, Prsh_use
if (Module_Usage(1)==1)                    
    OuterLoop_Setting = load('OuterLoop_Setting.txt');   % Kp / Ki / Fil_alpha
    Pre_name = [Pre_name, '+O'];
    Kp = OuterLoop_Setting(1);
    Ki = OuterLoop_Setting(2);
    Fil_alpha= OuterLoop_Setting(3);
    if (name_Kp==1)
        Name_Pmt_Set = [Name_Pmt_Set ,'_Kp',num2str(Kp)];
    end
    if (name_Ki==1)
        Name_Pmt_Set = [Name_Pmt_Set ,'_Ki',num2str(Ki)];
    end
    if (name_F_a==1)
        Name_Pmt_Set = [Name_Pmt_Set ,'_Fa',num2str(Fil_alpha)];
    end
   
    OuterLoop_Results = load('OuterLoop_Results.txt');   % y_flt / u_ctrl
    y_flt = OuterLoop_Results(:,1) ;
    u_ctrl = OuterLoop_Results(:,2) ;

    clear OuterLoop_Setting  OuterLoop_Results ;
end

if (Module_Usage(2)==1)
    MiddleLoop_Setting = load('MiddleLoop_Setting.txt'); % cf / Amp
    Pre_name = [Pre_name, '+M'];
    cf = MiddleLoop_Setting(1);
    Amp = MiddleLoop_Setting(2);
    if (name_cf==1)
        Name_Pmt_Set = [Name_Pmt_Set ,'_cf',num2str(cf)];
    end
    if (name_Amp==1)
        Name_Pmt_Set = [Name_Pmt_Set ,'_Amp',num2str(Amp)];
    end  
      
    if (Module_Usage(1)==0) 
        u_ctrl = load('MiddleLoop_Results.txt') ;
    end
    
    clear MiddleLoop_Setting ;
end
    
if (Module_Usage(3)==1)
    Uh_prev_Setting = load('Uh_prev_Setting.txt');       % f_ord / f_alpha
    Pre_name = [Pre_name, '+Uh'];
    f_ord = Uh_prev_Setting(1);
    f_alpha = Uh_prev_Setting(2);
    if (name_f_ord==1)
        Name_Pmt_Set = [Name_Pmt_Set ,'_f0rd',num2str(f_ord)];
    end
    if (name_f_a==1)
        Name_Pmt_Set = [Name_Pmt_Set ,'_fa',num2str(f_alpha)];
    end  
    
    Uh_prv_Results = load('Uh_prv_Results.txt');
    Uh_prev = Uh_prv_Results(:,1) ;
    Uh_prev_Flt = Uh_prv_Results(:,2) ;

    clear Uh_prev_Setting Uh_prv_Results ;
end
    
if (Module_Usage(4)==1)                              % Kp_A / Ki_A
    A_Setting = load('A_Setting.txt');
    Pre_name = [Pre_name, '+A'];
    Kp_A = A_Setting(1);
    Ki_A = A_Setting(2);
    if (name_KpA==1)
        Name_Pmt_Set = [Name_Pmt_Set ,'_KpA',num2str(Kp_A)];
    end
    if (name_KiA==1)
        Name_Pmt_Set = [Name_Pmt_Set ,'_KiA',num2str(Ki_A)];
    end  
    
    A_cmp = load('A_Results.txt');

    clear A_Setting ;
end
    
if (Module_Usage(5)==1)                              % Prsh_include / Pvar_flt_use / f_Prsh_alpha / Prsh_var_Amp
    Prsh_Setting = load('Prsh_Setting.txt');
    Pre_name = [Pre_name, '+Pr變動'];
    Prsh_include = Prsh_Setting(1);
    Pvar_flt_use = Prsh_Setting(2);
    Prsh_f_alpha = Prsh_Setting(3);
    Prsh_var_Amp = Prsh_Setting(4);
    if (name_incld==1)
        Name_Pmt_Set = [Name_Pmt_Set ,'_icld',num2str(Prsh_include)];
    end
    if (name_Pflt_a==1 && Pvar_flt_use==1 )
        Name_Pmt_Set = [Name_Pmt_Set ,'_Prf',num2str(Prsh_f_alpha)];
    end
    if (name_PAmp==1)
        Name_Pmt_Set = [Name_Pmt_Set ,'_Prg',num2str(Prsh_var_Amp)];
    end

    if (Prsh_Setting(2)==1)
        Prsh_Results = load('Prsh_Results.txt');
        Prsh = Prsh_Results(:,1);
        Prsh_var_Flt = Prsh_Results(:,2);
    else
        Prsh = load('Prsh_Results.txt');
    end
    
    Prsh_prv = [ 0 ; Prsh(1:end-1) ];
    Prsh_var = Prsh - Prsh_prv;
    
    clear Prsh_Setting Prsh_Results Prsh_prv ;
    
elseif (Module_Usage(5)==2)                     % 完整Preisach 補償
    Prsh_Setting = load('Prsh_Setting.txt');
    Pre_name = [Pre_name, '+Pr完整'];
    Prsh_include = Prsh_Setting(1);
    Pvar_flt_use = Prsh_Setting(2);
    Prsh_f_alpha = Prsh_Setting(3);
    Prsh_var_Amp = Prsh_Setting(4);
    if (name_incld==1)
        Name_Pmt_Set = [Name_Pmt_Set ,'_icld',num2str(Prsh_include)];
    end
    if (name_Pflt_a==1 && Pvar_flt_use==1 )
        Name_Pmt_Set = [Name_Pmt_Set ,'_Prf',num2str(Prsh_f_alpha)];
    end
    if (name_PAmp==1)
        Name_Pmt_Set = [Name_Pmt_Set ,'_Prg',num2str(Prsh_var_Amp)];
    end

    if (Prsh_Setting(2)==1)
        Prsh_Results = load('Prsh_Results.txt');
        Prsh = Prsh_Results(:,1);
        Prsh_var_Flt = Prsh_Results(:,2);
    else
        Prsh = load('Prsh_Results.txt');
    end
    
    Prsh_prv = [ 0 ; Prsh(1:end-1) ];
    Prsh_var = Prsh - Prsh_prv;
    
    clear Prsh_Setting Prsh_Results Prsh_prv ;
    
end

% DTAT LOAD 是否Unstable / 是否紀錄loop time
Stb_Tclc_Use = load('Stability_and_TimeClc.txt');   
Stability = Stb_Tclc_Use(1);
if (Stability==0)
    UstbPts = load('Ustb_Pts_Log.txt');
end
    
if (Stb_Tclc_Use(2)==1)
    Time_Log = load('Time_Log.txt');
  
end


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

%-------------------------------------Loop Time Plot 
if (Stb_Tclc_Use(2)==1) 
    figure; plot(Time_Log); xlabel('Data Point'); ylabel('Calculation Time'); title('Calculation  Time'); 
end

deg = asind(  ((y0-y)*1000)/(13*1000000) );

%-------------------------------------Ctrl Result Simple Plot 

plot(t,u ,t,Uh_Real);
%title([SAVE_dscp]);
ylabel('v');
xlabel('time (sec)');
legend('u','uh');

plot(u ,Uh_Real);
%plot(t , y*1000,t,r*1000,t,Error*1000);
%title([SAVE_dscp]);
%ylabel('Displacement (nm)');
%xlabel('time (sec)');
%legend('y','r','Error') ;

%figure;
%plot(t , r*1000, t , y*1000,t,y0*1000,t,(y-y0)*1000);
%ylabel('Displacement (nm)');
%xlabel('time (sec)');
%legend('r','y','y0','y-y0') ;

% figure;
% plot(t , r*1000, t , y*1000,t,Error*1000);
% ylabel('Displacement (nm)');
% xlabel('time (sec)');
% title(['y-r Error Mean = ',num2str(mean(Error)*1000),' nm   y-rError Std = ',num2str(std(Error)*1000),' nm']);
% legend('r','y','Error') ;

%legend(['Error Std = ',num2str(std(Error(1500:end))*1000),' nm']) ;


%figure;
%[AX,H1,H2] = plotyy(t,r*1000,t,deg);
%set(get(AX(1),'Ylabel'),'String','Displacement (nm)')
%set(get(AX(2),'Ylabel'),'String','Degree') 
%plotyy(t , r*1000, t ,deg);
%xlabel('time (sec)');
%legend('r','deg') ;

%figure;
%plot(t,deg);
%title(['deg Mean = ',num2str(mean(deg)*1000),' nm   deg Std = ',num2str(std(deg)*1000),' nm']);
%ylabel('Degree');


%figure;
%plot(t , vol,t,vol0,t,vol-vol0);
%ylabel('vol/vol0_v');
%xlabel('time (sec)');
%legend('vol','vol0','volError') ;

%figure;
%plot(t,vin);
%ylabel('t');
%xlabel('vin');
%legend('t','vin') ;

%xia mianzheg beiwo zhushi de 
%figure;
%plot(t , Error*1000 );
%yuanlai chengxu meiyouzhe yihang      axis([0 1 -400 400]);
%title([SAVE_dscp,'    error mean ='  num2str(mean(Error)*1000) 'nm  min=' num2str(min(Error(500:end))*1000) 'nm  max=' num2str(max(Error(500:end))*1000) 'nm' ]);
%ylabel('Displacement (nm)');
%xlabel('time (sec)');
%legend(['Error Std = ',num2str(std(Error(1500:end))*1000),' nm']) ;

% figure;
% plot(r,r,r,y);
% title([SAVE_dscp,' f ',num2str(Prsh_f_alpha),'a',num2str(Prsh_var_Amp)]);

% figure;
% plot(r,Uh_Real);

%beiwo zhushi
%figure;
%plot(u_ctrl,u_ctrl,u_ctrl,Up+0.1*ve);
% title([SAVE_dscp,num2str(Prsh_f_alpha)]);
%xlabel('uctrl');
%ylabel('Up+0.1*ve');
% title(['u-up',SAVE_dscp,' f ',num2str(Prsh_f_alpha),'a',num2str(Prsh_var_Amp)]);
% 

% 
% figure;
% plot(t,ve);
% title([(SAVE_dscp),'  ve']);
% figure;
% plot(r,y);
% title(['r-y',SAVE_dscp,' f ',num2str(Prsh_f_alpha),'a',num2str(Prsh_var_Amp)]);


%ErrStd_name = ['_Std',num2str(fix(Err_Std*100)/100)] ;
%save([[ datestr(now,22) ,'_', SAVE_dscp, Pre_name, Name_Pmt_Set, ErrStd_name] '.mat']) ;



