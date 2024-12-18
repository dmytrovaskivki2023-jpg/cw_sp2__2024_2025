name MN
data 
    long int BB
    long int DD
    long int EE
    long int KK
;

body
    KK << 25630;
    get ( BB );
    get ( DD );
    get ( EE );
    put ( KK ++ BB -- DD div 2 ++ EE ** 4);
end