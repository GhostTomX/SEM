
%-------------------------------------- Parameter Setting
Save_to_tst = 1 ;
Speed_level = 1 ;          % ��ܭnŪ����ƪ��t�׵���
Cx = 84 ;

% %-------------------------------------- Hyst data load 
% % Hys_Save_Dscb = 'Ref2Hys' ;
% % name = ['Hys_Coe_Data_',Hys_Save_Dscb,'_Spd_Lv_',num2str(Speed_level),'_C',num2str(Cx)] ;
% load(['Hys_Coe_Data_Ref2Uh_Spd_Lv_1_C84'+ '.mat']) ;
% 
% global ref order C
% order = 4 ;
% ref = r;
% %--------------------------------------�̷ӳt�צ۰ʦs���۹������ܼ�
% 
% Coef_name = ['Coef_C',num2str(Cx),'_Lv',num2str(Speed_level)];
% eval(['C = ', Coef_name ,';']) ;  


%--------------------------------------�̷ӳt�צ۰ʦs���۹������ܼƥH��TXT��
if (Save_to_tst == 1)
    name = ['C_coefficient_Lv_',num2str(Speed_level),'_C',num2str(Cx),'_r'] ;
    fid = fopen([name '.txt'], 'w');
    fprintf(fid, '%f %f %f %f %f %f %f %f %f %f %f %f %f %f\r\n', C(1), C(3), C(6), C(10), C(2), C(4), C(7), C(11), C(5), C(8), C(12), C(9), C(13), C(14)); 
                                                           % [ C01  C02   C03   C04    C10   C11   C12   C13    C20   C21   C22    C30   C31    C40 ]
    fclose(fid);
end




