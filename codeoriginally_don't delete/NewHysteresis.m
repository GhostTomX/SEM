%-------------------------------------- Some Parameter Class define
global ref order C
order = 3;   % �X�����fit�A�i��3/4/5
ref = r;
%-------------------------------------- Parameter Setting
Hys_Save_Dscb = 'Ref2Hys' ;             
Speed_level = 60 ;          % ��ܭnŪ����ƪ��t�׵���
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
% Up = Up / 20;                 % piezo �����X�b�P�ɱ��A��2 �F �٭즨�S�����q��j�����e���A��10 (��j����j10�� �A��10�~�O�q����X���q����)
% Uh_Real = u - Up - ve/10;
% t = 0 : Ts : Ts*(length(y)-1) ;

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

for i = 2:length(Mx_pts)               % ���X�ӳ̤j�ȴN���X�� peak
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
C = (A1'*A1)^-1*A1'*( z ); % C �� Preisach Model �h�����Y�ơA�� Ve-Uh Preisacj model ���Y��
%=======================================================
% Coef_name = ['Coef_C',num2str(Cx),'_Lv',num2str(Speed_level)];
% eval([Coef_name '= C;']) ; 
% save_name = ['Hys_Coe_Data_',Hys_Save_Dscb,'_Spd_Lv_',num2str(Speed_level),'_C',num2str(Cx)] ;
% String_Do_Save = [ ' save ', save_name,'.mat ', Coef_name,' Tem Cx ve Qx y r u t Up Uh_Real  ; '] ;
% eval ( String_Do_Save) ;
%=======================================================
%-------------------------------------- Parameter Setting

% ��ܭnŪ����ƪ��t�׵���
%-------------------------------------- Hyst data load 
% Hys_Save_Dscb = 'Ref2Hys' ;
%  name = ['Hys_Coe_Data_',Hys_Save_Dscb,'_Spd_Lv_',num2str(Speed_level),'_C',num2str(Cx)] ;
%load([save_name '.mat']) ;
%--------------------------------------�̷ӳt�צ۰ʦs���۹������ܼ�
% Coef_name = ['Coef_C',num2str(Cx),'_Lv',num2str(Speed_level)];
% eval(['C = ', Coef_name ,';']) ;  
%--------------------------------------�̷ӳt�צ۰ʦs���۹������ܼƥH��TXT��
   

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



