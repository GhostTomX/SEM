% �^���Y�����ɶ�
function [T,V,i,j]=Time_Cutting(T,V,t1,t2)  % (): ��J�ܼƦb�o��run���ɭԪ��W��, ���ξ�ߦW�٩M�{���ܼƬۦP�F��function�b�]���ɭԥu��global�ܼƷ|���ӥ�
                                            % []: ��X�ܼƪ��W��, �p�G�S���S�O���w�W��, �N�|�γo�ӦW�r
                                            % i = initial point
                                            % j = end point

% T=time
% V=Vlotage
% t1=Start time
% t2=End time

i=1; 
while (T(i)-t1)<0;   %����t1�H�e��, �u�n�ɶ���t1�p�A�N�~��]
    i=i+1;       
end

V=V(i:length(T));
Ttranscient=T(i:length(T));    %��t1�H�e��������, �Ȧs�� Ttranscient

  

j=1; 
while (Ttranscient(j)-t2)<0;   %����t2�H�᪺, �u�n�ɶ���t1�p�A�N�~��]
    j=j+1;       
end

V=V(1:j);               %��t2�H�᪺������
T=T(1:j);               %��ɶ��W�n�⤧�᪺V���ʨ�e���A�ҥH�٬O�q��lT���ɶ��}�l
 
j = j + i -1 ;
