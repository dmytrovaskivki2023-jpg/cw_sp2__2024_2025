NAME _PROGRAM ;
BODY DATA INTEGER16 _DDDDDDD, _YYYYYYY, _KKKKKKK
#*	else if (!strncmp(lexemStr, ">>", MAX_LEXEM_SIZE)) { // ! TODO: ...
		return false;
	}
    
    _DDDDDDD << 2
    REPEAT
    _YYYYYYY << 3
    REPEAT
        PUT (_YYYYYYY)
    _YYYYYYY << _YYYYYYY - 1
    UNTIL _YYYYYYY - 0
    _DDDDDDD << _DDDDDDD - 1
    UNTIL _DDDDDDD - 0   
    
    *#
;
#*IF(0 + 0) PUT (123);
ELSE PUT (0);*#
    _DDDDDDD << 10
    WHILE _DDDDDDD - 1 
    PUT (_DDDDDDD)
    IF(_DDDDDDD - 5) #*_DDDDDDD << 10*# ;#* ELSE;IF(_DDDDDDD - 5)*# ELSE EXIT WHILE ;
    _DDDDDDD << _DDDDDDD - 1
    #*_DDDDDDD << _DDDDDDD - 1*#
    END WHILE 
    PUT (_DDDDDDD)

END