function [Hys_y]=Hys_Fab_Hybrid_V5(r)


%========================================================Parameter


global Alpha a_i Beta b_i Hys_u_prev  C  order ref K data_points Fab_past type


%========================================================At start
if K == 1;  
    
    if ref(K)~=0   
   %----------------------------------------Store Alpha Beta
        type=1;
        Alpha(1)=r;
        
        a=r; b=Beta(1);
        
        switch order
            case{4}
                Fab=[    b     a   b^2   a*b   a^2   b^3   a*b^2   a^2*b   a^3   b^4   a*b^3   a^2*b^2   a^3*b   a^4  ]*C ;
                
                Correction_term = [    a     a   a^2   a*a   a^2   a^3   a*a^2   a^2*a   a^3   a^4   a*a^3   a^2*a^2   a^3*a   a^4  ]*C ;
                
            case {3}
                Fab=[    b     a   b^2   a*b   a^2   b^3   a*b^2   a^2*b   a^3   ]*C;
        end
          
        Hys_y = (Fab-Correction_term);
        type = 1;
                
    elseif ref(K)==0   
        
        Hys_y=0;
        type = 0;
        
    end

    
%========================================================Ascending
elseif ref(K) > ref(K-1);  
       
   %----------------------------------------Calculate Hysteresis
       
       a = r; 
       b = Beta(b_i) ;
 
       switch order
           case {4}
               Fab=[    b     a   b^2   a*b   a^2   b^3   a*b^2   a^2*b   a^3   b^4   a*b^3   a^2*b^2   a^3*b   a^4  ]*C;
               
               Correction_term = [    a     a   a^2   a*a   a^2   a^3   a*a^2   a^2*a   a^3   a^4   a*a^3   a^2*a^2   a^3*a   a^4  ]*C ;
         
           case {3}
               Fab=[    b     a   b^2   a*b   a^2   b^3   a*b^2   a^2*b   a^3   ]*C;
        end
            
       Hys_y = Fab_past + (Fab-Correction_term)  ;
       type = 1;
       
   %----------------------------------------Calculate Fab_past           
       if ( K == data_points )
           Alpha(a_i) = r;
           
       elseif ( ref(K) < ref(K+1) )           
           Alpha(a_i) = r; 
       
       elseif ( ref(K) > ref(K+1) ) % 升降轉折
           Alpha(a_i) = r;   
           b_i = b_i + 1 ;
           Fab_past = Hys_y ;
       end
       
%========================================================Descending
elseif ref(K) < ref(K-1);
    
       
   %----------------------------------------Calculate Hysteresis
       a = Alpha(a_i); 
       b = r ;
 
       switch order
           case {4}
               Fab=[    b     a   b^2   a*b   a^2   b^3   a*b^2   a^2*b   a^3   b^4   a*b^3   a^2*b^2   a^3*b   a^4  ]*C;
               
               Correction_term = [    a     a   a^2   a*a   a^2   a^3   a*a^2   a^2*a   a^3   a^4   a*a^3   a^2*a^2   a^3*a   a^4  ]*C ;
         
           case {3}
               Fab=[    b     a   b^2   a*b   a^2   b^3   a*b^2   a^2*b   a^3   ]*C;
        end
            
       Hys_y = Fab_past - (Fab-Correction_term)  ;
       type = -1;

   %----------------------------------------Calculate Fab_past           
       if ( K == data_points )
           Beta(b_i) = r;
           
       elseif ( ref(K) > ref(K+1) )           
           Beta(b_i) = r; 
       
       elseif ( ref(K) < ref(K+1) ) % 升降轉折
           Beta(b_i) = r;   
           a_i = a_i + 1 ;
           Fab_past = Hys_y ;
       end
       
%========================================================Maintain
elseif ref(K) == ref(K-1);
    Hys_y=Hys_u_prev;
    
    % Part 1
    if (type == 1)              % 在這之前是上升，然後停住 
        
       %----------------------------------------Calculate Hysteresis
       a = r; 
       b = Beta(b_i) ;
 
       switch order
           case {4}
               Fab=[    b     a   b^2   a*b   a^2   b^3   a*b^2   a^2*b   a^3   b^4   a*b^3   a^2*b^2   a^3*b   a^4  ]*C;
               
               Correction_term = [    a     a   a^2   a*a   a^2   a^3   a*a^2   a^2*a   a^3   a^4   a*a^3   a^2*a^2   a^3*a   a^4  ]*C;
         
           case {3}
               Fab=[    b     a   b^2   a*b   a^2   b^3   a*b^2   a^2*b   a^3   ]*C;
        end
            
       Hys_y = Fab_past + (Fab-Correction_term)  ; 

       %----------------------------------------Calculate Fab_past  
        if ( K == data_points )
            Alpha(a_i) = r;
           
        elseif (ref(K) > ref(K+1)) % 下一個點是下降
            Alpha(a_i) = r;   
            b_i = b_i + 1 ; 
            Fab_past = Hys_y ;
            
        else
            Alpha(a_i) = r;   % 下一個點是上升或持平
        end
        %--------------------------------------- 
        
     
    % Part 2
    elseif (type == -1)  % 在這之前是下降，然後停住     
                
     %----------------------------------------Calculate Hysteresis
       a = Alpha(a_i); 
       b = r ;
 
       switch order
           case {4}
               Fab=[    b     a   b^2   a*b   a^2   b^3   a*b^2   a^2*b   a^3   b^4   a*b^3   a^2*b^2   a^3*b   a^4  ]*C;
               
               Correction_term = [    a     a   a^2   a*a   a^2   a^3   a*a^2   a^2*a   a^3   a^4   a*a^3   a^2*a^2   a^3*a   a^4  ]*C;
         
           case {3}
               Fab=[    b     a   b^2   a*b   a^2   b^3   a*b^2   a^2*b   a^3   ]*C;
        end
            
       Hys_y = Fab_past - (Fab-Correction_term) ;    
       
     %----------------------------------------Calculate Fab_past  
        if ( K == data_points )
            Beta(b_i) = r;
            
        elseif (ref(K) < ref(K+1)) % 下一個點是上升
           Beta(b_i) = r;   
           a_i = a_i + 1 ;
           Fab_past = Hys_y ;
            
        else
            Beta(b_i) = r;   % 下一個點是下降或持平
        end
    %---------------------------------------
    
    
    % Part 3
    elseif (type == 0)         % 在這之前是下降，然後停住
        Hys_y = 0;
        
    end
end

Hys_u_prev = Hys_y ;


