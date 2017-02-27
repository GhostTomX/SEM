%-------------------------------------- Some Parameter Class define
global ref order C
order = 3;   % 幾次方來fit，可選3/4/5
ref = r;
%-------------------------------------- Parameter Setting
Hys_Save_Dscb = 'Ref2Hys' ;             
Speed_level = 60 ;          % 選擇要讀取資料的速度等級
P = 10; %peak num
%-------------------------------------- Basic Parameter
Ts = 1/5000;
Tem = 4.9492;
Cx = 84;

%-------------------------------------- Calculate Up ve y Q Uh_Real
% ve = ve(2:end) ;
% y = y(2:end) ;
% u = r(1:end-1) ;
% r = r(1:end-1) ;
% 
%Qx = Cx*ve;     
% Up = ( Qx - Tem * y ) / 3.6 ;
% Up = Up / 20;                 % piezo 有兩個X軸同時推，除2 ； 還原成沒有壓電放大器之前的，除10 (放大器放大10倍 ，除10才是電腦放出的電壓值)
% Uh_Real = u - Up - ve/10;
% t = 0 : Ts : Ts*(length(y)-1) ;

%---------------------------------------找最大最小值，這邊有可能要用手動找，尤其是速度很快的時候
[Mx_Value_input ,Mx_pts ]= findpeaks(r);
[Mn_Value_input, Mn_pts ]= findpeaks(-r);

Mx_pts = Mx_pts' ;
Mn_pts = Mn_pts' ;

Mn_pts = [1 Mn_pts length(r) ];   % 用findpeaks可以找到凹點的點編號，但是Beta還包含了一開始點點以及最後的點，這兩個點findpeaks不會找出來，所以要自行補上去
                    
%-------------------------------------- 建立 X-Axis Static Preisach Model
alpha = r(Mx_pts);
beta = r(Mn_pts);

zd = [];                               % 用來儲存每一段Local_MAX下降時候的下降量 (Uh)
m = [];                                % 把所有的Local_MAX下降量的column vector疊起來
betam = [];                            % 和下降有關，用來儲存下降時候的 Ve 
alpham = [];                           % 用來儲存上升到最大時候的local_Max，長度和betam相同

for i = 2:length(Mx_pts)               % 有幾個最大值就有幾個 peak
    for j = Mx_pts(i):Mn_pts(i+1)      % 下降時候的點編號
        zd = [ zd ; Uh_Real(Mx_pts(i)) - Uh_Real(j) ]; % 在這一個peak降到最低之前，每個降點造成的下降量 (Max-當下值)
        betam = [ betam ; r(j) ];   % 把下降時候的 Ve 值存到 betam
    end
    m = [m;zd];                        % 把每一個Max造成的下降量 column 疊起來 - > 下降量由 Uh 來找到
    alpham = [alpham;alpha(i)*ones(length(zd),1)];    % [ 每一段下降點度所對應的點數目 * 該段的Ve最大值 ] 把很多斷這種 column 疊起來
    zd = [];    
end

x = alpham ;    % Local max 時候的 Ve 值 (和Q成正比)
y = betam  ;    % 下降時候的 Ve 值 (和Q成正比)
z = m      ;    % 對應到Uh的下降量


A1 = [];
for i = 1:length(x)
    switch order
        case {3}
            A1 = [A1; y(i), x(i), y(i)^2, x(i)*y(i), x(i)^2, y(i)^3, x(i)*y(i)^2, x(i)^2*y(i), x(i)^3]; 
                   %[ C01,  C10,  C02,    C11,       C20,    C03,    C12,         C21,         C30]
        case {4}
            A1 = [A1; y(i), x(i), y(i)^2, x(i)*y(i), x(i)^2, y(i)^3, x(i)*y(i)^2, x(i)^2*y(i), x(i)^3, y(i)^4, x(i)*y(i)^3, x(i)^2*y(i)^2, x(i)^3*y(i), x(i)^4 ];   
                   %[ C01,  C10,  C02,    C11,       C20,    C03,    C12,         C21,         C30,    C04,    C13,         C22,           C31,          C40 ]    
        case {5}
            
            A1 = [A1; y(i), x(i), y(i)^2, x(i)*y(i), x(i)^2, y(i)^3, x(i)*y(i)^2, x(i)^2*y(i), x(i)^3, y(i)^4, x(i)*y(i)^3, x(i)^2*y(i)^2, x(i)^3*y(i), x(i)^4, y(i)^5, x(i)*y(i)^4, x(i)^2*y(i)^3, x(i)^3*y(i)^2, x(i)^4*y(i), x(i)^5 ];
                  %[ C01(1), C10(2), C02(3), C11(4),  C20(5),  C03(6),  C12(7),   C21(8),        C30(9), C04(10),   C13(11),     C22(12),   C31(13),    C40(14),C05(15),   C14(16),   C23(17),          C32(18),       C41(19)    C50(20)]   
        case {6}
            
            A1 = [A1; y(i), x(i), y(i)^2, x(i)*y(i), x(i)^2, y(i)^3, x(i)*y(i)^2, x(i)^2*y(i), x(i)^3, y(i)^4, x(i)*y(i)^3, x(i)^2*y(i)^2, x(i)^3*y(i), x(i)^4, y(i)^5, x(i)*y(i)^4, x(i)^2*y(i)^3, x(i)^3*y(i)^2, x(i)^4*y(i), x(i)^5, y(i)^6, x(i)*y(i)^5, x(i)^2*y(i)^4, x(i)^3*y(i)^3, x(i)^4*y(i)^2, x(i)^5*y(i), x(i)^6 ];
                  %[ C01(1), C10(2), C02(3), C11(4),  C20(5),  C03(6),  C12(7),   C21(8),        C30(9), C04(10),   C13(11),     C22(12),   C31(13),    C40(14),C05(15),   C14(16),   C23(17),          C32(18),    C41(19)    C50(20)  C06(21)   C15(22)    C24(23)        C33(24)        C42(25)        C51(26)      C60(27) ]   
    end
                      
end
C = (A1'*A1)^-1*A1'*( z ); % C 為 Preisach Model 多項式係數，為 Ve-Uh Preisacj model 的係數
%=======================================================
% Coef_name = ['Coef_C',num2str(Cx),'_Lv',num2str(Speed_level)];
% eval([Coef_name '= C;']) ; 
% save_name = ['Hys_Coe_Data_',Hys_Save_Dscb,'_Spd_Lv_',num2str(Speed_level),'_C',num2str(Cx)] ;
% String_Do_Save = [ ' save ', save_name,'.mat ', Coef_name,' Tem Cx ve Qx y r u t Up Uh_Real  ; '] ;
% eval ( String_Do_Save) ;
%=======================================================
%-------------------------------------- Parameter Setting

% 選擇要讀取資料的速度等級
%-------------------------------------- Hyst data load 
% Hys_Save_Dscb = 'Ref2Hys' ;
%  name = ['Hys_Coe_Data_',Hys_Save_Dscb,'_Spd_Lv_',num2str(Speed_level),'_C',num2str(Cx)] ;
%load([save_name '.mat']) ;
%--------------------------------------依照速度自動存成相對應的變數
% Coef_name = ['Coef_C',num2str(Cx),'_Lv',num2str(Speed_level)];
% eval(['C = ', Coef_name ,';']) ;  
%--------------------------------------依照速度自動存成相對應的變數以及TXT檔
   

    if (order == 3)
    name = ['C_coefficient_sin_',num2str(Speed_level),'_C',num2str(Cx),'_P_',num2str(P),'_r_order__', num2str(order)] ;
    fid = fopen([name '.txt'], 'w');
    fprintf(fid, '%d\r\n', order);
    fprintf(fid, '%f %f %f %f %f %f %f %f %f \r\n', C(1), C(3), C(6),  C(2), C(4), C(7),  C(5), C(8),  C(9)); 
                                                                 % [ C01  C02   C03   C10   C11   C12    C20   C21   C30  ]
    fclose(fid);

    elseif(order ==4)
    name = ['C_coefficient_sin_',num2str(Speed_level),'_C',num2str(Cx),'_P_',num2str(P),'_r_order__', num2str(order)] ;
    fid = fopen([name '.txt'], 'w');
    fprintf(fid, '%d\r\n', order);
    fprintf(fid, '%f %f %f %f %f %f %f %f %f %f %f %f %f %f\r\n', C(1), C(3), C(6), C(10), C(2), C(4), C(7), C(11), C(5), C(8), C(12), C(9), C(13), C(14)); 
                                                                 % [ C01  C02   C03   C04    C10   C11   C12   C13    C20   C21   C22    C30   C31    C40 ]
    fclose(fid);
    
    elseif(order ==5)
    name = ['C_coefficient_sin_',num2str(Speed_level),'_C',num2str(Cx),'_P_',num2str(P),'_r_order__', num2str(order)] ;
    fid = fopen([name '.txt'], 'w');
    fprintf(fid, '%d\r\n', order);
    fprintf(fid, '%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f \r \n',C(1), C(3), C(6), C(10), C(15), C(2), C(4), C(7), C(11), C(16), C(5), C(8), C(12), C(17), C(9), C(13), C(18), C(14), C(19), C(20)); 
                                                                                % [ C01  C02   C03   C04   'C05'   C10   C11   C12   C13   'C14'   C20   C21   C22  'C23'   C30    C31  'C32'  C40 'C41' 'C50' ]
    fclose(fid);
    
    elseif(order ==6)
    name = ['C_coefficient_sin_',num2str(Speed_level),'_C',num2str(Cx),'_P_',num2str(P),'_r_order__', num2str(order)] ;
    fid = fopen([name '.txt'], 'w');
    fprintf(fid, '%d\r\n', order);
    fprintf(fid, '%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f \r \n',C(1), C(3), C(6), C(10), C(15), C(21), C(2), C(4), C(7), C(11), C(16), C(22), C(5), C(8), C(12), C(17), C(23),  C(9), C(13), C(18), C(24),  C(14), C(19), C(25), C(20), C(26), C(27) ); 
                                                                                                    % [ C01  C02   C03   C04    C05    C06   C10   C11   C12   C13   'C14'     C15   C20   C21   C22    'C23'    C24    C30    C31  'C32'    C33    C40   'C41'    C42  'C50' C51   C60 ]
    fclose(fid);
     
    end



