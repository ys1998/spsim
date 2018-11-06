/*
	Class declaration for the flushing unit responsible for clearing 
	invalid instructions due to mispredicted branches.
*/

#ifndef _FLUSHER_HPP
#define _FLUSHER_HPP

#include "fetcher.hpp"
#include "decoder.hpp"
#include "issuer.hpp"
#include "writer.hpp"

class Flusher {
protected:
	Issuer *is;
	Fetcher *f;
	Decoder *d;
	Writer *w;

public:
	Flusher(Issuer* , Fetcher* , Decoder* , Writer* );
	void flush(int,int);
};

#endif