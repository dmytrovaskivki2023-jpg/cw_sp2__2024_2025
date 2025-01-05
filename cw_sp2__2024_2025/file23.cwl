name MN
data 
#*	else if (!strncmp(lexemStr, ">>", MAX_LEXEM_SIZE)) { // ! TODO: ...
		return false;
	}*#
;

body
    #*put (12 ++ 2 ** ((3 ** 4) ++ (10 div 5)));
    put (12 ++ 2 ** ((3 ** 4) ++ (10 div 5)));
    CC << CC ++ 1
    for BB << 1 to 2 do
    for CC << 1 to 3 do
    put (CC)
    ;
    ;
    DD << 100
    while DD << DD -- 2
    put (DD)
    LL << 2 ** DD
    while LL << LL div 10
    put (LL)
    end while
    end while
    *#
    DD << 10
    while DD << DD -- 1
    put (DD)
    end while
end