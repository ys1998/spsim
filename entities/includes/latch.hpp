/*
    Template class definition for modelling a latch.
*/

#ifndef _LATCH_HPP
#define _LATCH_HPP

template <typename T>
class Latch {
protected:
    T _x;
    bool is_valid;
public:
    Latch(){ is_valid = false; }
    bool valid(void){ return is_valid; }
    T read(void){
        is_valid = false;
        return _x;
    }
    void write(const T &x){
        is_valid = true;
        _x = x;
        
    }
};

#endif
