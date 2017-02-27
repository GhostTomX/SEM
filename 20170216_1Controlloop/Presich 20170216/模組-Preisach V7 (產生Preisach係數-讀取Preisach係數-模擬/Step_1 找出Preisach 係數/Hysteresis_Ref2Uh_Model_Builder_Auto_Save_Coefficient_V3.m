
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
order = 4 ;                % �X�����fit�A�i��3�άO4
%-------------------------------------- Basic Parameter
Ts = 1/100;
%Tem = 6.625;
Tem = 4.9492;
Cx = 84;

%-------------------------------------- Calculate Up ve y Q Uh_Real

Qx = 0.5*Cx*ve;     
%Up = ( Qx - Tem * y ) / 1.8 ;
Up = ( Qx - Tem * y ) / 7.2 ;
Up = Up / 10;                 % piezo �����X�b�P�ɱ��A��2 �F �٭즨�S�����q��j�����e���A��10 (��j����j10�� �A��10�~�O�q����X���q����)


Uh_Real = u - Up - ve/10;
t = 0 : Ts : Ts*(length(y)-1) ;


%---------------------------------------
Vol_dis = Speed_level/4/5000 ; % ���I�����q���t��
Speel_V_sec = Speed_level/4;   % Speed V/s

%-------------------------------------- Some Parameter Class define
global ref order C
order = 4 ;
ref = r;


%---------------------------------------��̤j�̤p�ȡA�o�䦳�i��n�Τ�ʧ�A�ר�O�t�׫ܧ֪��ɭ�

[Mx_Value_input ,Mx_pts ]= findpeaks(r);
[Mn_Value_input, Mn_pts ]= findpeaks(-r);

Mx_pts = Mx_pts' ;
Mn_pts = Mn_pts' ;

Mn_pts = [1 Mn_pts length(r) ];   % ��findpeaks�i�H���W�I���I�s���A���OBeta�٥]�t�F�@�}�l�I�I�H�γ̫᪺�I�A�o����Ifindpeaks���|��X�ӡA�ҥH�n�ۦ�ɤW�h
                      
%-------------------------------------- �إ� X-Axis Static Preisach Model
alpha = r(Mx_pts);
beta = r(Mn_pts);

zd = [];                               % �Ψ��x�s�C�@�qLocal_MAX�U���ɭԪ��U���q (Uh)
m = [];                                % ��Ҧ���Local_MAX�U���q��column vector�|�_��
betam = [];                            % �M�U�������A�Ψ��x�s�U���ɭԪ� Ve 
alpham = [];                           % �Ψ��x�s�W�ɨ�̤j�ɭԪ�local_Max�A���שMbetam�ۦP
for i = 1:length(Mx_pts)               % ���X�ӳ̤j�ȴN���X�� peak
    for j = Mx_pts(i):Mn_pts(i+1)      % �U���ɭԪ��I�s��
        zd = [ zd ; Uh_Real(Mx_pts(i)) - Uh_Real(j) ]; % �b�o�@��peak����̧C���e�A�C�ӭ��I�y�����U���q (Max-��U��)
        betam = [ betam ; r(j) ];   % ��U���ɭԪ� Ve �Ȧs�� betam
    end
    m = [m;zd];                        % ��C�@��Max�y�����U���q column �|�_�� - > �U���q�� Uh �ӧ��
    alpham = [alpham;alpha(i)*ones(length(zd),1)];    % [ �C�@�q�U���I�שҹ������I�ƥ� * �Ӭq��Ve�̤j�� ] ��ܦh�_�o�� column �|�_��
    zd = [];    
end

x = alpham ;    % Local max �ɭԪ� Ve �� (�MQ������)
y = betam  ;    % �U���ɭԪ� Ve �� (�MQ������)
z = m      ;    % ������Uh���U���q


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


C = (A1'*A1)^-1*A1'*( z ); % C �� Preisach Model �h�����Y�ơA�� Ve-Uh Preisacj model ���Y��



name = ['C_coefficient'] ;
fid = fopen([name '.txt'], 'w');
fprintf(fid, '%f %f %f %f %f %f %f %f %f %f %f %f %f %f\r\n', C(1), C(3), C(6), C(10), C(2), C(4), C(7), C(11), C(5), C(8), C(12), C(9), C(13), C(14)); 
                                                       % [ C01  C02   C03   C04    C10   C11   C12   C13    C20   C21   C22    C30   C31    C40 ]
fclose(fid);



