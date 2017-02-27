
Ref = load('Ref_Zig20167_p10_Speed_Lv_28_prezero_0.txt'); 
r = Ref (2:end);

Measurement_Data = load('Preh_ID.txt');
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
Speed_level = 1 ;    
order = 4 ;                % 幾次方來fit，可選3或是4
%-------------------------------------- Basic Parameter
Ts = 1/100;
%Tem = 6.625;
Tem = 4.9492;
Cx = 84;

%-------------------------------------- Calculate Up ve y Q Uh_Real

Qx = 0.5*Cx*ve;     
%Up = ( Qx - Tem * y ) / 1.8 ;
Up = ( Qx - Tem * y ) / 7.2 ;
Up = Up / 10;                 % piezo 有兩個X軸同時推，除2 ； 還原成沒有壓電放大器之前的，除10 (放大器放大10倍 ，除10才是電腦放出的電壓值)


Uh_Real = u - Up - ve/10;
t = 0 : Ts : Ts*(length(y)-1) ;


%---------------------------------------
Vol_dis = Speed_level/4/5000 ; % 兩點之間電壓差異
Speel_V_sec = Speed_level/4;   % Speed V/s

%-------------------------------------- Some Parameter Class define
global ref order C
order = 4 ;
ref = r;


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
for i = 1:length(Mx_pts)               % 有幾個最大值就有幾個 peak
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
            
        case {4}
            A1 = [A1; y(i), x(i), y(i)^2, x(i)*y(i), x(i)^2, y(i)^3, x(i)*y(i)^2, x(i)^2*y(i), x(i)^3, y(i)^4, x(i)*y(i)^3, x(i)^2*y(i)^2, x(i)^3*y(i), x(i)^4 ];        
    end
           %[ C01 , C10, C02, C11, C20, C03, C12, C21, C30,  C04, C13, C22, C31, C40 ]
                   
end


C = (A1'*A1)^-1*A1'*( z ); % C 為 Preisach Model 多項式係數，為 Ve-Uh Preisacj model 的係數



name = ['C_coefficient'] ;
fid = fopen([name '.txt'], 'w');
fprintf(fid, '%f %f %f %f %f %f %f %f %f %f %f %f %f %f\r\n', C(1), C(3), C(6), C(10), C(2), C(4), C(7), C(11), C(5), C(8), C(12), C(9), C(13), C(14)); 
                                                       % [ C01  C02   C03   C04    C10   C11   C12   C13    C20   C21   C22    C30   C31    C40 ]
fclose(fid);



