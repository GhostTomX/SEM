Prsh_Mode = 2;
Temx = 6.625;
Cx = 84;
Time_Clip = 0;     t1_t2 = [0 2.7] ;   

Simulation_Title = 'u to Uh Preisach Model' ;
Error_Title = 'u to Uh Preisach Error' ;
%============================Switch to Negative lowerbound type or Bias type 
switch Prsh_Mode
    case {1}
        ab0=0;       
        Bias_chsn = 0;     
        
    case {2}
        ab0=0;
        Bias_chsn= -(1.686-0.7245);%-0.555;  % Dynamic is proper
end

%======================================Hysteresis Compensate Mode
Pure_Preisach = 1 ;
Hybrid_cmp    = 0 ;

%======================================Pre Setting of Preisach Model

global Alpha Beta a_i b_i Hys_u_prev Fab_past K data_points type ref
Alpha= ab0*ones(100,1); 
 Beta= ab0*ones(100,1);

a_i = 1;   
b_i = 1;

ref = r;
Hys_u_prev=0;
Fab_past = 0;
type = 0;     


%======================================Time Clip
switch Time_Clip
    case {0}
        t_plot = t ;
        j = 1 ;
        k = length(t) ;
    case {1}
        t1 = t1_t2(1) ;
        t2 = t1_t2(2) ;
        [t_plot,Uh_Real_clip,j,k]=Time_Cutting(t,Uh_Real,t1,t2) ;   % Uh_Real_clip 純粹是為了符合輸出格式而產生的
        clear Uh_Real_clip ;
end

data_points = k - j + 1;
%======================================Main Code

Uh_sim_Prsh = zeros(data_points,1);
Uh_sim_Hyb = zeros(data_points,1);



for K= j : k
        
    if (Pure_Preisach + Hybrid_cmp ~= 0 )
        Preisach_now=Hys_Fab_Hybrid_V6(ref(K));  % 帶入當下的 ve
    end


    if ( Pure_Preisach ~=0 )
        Uh_sim_Prsh (K) = Preisach_now + Bias_chsn ;
    end

    if (Hybrid_cmp ~= 0)
        if (K==1)
            Uh_sim_Hyb(K) = Preisach_now ;
        else
            Uh_sim_Hyb(K) = Uh_Real(K-1) + (Preisach_now - Preisach_prev) ;
        end
    end
    

    if (Pure_Preisach + Hybrid_cmp ~= 0)
        Preisach_prev = Preisach_now;
    end
end




%-------------------------Plot Real and Simulated
Plot_element = Uh_Real(j : k);
Error_element = [];
Plot_legend = ['  Uh Real   '];
Error_legend = [];
data_num = 1 : 1 : length(ve) ;  data_num = data_num' ;

if (Pure_Preisach~=0) ;
    Plot_element = [Plot_element Uh_sim_Prsh] ;
    Error_element = [Error_element Uh_sim_Prsh-Uh_Real(j : k)] ;
    Plot_legend = [Plot_legend ; 'Uh sim Prsh '] ;
    Error_legend = [Error_legend ; 'Prch Error '] ;
end
    

if (Hybrid_cmp~=0) ;
    Plot_element = [Plot_element Uh_sim_Hyb] ;   
    Error_element = [Error_element Uh_sim_Hyb-Uh_Real(j : k)] ;
    Plot_legend = [Plot_legend ; 'Uh sim Hyb  '] ;
    Error_legend = [Error_legend ; 'Hybrd Error'] ;
    
end


figure;
plot(t(j : k),Plot_element) ;
legend(Plot_legend) ;
title(Simulation_Title) ;
savefig('Uh Real & Uh Simulate');

figure;
plot(t(j : k),Error_element) ;
legend(Error_legend,2) ;
title([Error_Title]);
savefig('Uh Simulation Error');

figure;
plot(ref(j : k),Plot_element,ref(j : k),ref(j : k)); title('Ref - Uh'); legend([ Plot_legend ; '1 : 1 Line  '],2) ; xlabel('ref'); ylabel('Uh');
savefig('Uh Real & Preisach Simulate');




