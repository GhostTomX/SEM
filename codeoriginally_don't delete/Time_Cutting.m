% 擷取某部分時間
function [T,V,i,j]=Time_Cutting(T,V,t1,t2)  % (): 輸入變數在這邊run的時候的名稱, 不用擔心名稱和現有變數相同；當function在跑的時候只有global變數會拿來用
                                            % []: 輸出變數的名稱, 如果沒有特別指定名稱, 就會用這個名字
                                            % i = initial point
                                            % j = end point

% T=time
% V=Vlotage
% t1=Start time
% t2=End time

i=1; 
while (T(i)-t1)<0;   %切掉t1以前的, 只要時間比t1小，就繼續跑
    i=i+1;       
end

V=V(i:length(T));
Ttranscient=T(i:length(T));    %把t1以前的都切掉, 暫存到 Ttranscient

  

j=1; 
while (Ttranscient(j)-t2)<0;   %切掉t2以後的, 只要時間比t1小，就繼續跑
    j=j+1;       
end

V=V(1:j);               %把t2以後的都切掉
T=T(1:j);               %把時間上要把之後的V移動到前面，所以還是從初始T的時間開始
 
j = j + i -1 ;
