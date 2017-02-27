% Sine_Reference_Generator vol2.0 By Ethan Liu  (1.0 by SF)
Ts = 1/5000;

Sig_mode = 'Chirp' ;


switch Sig_mode
   
%---------------Mode 00 'SineDecay'
case{'SineDecay'}
    
wavenum =50;
A_sin =3;         % Amplitude
Sin_freq = 5 ;     % sine input frequency
Phase_shift = 0;
r =[];
t=[];


%---------------Mode 0 'Sine'
case{'Sine'}
A_sin = 2;         % Amplitude
T_end = 1 ;         % 大約的終止時間，程式碼會自動把時間修正為降到0的地方
Sin_freq =30;     % sine input frequency
Phase_shift = 0;
prezero = 0;
precon = 0;
%---------------Mode 1 'Zig'
case{'Zig'}
A_Zig = 7 ;            % Max Peak
peak_num = 10 ;     % Peak 的數目
speed_lv = 28;    %  DAQ min resolution 0.0025V *5000=12.5V
Zig_Decay = 1;
prezero = 0;
offset = 0;
%---------------Mode 3 'Cherp'
case{'Chirp'}
A_Chp = 1;           % Max Value
t_end = 0.5 ;        % 終止時間
f_end = 1000;        % 終止頻率
Filter = 1 ;         % 是否啟用Filter
Corner_Freq = 300 ;  % 轉角頻率Hz  
%---------------Mode 4 'Rdm_Step'

%---------------Mode 6 'Multi_Step'   <--------------Jordan Wu
case{'Multi_Step'}
PreZero = 1;
Max_Heigh = 2.8;
t_end= 15; 
Climb_num= 7 ; 

Filter = 1 ;         % 是否啟用Filter
Corner_Freq = 30 ;  % 轉角頻率Hz 
Order = 8;
%---------------Mode 7 'Step'  
case{'Step'}
Max_Heigh = 0.5;
t_end= 1; 
t_zero = 0.1;
%---------------Mode 7 'Free'  
case{'Free'}
Save_name = 'IEEE_Drct_Inverse_Low';
t_end= 1 ;
Signal = '3 + sin(2*pi*t - 0.5*pi) + sin(10*pi*t - 0.5*pi) + sin(18*pi*t - 0.5*pi)';
end


switch Sig_mode      
        
    case{'SineDecay'}
        if ( Phase_shift == 0 )
            name = ['Ref_',num2str(A_sin),'SineDecay_',num2str(Sin_freq),'Hz_','P_',num2str(wavenum)] ;
            
        else name = ['Ref_',num2str(A_sin),'SineDecay_',num2str(Sin_freq),'Hz_Phase',num2str(Phase_shift)] ;
        end

        
        for i =1:wavenum
        Amp = A_sin-(i-1)* A_sin/wavenum;
        t1 = (i-1)/Sin_freq:Ts:i/Sin_freq;
        t = [t t1];
        ref = -Amp*cos(2*pi*Sin_freq*t1 + Phase_shift/180*pi);
        ref = ref+Amp;
        r = [r ref];            
        end
        
        figure;
        plot(t,r); xlabel('t'); ylabel('Sine'); title([name]);      
        
        %--------------------------------------------Save to txt
        fid = fopen([name '.txt'], 'w');
        fprintf(fid, '%d\r\n', length(r));
        for i = 1:length(r)
            fprintf(fid, '%f\r\n', r(i));
        end
        fclose(fid);
        
    case{'Sine'}
        if ( Phase_shift == 0 )
            name = ['Ref_',num2str(A_sin),'Sine_',num2str(Sin_freq),'Hz','_prezero_',num2str(prezero),'_precon_',num2str(precon)] ;
            
        else name = ['Ref_',num2str(A_sin),'Sine_',num2str(Sin_freq),'Hz_Phase',num2str(Phase_shift),'_prezero_',num2str(prezero)] ;
        end

        T_end = T_end - mod( T_end , 1/Sin_freq ) ;  % 調整時間，讓sine波跑整數個週期
                                                     % 1/Sin_freq = 週期
        t1 = 0:Ts:T_end;
        ref = -A_sin*cos(2*pi*Sin_freq*t1 + Phase_shift/180*pi);
        ref = ref+A_sin;  % to ensure the reference input is always > 0
%         ref = ref' ;
        
        if prezero ==1;
        t1 = 0:Ts:T_end+prezero;
        ref = [zeros(1,prezero/Ts) ref] ;
        end
        
        if precon ==1;
        t1 = 0:Ts:2*T_end+prezero+0.5+1/5000;
        ref = [ref zeros(1,0.5/Ts) ref] ;
        end
                
        figure;
        plot(t1,ref); xlabel('t'); ylabel('Sine'); title([name]);
        
        
        %--------------------------------------------Save to txt
        fid = fopen([name '.txt'], 'w');
        fprintf(fid, '%d\r\n', length(ref));
        for i = 1:length(ref)
            fprintf(fid, '%f\r\n', ref(i));
        end
        fclose(fid);
        
        
    case{'Zig'}
        Dis_basic = 0.0025;   % Basic distance between two voltage point
        
        if ( Zig_Decay ==1 )
            Local_Mx = [ A_Zig : -A_Zig/peak_num : A_Zig/peak_num ] ;
            Vin = [];
            Freq_smp = 1/Ts ;
            Dis_basic = 0.0025*speed_lv/50*4 ;  %兩點之間的速度上升數值

        
            for i = 1 : length(Local_Mx)
                Vi_tns_R = [ Dis_basic : Dis_basic : Local_Mx(i) ] ;
    
                if mod(Local_Mx(i),Dis_basic) ~= 0;
                    Vi_tns_D = [ Local_Mx(i) - mod(Local_Mx(i),Dis_basic) : -Dis_basic : 0 ] ;
                else
                    Vi_tns_D = [ Local_Mx(i) - Dis_basic : -Dis_basic : 0 ] ;
                end
                Vin = [ Vin , Vi_tns_R , Vi_tns_D ] ;
            end
            Vin = Vin+offset
            ref = Vin' ;
            
        
        else
            Local_Mx = [ A_Zig : -A_Zig/peak_num : A_Zig/peak_num ] ;
            Vin = [];
            Freq_smp = 1/Ts ;
            Dis_basic = 0.0025*speed_lv/50*4 ;  %兩點之間的速度上升數值
            
            Vi_tns_R = [ Dis_basic : Dis_basic : A_Zig ] ;
            if mod(A_Zig,Dis_basic) ~= 0;
                Vi_tns_D = [ A_Zig - mod(A_Zig,Dis_basic) : -Dis_basic : 0 ] ;
            else
                Vi_tns_D = [ A_Zig - Dis_basic : -Dis_basic : 0 ] ;
            end
            Vi_basic = [ Vi_tns_R , Vi_tns_D ] ;
            
            for i = 1 : peak_num
                Vin = [ Vin , Vi_basic ] +offset;
            end    
            ref = Vin' ;            
        end
        
        t = [Ts : Ts : Ts*length(Vin)] ;
        if prezero ==1 ;
        Vin = [zeros(1,1/Ts) Vin];    
        t = [Ts : Ts : Ts*length(Vin)] ;
        end
        
        
        %------------------------Plot
        figure;
        plot(t,Vin); xlabel('t'); ylabel('Zig');
        title(['Speed  =  ',num2str(speed_lv),' v/s  Sampling  Freq  =  ',num2str(Freq_smp)]);
        
        %--------------------------------------------Save to txt
        name = ['Ref_Zig2016',num2str(A_Zig),'_p',num2str(peak_num),'_Speed_Lv_',num2str(speed_lv),'_prezero_',num2str(prezero)] ;
        fid = fopen([name '.txt'], 'w');
        
        fprintf(fid, '%d\r\n', length(Vin));
        for i = 1:length(Vin)
            fprintf(fid, '%f\r\n', Vin(i));
        end
        fclose(fid);
        
        
        
    case{'Chirp'}
        t = 0:Ts:t_end-Ts;  
        f0 = 0.1;                % freq. start
        k = (f_end/f0)^(1/t_end);  % used to modify the frequency variation
        Freq = f0 * k.^t;           % frequency is varying as time goes on
        A_Chp = A_Chp/2;                 % Amplitude = Max/2
        x = A_Chp + A_Chp*sin( (2*pi*f0/log(k) ) * (k.^t-1) + pi*1.5);   % the cherp signal
        x = x';
        
        name_Flt ='';
        if (Filter == 1 )
            s = tf('s'); 
            G_flt = (2*pi*Corner_Freq)/(s + 2*pi*Corner_Freq) ;
            x = lsim(G_flt, x, t ) ;
            name_Flt = ['_F_',num2str(Corner_Freq),'Hz'];
        end;         

        
        figure;plot(t,Freq); title('t - Freq (Hz)'); xlabel('t'); ylabel('Hz');
        figure;plot(t,x); title(['Chirp  End freq = ',num2str(f_end),'  End time = ',num2str(t_end)]); xlabel('t (sec)'); ylabel('u');
        
        
        %-------------------------------------------Save to txt file
        name = ['Ref_Chirp',num2str(2*A_Chp),'_0to' num2str(f_end),'Hz_',num2str(t_end),'sec',name_Flt];
        fid = fopen([name '.txt'], 'w');
        fprintf(fid, '%d\r\n', length(x));
        for i = 1:length(x)
            fprintf(fid, '%f\r\n', x(i));
        end
        fclose(fid);
        
        
     case{'Rdm_Step'}
        len = 1 / Ts;
        Random_Step = zeros(1,len);
        for i = 2:20
            TargetPt = 3 * rand(1,1);
            Random_Step = [Random_Step TargetPt*ones(1,len)];
        end
        t = 0:Ts:(length(Random_Step)-1)*Ts;
        
        figure;
        plot(t,Random_Step); xlabel('t'); ylabel('Ref');
        title('Random Step') ;
        
        %-------------------------------------------Save to txt file
        name = ['IDTest_Random_Step'] ;
        fid = fopen([name '.txt'], 'w');
        fprintf(fid, '%d\r\n', length(Random_Step));
        for i = 1:length(Random_Step)
            fprintf(fid, '%f\r\n', Random_Step(i));
        end
        fclose(fid);
        
        
    case{'Multi_Step'}  %<-----------Jordan !!
        if (PreZero ==1)
            multistep = [];
            Step_num = Climb_num * 2 - 1;  % 幾個高度大於0的梯
            Step_Width = ceil (5000 * t_end / ( Step_num + 2 ) ); % 一個Step的點數目 = t_end / (總梯數 + 加上兩個高度等於0的梯) * 5000 ; ceil = 無條件進位
            stepwid = ones(1,Step_Width); 
        
            for i = 1:(Step_num+1)/2+1 
                TargetPt = (i-1) * Max_Heigh/(Step_num+1)*2; 
                multistep = [multistep TargetPt*stepwid]; 
            end
        
            for i= (Step_num+1)/2+2: Step_num +2 
                TargetPt = Max_Heigh- (i-(Step_num+1)/2-1) * Max_Heigh/(Step_num+1)*2; 
                multistep = [multistep TargetPt*stepwid]; 
            end
            First_Title = 'Multi_Step_H';
            
            
        elseif (PreZero ==0)
            multistep = []; 
            data_pts = 5000*t_end;
            Step_pt_per = fix (data_pts / ( Climb_num*2 -1 ) ); 
            
            for i = 1: Climb_num-1 
                TargetPt = (i) * Max_Heigh/( Climb_num ); 
                multistep = [multistep TargetPt*ones(1, Step_pt_per) ]; 
            end
            multistep = [multistep Max_Heigh*ones(1, Step_pt_per) fliplr(multistep)]; 
            multistep = [multistep zeros(1,length(data_pts)-length(multistep))]; 
            multistep(1) = 0; 
            multistep(end) = 0;
            First_Title = 'Pre_Zero_Multi_Step_H';
        end
   

            
        t = 0:Ts:(length(multistep)-1)*Ts;
        
        %--------------------------------------------Filter
        opt = bodeoptions('cstprefs');
        opt.MagUnits = 'abs';
        opt.FreqScale = 'linear';
        opt.FreqUnits = 'Hz';
        opt.XLim = [0,200];
        
        [a,b] = butter(Order,Corner_Freq*2*Ts) ;
        G_flt = tf(a,b,Ts);
        
        name_Flt = '';
        if (Filter == 1 )
            multistep = lsim(G_flt, multistep, t ) ;
            figure; bode(G_flt,opt); title(['Butterworth order ',num2str(Order)]);
            name_Flt = ['_F_ord',num2str(Order),'_',num2str(Corner_Freq),'Hz'];
        end  
        
        figure; plot(t,multistep); xlabel('t'); ylabel('Ref'); title('Multi Step') ; 
        ref = multistep;
        
        %-------------------------------------------Save to txt file 
        name = [First_Title,num2str(Max_Heigh),'_t',num2str(t_end),'_Clmb',num2str(Climb_num),name_Flt] ; 
        fid = fopen([name '.txt'], 'w'); 
        fprintf(fid, '%d\r\n', length(multistep)); 
        for i = 1:length(multistep) 
            fprintf(fid, '%f\r\n', multistep(i));
        end
        fclose(fid);
        
        
        

    case('Step')
        
        Data_pts = t_end*5000;
        Step = Max_Heigh * ones(1,Data_pts) ;
        Step([1:1:t_zero*5000])=0;
        t = 0:Ts:(Data_pts-1)*Ts;
       
        figure; plot(t,Step); xlabel('t'); ylabel('Step'); legend('Step');
        %-------------------------------------------Save to txt file 
        name = ['Step',num2str(Max_Heigh),'_t',num2str(t_end),'_tZ',num2str(t_zero)] ; 
        fid = fopen([name '.txt'], 'w'); 
        fprintf(fid, '%d\r\n', length(Step)); 
        for i = 1:length(Step) 
            fprintf(fid, '%f\r\n', Step(i));
        end
        fclose(fid);
        
        
        
     case('Free')
        
        Data_pts = t_end*5000;
        t = 0:Ts:(Data_pts-1)*Ts;
        eval(['ref =', Signal ;]) ;
        ref = ref' ;
        figure; plot(t,ref); xlabel('t'); ylabel('Free Signal'); legend(Save_name) ; title(Signal);
        %-------------------------------------------Save to txt file 
        fid = fopen([Save_name '.txt'], 'w'); 
        fprintf(fid, '%d\r\n', length(ref)); 
        for i = 1:length(ref) 
            fprintf(fid, '%f\r\n', ref(i));
        end
        fclose(fid);
end
        




