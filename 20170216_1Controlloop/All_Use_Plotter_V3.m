
%------------------------------------選擇要標示的Title : 標示選 1 / 不標選 0
Title_Discription =  ' 60Hz_Test         ';
Free_Plot_Title =   ' Up - y ';

%----------------選則是否時間擷取 t1_t2 = [起始 / 終止時間]
Time_Clip = 0;     t1_t2 = [0.2002 0.225] ;   
LimY      = 0 ;  y_limit = [-0.5 3]    ;  % 限制Y軸顯示範圍 on / off

%----------------Same Time Plot A / B / C / D / E  ； 
% 選則要畫的東西 :要畫的再等號右邊輸入變數名稱 / 不畫的輸入 []

Time = t;
%----------------Horizon Line
Horizon_Plot  = 1 ;

%----------------Plot Element
A_y =[y] ;   A_legend =  '  ss     ';
B_y =[u] ;   B_legend =  '  ss  ';
C_y =[] ;   C_legend =  '  0.6r         ';  
D_y =[] ;   D_legend =  've2           ';   
E_y =[] ;   E_legend =  '           ';   


%----------------Free Style Plot A / B / C / D / E  ； 
% 選則要畫的東西 :要畫的再等號右邊輸入變數名稱 / 不畫的輸入 []
S_E = [1 2499];   % 起始 / 終止點

fA_x =[] ;    
fA_y =[] ;          
fA_legend =  've - y               ';


fB_x =[] ;    
fB_y =[] ;          
fB_legend =  'Up2 - y                      ';


fC_x =[] ;   
fC_y =[] ;    
fC_legend =  've - Uh               ';  






%-------------------------------------Plot Progress Main Plot
plot_element = [];
plot_legend = cell(6,1);  
plot_sequenc = 0;

switch Time_Clip
    case {0}
        t_plot = Time ;
        j = 1 ;
        k = length(Time) ;
    case {1}
        t1 = t1_t2(1) ;
        t2 = t1_t2(2) ;
        [t_plot,A_y_clip,j,k]=Time_Cutting(Time,A_y,t1,t2) ;   % Uh_Real_clip 純粹是為了符合輸出格式而產生的
        t_plot = Time(j:k) ;
        clear A_y_clip ;
end



if (length(A_y) ~= 0)
        plot_sequenc = plot_sequenc + 1;
        plot_legend(plot_sequenc) = cellstr(A_legend) ;
        plot_element = [plot_element , A_y(j : k)] ;

end   

if (length(B_y) ~= 0)
        plot_sequenc = plot_sequenc + 1;
        plot_legend(plot_sequenc) = cellstr(B_legend) ;
        plot_element = [plot_element , B_y(j : k)] ;
end
    

if (length(C_y) ~= 0)
        plot_sequenc = plot_sequenc + 1;
        plot_legend(plot_sequenc) = cellstr(C_legend) ;
        plot_element = [plot_element , C_y(j : k)] ;
end
    

if (length(D_y) ~= 0)
        plot_sequenc = plot_sequenc + 1;
        plot_legend(plot_sequenc) = cellstr(D_legend) ;
        plot_element = [plot_element , D_y(j : k)] ;
end

if (length(E_y) ~= 0)
        plot_sequenc = plot_sequenc + 1;
        plot_legend(plot_sequenc) = cellstr(E_legend) ;
        plot_element = [plot_element , E_y(j : k)] ;
end


%-------------------------------------Free Style Plot Progress Main Plot
fplot_legend = cell(3,1);  
fplot_sequenc = 0;

if (length(fA_y) ~= 0)
        fplot_sequenc = fplot_sequenc + 1;
        fplot_legend(fplot_sequenc) = cellstr(fA_legend) ;
end   

if (length(fB_y) ~= 0)
        fplot_sequenc = fplot_sequenc + 1;
        fplot_legend(fplot_sequenc) = cellstr(fB_legend) ;
end
    

if (length(fC_y) ~= 0)
        fplot_sequenc = fplot_sequenc + 1;
        fplot_legend(fplot_sequenc) = cellstr(fC_legend) ;
end



    

%-------------------------------------畫圖
if ( length(plot_element) ~= 0 )
    figure;
    if (Horizon_Plot ~= 0)
        hold on ;
        plot_legend = ([ cellstr('Zero Line') ; plot_legend ]) ;
        plot( t_plot' , zeros(size(t_plot')) , 'r') ;
        plot_sequenc = plot_sequenc + 1;

    end
    
    plot( t_plot' , plot_element ) ;
    legend(plot_legend(1:plot_sequenc)) ;
    title(cellstr(Title_Discription)) ;
    xlabel('t (sec)') ;
    if ( LimY ==1 ) ylim(y_limit);end
end


if ( fplot_sequenc ~= 0 )
    figure;
    hold on;
    if (length(fA_y)~=0)
        plot(fA_x( S_E(1) : S_E(2) ) , fA_y( S_E(1) : S_E(2) ) , 'b') ;
    end
    if (length(fB_y)~=0)
        plot(fB_x( S_E(1) : S_E(2) ) , fB_y( S_E(1) : S_E(2) ) , 'r') ;
    end
    if (length(fC_y)~=0)
        plot(fC_x( S_E(1) : S_E(2) ) , fC_y( S_E(1) : S_E(2) ) , 'color',[0 0.5 0]) ;
    end
    hold off;
    legend(fplot_legend(1:fplot_sequenc)) ;
    title(cellstr(Free_Plot_Title)) ;
end

clear Free_Plot_Title t1_t2 Title_Discription
clear fplot_sequenc fplot_legend Free_Plot_Title fA_y fB_y fC_y S_E plot_sequenc LimY y_limit plot_legend Horizon_Plot A_y B_y C_y D_y E_y Time_Clip fA_legend fB_legend fC_legend A_legend B_legend C_legend D_legend E_legend